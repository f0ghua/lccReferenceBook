#include "strsafe.h"

// protos
STRSAFEAPI StringLengthWorkerA(const char* psz, size_t cchMax, size_t* pcch);
STRSAFEAPI StringLengthWorkerW(const wchar_t* psz, size_t cchMax, size_t* pcch);

// these are the worker functions that actually do the work
// take a look in strsafe.h for the user functions.
STRSAFEAPI StringCopyWorkerA(char* pszDest, size_t cchDest, const char* pszSrc)
{
    HRESULT hr = S_OK;

    if (cchDest == 0)
    {
        // can not null terminate a zero-byte dest buffer
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        while (cchDest && (*pszSrc != '\0'))
        {
            *pszDest++ = *pszSrc++;
            cchDest--;
        }

        if (cchDest == 0)
        {
            // we are going to truncate pszDest
            pszDest--;
            hr = STRSAFE_E_INSUFFICIENT_BUFFER;
        }

        *pszDest= '\0';
    }

    return hr;
}

STRSAFEAPI StringCopyWorkerW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc)
{
    HRESULT hr = S_OK;

    if (cchDest == 0)
    {
        // can not null terminate a zero-byte dest buffer
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        while (cchDest && (*pszSrc != L'\0'))
        {
            *pszDest++ = *pszSrc++;
            cchDest--;
        }

        if (cchDest == 0)
        {
            // we are going to truncate pszDest
            pszDest--;
            hr = STRSAFE_E_INSUFFICIENT_BUFFER;
        }

        *pszDest= L'\0';
    }

    return hr;
}

STRSAFEAPI StringCopyExWorkerA(char* pszDest, size_t cchDest, size_t cbDest, const char* pszSrc, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr = S_OK;
    char* pszDestEnd = pszDest;
    size_t cchRemaining = 0;

    // ASSERT(cbDest == (cchDest * sizeof(char))    ||
    //        cbDest == (cchDest * sizeof(char)) + (cbDest % sizeof(char)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest != 0) || (cbDest != 0))
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }

            if (pszSrc == NULL)
            {
                pszSrc = "";
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest == 0)
            {
                pszDestEnd = pszDest;
                cchRemaining = 0;

                // only fail if there was actually src data to copy
                if (*pszSrc != '\0')
                {
                    if (pszDest == NULL)
                    {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    }
                    else
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            }
            else
            {
                pszDestEnd = pszDest;
                cchRemaining = cchDest;

                while (cchRemaining && (*pszSrc != '\0'))
                {
                    *pszDestEnd++= *pszSrc++;
                    cchRemaining--;
                }

                if (cchRemaining > 0)
                {
                    if (dwFlags & STRSAFE_FILL_BEHIND_NULL)
                    {
                        memset(pszDestEnd + 1, STRSAFE_GET_FILL_PATTERN(dwFlags), ((cchRemaining - 1) * sizeof(char)) + (cbDest % sizeof(char)));
                    }
                }
                else
                {
                    // we are going to truncate pszDest
                    pszDestEnd--;
                    cchRemaining++;

                    hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                }

                *pszDestEnd = '\0';
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = '\0';
                }
            }

            if (dwFlags & (STRSAFE_NULL_ON_FAILURE | STRSAFE_NO_TRUNCATION))
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = '\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}

STRSAFEAPI StringCopyExWorkerW(wchar_t* pszDest, size_t cchDest, size_t cbDest, const wchar_t* pszSrc, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr = S_OK;
    wchar_t* pszDestEnd = pszDest;
    size_t cchRemaining = 0;

    // ASSERT(cbDest == (cchDest * sizeof(wchar_t)) ||
    //        cbDest == (cchDest * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest != 0) || (cbDest != 0))
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }

            if (pszSrc == NULL)
            {
                pszSrc = L"";
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest == 0)
            {
                pszDestEnd = pszDest;
                cchRemaining = 0;

                // only fail if there was actually src data to copy
                if (*pszSrc != L'\0')
                {
                    if (pszDest == NULL)
                    {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    }
                    else
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            }
            else
            {
                pszDestEnd = pszDest;
                cchRemaining = cchDest;

                while (cchRemaining && (*pszSrc != L'\0'))
                {
                    *pszDestEnd++= *pszSrc++;
                    cchRemaining--;
                }

                if (cchRemaining > 0)
                {
                    if (dwFlags & STRSAFE_FILL_BEHIND_NULL)
                    {
                        memset(pszDestEnd + 1, STRSAFE_GET_FILL_PATTERN(dwFlags), ((cchRemaining - 1) * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t)));
                    }
                }
                else
                {
                    // we are going to truncate pszDest
                    pszDestEnd--;
                    cchRemaining++;

                    hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                }

                *pszDestEnd = L'\0';
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = L'\0';
                }
            }

            if (dwFlags & (STRSAFE_NULL_ON_FAILURE | STRSAFE_NO_TRUNCATION))
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = L'\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}

STRSAFEAPI StringCopyNWorkerA(char* pszDest, size_t cchDest, const char* pszSrc, size_t cchSrc)
{
    HRESULT hr = S_OK;

    if (cchDest == 0)
    {
        // can not null terminate a zero-byte dest buffer
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        while (cchDest && cchSrc && (*pszSrc != '\0'))
        {
            *pszDest++= *pszSrc++;
            cchDest--;
            cchSrc--;
        }

        if (cchDest == 0)
        {
            // we are going to truncate pszDest
            pszDest--;
            hr = STRSAFE_E_INSUFFICIENT_BUFFER;
        }

        *pszDest= '\0';
    }

    return hr;
}

STRSAFEAPI StringCopyNWorkerW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, size_t cchSrc)
{
    HRESULT hr = S_OK;

    if (cchDest == 0)
    {
        // can not null terminate a zero-byte dest buffer
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        while (cchDest && cchSrc && (*pszSrc != L'\0'))
        {
            *pszDest++= *pszSrc++;
            cchDest--;
            cchSrc--;
        }

        if (cchDest == 0)
        {
            // we are going to truncate pszDest
            pszDest--;
            hr = STRSAFE_E_INSUFFICIENT_BUFFER;
        }

        *pszDest= L'\0';
    }

    return hr;
}

STRSAFEAPI StringCopyNExWorkerA(char* pszDest, size_t cchDest, size_t cbDest, const char* pszSrc, size_t cchSrc, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr = S_OK;
    char* pszDestEnd = pszDest;
    size_t cchRemaining = 0;

    // ASSERT(cbDest == (cchDest * sizeof(char))    ||
    //        cbDest == (cchDest * sizeof(char)) + (cbDest % sizeof(char)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest != 0) || (cbDest != 0))
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }

            if (pszSrc == NULL)
            {
                pszSrc = "";
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest == 0)
            {
                pszDestEnd = pszDest;
                cchRemaining = 0;

                // only fail if there was actually src data to copy
                if (*pszSrc != '\0')
                {
                    if (pszDest == NULL)
                    {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    }
                    else
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            }
            else
            {
                pszDestEnd = pszDest;
                cchRemaining = cchDest;

                while (cchRemaining && cchSrc && (*pszSrc != '\0'))
                {
                    *pszDestEnd++= *pszSrc++;
                    cchRemaining--;
                    cchSrc--;
                }

                if (cchRemaining > 0)
                {
                    if (dwFlags & STRSAFE_FILL_BEHIND_NULL)
                    {
                        memset(pszDestEnd + 1, STRSAFE_GET_FILL_PATTERN(dwFlags), ((cchRemaining - 1) * sizeof(char)) + (cbDest % sizeof(char)));
                    }
                }
                else
                {
                    // we are going to truncate pszDest
                    pszDestEnd--;
                    cchRemaining++;

                    hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                }

                *pszDestEnd = '\0';
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = '\0';
                }
            }

            if (dwFlags & (STRSAFE_NULL_ON_FAILURE | STRSAFE_NO_TRUNCATION))
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = '\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}

