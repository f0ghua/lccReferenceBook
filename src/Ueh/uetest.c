/* This is the testing portion of a rudimentary crash report logger meant 
to aid tracking down bugs in programs compiled without debug information. 
It is based on sample code from Matt Pietrek's "Under the Hood" column in 
the April 1997 issue of Microsoft Systems Journal.

Date: 10/31/2003
Author: Mike Caetano
License: Whatever follows from the Pietrek code plus those portions of code
taken from the MS Platform SDK plus a general "use at your own risk" - and
of course, some credit for the added value of my effort would be nice too! :)
*/

#include <windows.h>
#include <tchar.h>
#include "ueh.h"

#if __LCCDEBUGLEVEL > 0
#define _DEBUG
#endif

#ifdef _DEBUG
#include <seh.h>
#include <limits.h>
#include <float.h>
extern LONG __stdcall CatchAll(PEXCEPTION_POINTERS pExceptionInfo);
extern int IsDebuggerAttached(void);
#endif

// ---------------------------------------------------------------------------
// NOTE: These functions are intended to produce exceptions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

void AccessViolation(void)
{
	// attempt to write to address 0x00000000 produces an access violation
	int *p = NULL;
	*p = 0;
}

// ---------------------------------------------------------------------------

void StackOverflow(void)
{
	char szBigBuf[0x100000];
	if ( szBigBuf != szBigBuf);	// suppress compiler warning
}


// ---------------------------------------------------------------------------
/*
// int3 doesn't play nicely with the lcc-win32 debugger on W9x
// use the old style assert to kick the debugger instead
#if 0
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#define DebugBreak() (char __DebugBreak__[-1])
#define BreakPoint() _asm(".byte 0xCC");
#endif
*/
void BreakPoint(void)
{
	_asm("int3");
}


// ---------------------------------------------------------------------------

/*
How to "emit" opcodes:
Use .byte assembler directive

; this is  movl fs:0,%eax
	.byte	100		// 0x64
	.byte	161		// 0xA1
	.long	0

*/

// ---------------------------------------------------------------------------

void IllegalInstruction(void)
{
	// _asm("ud2");
	// no such instruction under lcc-win32
	// ud2 - "undefined instruction"
	// a NOP that raises "invalid opcode exception"
	// opcode 0F 0B
	_asm(".byte 0x0F \n"
	     ".byte 0x0B   ");

}

// ---------------------------------------------------------------------------

void PrivilegedInstruction(void)
{
	// _asm("wrmsr");
	// no such instruction under lcc-win32
	// wrmsr - "Write to Model Specific Register"
	// opcode 0F 30

	_asm(".byte 0x0F \n"
	     ".byte 0x30   ");

}


// ---------------------------------------------------------------------------

void SignedIntegerDivideByZero(void)
{
	_asm("movl $0x4, %eax \n"
		 "movl $0x0, %ecx \n"
		 "cdq             \n"
		 "idivl %ecx        ");

}


// ---------------------------------------------------------------------------

void UnsignedIntegerDivideByZero(void)
{
	_asm("movl $4, %eax   \n"
		 "movl $0, %ecx   \n"
		 "xorl %edx, %edx \n"
		 "divl %ecx         ");

}

// ---------------------------------------------------------------------------

void AccessViolationInCRT(void)
{
    // access violation in crtdll.dll
    strcpy(0,0);
}

void AccessViolationInK32(void)
{
 	// access violation in ntdll.dll
	GetSystemDirectory(NULL,260);
}

void AccessViolationInAdvapi32(void)
{
	DWORD nSize = 260;
	GetUserName(NULL,&nSize);
}


// ---------------------------------------------------------------------------

// doesn't work yet
void AccessViolationInUser32(void)
{
	GetClassName(NULL, NULL, 260);
}


// doesn't work yet
void AccessViolationInGdi32(void)
{
	GetCharWidth(NULL, 0, 1, NULL);
}


// ---------------------------------------------------------------------------

/*
bound index, mem

Where mem contains two signed dwords { lowerbound, upperbound }
Array Bounds Exceeded is thrown when index exceeds array boundaries

index must be a register

*/

void ArrayBoundsExceeded(void)
{
	struct {
		int lower;
		int upper;
	} bounds = { 0, 0 };

	_asm("movl $1, %ecx     \n"
		 "boundl %ecx, %bounds");

}


// ---------------------------------------------------------------------------

/*
The INTO instruction is a special mnemonic for calling overflow exception (#OF),
interrupt vector number 4. The overflow interrupt checks the OF flag in the
EFLAGS register and calls the overflow interrupt handler if the OF flag is set
to 1.

The OF flag is affected only on 1-bit shifts. For left shifts, the OF flag is
cleared to 0 if the most significant bit of the result is the same as the CF
flag (that is, the top two bits of the original operand were the same);
otherwise, it is set to 1. For the SAR instruction, the OF flag is cleared for
all 1-bit shifts. For the SHR instruction, the OF flag is set to the
most-significant bit of the original operand.

*/

void IntegerOverflow(void)
{
	_asm("movl $-1, %eax \n"
		 "shrl %eax      \n"
		 "into             ");
}


// ---------------------------------------------------------------------------
// this sets the float exception flag to 4
// - but doesn't generate an exception in the thread ???
void FloatDivideByZero(void)
{

	_asm("fldz \n"
         "fld1 \n"
		 "fdiv \n"
		 "fwait");

}


// ---------------------------------------------------------------------------
// this works for testing the floatsave area of the context structure
void FloatSaveRegisters(void)
{
	_asm("fld1   \n"	// push 1
		 "fldl2t \n"	// push log base 2 of 10
		 "fldl2e \n"	// push log base 2 of e
		 "fldpi  \n"	// push pi
		 "fldlg2 \n"	// push log base 10 of 2
		 "fldln2 \n"	// push log base e of 2
		 "fldz   \n"	// push 0
		 "int3");
}



// ---------------------------------------------------------------------------
// NOTE: These functions are intended to generate stackframes to examine
// ---------------------------------------------------------------------------

int f4(int a, int b, int c, int d)
{

    // call faulting function here
#ifdef _DEBUG
	if ( IsDebuggerAttached() ) {

		__try {

			// these will allow for entering CatchAll via the debugger
			AccessViolation();
			// AccessViolationInCRT();
		 	// AccessViolationInK32();
			// AccessViolationInAdvapi32();

		}
		__except( CatchAll( GetExceptionInformation() ) )
		{
		}

	}
#endif

	// these do not allow for entering CatchAll via the debugger

	// BreakPoint();
	// SignedIntegerDivideByZero();
	// UnsignedIntegerDivideByZero();
	// IntegerOverflow();
	// StackOverflow();
	// ArrayBoundsExceeded();
	// IllegalInstruction();
	PrivilegedInstruction();
	// FloatSaveRegisters();

	// these still need work
	// AccessViolationInUser32();
	// AccessViolationInGdi32();
	// FloatDivideByZero();

    return 0;

}


int f3(int a, int b, int c, int d)
{
    a = b + c;
    return f4(a, b, c, d);
}

int f2(int a, int b, int c, int d)
{
    b = c + d;
    return f3(a, b, c, d);
}

int f1(int a, int b, int c, int d)
{
    c = d;
    return f2(a, b, c, d);
}


// ---------------------------------------------------------------------------

int main(void)
{

	InitializeExceptionHandler();

    f1(0, 0, 0, 7);

	return 0;
}



