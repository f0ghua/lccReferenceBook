/* A rudimentary crash report logger meant to aid tracking down bugs in 
programs compiled without debug information. It is based on sample code 
from Matt Pietrek's "Under the Hood" column in the April 1997 issue of 
Microsoft Systems Journal.

Date: 10/31/2003
Author: Mike Caetano
License: Whatever follows from the Pietrek code plus those portions of code
taken from the MS Platform SDK plus a general "use at your own risk" - and
of course, some credit for the added value of my effort would be nice too! :)
*/
#include <windows.h>
#include <tchar.h>
#include <stdarg.h> // for va_list et al.
#include <stdio.h>	// for _vsntprintf

#include "ueh.h"

// ---------------------------------------------------------------------------

#ifdef __LCC__

// the lcc-win32 headers define this incorrectly
#ifdef EXCEPTION_IN_PAGE_ERROR
#undef EXCEPTION_IN_PAGE_ERROR
#define EXCEPTION_IN_PAGE_ERROR ((DWORD)0xC0000006L)
#endif

// the lcc-win32 headers lack these:
#ifndef EXCEPTION_ILLEGAL_INSTRUCTION
#define EXCEPTION_ILLEGAL_INSTRUCTION  ((DWORD)0xC000001DL)
#endif

#ifndef EXCEPTION_INVALID_HANDLE
#define EXCEPTION_INVALID_HANDLE ((DWORD)0xC0000008L)
#endif

#ifndef SIZE_OF_80387_REGISTERS
#define SIZE_OF_80387_REGISTERS 80
#endif

#ifndef PROCESSOR_ARCHITECTURE_INTEL
#define PROCESSOR_ARCHITECTURE_INTEL 0
#endif

#ifndef PROCESSOR_ARCHITECTURE_UNKNOWN
#define PROCESSOR_ARCHITECTURE_UNKNOWN 0xFFFF
#endif

// ---------------------------------------------------------------------------
// A helpful CreateFile flag that MS declined to make explicit
// this flag prevents a file from being shared
#define FILE_SHARE_NONE 0

// ---------------------------------------------------------------------------
// A helpful RaiseException flag that MS declined to make explicit
// this flag indicates that an exception is not fatal
#define EXCEPTION_CONTINUABLE 0

// ---------------------------------------------------------------------------
// A helpful SetErrorMode flag that MS declined to make explicit
// the system default error mode displays all error dialog boxes
#define SEM_SYSTEMDEFAULT 0

// ---------------------------------------------------------------------------
// is debug info present?
#if __LCCDEBUGLEVEL > 0
#define _DEBUG
#endif

#endif

// ----------------------------------------------------------------------------
// INTERNAL STRUCTURE
//
#define REPORT_BUFFER_LENGTH 0x1000 // 4096 bytes
#define BUFFER_FULL_WARNING 0x50

typedef struct tagExceptionReport {
	TCHAR m_szFileName[MAX_PATH];           // file name for exception report
    TCHAR m_szBuffer[REPORT_BUFFER_LENGTH]; // exception report buffer
	DWORD m_dwIndex;                        // characters written to buffer
	DWORD m_dwRemaining;                    // characters remaining
}
ExceptionReport;

// ensure that space for this structure is embedded in the data section of the pe file
static ExceptionReport g_ExceptionReport = { 0 };


// ---------------------------------------------------------------------------
//
static const LPTSTR g_horzrule = _T("// ---------------------------------------------------------------------------\n");


// ----------------------------------------------------------------------------
// STATIC FUNCTION DECLARATIONS

static int _cdecl ExceptionPrint(LPCTSTR pszFmt, ... );

static LPTSTR _stdcall SetFileNameExt(LPTSTR pszFileName, LPTSTR pszExt);

static LPTSTR _stdcall FormatExceptionString(DWORD dwCode);

static HMODULE _stdcall ModuleFromAddress(PVOID addr);

static BOOL
GetLogicalAddress(
    PVOID addr,
	LPTSTR szModule,
	DWORD len,      // size in TCHARs
	DWORD *section,
	DWORD *offset
    );


static void _stdcall WalkX86CallStack(PCONTEXT pContext);
static void _stdcall GenerateExceptionReport(PEXCEPTION_POINTERS pExceptionInfo);
static HRESULT _stdcall LogExceptionReport(void);