STRSAFEAPI StringCopyNExWorkerW(wchar_t* pszDest, size_t cchDest, size_t cbDest, const wchar_t* pszSrc, size_t cchSrc, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr = S_OK;
    wchar_t* pszDestEnd = pszDest;
    size_t cchRemaining = 0;

    // ASSERT(cbDest == (cchDest * sizeof(wchar_t)) ||
    //        cbDest == (cchDest * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest != 0) || (cbDest != 0))
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }

            if (pszSrc == NULL)
            {
                pszSrc = L"";
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest == 0)
            {
                pszDestEnd = pszDest;
                cchRemaining = 0;

                // only fail if there was actually src data to copy
                if (*pszSrc != L'\0')
                {
                    if (pszDest == NULL)
                    {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    }
                    else
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            }
            else
            {
                pszDestEnd = pszDest;
                cchRemaining = cchDest;

                while (cchRemaining && cchSrc && (*pszSrc != L'\0'))
                {
                    *pszDestEnd++= *pszSrc++;
                    cchRemaining--;
                    cchSrc--;
                }

                if (cchRemaining > 0)
                {
                    if (dwFlags & STRSAFE_FILL_BEHIND_NULL)
                    {
                        memset(pszDestEnd + 1, STRSAFE_GET_FILL_PATTERN(dwFlags), ((cchRemaining - 1) * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t)));
                    }
                }
                else
                {
                    // we are going to truncate pszDest
                    pszDestEnd--;
                    cchRemaining++;

                    hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                }

                *pszDestEnd = L'\0';
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = L'\0';
                }
            }

            if (dwFlags & (STRSAFE_NULL_ON_FAILURE | STRSAFE_NO_TRUNCATION))
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = L'\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}

STRSAFEAPI StringCatWorkerA(char* pszDest, size_t cchDest, const char* pszSrc)
{
   HRESULT hr;
   size_t cchDestCurrent;

   hr = StringLengthWorkerA(pszDest, cchDest, &cchDestCurrent);

   if (SUCCEEDED(hr))
   {
       hr = StringCopyWorkerA(pszDest + cchDestCurrent,
                              cchDest - cchDestCurrent,
                              pszSrc);
   }

   return hr;
}

STRSAFEAPI StringCatWorkerW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc)
{
   HRESULT hr;
   size_t cchDestCurrent;

   hr = StringLengthWorkerW(pszDest, cchDest, &cchDestCurrent);

   if (SUCCEEDED(hr))
   {
       hr = StringCopyWorkerW(pszDest + cchDestCurrent,
                              cchDest - cchDestCurrent,
                              pszSrc);
   }

   return hr;
}

STRSAFEAPI StringCatExWorkerA(char* pszDest, size_t cchDest, size_t cbDest, const char* pszSrc, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr = S_OK;
    char* pszDestEnd = pszDest;
    size_t cchRemaining = 0;

    // ASSERT(cbDest == (cchDest * sizeof(char))    ||
    //        cbDest == (cchDest * sizeof(char)) + (cbDest % sizeof(char)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cchDestCurrent;

        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest == 0) && (cbDest == 0))
                {
                    cchDestCurrent = 0;
                }
                else
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }
            else
            {
                hr = StringLengthWorkerA(pszDest, cchDest, &cchDestCurrent);

                if (SUCCEEDED(hr))
                {
                    pszDestEnd = pszDest + cchDestCurrent;
                    cchRemaining = cchDest - cchDestCurrent;
                }
            }

            if (pszSrc == NULL)
            {
                pszSrc = "";
            }
        }
        else
        {
            hr = StringLengthWorkerA(pszDest, cchDest, &cchDestCurrent);

            if (SUCCEEDED(hr))
            {
                pszDestEnd = pszDest + cchDestCurrent;
                cchRemaining = cchDest - cchDestCurrent;
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest == 0)
            {
                // only fail if there was actually src data to append
                if (*pszSrc != '\0')
                {
                    if (pszDest == NULL)
                    {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    }
                    else
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            }
            else
            {
                // we handle the STRSAFE_FILL_ON_FAILURE and STRSAFE_NULL_ON_FAILURE cases below, so do not pass
                // those flags through
                hr = StringCopyExWorkerA(pszDestEnd,
                                         cchRemaining,
                                         (cchRemaining * sizeof(char)) + (cbDest % sizeof(char)),
                                         pszSrc,
                                         &pszDestEnd,
                                         &cchRemaining,
                                         dwFlags & (~(STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)));
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            // STRSAFE_NO_TRUNCATION is taken care of by StringCopyExWorkerA()

            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = '\0';
                }
            }

            if (dwFlags & STRSAFE_NULL_ON_FAILURE)
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = '\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}

STRSAFEAPI StringCatExWorkerW(wchar_t* pszDest, size_t cchDest, size_t cbDest, const wchar_t* pszSrc, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr = S_OK;
    wchar_t* pszDestEnd = pszDest;
    size_t cchRemaining = 0;

    // ASSERT(cbDest == (cchDest * sizeof(wchar_t)) ||
    //        cbDest == (cchDest * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cchDestCurrent;

        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest == 0) && (cbDest == 0))
                {
                    cchDestCurrent = 0;
                }
                else
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }
            else
            {
                hr = StringLengthWorkerW(pszDest, cchDest, &cchDestCurrent);

                if (SUCCEEDED(hr))
                {
                    pszDestEnd = pszDest + cchDestCurrent;
                    cchRemaining = cchDest - cchDestCurrent;
                }
            }

            if (pszSrc == NULL)
            {
                pszSrc = L"";
            }
        }
        else
        {
            hr = StringLengthWorkerW(pszDest, cchDest, &cchDestCurrent);

            if (SUCCEEDED(hr))
            {
                pszDestEnd = pszDest + cchDestCurrent;
                cchRemaining = cchDest - cchDestCurrent;
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest == 0)
            {
                // only fail if there was actually src data to append
                if (*pszSrc != L'\0')
                {
                    if (pszDest == NULL)
                    {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    }
                    else
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            }
            else
            {
                // we handle the STRSAFE_FILL_ON_FAILURE and STRSAFE_NULL_ON_FAILURE cases below, so do not pass
                // those flags through
                hr = StringCopyExWorkerW(pszDestEnd,
                                         cchRemaining,
                                         (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t)),
                                         pszSrc,
                                         &pszDestEnd,
                                         &cchRemaining,
                                         dwFlags & (~(STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)));
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            // STRSAFE_NO_TRUNCATION is taken care of by StringCopyExWorkerW()

            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = L'\0';
                }
            }

            if (dwFlags & STRSAFE_NULL_ON_FAILURE)
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = L'\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}

