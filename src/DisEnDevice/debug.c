//
// Microsoft (all rights reserved)
//

#include "common.h"

#if DBG

/* Routine Description:  This Routine will output a printf style string to the debugger.

	Arguments:
     lpszFormat - printf() style formatting string.

    ..... - Other arguments to be formatted.

	Return Value:
     NONE.
*/
void __DbgOut__(LPTSTR lpszFormat, ...)
{
    TCHAR buf[256];
    va_list va;

    OutputDebugString(_T("Enable: "));

    va_start(va, lpszFormat);
    wvsprintf(buf, lpszFormat, va);
    va_end(va);

    OutputDebugString(buf);
    OutputDebugString(_T("\n"));
}

/* Routine Description: This Routine will display the LastError in human readable
    					form when possible.

    If the return value is a 32-bit number, and falls in the range:
        ERROR_NO_ASSOCIATED_CLASS   0xE0000200
    To
        ERROR_CANT_REMOVE_DEVINST   0xE0000232

    The values defined in setupapi.h can help to determine the error.
    Start by searching for the text string ERROR_NO_ASSOCIATED_CLASS.

Arguments:
    ErrorName: Human readable description of the last Function called.

Return Value:
    NONE.
*/
void DisplayError(TCHAR * ErrorName)
{
    DWORD Err = GetLastError();
    LPVOID lpMessageBuffer;

    if (FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        Err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMessageBuffer,
        0,
        NULL ))
        __DbgOut__(_T("%s FAILURE: %s"),ErrorName,(char *) lpMessageBuffer);
    else
        __DbgOut__(_T("%s FAILURE: (0x%08x)"),ErrorName,Err);

    LocalFree( lpMessageBuffer ); // Free the buffer allocated by the system
    SetLastError(Err);
}


#endif //DBG