// ----------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
/* Write to a special exception report buffer in printf style

*/
static int _cdecl ExceptionPrint(LPCTSTR pszFmt, ...)
{
    ExceptionReport *pExceptionReport = &g_ExceptionReport;
    if ( NULL == pExceptionReport ) { return 0; }

    // don't write to the buffer if it's full
	if ( 0 == pExceptionReport->m_dwRemaining ) { return 0; }

    int cch = 0;

    // issue a warning if the buffer is near full
	if ( pExceptionReport->m_dwRemaining < BUFFER_FULL_WARNING )
	{

		cch = _sntprintf(
				pExceptionReport->m_szBuffer + pExceptionReport->m_dwIndex,
				pExceptionReport->m_dwRemaining,
				_T("\n\n *** Print Buffer Overrun ***"));

		pExceptionReport->m_dwIndex += ( cch == -1 ) ? pExceptionReport->m_dwRemaining : cch;
		pExceptionReport->m_dwRemaining = 0;

		return cch;
	}

    va_list arglist;
    va_start( arglist, pszFmt );

	cch = _vsntprintf( pExceptionReport->m_szBuffer + pExceptionReport->m_dwIndex,
                    pExceptionReport->m_dwRemaining, pszFmt, arglist );

    va_end(arglist);

    // _vsntprintf:
    // returns the number of characters written,
    // not including the terminating null character
	// if number of characters to write exceeds pExceptionReport->m_dwRemaining
	// then pExceptionReport->m_dwRemaining are written and -1 is returned

    // accounting for null terminator produces artifacts in the report

    // account for possibility of exceeding remaining buffer
    cch = ( cch == -1 ) ? pExceptionReport->m_dwRemaining : cch;

    // forward adjust write buffer pointer
    pExceptionReport->m_dwIndex += cch;
	pExceptionReport->m_dwRemaining -= cch;

    // return number of characters written, including null terminator
    return cch;

}



// ----------------------------------------------------------------------------
/*
Function crafts a new file name by replacing the extension of the
filename contained in LPTSTR pszFileName with contents of LPTSTR pszExt
*/
static LPTSTR _stdcall SetFileNameExt(LPTSTR pszFileName, LPTSTR pszExt)
{
    if ( NULL == pszFileName || NULL == pszExt ) { return NULL; }

	// Look for the '.' before the "EXE" extension.
	PTSTR pszDot = _tcsrchr( pszFileName, _T('.') );
	if ( NULL != pszDot )
	{
		pszDot++;   // Advance past the '.'
		if ( _tcslen(pszDot) >= 3 )
		{
            _tcscpy(pszDot, pszExt);
		}
	}

	return pszFileName;

}


// ----------------------------------------------------------------------------
// function checks if a debugger is attached to the process
#ifdef _DEBUG
int __declspec(naked) IsDebuggerAttached(void)
{
	_asm("fs                     \n"
	     "movl (0x18), %eax      \n" // get linear TIB address
	     "movl 0x30(%eax), %eax  \n" // get value at offset 0x30
	     "movzwl 0x2(%eax), %eax \n" // get value of hiword located there
	     "ret");
}
#endif

// ----------------------------------------------------------------------------
// it should be obvious what this function does

static HRESULT _stdcall DumpTimeStamp(void)
{

	TIME_ZONE_INFORMATION tzi;
	LPTSTR timezone;
	SYSTEMTIME st;

	switch( GetTimeZoneInformation(&tzi) )
	{
		case TIME_ZONE_ID_UNKNOWN :
			timezone = _T("UNKNOWN");
			break;
		case TIME_ZONE_ID_STANDARD :
			timezone = _T("STANDARD");
			break;
		case TIME_ZONE_ID_DAYLIGHT :
			timezone = _T("DAYLIGHT");
			break;
		default :
			timezone = _T("ERROR");
	}

	GetSystemTime(&st);

	ExceptionPrint(_T("UTC|%2d|%2d|%4d|%2d|%2d|%2d|%s|%ls|%4d|%4d\n"),
		st.wMonth, st.wDay, st.wYear,
		st.wHour, st.wMinute, st.wSecond,
		timezone, tzi.StandardName, tzi.Bias, tzi.DaylightBias );

	return S_OK;

}