STRSAFEAPI StringCatNWorkerA(char* pszDest, size_t cchDest, const char* pszSrc, size_t cchMaxAppend)
{
    HRESULT hr;
    size_t cchDestCurrent;

    hr = StringLengthWorkerA(pszDest, cchDest, &cchDestCurrent);

    if (SUCCEEDED(hr))
    {
        hr = StringCopyNWorkerA(pszDest + cchDestCurrent,
                                cchDest - cchDestCurrent,
                                pszSrc,
                                cchMaxAppend);
    }

    return hr;
}

STRSAFEAPI StringCatNWorkerW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, size_t cchMaxAppend)
{
    HRESULT hr;
    size_t cchDestCurrent;

    hr = StringLengthWorkerW(pszDest, cchDest, &cchDestCurrent);

    if (SUCCEEDED(hr))
    {
        hr = StringCopyNWorkerW(pszDest + cchDestCurrent,
                                cchDest - cchDestCurrent,
                                pszSrc,
                                cchMaxAppend);
    }

    return hr;
}

STRSAFEAPI StringCatNExWorkerA(char* pszDest, size_t cchDest, size_t cbDest, const char* pszSrc, size_t cchMaxAppend, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr = S_OK;
    char* pszDestEnd = pszDest;
    size_t cchRemaining = 0;
    size_t cchDestCurrent = 0;

    // ASSERT(cbDest == (cchDest * sizeof(char))    ||
    //        cbDest == (cchDest * sizeof(char)) + (cbDest % sizeof(char)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest == 0) && (cbDest == 0))
                {
                    cchDestCurrent = 0;
                }
                else
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }
            else
            {
                hr = StringLengthWorkerA(pszDest, cchDest, &cchDestCurrent);

                if (SUCCEEDED(hr))
                {
                    pszDestEnd = pszDest + cchDestCurrent;
                    cchRemaining = cchDest - cchDestCurrent;
                }
            }

            if (pszSrc == NULL)
            {
                pszSrc = "";
            }
        }
        else
        {
            hr = StringLengthWorkerA(pszDest, cchDest, &cchDestCurrent);

            if (SUCCEEDED(hr))
            {
                pszDestEnd = pszDest + cchDestCurrent;
                cchRemaining = cchDest - cchDestCurrent;
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest == 0)
            {
                // only fail if there was actually src data to append
                if (*pszSrc != '\0')
                {
                    if (pszDest == NULL)
                    {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    }
                    else
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            }
            else
            {
                // we handle the STRSAFE_FILL_ON_FAILURE and STRSAFE_NULL_ON_FAILURE cases below, so do not pass
                // those flags through
                hr = StringCopyNExWorkerA(pszDestEnd,
                                          cchRemaining,
                                          (cchRemaining * sizeof(char)) + (cbDest % sizeof(char)),
                                          pszSrc,
                                          cchMaxAppend,
                                          &pszDestEnd,
                                          &cchRemaining,
                                          dwFlags & (~(STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)));
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            // STRSAFE_NO_TRUNCATION is taken care of by StringCopyNExWorkerA()

            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = '\0';
                }
            }

            if (dwFlags & (STRSAFE_NULL_ON_FAILURE))
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = '\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}

STRSAFEAPI StringCatNExWorkerW(wchar_t* pszDest, size_t cchDest, size_t cbDest, const wchar_t* pszSrc, size_t cchMaxAppend, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr = S_OK;
    wchar_t* pszDestEnd = pszDest;
    size_t cchRemaining = 0;
    size_t cchDestCurrent = 0;


    // ASSERT(cbDest == (cchDest * sizeof(wchar_t)) ||
    //        cbDest == (cchDest * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest == 0) && (cbDest == 0))
                {
                    cchDestCurrent = 0;
                }
                else
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }
            else
            {
                hr = StringLengthWorkerW(pszDest, cchDest, &cchDestCurrent);

                if (SUCCEEDED(hr))
                {
                    pszDestEnd = pszDest + cchDestCurrent;
                    cchRemaining = cchDest - cchDestCurrent;
                }
            }

            if (pszSrc == NULL)
            {
                pszSrc = L"";
            }
        }
        else
        {
            hr = StringLengthWorkerW(pszDest, cchDest, &cchDestCurrent);

            if (SUCCEEDED(hr))
            {
                pszDestEnd = pszDest + cchDestCurrent;
                cchRemaining = cchDest - cchDestCurrent;
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest == 0)
            {
                // only fail if there was actually src data to append
                if (*pszSrc != L'\0')
                {
                    if (pszDest == NULL)
                    {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    }
                    else
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            }
            else
            {
                // we handle the STRSAFE_FILL_ON_FAILURE and STRSAFE_NULL_ON_FAILURE cases below, so do not pass
                // those flags through
                hr = StringCopyNExWorkerW(pszDestEnd,
                                          cchRemaining,
                                          (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t)),
                                          pszSrc,
                                          cchMaxAppend,
                                          &pszDestEnd,
                                          &cchRemaining,
                                          dwFlags & (~(STRSAFE_FILL_ON_FAILURE | STRSAFE_NULL_ON_FAILURE)));
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            // STRSAFE_NO_TRUNCATION is taken care of by StringCopyNExWorkerW()

            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = L'\0';
                }
            }

            if (dwFlags & (STRSAFE_NULL_ON_FAILURE))
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = L'\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}

STRSAFEAPI StringVPrintfWorkerA(char* pszDest, size_t cchDest, const char* pszFormat, va_list argList)
{
    HRESULT hr = S_OK;

    if (cchDest == 0)
    {
        // can not null terminate a zero-byte dest buffer
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        int iRet;
        size_t cchMax;

        // leave the last space for the null terminator
        cchMax = cchDest - 1;

        iRet = _vsnprintf(pszDest, cchMax, pszFormat, argList);
        // ASSERT((iRet < 0) || (((size_t)iRet) <= cchMax));

        if ((iRet < 0) || (((size_t)iRet) > cchMax))
        {
            // need to null terminate the string
            pszDest += cchMax;
            *pszDest = '\0';

            // we have truncated pszDest
            hr = STRSAFE_E_INSUFFICIENT_BUFFER;
        }
        else if (((size_t)iRet) == cchMax)
        {
            // need to null terminate the string
            pszDest += cchMax;
            *pszDest = '\0';
        }
    }

    return hr;
}

