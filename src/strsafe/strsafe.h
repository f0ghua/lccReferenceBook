/******************************************************************
*                                                                 *
*  strsafe.h -- This module defines safer C library string        *
*               routine replacements. These are meant to make C   *
*               a bit more safe in reference to security and      *
*               robustness                                        *
*                                                                 *
*  Copyright (c) Microsoft Corp.  All rights reserved.            *
*                                                                 *
******************************************************************/
#ifndef _STRSAFE_H_INCLUDED_
#define _STRSAFE_H_INCLUDED_

#include <stdio.h>      // for _vsnprintf, _vsnwprintf, getc, getwc
#include <string.h>     // for memset
#include <stdarg.h>     // for va_start, etc.

#define STRSAFEAPI  _STRSAFE_EXTERN_C HRESULT __stdcall

#ifndef _SIZE_T_DEFINED
typedef __w64 unsigned int  size_t;
#define _SIZE_T_DEFINED
#endif

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef long HRESULT;
#endif

#ifndef SUCCEEDED
#define SUCCEEDED(hr)  ((HRESULT)(hr) >= 0)
#endif

#ifndef FAILED
#define FAILED(hr)  ((HRESULT)(hr) < 0)
#endif

#ifndef S_OK
#define S_OK  ((HRESULT)0x00000000L)
#endif

#define _STRSAFE_EXTERN_C    extern

#define STRSAFE_MAX_CCH  2147483647 // max # of characters we support (same as INT_MAX)

// STRSAFE error return codes
//
#define STRSAFE_E_INSUFFICIENT_BUFFER       ((HRESULT)0x8007007AL)  // 0x7A = 122L = ERROR_INSUFFICIENT_BUFFER
#define STRSAFE_E_INVALID_PARAMETER         ((HRESULT)0x80070057L)  // 0x57 =  87L = ERROR_INVALID_PARAMETER
#define STRSAFE_E_END_OF_FILE               ((HRESULT)0x80070026L)  // 0x26 =  38L = ERROR_HANDLE_EOF

// Flags for controling the Ex functions
//
//      STRSAFE_FILL_BYTE(0xFF)     0x000000FF  // bottom byte specifies fill pattern
#define STRSAFE_IGNORE_NULLS        0x00000100  // treat null as TEXT("") -- don't fault on NULL buffers
#define STRSAFE_FILL_BEHIND_NULL    0x00000200  // fill in extra space behind the null terminator
#define STRSAFE_FILL_ON_FAILURE     0x00000400  // on failure, overwrite pszDest with fill pattern and null terminate it
#define STRSAFE_NULL_ON_FAILURE     0x00000800  // on failure, set *pszDest = TEXT('\0')
#define STRSAFE_NO_TRUNCATION       0x00001000  // instead of returning a truncated result, copy/append nothing to pszDest and null terminate it

#define STRSAFE_VALID_FLAGS         (0x000000FF | STRSAFE_IGNORE_NULLS | STRSAFE_FILL_BEHIND_NULL | STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE | STRSAFE_NO_TRUNCATION)

// helper macro to set the fill character and specify buffer filling
#define STRSAFE_FILL_BYTE(x)        ((unsigned long)((x & 0x000000FF) | STRSAFE_FILL_BEHIND_NULL))
#define STRSAFE_FAILURE_BYTE(x)     ((unsigned long)((x & 0x000000FF) | STRSAFE_FILL_ON_FAILURE))

#define STRSAFE_GET_FILL_PATTERN(dwFlags)  ((int)(dwFlags & 0x000000FF))