// ---------------------------------------------------------------------------
/*
patterned after the sdk example
*/
static HRESULT _stdcall DumpSystemVersion(void)
{

   OSVERSIONINFO osvi;

   ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

   GetVersionEx(&osvi);

   switch ( osvi.dwPlatformId )
   {
      case VER_PLATFORM_WIN32_NT:

      // Test for the product.

         if ( osvi.dwMajorVersion <= 4 ) {
            ExceptionPrint(_T("Microsoft Windows NT, "));
         }

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 ) {
            ExceptionPrint(_T("Microsoft Windows 2000, "));
         }

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 ) {
            ExceptionPrint(_T("Microsoft Windows XP, "));
         }

            HKEY hKey;
            TCHAR szProductType[80];
            DWORD dwBufLen;

            RegOpenKeyEx( HKEY_LOCAL_MACHINE,
               _T("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
               0, KEY_QUERY_VALUE, &hKey );
            RegQueryValueEx( hKey, _T("ProductType"), NULL, NULL,
               (LPBYTE) szProductType, &dwBufLen);
            RegCloseKey( hKey );

            if ( lstrcmpi( _T("WINNT"), szProductType) == 0 ) {
               ExceptionPrint(_T("Workstation, ") );
            }

            if ( lstrcmpi( _T("SERVERNT"), szProductType) == 0 ) {
               ExceptionPrint(_T("Server, ") );
            }

      // Display version, service pack (if any), and build number.

         if ( osvi.dwMajorVersion <= 4 )
         {
            ExceptionPrint(_T("version %d.%d %s (Build %d) "),
               osvi.dwMajorVersion,
               osvi.dwMinorVersion,
               osvi.szCSDVersion,
               osvi.dwBuildNumber & 0xFFFF);
         }
         else
         {
            ExceptionPrint(_T("%s (Build %d) "),
               osvi.szCSDVersion,
               osvi.dwBuildNumber & 0xFFFF);
         }
         break;

      case VER_PLATFORM_WIN32_WINDOWS:

         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
         {
             ExceptionPrint(_T("Microsoft Windows 95, "));
             if ( osvi.szCSDVersion[1] == 'C' ) {
                ExceptionPrint(_T("OSR2 ") );
             }
         }

         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
         {
             ExceptionPrint(_T("Microsoft Windows 98, "));
             if ( osvi.szCSDVersion[1] == 'A' ) {
                ExceptionPrint(_T("SE ") );
             }
         }

         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
         {
             ExceptionPrint(_T("Microsoft Windows Me, "));
         }
         break;

      case VER_PLATFORM_WIN32s:

         ExceptionPrint(_T("Microsoft Win32s "));
         break;

   }

   ExceptionPrint(_T("\n"));

   return S_OK;

}