STRSAFEAPI StringVPrintfWorkerW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszFormat, va_list argList)
{
    HRESULT hr = S_OK;

    if (cchDest == 0)
    {
        // can not null terminate a zero-byte dest buffer
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        int iRet;
        size_t cchMax;

        // leave the last space for the null terminator
        cchMax = cchDest - 1;

        iRet = _vsnwprintf(pszDest, cchMax, pszFormat, argList);
        // ASSERT((iRet < 0) || (((size_t)iRet) <= cchMax));

        if ((iRet < 0) || (((size_t)iRet) > cchMax))
        {
            // need to null terminate the string
            pszDest += cchMax;
            *pszDest = L'\0';

            // we have truncated pszDest
            hr = STRSAFE_E_INSUFFICIENT_BUFFER;
        }
        else if (((size_t)iRet) == cchMax)
        {
            // need to null terminate the string
            pszDest += cchMax;
            *pszDest = L'\0';
        }
    }

    return hr;
}

STRSAFEAPI StringVPrintfExWorkerA(char* pszDest, size_t cchDest, size_t cbDest, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags, const char* pszFormat, va_list argList)
{
    HRESULT hr = S_OK;
    char* pszDestEnd = pszDest;
    size_t cchRemaining = 0;

    // ASSERT(cbDest == (cchDest * sizeof(char))    ||
    //        cbDest == (cchDest * sizeof(char)) + (cbDest % sizeof(char)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest != 0) || (cbDest != 0))
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }

            if (pszFormat == NULL)
            {
                pszFormat = "";
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest == 0)
            {
                pszDestEnd = pszDest;
                cchRemaining = 0;

                // only fail if there was actually a non-empty format string
                if (*pszFormat != '\0')
                {
                    if (pszDest == NULL)
                    {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    }
                    else
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            }
            else
            {
                int iRet;
                size_t cchMax;

                // leave the last space for the null terminator
                cchMax = cchDest - 1;

                iRet = _vsnprintf(pszDest, cchMax, pszFormat, argList);
                // ASSERT((iRet < 0) || (((size_t)iRet) <= cchMax));

                if ((iRet < 0) || (((size_t)iRet) > cchMax))
                {
                    // we have truncated pszDest
                    pszDestEnd = pszDest + cchMax;
                    cchRemaining = 1;

                    // need to null terminate the string
                    *pszDestEnd = '\0';

                    hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                }
                else if (((size_t)iRet) == cchMax)
                {
                    // string fit perfectly
                    pszDestEnd = pszDest + cchMax;
                    cchRemaining = 1;

                    // need to null terminate the string
                    *pszDestEnd = '\0';
                }
                else if (((size_t)iRet) < cchMax)
                {
                    // there is extra room
                    pszDestEnd = pszDest + iRet;
                    cchRemaining = cchDest - iRet;

                    if (dwFlags & STRSAFE_FILL_BEHIND_NULL)
                    {
                        memset(pszDestEnd + 1, STRSAFE_GET_FILL_PATTERN(dwFlags), ((cchRemaining - 1) * sizeof(char)) + (cbDest % sizeof(char)));
                    }
                }
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = '\0';
                }
            }

            if (dwFlags & (STRSAFE_NULL_ON_FAILURE | STRSAFE_NO_TRUNCATION))
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = '\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}

STRSAFEAPI StringVPrintfExWorkerW(wchar_t* pszDest, size_t cchDest, size_t cbDest, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags, const wchar_t* pszFormat, va_list argList)
{
    HRESULT hr = S_OK;
    wchar_t* pszDestEnd = pszDest;
    size_t cchRemaining = 0;

    // ASSERT(cbDest == (cchDest * sizeof(wchar_t)) ||
    //        cbDest == (cchDest * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest != 0) || (cbDest != 0))
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }

            if (pszFormat == NULL)
            {
                pszFormat = L"";
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest == 0)
            {
                pszDestEnd = pszDest;
                cchRemaining = 0;

                // only fail if there was actually a non-empty format string
                if (*pszFormat != L'\0')
                {
                    if (pszDest == NULL)
                    {
                        hr = STRSAFE_E_INVALID_PARAMETER;
                    }
                    else
                    {
                        hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                    }
                }
            }
            else
            {
                int iRet;
                size_t cchMax;

                // leave the last space for the null terminator
                cchMax = cchDest - 1;

                iRet = _vsnwprintf(pszDest, cchMax, pszFormat, argList);
                // ASSERT((iRet < 0) || (((size_t)iRet) <= cchMax));

                if ((iRet < 0) || (((size_t)iRet) > cchMax))
                {
                    // we have truncated pszDest
                    pszDestEnd = pszDest + cchMax;
                    cchRemaining = 1;

                    // need to null terminate the string
                    *pszDestEnd = L'\0';

                    hr = STRSAFE_E_INSUFFICIENT_BUFFER;
                }
                else if (((size_t)iRet) == cchMax)
                {
                    // string fit perfectly
                    pszDestEnd = pszDest + cchMax;
                    cchRemaining = 1;

                    // need to null terminate the string
                    *pszDestEnd = L'\0';
                }
                else if (((size_t)iRet) < cchMax)
                {
                    // there is extra room
                    pszDestEnd = pszDest + iRet;
                    cchRemaining = cchDest - iRet;

                    if (dwFlags & STRSAFE_FILL_BEHIND_NULL)
                    {
                        memset(pszDestEnd + 1, STRSAFE_GET_FILL_PATTERN(dwFlags), ((cchRemaining - 1) * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t)));
                    }
                }
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = L'\0';
                }
            }

            if (dwFlags & (STRSAFE_NULL_ON_FAILURE | STRSAFE_NO_TRUNCATION))
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = L'\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}

STRSAFEAPI StringLengthWorkerA(const char* psz, size_t cchMax, size_t* pcch)
{
    HRESULT hr = S_OK;
    size_t cchMaxPrev = cchMax;

    while (cchMax && (*psz != '\0'))
    {
        psz++;
        cchMax--;
    }

    if (cchMax == 0)
    {
        // the string is longer than cchMax
        hr = STRSAFE_E_INVALID_PARAMETER;
    }

    if (SUCCEEDED(hr) && pcch)
    {
        *pcch = cchMaxPrev - cchMax;
    }

    return hr;
}

STRSAFEAPI StringLengthWorkerW(const wchar_t* psz, size_t cchMax, size_t* pcch)
{
    HRESULT hr = S_OK;
    size_t cchMaxPrev = cchMax;

    while (cchMax && (*psz != L'\0'))
    {
        psz++;
        cchMax--;
    }

    if (cchMax == 0)
    {
        // the string is longer than cchMax
        hr = STRSAFE_E_INVALID_PARAMETER;
    }

    if (SUCCEEDED(hr) && pcch)
    {
        *pcch = cchMaxPrev - cchMax;
    }

    return hr;
}