STRSAFEAPI StringCchCopyA(char* pszDest, size_t cchDest, const char* pszSrc);
STRSAFEAPI StringCchCopyW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc);
STRSAFEAPI StringCbCopyA(char* pszDest, size_t cbDest, const char* pszSrc);
STRSAFEAPI StringCbCopyW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc);
STRSAFEAPI StringCchCopyExA(char* pszDest, size_t cchDest, const char* pszSrc, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags);
STRSAFEAPI StringCchCopyExW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags);
STRSAFEAPI StringCbCopyExA(char* pszDest, size_t cbDest, const char* pszSrc, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags);
STRSAFEAPI StringCbCopyExW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags);
STRSAFEAPI StringCchCopyNA(char* pszDest, size_t cchDest, const char* pszSrc, size_t cchSrc);
STRSAFEAPI StringCchCopyNW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, size_t cchSrc);
STRSAFEAPI StringCbCopyNA(char* pszDest, size_t cbDest, const char* pszSrc, size_t cbSrc);
STRSAFEAPI StringCbCopyNW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, size_t cbSrc);
STRSAFEAPI StringCchCopyNExA(char* pszDest, size_t cchDest, const char* pszSrc, size_t cchSrc, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags);
STRSAFEAPI StringCchCopyNExW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, size_t cchSrc, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags);
STRSAFEAPI StringCbCopyNExA(char* pszDest, size_t cbDest, const char* pszSrc, size_t cbSrc, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags);
STRSAFEAPI StringCbCopyNExW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, size_t cbSrc, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags);
STRSAFEAPI StringCchCatA(char* pszDest, size_t cchDest, const char* pszSrc);
STRSAFEAPI StringCchCatW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc);
STRSAFEAPI StringCbCatA(char* pszDest, size_t cbDest, const char* pszSrc);
STRSAFEAPI StringCbCatW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc);
STRSAFEAPI StringCchCatExA(char* pszDest, size_t cchDest, const char* pszSrc, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags);
STRSAFEAPI StringCchCatExW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags);
STRSAFEAPI StringCbCatExA(char* pszDest, size_t cbDest, const char* pszSrc, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags);
STRSAFEAPI StringCbCatExW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags);
STRSAFEAPI StringCchCatNA(char* pszDest, size_t cchDest, const char* pszSrc, size_t cchMaxAppend);
STRSAFEAPI StringCchCatNW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, size_t cchMaxAppend);
STRSAFEAPI StringCbCatNA(char* pszDest, size_t cbDest, const char* pszSrc, size_t cbMaxAppend);
STRSAFEAPI StringCbCatNW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, size_t cbMaxAppend);
STRSAFEAPI StringCchCatNExA(char* pszDest, size_t cchDest, const char* pszSrc, size_t cchMaxAppend, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags);
STRSAFEAPI StringCchCatNExW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, size_t cchMaxAppend, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags);
STRSAFEAPI StringCbCatNExA(char* pszDest, size_t cbDest, const char* pszSrc, size_t cbMaxAppend, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags);
STRSAFEAPI StringCbCatNExW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, size_t cbMaxAppend, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags);
STRSAFEAPI StringCchVPrintfA(char* pszDest, size_t cchDest, const char* pszFormat, va_list argList);
STRSAFEAPI StringCchVPrintfW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszFormat, va_list argList);
STRSAFEAPI StringCbVPrintfA(char* pszDest, size_t cbDest, const char* pszFormat, va_list argList);
STRSAFEAPI StringCbVPrintfW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszFormat, va_list argList);
STRSAFEAPI StringCchPrintfA(char* pszDest, size_t cchDest, const char* pszFormat, ...);
STRSAFEAPI StringCchPrintfW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszFormat, ...);
STRSAFEAPI StringCbPrintfA(char* pszDest, size_t cbDest, const char* pszFormat, ...);
STRSAFEAPI StringCbPrintfW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszFormat, ...);
STRSAFEAPI StringCchPrintfExA(char* pszDest, size_t cchDest, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags, const char* pszFormat, ...);
STRSAFEAPI StringCchPrintfExW(wchar_t* pszDest, size_t cchDest, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags, const wchar_t* pszFormat, ...);
STRSAFEAPI StringCbPrintfExA(char* pszDest, size_t cbDest, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags, const char* pszFormat, ...);
STRSAFEAPI StringCbPrintfExW(wchar_t* pszDest, size_t cbDest, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags, const wchar_t* pszFormat, ...);
STRSAFEAPI StringCchVPrintfExA(char* pszDest, size_t cchDest, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags, const char* pszFormat, va_list argList);
STRSAFEAPI StringCchVPrintfExW(wchar_t* pszDest, size_t cchDest, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags, const wchar_t* pszFormat, va_list argList);
STRSAFEAPI StringCbVPrintfExA(char* pszDest, size_t cbDest, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags, const char* pszFormat, va_list argList);
STRSAFEAPI StringCbVPrintfExW(wchar_t* pszDest, size_t cbDest, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags, const wchar_t* pszFormat, va_list argList);
STRSAFEAPI StringCchGetsA(char* pszDest, size_t cchDest);
STRSAFEAPI StringCchGetsW(wchar_t* pszDest, size_t cchDest);
STRSAFEAPI StringCbGetsA(char* pszDest, size_t cbDest);
STRSAFEAPI StringCbGetsW(wchar_t* pszDest, size_t cbDest);
STRSAFEAPI StringCchGetsExA(char* pszDest, size_t cchDest, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags);
STRSAFEAPI StringCchGetsExW(wchar_t* pszDest, size_t cchDest, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags);
STRSAFEAPI StringCbGetsExA(char* pszDest, size_t cbDest, char** ppszDestEnd, size_t* pbRemaining, unsigned long dwFlags);
STRSAFEAPI StringCbGetsExW(wchar_t* pszDest, size_t cbDest, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags);
STRSAFEAPI StringCchLengthA(const char* psz, size_t cchMax, size_t* pcch);
STRSAFEAPI StringCchLengthW(const wchar_t* psz, size_t cchMax, size_t* pcch);
STRSAFEAPI StringCbLengthA(const char* psz, size_t cchMax, size_t* pcch);
STRSAFEAPI StringCbLengthW(const wchar_t* psz, size_t cchMax, size_t* pcch);