// ---------------------------------------------------------------------------
/*
patterned after the sdk example
*/
static HRESULT _stdcall DumpCPUInfo(void)
{

    OSVERSIONINFO vi = { sizeof(vi) };
    GetVersionEx(&vi);
    BOOL fWin95IsHost = (vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
						(vi.dwMajorVersion == 4) && (vi.dwMinorVersion == 0);

    SYSTEM_INFO sinf;
    GetSystemInfo(&sinf);

    if ( fWin95IsHost) {
        sinf.wProcessorLevel = (WORD)(sinf.dwProcessorType / 100);
    }

    switch ( sinf.wProcessorArchitecture ) {

        case PROCESSOR_ARCHITECTURE_INTEL:

		//Processor Architecture:
        ExceptionPrint(_T("Intel "));

        switch (sinf.wProcessorLevel) {

	        case 3:
	        case 4:
				// Processor Level:
	            ExceptionPrint(_T("80%c86 "), sinf.wProcessorLevel + _T('0'));
	            if ( fWin95IsHost ) {
					// Processor Revision:
	                ExceptionPrint(_T("%c%d "),
	                  HIBYTE(sinf.wProcessorRevision) + _T('A'),
	                  LOBYTE(sinf.wProcessorRevision));
	            } else {
					// Processor Revision:
	                ExceptionPrint(_T("(unknown)"));
	            }
	            break;
	        case 5:
				// Processor Level:
	            ExceptionPrint(_T("Pentium "));
	            if ( !fWin95IsHost ) {
					// Processor Revision:
	                ExceptionPrint(_T("Model %d, Stepping %d"),
	                    HIBYTE(sinf.wProcessorRevision), LOBYTE(sinf.wProcessorRevision));
	            } else {
					// Processor Revision:
	                ExceptionPrint(_T("(unknown)"));
	            }
	            break;

	        case 6:
	            ExceptionPrint(_T("Pentium Pro "));
	            if ( !fWin95IsHost ) {
	                ExceptionPrint(_T("Model %d, Stepping %d "),
	                    HIBYTE(sinf.wProcessorRevision), LOBYTE(sinf.wProcessorRevision));
	            } else {
					// Processor Revision:
	                ExceptionPrint(_T("(unknown)"));
	            }
	            break;

	        default:
	            ExceptionPrint(_T("%d "), sinf.wProcessorLevel);
	            ExceptionPrint(_T("%d "), sinf.wProcessorRevision);

		}

        break;

    case PROCESSOR_ARCHITECTURE_UNKNOWN:
    default:
        ExceptionPrint(_T("Unknown "));
		ExceptionPrint(_T("%d "), sinf.wProcessorLevel);
	    ExceptionPrint(_T("%d"), sinf.wProcessorRevision);
        break;
    }

    ExceptionPrint(_T("\n"));

    return S_OK;
}


// ---------------------------------------------------------------------------
/*
Function returns a string message translation of the exception
*/

static LPTSTR _stdcall FormatExceptionString(DWORD dwCode)
{

#define EXCEPTION( x ) case EXCEPTION_##x: return _T(#x);

	switch ( dwCode )
	{
		EXCEPTION( ACCESS_VIOLATION )
		EXCEPTION( ARRAY_BOUNDS_EXCEEDED )
		EXCEPTION( BREAKPOINT )
		EXCEPTION( DATATYPE_MISALIGNMENT )
		EXCEPTION( FLT_DENORMAL_OPERAND )
		EXCEPTION( FLT_DIVIDE_BY_ZERO )
		EXCEPTION( FLT_INEXACT_RESULT )
		EXCEPTION( FLT_INVALID_OPERATION )
		EXCEPTION( FLT_OVERFLOW )
		EXCEPTION( FLT_STACK_CHECK )
		EXCEPTION( FLT_UNDERFLOW )
		EXCEPTION( ILLEGAL_INSTRUCTION )
		EXCEPTION( IN_PAGE_ERROR )
		EXCEPTION( INT_DIVIDE_BY_ZERO )
		EXCEPTION( INT_OVERFLOW )
		EXCEPTION( INVALID_DISPOSITION )
		EXCEPTION( NONCONTINUABLE_EXCEPTION )
		EXCEPTION( PRIV_INSTRUCTION )
		EXCEPTION( SINGLE_STEP )
		EXCEPTION( STACK_OVERFLOW )
		EXCEPTION( GUARD_PAGE )
		EXCEPTION( INVALID_HANDLE )
	}

    // search for system messages

    DWORD flags = FORMAT_MESSAGE_MAX_WIDTH_MASK & 80;
    flags |= FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

	static TCHAR szBuffer[512] = { 0 };

	FormatMessage(
        flags,
        NULL,
        dwCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        szBuffer,
        512,    // max TCHARs
        NULL
        );

	return szBuffer;

}



// ---------------------------------------------------------------------------
/*
Function retrieves the address of the base of the thread stack and the current
limit of the thread stack. This limit changes as the stack grows.
*/
static void __declspec(naked)
RtlpGetStackLimits(DWORD *pStackLimit, DWORD *pStackBase)
{

_asm(
	"pushl %ebp          \n"
	"movl %esp,%ebp      \n"
	"movl 12(%ebp),%ecx  \n"
	"movl $0, (,%ecx)    \n" // *pStackBase = 0;
	"fs                  \n"
	"movl (0x4), %eax    \n" // stack base found at %fs:(0x4)
	"movl %eax, (,%ecx)  \n"
	"movl 8(%ebp),%ecx   \n"
	"movl $0, (,%ecx)    \n" // *pStackLimit = 0;
	"fs                  \n"
	"movl (0x8), %eax    \n" // stack limit found at %fs:(0x8)
	"movl %eax, (,%ecx)  \n"
	"popl %ebp           \n"
	"ret" // lcc-win32 treats naked functions that accept arguments as cdecl
	);

}

// ---------------------------------------------------------------------------

HRESULT _stdcall DumpStackLimits(void)
{
	// beginning address of the stack
	DWORD dwStackBase = 0;

	// farthest extent of memory available to the stack
	DWORD dwStackLimit = 0;

	RtlpGetStackLimits(&dwStackLimit, &dwStackBase);

	ExceptionPrint(_T("\nThread Stack Limits:\n"));
	ExceptionPrint(_T("Base      %08X\n"), dwStackBase);
	ExceptionPrint(_T("Top       %08X\n"), dwStackLimit);
	ExceptionPrint(_T("Size      %08X\n"), dwStackBase - dwStackLimit);

	return S_OK;
}

// ---------------------------------------------------------------------------
//
static HMODULE _stdcall ModuleFromAddress(PVOID addr)
{
	DWORD hMod = 0;

	MEMORY_BASIC_INFORMATION mbi;

	// VirtualQuery locates module containing the linear address in question
	if ( !VirtualQuery( addr, &mbi, sizeof(mbi) ) )
    {
        return (HMODULE)hMod;
    }

	// allocation base === hmodule
	hMod = (DWORD)mbi.AllocationBase;

    return (HMODULE)hMod;

}


// ---------------------------------------------------------------------------
/*
The logical address consists of the name of the module that the linear address
falls within, and the section number and offset within that section.

The function address corresponds with addresses listed in the .map file
generated for the faulting module.

Locate the faulting function by identifying the .map file symbol with the
greatest logical address that is less than or equal to the faulting address.

Thus, in a sorted list of symbol addresses, that would be the symbol before
the first symbol with an address greater than the fault address

if ( symbol_logical_address[n] > fault_logical_address )
    symbol of interest = symbol_logical_address[n-1]

Looking back at the .MAP file, we need to find the function with the
largest load address that is still smaller than the crash address.

// ---------------------------------------------------------------------------
Quick route - search for the function address in the .map file

*/

static BOOL
GetLogicalAddress(
    PVOID addr,
	LPTSTR szModule,
	DWORD len,      // size in TCHARs
	DWORD *section,
	DWORD *offset
    )
{

// ---------------------------------------------------------------------------

    HMODULE hMod = ModuleFromAddress(addr);
	if ( NULL == hMod ) { return FALSE; }

    GetModuleFileName(hMod, szModule, len);

// ---------------------------------------------------------------------------

	// overlay a DOS header structure on the module
	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;

	// locate the pe header
	PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)((DWORD)hMod + pDosHdr->e_lfanew);

// ---------------------------------------------------------------------------

	// IMAGE_FIRST_SECTION macro is located in winnt.h
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );

	// RVA is offset from module load address
	DWORD rva = (DWORD)addr - (DWORD)hMod;

	// iterate through the section table
	// seeking the section encompassing the linear address

	for (   unsigned int i = 0;
            i < pNtHdr->FileHeader.NumberOfSections;
            i++, pSection++ )
	    {

		DWORD sectionStart = pSection->VirtualAddress;
		DWORD sectionEnd = sectionStart
		    + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

		// is the address in this section???
		if ( ( rva >= sectionStart ) && ( rva <= sectionEnd ) )
		{
			// calculate and store section and offset
			*section = i+1;
			*offset = rva - sectionStart;

			// why not return a pointer to the section name?
			// the section name always turns out to be .text

			return TRUE;
		}
	}

	return FALSE;   // should never get here

}