STRSAFEAPI StringGetsExWorkerA(char* pszDest, size_t cchDest, size_t cbDest, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr = S_OK;
    char* pszDestEnd = pszDest;
    size_t cchRemaining = 0;

    // ASSERT(cbDest == (cchDest * sizeof(char))    ||
    //        cbDest == (cchDest * sizeof(char)) + (cbDest % sizeof(char)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest != 0) || (cbDest != 0))
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest <= 1)
            {
                pszDestEnd = pszDest;
                cchRemaining = cchDest;

                if (cchDest == 1)
                {
                    *pszDestEnd = '\0';
                }

                hr = STRSAFE_E_INSUFFICIENT_BUFFER;
            }
            else
            {
                char ch;

                pszDestEnd = pszDest;
                cchRemaining = cchDest;

                while ((cchRemaining > 1) && (ch = (char)getc(stdin)) != '\n')
                {
                    if (ch == EOF)
                    {
                        if (pszDestEnd == pszDest)
                        {
                            // we failed to read anything from stdin
                            hr = STRSAFE_E_END_OF_FILE;
                        }
                        break;
                    }

                    *pszDestEnd = ch;

                    pszDestEnd++;
                    cchRemaining--;
                }

                if (cchRemaining > 0)
                {
                    // there is extra room
                    if (dwFlags & STRSAFE_FILL_BEHIND_NULL)
                    {
                        memset(pszDestEnd + 1, STRSAFE_GET_FILL_PATTERN(dwFlags), ((cchRemaining - 1) * sizeof(char)) + (cbDest % sizeof(char)));
                    }
                }

                *pszDestEnd = '\0';
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = '\0';
                }
            }

            if (dwFlags & (STRSAFE_NULL_ON_FAILURE | STRSAFE_NO_TRUNCATION))
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = '\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr)                           ||
        (hr == STRSAFE_E_INSUFFICIENT_BUFFER)   ||
        (hr == STRSAFE_E_END_OF_FILE))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}

STRSAFEAPI StringGetsExWorkerW(wchar_t* pszDest, size_t cchDest, size_t cbDest, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr = S_OK;
    wchar_t* pszDestEnd = pszDest;
    size_t cchRemaining = 0;

    // ASSERT(cbDest == (cchDest * sizeof(char))    ||
    //        cbDest == (cchDest * sizeof(char)) + (cbDest % sizeof(char)));

    // only accept valid flags
    if (dwFlags & (~STRSAFE_VALID_FLAGS))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        if (dwFlags & STRSAFE_IGNORE_NULLS)
        {
            if (pszDest == NULL)
            {
                if ((cchDest != 0) || (cbDest != 0))
                {
                    // NULL pszDest and non-zero cchDest/cbDest is invalid
                    hr = STRSAFE_E_INVALID_PARAMETER;
                }
            }
        }

        if (SUCCEEDED(hr))
        {
            if (cchDest <= 1)
            {
                pszDestEnd = pszDest;
                cchRemaining = cchDest;

                if (cchDest == 1)
                {
                    *pszDestEnd = L'\0';
                }

                hr = STRSAFE_E_INSUFFICIENT_BUFFER;
            }
            else
            {
                wchar_t ch;

                pszDestEnd = pszDest;
                cchRemaining = cchDest;

                while ((cchRemaining > 1) && (ch = (wchar_t)getwc(stdin)) != L'\n')
                {
                    if (ch == EOF)
                    {
                        if (pszDestEnd == pszDest)
                        {
                            // we failed to read anything from stdin
                            hr = STRSAFE_E_END_OF_FILE;
                        }
                        break;
                    }

                    *pszDestEnd = ch;

                    pszDestEnd++;
                    cchRemaining--;
                }

                if (cchRemaining > 0)
                {
                    // there is extra room
                    if (dwFlags & STRSAFE_FILL_BEHIND_NULL)
                    {
                        memset(pszDestEnd + 1, STRSAFE_GET_FILL_PATTERN(dwFlags), ((cchRemaining - 1) * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t)));
                    }
                }

                *pszDestEnd = L'\0';
            }
        }
    }

    if (FAILED(hr))
    {
        if (pszDest)
        {
            if (dwFlags & STRSAFE_FILL_ON_FAILURE)
            {
                memset(pszDest, STRSAFE_GET_FILL_PATTERN(dwFlags), cbDest);

                if (STRSAFE_GET_FILL_PATTERN(dwFlags) == 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;
                }
                else if (cchDest > 0)
                {
                    pszDestEnd = pszDest + cchDest - 1;
                    cchRemaining = 1;

                    // null terminate the end of the string
                    *pszDestEnd = L'\0';
                }
            }

            if (dwFlags & (STRSAFE_NULL_ON_FAILURE | STRSAFE_NO_TRUNCATION))
            {
                if (cchDest > 0)
                {
                    pszDestEnd = pszDest;
                    cchRemaining = cchDest;

                    // null terminate the beginning of the string
                    *pszDestEnd = L'\0';
                }
            }
        }
    }

    if (SUCCEEDED(hr)                           ||
        (hr == STRSAFE_E_INSUFFICIENT_BUFFER)   ||
        (hr == STRSAFE_E_END_OF_FILE))
    {
        if (ppszDestEnd)
        {
            *ppszDestEnd = pszDestEnd;
        }

        if (pcchRemaining)
        {
            *pcchRemaining = cchRemaining;
        }
    }

    return hr;
}


//==========================================
// user functions
STRSAFEAPI StringCchCopyA(char* pszDest, size_t cchDest, const char* pszSrc)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyWorkerA(pszDest, cchDest, pszSrc);
    }

    return hr;
}

STRSAFEAPI StringCchCopyW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyWorkerW(pszDest, cchDest, pszSrc);
    }

    return hr;
}

STRSAFEAPI StringCbCopyA(char* pszDest, size_t cbDest, const char* pszSrc)
{
    HRESULT hr;
    size_t cchDest;

    // convert to count of characters
    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyWorkerA(pszDest, cchDest, pszSrc);
    }

    return hr;
}

STRSAFEAPI StringCbCopyW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc)
{
    HRESULT hr;
    size_t cchDest;

    // convert to count of characters
    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyWorkerW(pszDest, cchDest, pszSrc);
    }

    return hr;
}

STRSAFEAPI StringCchCopyExA(char* pszDest, size_t cchDest, const char* pszSrc, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(char) since cchDest < STRSAFE_MAX_CCH and sizeof(char) is 1
        cbDest = cchDest * sizeof(char);

        hr = StringCopyExWorkerA(pszDest, cchDest, cbDest, pszSrc, ppszDestEnd, pcchRemaining, dwFlags);
    }

    return hr;
}

STRSAFEAPI StringCchCopyExW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(wchar_t) since cchDest < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
        cbDest = cchDest * sizeof(wchar_t);

        hr = StringCopyExWorkerW(pszDest, cchDest, cbDest, pszSrc, ppszDestEnd, pcchRemaining, dwFlags);
    }

    return hr;
}

STRSAFEAPI StringCbCopyExA(char* pszDest, size_t cbDest, const char* pszSrc, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyExWorkerA(pszDest, cchDest, cbDest, pszSrc, ppszDestEnd, &cchRemaining, dwFlags);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(char) since cchRemaining < STRSAFE_MAX_CCH and sizeof(char) is 1
            *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
        }
    }

    return hr;
}

STRSAFEAPI StringCbCopyExW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyExWorkerW(pszDest, cchDest, cbDest, pszSrc, ppszDestEnd, &cchRemaining, dwFlags);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(wchar_t) since cchRemaining < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
            *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
        }
    }

    return hr;
}