#ifdef UNICODE
#define StringCchCopy  StringCchCopyW
#define StringCbCopy  StringCbCopyW
#define StringCchCopyEx  StringCchCopyExW
#define StringCbCopyEx  StringCbCopyExW
#define StringCchCopyN  StringCchCopyNW
#define StringCbCopyN  StringCbCopyNW
#define StringCchCopyNEx  StringCchCopyNExW
#define StringCbCopyNEx  StringCbCopyNExW
#define StringCchCat  StringCchCatW
#define StringCbCat  StringCbCatW
#define StringCchCatEx  StringCchCatExW
#define StringCbCatEx  StringCbCatExW
#define StringCchCatN  StringCchCatNW
#define StringCbCatN  StringCbCatNW
#define StringCchCatNEx  StringCchCatNExW
#define StringCbCatNEx  StringCbCatNExW
#define StringCchVPrintf  StringCchVPrintfW
#define StringCbVPrintf  StringCbVPrintfW
#define StringCchPrintf  StringCchPrintfW
#define StringCbPrintf  StringCbPrintfW
#define StringCchPrintfEx  StringCchPrintfExW
#define StringCbPrintfEx  StringCbPrintfExW
#define StringCchVPrintfEx  StringCchVPrintfExW
#define StringCbVPrintfEx  StringCbVPrintfExW
#define StringCchGets  StringCchGetsW
#define StringCbGets  StringCbGetsW
#define StringCchGetsEx  StringCchGetsExW
#define StringCbGetsEx  StringCbGetsExW
#define StringCchLength  StringCchLengthW
#define StringCbLength  StringCbLengthW
#else
#define StringCchCopy  StringCchCopyA
#define StringCbCopy  StringCbCopyA
#define StringCchCopyEx  StringCchCopyExA
#define StringCbCopyEx  StringCbCopyExA
#define StringCchCopyN  StringCchCopyNA
#define StringCbCopyN  StringCbCopyNA
#define StringCchCopyNEx  StringCchCopyNExA
#define StringCbCopyNEx  StringCbCopyNExA
#define StringCchCat  StringCchCatA
#define StringCbCat  StringCbCatA
#define StringCchCatEx  StringCchCatExA
#define StringCbCatEx  StringCbCatExA
#define StringCchCatN  StringCchCatNA
#define StringCbCatN  StringCbCatNA
#define StringCchCatNEx  StringCchCatNExA
#define StringCbCatNEx  StringCbCatNExA
#define StringCchVPrintf  StringCchVPrintfA
#define StringCbVPrintf  StringCbVPrintfA
#define StringCchPrintf  StringCchPrintfA
#define StringCbPrintf  StringCbPrintfA
#define StringCchPrintfEx  StringCchPrintfExA
#define StringCbPrintfEx  StringCbPrintfExA
#define StringCchVPrintfEx  StringCchVPrintfExA
#define StringCbVPrintfEx  StringCbVPrintfExA
#define StringCchGets  StringCchGetsA
#define StringCbGets  StringCbGetsA
#define StringCchGetsEx  StringCchGetsExA
#define StringCbGetsEx  StringCbGetsExA
#define StringCchLength  StringCchLengthA
#define StringCbLength  StringCbLengthA
#endif // !UNICODE


#endif //_STRSAFE_H_INCLUDED_