// ---------------------------------------------------------------------------
//
//
static void _stdcall DumpRawStackData(CONTEXT *ctx)
{
	if ( NULL == ctx ) { return; }

	DWORD dwStackBase = 0;
	DWORD dwStackLimit = 0;

	// get stack base and stack limit
	RtlpGetStackLimits(&dwStackLimit, &dwStackBase);

	// confirm that esp is within the stack range
	if ( dwStackBase < ctx->Esp || ctx->Esp < dwStackLimit ) { return; }

	// deduct esp from the base of the stack
	DWORD dwStackSize = dwStackBase - ctx->Esp;

	// print header
	ExceptionPrint(_T("\nRaw Stack Data: %d bytes\n"), dwStackSize);

	// limit to a maximum of 1024 bytes
	dwStackSize = min(dwStackSize, 1024);

	// convert bytes into dwords
	DWORD length = dwStackSize >> 2;

	// establish a pointer to the top of the stack
	DWORD *stack = (DWORD *)ctx->Esp;

	ExceptionPrint(_T("\nAddress   | Contents +4       +8       +16      +20      +24\n"));

	for (int i = 0; i < length; i++)
	{

		if ( !(i % 6) ) {

			ExceptionPrint(_T("\n%08X  | %08X "), (ctx->Esp + i * 4), stack[i]);

		} else {

			// if the print buffer is too small,
			// it might look like there is a problem here
			ExceptionPrint(_T("%08X "), stack[i]);

		}

	}

}


// ---------------------------------------------------------------------------
/*
When executing a near call, the processor does the following (see Figure 6-4):
1. Pushes the current value of the EIP register on the stack.
2. Loads the offset of the called procedure in the EIP register.
3. Begins execution of the called procedure.

When executing a near return, the processor performs these actions:
1. Pops the top-of-stack value (the return instruction pointer) into the EIP register.
2. (If the RET instruction has an optional n argument.) Increments the stack pointer by the
number of bytes specified with the n operand to release parameters from the stack.
3. Resumes execution of the calling procedure.

// ---------------------------------------------------------------------------
E8 cd CALL rel32 Call near, relative, displacement relative to next instruction

The function address can be determined by adding the displacement
found in the call instruction to the return address on the stack.

// ---------------------------------------------------------------------------
FF /2 CALL r/m32 Call near, absolute indirect, address given in r/m32

For a near call, an absolute offset is specified indirectly in a general-purpose register or a
memory location (r/m16 or r/m32). The operand-size attribute determines the size of the target
operand (16 or 32 bits).

The target operand specifies an absolute offset in the code segment
(that is an offset from the base of the code segment)

// ---------------------------------------------------------------------------
FF /3 CALL m16:32 Call far, absolute indirect, address given in m16:32

disassembly examples from kernel32.dll

FF154C134E7C   CALL     DWORD PTR [0x7C4E134C]
FF15FC102D7C   CALL     DWORD PTR [0x7C2D10FC]
FF151014547C   CALL     DWORD PTR [0x7C541410]

if 5th byte back == E8 ==> near relative call
if 5th byte back == 15 ==> far absolute call

check if always 15 - it looks to be the case

a ModRM byte of 15 indicates a 32 bit absolute displacement follows

*/