STRSAFEAPI StringCchCopyNA(char* pszDest, size_t cchDest, const char* pszSrc, size_t cchSrc)
{
    HRESULT hr;

    if ((cchDest > STRSAFE_MAX_CCH) ||
        (cchSrc > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyNWorkerA(pszDest, cchDest, pszSrc, cchSrc);
    }

    return hr;
}

STRSAFEAPI StringCchCopyNW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, size_t cchSrc)
{
    HRESULT hr;

    if ((cchDest > STRSAFE_MAX_CCH) ||
        (cchSrc > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyNWorkerW(pszDest, cchDest, pszSrc, cchSrc);
    }

    return hr;
}

STRSAFEAPI StringCbCopyNA(char* pszDest, size_t cbDest, const char* pszSrc, size_t cbSrc)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchSrc;

    // convert to count of characters
    cchDest = cbDest / sizeof(char);
    cchSrc = cbSrc / sizeof(char);

    if ((cchDest > STRSAFE_MAX_CCH) ||
        (cchSrc > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyNWorkerA(pszDest, cchDest, pszSrc, cchSrc);
    }

    return hr;
}

STRSAFEAPI StringCbCopyNW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, size_t cbSrc)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchSrc;

    // convert to count of characters
    cchDest = cbDest / sizeof(wchar_t);
    cchSrc = cbSrc / sizeof(wchar_t);

    if ((cchDest > STRSAFE_MAX_CCH) ||
        (cchSrc > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyNWorkerW(pszDest, cchDest, pszSrc, cchSrc);
    }

    return hr;
}

STRSAFEAPI StringCchCopyNExA(char* pszDest, size_t cchDest, const char* pszSrc, size_t cchSrc, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr;

    if ((cchDest > STRSAFE_MAX_CCH) ||
        (cchSrc > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(char) since cchDest < STRSAFE_MAX_CCH and sizeof(char) is 1
        cbDest = cchDest * sizeof(char);

        hr = StringCopyNExWorkerA(pszDest, cchDest, cbDest, pszSrc, cchSrc, ppszDestEnd, pcchRemaining, dwFlags);
    }

    return hr;
}

STRSAFEAPI StringCchCopyNExW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, size_t cchSrc, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr;

    if ((cchDest > STRSAFE_MAX_CCH) ||
        (cchSrc > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(wchar_t) since cchDest < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
        cbDest = cchDest * sizeof(wchar_t);

        hr = StringCopyNExWorkerW(pszDest, cchDest, cbDest, pszSrc, cchSrc, ppszDestEnd, pcchRemaining, dwFlags);
    }

    return hr;
}

STRSAFEAPI StringCbCopyNExA(char* pszDest, size_t cbDest, const char* pszSrc, size_t cbSrc, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchSrc;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(char);
    cchSrc = cbSrc / sizeof(char);

    if ((cchDest > STRSAFE_MAX_CCH) ||
        (cchSrc > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyNExWorkerA(pszDest, cchDest, cbDest, pszSrc, cchSrc, ppszDestEnd, &cchRemaining, dwFlags);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(char) since cchRemaining < STRSAFE_MAX_CCH and sizeof(char) is 1
            *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
        }
    }

    return hr;
}

STRSAFEAPI StringCbCopyNExW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, size_t cbSrc, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchSrc;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(wchar_t);
    cchSrc = cbSrc / sizeof(wchar_t);

    if ((cchDest > STRSAFE_MAX_CCH) ||
        (cchSrc > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCopyNExWorkerW(pszDest, cchDest, cbDest, pszSrc, cchSrc, ppszDestEnd, &cchRemaining, dwFlags);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(wchar_t) since cchRemaining < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
            *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
        }
    }

    return hr;
}

STRSAFEAPI StringCchCatA(char* pszDest, size_t cchDest, const char* pszSrc)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCatWorkerA(pszDest, cchDest, pszSrc);
    }

    return hr;
}

STRSAFEAPI StringCchCatW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCatWorkerW(pszDest, cchDest, pszSrc);
    }

    return hr;
}

STRSAFEAPI StringCbCatA(char* pszDest, size_t cbDest, const char* pszSrc)
{
    HRESULT hr;
    size_t cchDest;

    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCatWorkerA(pszDest, cchDest, pszSrc);
    }

    return hr;
}

STRSAFEAPI StringCbCatW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc)
{
    HRESULT hr;
    size_t cchDest;

    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCatWorkerW(pszDest, cchDest, pszSrc);
    }

    return hr;
}

STRSAFEAPI StringCchCatExA(char* pszDest, size_t cchDest, const char* pszSrc, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(char) since cchDest < STRSAFE_MAX_CCH and sizeof(char) is 1
        cbDest = cchDest * sizeof(char);

        hr = StringCatExWorkerA(pszDest, cchDest, cbDest, pszSrc, ppszDestEnd, pcchRemaining, dwFlags);
    }

    return hr;
}

STRSAFEAPI StringCchCatExW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(wchar_t) since cchDest < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
        cbDest = cchDest * sizeof(wchar_t);

        hr = StringCatExWorkerW(pszDest, cchDest, cbDest, pszSrc, ppszDestEnd, pcchRemaining, dwFlags);
    }

    return hr;
}

STRSAFEAPI StringCbCatExA(char* pszDest, size_t cbDest, const char* pszSrc, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCatExWorkerA(pszDest, cchDest, cbDest, pszSrc, ppszDestEnd, &cchRemaining, dwFlags);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(char) since cchRemaining < STRSAFE_MAX_CCH and sizeof(char) is 1
            *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
        }
    }

    return hr;
}

STRSAFEAPI StringCbCatExW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCatExWorkerW(pszDest, cchDest, cbDest, pszSrc, ppszDestEnd, &cchRemaining, dwFlags);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(wchar_t) since cchRemaining < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
            *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
        }
    }

    return hr;
}

STRSAFEAPI StringCchCatNA(char* pszDest, size_t cchDest, const char* pszSrc, size_t cchMaxAppend)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCatNWorkerA(pszDest, cchDest, pszSrc, cchMaxAppend);
    }

    return hr;
}

STRSAFEAPI StringCchCatNW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, size_t cchMaxAppend)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringCatNWorkerW(pszDest, cchDest, pszSrc, cchMaxAppend);
    }

    return hr;
}

STRSAFEAPI StringCbCatNA(char* pszDest, size_t cbDest, const char* pszSrc, size_t cbMaxAppend)
{
    HRESULT hr;
    size_t cchDest;

    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cchMaxAppend;

        cchMaxAppend = cbMaxAppend / sizeof(char);

        hr = StringCatNWorkerA(pszDest, cchDest, pszSrc, cchMaxAppend);
    }

    return hr;
}

STRSAFEAPI StringCbCatNW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, size_t cbMaxAppend)
{
    HRESULT hr;
    size_t cchDest;

    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cchMaxAppend;

        cchMaxAppend = cbMaxAppend / sizeof(wchar_t);

        hr = StringCatNWorkerW(pszDest, cchDest, pszSrc, cchMaxAppend);
    }

    return hr;
}

