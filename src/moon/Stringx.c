
/*****************************************************************************\
*                                                                             *
* stringx.c - - Basic String functions                                        *
*                                                                             *
* Version 1.0                                                                 *
*                                                                             *
* Copyright 2001 by Eberhard funck      All rights reserved.                  *
*                                                                             *
\*****************************************************************************/



#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>


// Global variable
char szBuffer[1024];


LPSTR GetString(LPSTR pszString, int iString)
{
	int i, cb;
	LPSTR pString;

	pString = pszString;
	for (i = 0; i < iString; i++)
	{
		cb = lstrlen(pString);
		pString += (cb + 1);
	}

	return pString;
}


LPSTR LeftStr(LPSTR pString, int iVal)
{
	if (iVal > lstrlen(pString))
		iVal = lstrlen(pString);
	else if (iVal < 0)
		iVal = 0;
	else
		iVal++;

	lstrcpyn(szBuffer, pString, iVal);

	return szBuffer;
}



LPSTR RightStr(LPSTR pString, int iVal)
{
	lstrcpy(szBuffer, pString);

	if (iVal == 0)
		return "";
	else if (iVal > lstrlen(pString))
		return szBuffer;
	else
		return szBuffer + (lstrlen(szBuffer) - iVal);
}



LPSTR MidStr(LPSTR pString, int iVal1, ...)
{
	int     iVal2;
	va_list ap;

	va_start(ap, iVal1);
	iVal2 = va_arg(ap, int);
	va_end(ap);

	if (iVal1 > lstrlen(pString))
		iVal1 = lstrlen(pString);

	if (iVal2 > lstrlen(pString))
		iVal2 = lstrlen(pString);
	else
		iVal2++;

	lstrcpyn(szBuffer, pString, iVal1 + iVal2);

	return (szBuffer + iVal1);
}

int InStr(LPSTR pString, LPSTR pSearch)
{
	int i = 0;
	int z = 1;

	while (z != 0)
		{
			z = strncmp(pString + i, pSearch, lstrlen(pSearch));
			i++;
			if (i > lstrlen(pString))
				z = 0;
	    }

	if (i > lstrlen(pString))
		return 0;
	else
		return (i - 1);
}



int RinStr(LPSTR pString, LPSTR pSearch)
{
	int i = lstrlen(pString);
	int z = 1;

	while (z != 0)
		{
			z = strncmp(pString + i, pSearch, lstrlen(pSearch));
			i--;
			if (i < 0)
				z = 0;
	    }

	if (i < 0)
		return 0;
	else
		return (i + 1);
}



LPSTR Str(double dVal, ...)
{
	char    szText[10];
	int     iVal;
	va_list ap;

	va_start(ap, dVal);
	iVal = va_arg(ap, int);
	va_end(ap);

	if (iVal > 10)
		iVal = 0;

	// "%.af", dVal)
	// %f= Flieﬂkommazahl
    //.a = Nachkommastellen (0-...)
	sprintf(szText, "%%.%df", iVal);
	sprintf(szBuffer, szText, dVal);

	return (szBuffer);
}



LPSTR Format(int iVal)
{
	int  cb[3];

	if (iVal < 1000)
	{
		wsprintf(szBuffer, "%d", iVal);
	}
	else if (iVal > 999 && iVal < 1000000)
	{
		cb[0] = iVal/1000;
		cb[1] = (iVal - cb[0] * 1000);
		wsprintf(szBuffer, "%d.%03d", cb[0], cb[1]);
	}
	else if (iVal > 999999 && iVal < 1000000000)
	{
		cb[0] = iVal/1000000;
		cb[1] = (iVal - cb[0] * 1000000) / 1000;
		cb[2] = ((iVal - cb[0] * 1000000) - cb[1] * 1000);
		wsprintf(szBuffer, "%d.%03d.%03d", cb[0], cb[1], cb[2]);
	}
	else
	{
		wsprintf(szBuffer, "%d", iVal);
	}

	return (szBuffer);
}


double Round(double dVal, ...)
{
	double dVal1, dVal2;
	char   szTemp[50];
	int    iVal;
	va_list ap;

	va_start(ap, dVal);
	iVal = va_arg(ap, int);
	va_end(ap);

	if (iVal > 10)
	{
		dVal2 = modf(dVal, &dVal1);

		if (dVal2 <= 0.5)
			return dVal1;
		else
			return dVal1 + 1;
	}
	else
	{
		lstrcpy(szTemp, Str(dVal, iVal));

		return atof(szTemp);
	}
}