DWORD _stdcall GetFunctionAddress(DWORD dwReturn)
{
	if ( 0 != dwReturn ) {

		// near relative call
		if ( 0xE8 == *(((BYTE *)(DWORD *)dwReturn) - 5) ) {
			// return address + displacement == function address
			return dwReturn + *(((DWORD *)dwReturn) - 1);
		}

		// far absolute indirect call typical of ntdll.dll
 		if ( 0x15FF == *(((WORD *)(DWORD *)dwReturn) - 3) ) {
			return *(((DWORD *)dwReturn) - 1);
		}

		// near absolute indirect
		// instruction could be two, three or four bytes
		// return *(((DWORD *)dwReturn) - 1);
		// just fall through (a length disassembler is overkill)

	}

	return 0;

}


// ---------------------------------------------------------------------------
/*
X86 Specific Routine for walking the callstack

To determine if a frame address is valid:

1. each frame must be at a higher address than the preceding frame
2. the frame address has to be a multiple of four bytes
    because the stack pointer is always a multiple of four
3. memory at the frame address must be read-write able

*/

static void _stdcall WalkX86CallStack(PCONTEXT pContext)
{

	ExceptionPrint( _T("\nThread Callstack:\n") );
	ExceptionPrint( _T("\nAddress   Frame     Function  Logical Address  Module\n") );

	PDWORD pFrame, pPrevFrame;

	// address of faulting instruction
	DWORD pEip = pContext->Eip;

	// pointer to first frame
	pFrame = (PDWORD)pContext->Ebp;

	do
	{

		TCHAR szModule[MAX_PATH];
		DWORD section = 0;
        DWORD offset = 0;

		if ( !GetLogicalAddress(
                (PVOID)pEip,
                szModule,
                MAX_PATH, // TCHAR count
				&section,
                &offset
                )
			)
		{
			// break from loop on false return
			break;
		}

		DWORD dwFunk = GetFunctionAddress(pFrame[1]);

		ExceptionPrint( _T("%08X  %08X  %08X  %04X:%08X    %s\n"),
    			pEip, pFrame, dwFunk, section, offset, szModule);

// an example of what the above prints
// 7C4EEE3B  0012FEB4  00402898  0001:0000DE3B    C:\WINNT\system32\KERNEL32.DLL

		// advance to next return address
		pEip = pFrame[1];

		// storethis frame in previous frame
		pPrevFrame = pFrame;

		// precede to next higher frame on stack
		pFrame = (PDWORD)pFrame[0];

        // determine if this frame address is valid

		// frame pointer must be aligned on a DWORD boundary
		if ( (DWORD)pFrame & 3 ) { break; }

		// frame pointer must be higher on stack
		if ( pFrame <= pPrevFrame ) { break; }

		// must be able to read and write memory at the frame's address
		// must be able to read 2 DWORDS ie { dw this frame, dw prev frame }
		if ( IsBadWritePtr(pFrame, sizeof(PVOID)*2) ) { break; }

	}
	while ( TRUE ); // this is a bit crappy for a loop...

}


// ---------------------------------------------------------------------------
//