STRSAFEAPI StringCchCatNExA(char* pszDest, size_t cchDest, const char* pszSrc, size_t cchMaxAppend, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(char) since cchDest < STRSAFE_MAX_CCH and sizeof(char) is 1
        cbDest = cchDest * sizeof(char);

        hr = StringCatNExWorkerA(pszDest, cchDest, cbDest, pszSrc, cchMaxAppend, ppszDestEnd, pcchRemaining, dwFlags);
    }

    return hr;
}

STRSAFEAPI StringCchCatNExW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszSrc, size_t cchMaxAppend, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(wchar_t) since cchDest < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
        cbDest = cchDest * sizeof(wchar_t);

        hr = StringCatNExWorkerW(pszDest, cchDest, cbDest, pszSrc, cchMaxAppend, ppszDestEnd, pcchRemaining, dwFlags);
    }

    return hr;
}

STRSAFEAPI StringCbCatNExA(char* pszDest, size_t cbDest, const char* pszSrc, size_t cbMaxAppend, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cchMaxAppend;

        cchMaxAppend = cbMaxAppend / sizeof(char);

        hr = StringCatNExWorkerA(pszDest, cchDest, cbDest, pszSrc, cchMaxAppend, ppszDestEnd, &cchRemaining, dwFlags);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(char) since cchRemaining < STRSAFE_MAX_CCH and sizeof(char) is 1
            *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
        }
    }

    return hr;
}

STRSAFEAPI StringCbCatNExW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszSrc, size_t cbMaxAppend, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cchMaxAppend;

        cchMaxAppend = cbMaxAppend / sizeof(wchar_t);

        hr = StringCatNExWorkerW(pszDest, cchDest, cbDest, pszSrc, cchMaxAppend, ppszDestEnd, &cchRemaining, dwFlags);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(wchar_t) since cchRemaining < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
            *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
        }
    }

    return hr;
}

STRSAFEAPI StringCchVPrintfA(char* pszDest, size_t cchDest, const char* pszFormat, va_list argList)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringVPrintfWorkerA(pszDest, cchDest, pszFormat, argList);
    }

    return hr;
}

STRSAFEAPI StringCchVPrintfW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszFormat, va_list argList)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringVPrintfWorkerW(pszDest, cchDest, pszFormat, argList);
    }

    return hr;
}

STRSAFEAPI StringCbVPrintfA(char* pszDest, size_t cbDest, const char* pszFormat, va_list argList)
{
    HRESULT hr;
    size_t cchDest;

    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringVPrintfWorkerA(pszDest, cchDest, pszFormat, argList);
    }

    return hr;
}

STRSAFEAPI StringCbVPrintfW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszFormat, va_list argList)
{
    HRESULT hr;
    size_t cchDest;

    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringVPrintfWorkerW(pszDest, cchDest, pszFormat, argList);
    }

    return hr;
}

STRSAFEAPI StringCchPrintfA(char* pszDest, size_t cchDest, const char* pszFormat, ...)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        va_list argList;

        va_start(argList, pszFormat);

        hr = StringVPrintfWorkerA(pszDest, cchDest, pszFormat, argList);

        va_end(argList);
    }

    return hr;
}

STRSAFEAPI StringCchPrintfW(wchar_t* pszDest, size_t cchDest, const wchar_t* pszFormat, ...)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        va_list argList;

        va_start(argList, pszFormat);

        hr = StringVPrintfWorkerW(pszDest, cchDest, pszFormat, argList);

        va_end(argList);
    }

    return hr;
}

STRSAFEAPI StringCbPrintfA(char* pszDest, size_t cbDest, const char* pszFormat, ...)
{
    HRESULT hr;
    size_t cchDest;

    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        va_list argList;

        va_start(argList, pszFormat);

        hr = StringVPrintfWorkerA(pszDest, cchDest, pszFormat, argList);

        va_end(argList);
    }

    return hr;
}

STRSAFEAPI StringCbPrintfW(wchar_t* pszDest, size_t cbDest, const wchar_t* pszFormat, ...)
{
    HRESULT hr;
    size_t cchDest;

    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        va_list argList;

        va_start(argList, pszFormat);

        hr = StringVPrintfWorkerW(pszDest, cchDest, pszFormat, argList);

        va_end(argList);
    }

    return hr;
}

STRSAFEAPI StringCchPrintfExA(char* pszDest, size_t cchDest, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags, const char* pszFormat, ...)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;
        va_list argList;

        // safe to multiply cchDest * sizeof(char) since cchDest < STRSAFE_MAX_CCH and sizeof(char) is 1
        cbDest = cchDest * sizeof(char);
        va_start(argList, pszFormat);

        hr = StringVPrintfExWorkerA(pszDest, cchDest, cbDest, ppszDestEnd, pcchRemaining, dwFlags, pszFormat, argList);

        va_end(argList);
    }

    return hr;
}

STRSAFEAPI StringCchPrintfExW(wchar_t* pszDest, size_t cchDest, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags, const wchar_t* pszFormat, ...)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;
        va_list argList;

        // safe to multiply cchDest * sizeof(wchar_t) since cchDest < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
        cbDest = cchDest * sizeof(wchar_t);
        va_start(argList, pszFormat);

        hr = StringVPrintfExWorkerW(pszDest, cchDest, cbDest, ppszDestEnd, pcchRemaining, dwFlags, pszFormat, argList);

        va_end(argList);
    }

    return hr;
}

STRSAFEAPI StringCbPrintfExA(char* pszDest, size_t cbDest, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags, const char* pszFormat, ...)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        va_list argList;

        va_start(argList, pszFormat);

        hr = StringVPrintfExWorkerA(pszDest, cchDest, cbDest, ppszDestEnd, &cchRemaining, dwFlags, pszFormat, argList);

        va_end(argList);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(char) since cchRemaining < STRSAFE_MAX_CCH and sizeof(char) is 1
            *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
        }
    }

    return hr;
}

STRSAFEAPI StringCbPrintfExW(wchar_t* pszDest, size_t cbDest, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags, const wchar_t* pszFormat, ...)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        va_list argList;

        va_start(argList, pszFormat);

        hr = StringVPrintfExWorkerW(pszDest, cchDest, cbDest, ppszDestEnd, &cchRemaining, dwFlags, pszFormat, argList);

        va_end(argList);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(wchar_t) since cchRemaining < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
            *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
        }
    }

    return hr;
}

STRSAFEAPI StringCchVPrintfExA(char* pszDest, size_t cchDest, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags, const char* pszFormat, va_list argList)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(char) since cchDest < STRSAFE_MAX_CCH and sizeof(char) is 1
        cbDest = cchDest * sizeof(char);

        hr = StringVPrintfExWorkerA(pszDest, cchDest, cbDest, ppszDestEnd, pcchRemaining, dwFlags, pszFormat, argList);
    }

    return hr;
}