static void _stdcall GenerateExceptionReport(
    PEXCEPTION_POINTERS pExceptionInfo
    )
{

	PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;

// print information about the type of fault
// ---------------------------------------------------------------------------

	ExceptionPrint(_T("\nException code: %08X "),
            pExceptionRecord->ExceptionCode);

	ExceptionPrint(_T("%s\n"),
			FormatExceptionString(pExceptionRecord->ExceptionCode));

// print information about where the fault occured
// ---------------------------------------------------------------------------

	TCHAR szFaultingModule[MAX_PATH];
	DWORD section;
    DWORD offset;

	GetLogicalAddress(
            pExceptionRecord->ExceptionAddress,
            szFaultingModule,
            sizeof(szFaultingModule)/sizeof(TCHAR), // size in TCHARs
			&section,
            &offset
            );

	ExceptionPrint(_T("Fault address:  %08X %04X:%08X %s\n"),
            pExceptionRecord->ExceptionAddress, section, offset, szFaultingModule);

	// Report fault Time
	ExceptionPrint( _T("Fault time:     ") );
	DumpTimeStamp();

	ExceptionPrint( _T("\n") );

	PCONTEXT pContext = pExceptionInfo->ContextRecord;

// X86 processors only
#ifdef _X86_

// ---------------------------------------------------------------------------
// the ContextFlags word
	ExceptionPrint( _T(" ContextFlags:  %08X\n"), pContext->ContextFlags);

// ---------------------------------------------------------------------------
// print the register values
	ExceptionPrint( _T("\nGeneral Purpose Registers\n\n") );

// ---------------------------------------------------------------------------
// these are returned if ContextFlags word contains the flag CONTEXT_CONTROL
// winnt.h denotes need to "sanitize" Eflags and SegCs - what does that mean?

	if ( LOWORD(pContext->ContextFlags) & LOWORD(CONTEXT_CONTROL) ) {

		ExceptionPrint( _T("        SegSS:  %08X\n"), pContext->SegSs );
		ExceptionPrint( _T("          ESP:  %08X\n"), pContext->Esp );
		ExceptionPrint( _T("       EFlags:  %08X\n"), pContext->EFlags );
		ExceptionPrint( _T("        SegCS:  %08X\n"), pContext->SegCs );
		ExceptionPrint( _T("          EIP:  %08X\n"), pContext->Eip );
   		ExceptionPrint( _T("          EBP:  %08X\n"), pContext->Ebp );

	}

// ---------------------------------------------------------------------------
// these are returned if ContextFlags word contains the flag CONTEXT_INTEGER

	if ( LOWORD(pContext->ContextFlags) & LOWORD(CONTEXT_INTEGER) ) {

		ExceptionPrint( _T("          EAX:  %08X\n"), pContext->Eax );
		ExceptionPrint( _T("          EBX:  %08X\n"), pContext->Ebx );
		ExceptionPrint( _T("          ECX:  %08X\n"), pContext->Ecx );
		ExceptionPrint( _T("          EDX:  %08X\n"), pContext->Edx );
		ExceptionPrint( _T("          ESI:  %08X\n"), pContext->Esi );
		ExceptionPrint( _T("          EDI:  %08X\n"), pContext->Edi );

	}

// ---------------------------------------------------------------------------
// these are returned if ContextFlags word contains the flag CONTEXT_SEGMENTS

	if ( LOWORD(pContext->ContextFlags) & LOWORD(CONTEXT_SEGMENTS) ) {

		ExceptionPrint( _T("        SegDS:  %08X\n"), pContext->SegDs );
		ExceptionPrint( _T("        SegES:  %08X\n"), pContext->SegEs );
		ExceptionPrint( _T("        SegFS:  %08X\n"), pContext->SegFs );
		ExceptionPrint( _T("        SegGS:  %08X\n"), pContext->SegGs );

	}

// ---------------------------------------------------------------------------
// walk the call stack
    WalkX86CallStack(pContext);

// ---------------------------------------------------------------------------
// dump the limits of the stack
	DumpStackLimits();

// ---------------------------------------------------------------------------
// dump a maximum of 1024 bytes from the top of the stack
	DumpRawStackData(pContext);

// ---------------------------------------------------------------------------
// these are returned if ContextFlags word contains the flag CONTEXT_FLOATING_POINT
// FLOATING_SAVE_AREA pContext->FloatSave - laid out in reverse structure order

	if ( LOWORD(pContext->ContextFlags) & LOWORD(CONTEXT_FLOATING_POINT) ) {

		ExceptionPrint( _T("\n\n  Cr0NpxState:  %08X\n"), pContext->FloatSave.Cr0NpxState );
		ExceptionPrint( _T("\nFloatSave.RegisterArea\n") );

		// dump these in groups of 10 bytes - ie. long double
	    for ( int i = 0; i < SIZE_OF_80387_REGISTERS; i++)
	    {
			if ( !(i % 10 ) ) {
	    		ExceptionPrint( _T("\n0x%02X "), pContext->FloatSave.RegisterArea[i] );
			} else {
	    		ExceptionPrint( _T("0x%02X "), pContext->FloatSave.RegisterArea[i] );
			}

	    }

		ExceptionPrint( _T("\n\n") );

	    ExceptionPrint( _T(" DataSelector:  %08X\n"), pContext->FloatSave.DataSelector );
    	ExceptionPrint( _T("   DataOffset:  %08X\n"), pContext->FloatSave.DataOffset );
    	ExceptionPrint( _T("ErrorSelector:  %08X\n"), pContext->FloatSave.ErrorSelector );
    	ExceptionPrint( _T("  ErrorOffset:  %08X\n"), pContext->FloatSave.ErrorOffset );
    	ExceptionPrint( _T("      TagWord:  %08X\n"), pContext->FloatSave.TagWord );
    	ExceptionPrint( _T("   StatusWord:  %08X\n"), pContext->FloatSave.StatusWord );
    	ExceptionPrint( _T("  ControlWord:  %08X\n"), pContext->FloatSave.ControlWord );

	}


#else
#warning "These routines are designed for an x86 processor"
#endif

	ExceptionPrint( _T("\n") );

}