STRSAFEAPI StringCchVPrintfExW(wchar_t* pszDest, size_t cchDest, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags, const wchar_t* pszFormat, va_list argList)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(wchar_t) since cchDest < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
        cbDest = cchDest * sizeof(wchar_t);

        hr = StringVPrintfExWorkerW(pszDest, cchDest, cbDest, ppszDestEnd, pcchRemaining, dwFlags, pszFormat, argList);
    }

    return hr;
}

STRSAFEAPI StringCbVPrintfExA(char* pszDest, size_t cbDest, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags, const char* pszFormat, va_list argList)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringVPrintfExWorkerA(pszDest, cchDest, cbDest, ppszDestEnd, &cchRemaining, dwFlags, pszFormat, argList);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(char) since cchRemaining < STRSAFE_MAX_CCH and sizeof(char) is 1
            *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
        }
    }

    return hr;
}

STRSAFEAPI StringCbVPrintfExW(wchar_t* pszDest, size_t cbDest, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags, const wchar_t* pszFormat, va_list argList)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringVPrintfExWorkerW(pszDest, cchDest, cbDest, ppszDestEnd, &cchRemaining, dwFlags, pszFormat, argList);
    }

    if (SUCCEEDED(hr) || (hr == STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(wchar_t) since cchRemaining < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
            *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
        }
    }

    return hr;
}

STRSAFEAPI StringCchGetsA(char* pszDest, size_t cchDest)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(char) since cchDest < STRSAFE_MAX_CCH and sizeof(char) is 1
        cbDest = cchDest * sizeof(char);

        hr = StringGetsExWorkerA(pszDest, cchDest, cbDest, NULL, NULL, 0);
    }

    return hr;
}

STRSAFEAPI StringCchGetsW(wchar_t* pszDest, size_t cchDest)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(wchar_t) since cchDest < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
        cbDest = cchDest * sizeof(wchar_t);

        hr = StringGetsExWorkerW(pszDest, cchDest, cbDest, NULL, NULL, 0);
    }

    return hr;
}

STRSAFEAPI StringCbGetsA(char* pszDest, size_t cbDest)
{
    HRESULT hr;
    size_t cchDest;

    // convert to count of characters
    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringGetsExWorkerA(pszDest, cchDest, cbDest, NULL, NULL, 0);
    }

    return hr;
}

STRSAFEAPI StringCbGetsW(wchar_t* pszDest, size_t cbDest)
{
    HRESULT hr;
    size_t cchDest;

    // convert to count of characters
    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringGetsExWorkerW(pszDest, cchDest, cbDest, NULL, NULL, 0);
    }

    return hr;
}

STRSAFEAPI StringCchGetsExA(char* pszDest, size_t cchDest, char** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(char) since cchDest < STRSAFE_MAX_CCH and sizeof(char) is 1
        cbDest = cchDest * sizeof(char);

        hr = StringGetsExWorkerA(pszDest, cchDest, cbDest, ppszDestEnd, pcchRemaining, dwFlags);
    }

    return hr;
}

STRSAFEAPI StringCchGetsExW(wchar_t* pszDest, size_t cchDest, wchar_t** ppszDestEnd, size_t* pcchRemaining, unsigned long dwFlags)
{
    HRESULT hr;

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        size_t cbDest;

        // safe to multiply cchDest * sizeof(wchar_t) since cchDest < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
        cbDest = cchDest * sizeof(wchar_t);

        hr = StringGetsExWorkerW(pszDest, cchDest, cbDest, ppszDestEnd, pcchRemaining, dwFlags);
    }

    return hr;
}

STRSAFEAPI StringCbGetsExA(char* pszDest, size_t cbDest, char** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(char);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringGetsExWorkerA(pszDest, cchDest, cbDest, ppszDestEnd, &cchRemaining, dwFlags);
    }

    if (SUCCEEDED(hr)                           ||
        (hr == STRSAFE_E_INSUFFICIENT_BUFFER)   ||
        (hr == STRSAFE_E_END_OF_FILE))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(char) since cchRemaining < STRSAFE_MAX_CCH and sizeof(char) is 1
            *pcbRemaining = (cchRemaining * sizeof(char)) + (cbDest % sizeof(char));
        }
    }

    return hr;
}

STRSAFEAPI StringCbGetsExW(wchar_t* pszDest, size_t cbDest, wchar_t** ppszDestEnd, size_t* pcbRemaining, unsigned long dwFlags)
{
    HRESULT hr;
    size_t cchDest;
    size_t cchRemaining = 0;

    cchDest = cbDest / sizeof(wchar_t);

    if (cchDest > STRSAFE_MAX_CCH)
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringGetsExWorkerW(pszDest, cchDest, cbDest, ppszDestEnd, &cchRemaining, dwFlags);
    }

    if (SUCCEEDED(hr)                           ||
        (hr == STRSAFE_E_INSUFFICIENT_BUFFER)   ||
        (hr == STRSAFE_E_END_OF_FILE))
    {
        if (pcbRemaining)
        {
            // safe to multiply cchRemaining * sizeof(wchar_t) since cchRemaining < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
            *pcbRemaining = (cchRemaining * sizeof(wchar_t)) + (cbDest % sizeof(wchar_t));
        }
    }

    return hr;
}

STRSAFEAPI StringCchLengthA(const char* psz, size_t cchMax, size_t* pcch)
{
    HRESULT hr;

    if ((psz == NULL) || (cchMax > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringLengthWorkerA(psz, cchMax, pcch);
    }

    return hr;
}

STRSAFEAPI StringCchLengthW(const wchar_t* psz, size_t cchMax, size_t* pcch)
{
    HRESULT hr;

    if ((psz == NULL) || (cchMax > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringLengthWorkerW(psz, cchMax, pcch);
    }

    return hr;
}

STRSAFEAPI StringCbLengthA(const char* psz, size_t cbMax, size_t* pcb)
{
    HRESULT hr;
    size_t cchMax;
    size_t cch = 0;

    cchMax = cbMax / sizeof(char);

    if ((psz == NULL) || (cchMax > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringLengthWorkerA(psz, cchMax, &cch);
    }

    if (SUCCEEDED(hr) && pcb)
    {
        // safe to multiply cch * sizeof(char) since cch < STRSAFE_MAX_CCH and sizeof(char) is 1
        *pcb = cch * sizeof(char);
    }

    return hr;
}

STRSAFEAPI StringCbLengthW(const wchar_t* psz, size_t cbMax, size_t* pcb)
{
    HRESULT hr;
    size_t cchMax;
    size_t cch = 0;

    cchMax = cbMax / sizeof(wchar_t);

    if ((psz == NULL) || (cchMax > STRSAFE_MAX_CCH))
    {
        hr = STRSAFE_E_INVALID_PARAMETER;
    }
    else
    {
        hr = StringLengthWorkerW(psz, cchMax, &cch);
    }

    if (SUCCEEDED(hr) && pcb)
    {
        // safe to multiply cch * sizeof(wchar_t) since cch < STRSAFE_MAX_CCH and sizeof(wchar_t) is 2
        *pcb = cch * sizeof(wchar_t);
    }

    return hr;
}