// ---------------------------------------------------------------------------
//
static HRESULT _stdcall LogExceptionReport(void)
{
    ExceptionReport *pExceptionReport = &g_ExceptionReport;
    if ( NULL == pExceptionReport ) { return E_FAIL; }

	HANDLE hLogFile = CreateFile(
                            pExceptionReport->m_szFileName,
                            GENERIC_WRITE,
                            FILE_SHARE_NONE,
                            NULL,
                            OPEN_ALWAYS,
                            FILE_FLAG_WRITE_THROUGH, // ensure a disk write
                            NULL
                        );
	if ( INVALID_HANDLE_VALUE != hLogFile )
	{

		// append report to log file
		SetFilePointer(hLogFile, 0, 0, FILE_END);

		DWORD cbWritten;

        WriteFile(hLogFile,
                  pExceptionReport->m_szBuffer,
		          pExceptionReport->m_dwIndex * sizeof(TCHAR),
                  &cbWritten,
                  NULL);

        CloseHandle(hLogFile);

        hLogFile = INVALID_HANDLE_VALUE;

		return S_OK;

	}

	return E_FAIL;

}

// ---------------------------------------------------------------------------
// The unhandled exception filter
//
LONG __stdcall CatchAll(PEXCEPTION_POINTERS pExceptionInfo)
{

// ---------------------------------------------------------------------------
// GENERATE EXCEPTION REPORT

// to examine this unhandled exception handler in the lcc-win32 debugger
// compile with debug information and call one of the AccessViolation functions
#ifdef _DEBUG
	if ( IsDebuggerAttached() ) {
		// hardcoded breakpoint prompts the lcc-win32 debugger to stop here
		// note: int3 doesn't work with lcc-win32 under W95
		_asm("int3");
	}
#endif

    // fill report buffer with exception information
    GenerateExceptionReport(pExceptionInfo);

    // dump report buffer to file
    LogExceptionReport();

// ---------------------------------------------------------------------------
// Inform the user - suggest emailing the report

    MessageBox(NULL,
                _T("An error has occured. Please consult the exception log for details."),
                _T("Error"),
                MB_OK | MB_ICONERROR
                );

// ---------------------------------------------------------------------------
// kill the process - watch out when inside a dll
/*
Warning  Calling ExitProcess in a DLL can lead to unexpected application or system errors.
Be sure to call ExitProcess from a DLL only if you know which applications or system
components will load the DLL and that it is safe to call ExitProcess in this context.
*/
    ExitProcess(pExceptionInfo->ExceptionRecord->ExceptionCode);

    // never gets here
    return EXCEPTION_EXECUTE_HANDLER;

}


// ----------------------------------------------------------------------------
//
//
HRESULT _stdcall InitializeExceptionHandler(void)
{
    ExceptionReport *pExceptionReport = &g_ExceptionReport;
    if ( NULL == pExceptionReport ) { return E_FAIL; }

    ZeroMemory(pExceptionReport, sizeof(*pExceptionReport));

	pExceptionReport->m_dwRemaining = REPORT_BUFFER_LENGTH;

    GetModuleFileName(NULL, pExceptionReport->m_szFileName, MAX_PATH);

// ----------------------------------------------------------------------------
// Prepare Report Header

	ExceptionPrint(g_horzrule);

    ExceptionPrint(_T(" *** Exception Report ***\n\n"));

// Report program name
    ExceptionPrint(_T("Program: %s\n"), pExceptionReport->m_szFileName);

// Report Initialization Time
    ExceptionPrint(_T("Launch:  "));
	DumpTimeStamp();

// Report OS version
    ExceptionPrint(_T("System:  "));
    DumpSystemVersion();

// Report CPU Information
    ExceptionPrint(_T("CPU:     "));
    DumpCPUInfo();

// ----------------------------------------------------------------------------
    SetFileNameExt(pExceptionReport->m_szFileName, _T("log"));

// ----------------------------------------------------------------------------
// disable unhandled exception handling while testing in debugger
#ifdef _DEBUG
	if ( IsDebuggerAttached() ) {
	    return S_OK;
	}
#endif

// if ueh is already set, restore the previous setting and bail
// both of these conditions should fail as a pair

    // suppress system error message box
    UINT uiThisErrorMode = ( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );

    // docs say this api doesn't fail
    UINT uiPrevErrorMode = SetErrorMode(uiThisErrorMode);

    if ( SEM_SYSTEMDEFAULT != uiPrevErrorMode )
    {
        SetErrorMode(uiPrevErrorMode);
    }

    LPTOP_LEVEL_EXCEPTION_FILTER pfnPrevUEFilter = NULL;

    // docs say this api doesn't fail
    pfnPrevUEFilter = SetUnhandledExceptionFilter(CatchAll);

    // if ueh is already set restore it
    if ( NULL != pfnPrevUEFilter )
    {
        SetUnhandledExceptionFilter(pfnPrevUEFilter);
    }

    return S_OK;

}

// ---------------------------------------------------------------------------
// EOF

