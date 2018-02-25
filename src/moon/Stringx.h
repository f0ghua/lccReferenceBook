
/*****************************************************************************\
*                                                                             *
* stringx.h - - string functions                                              *
*                                                                             *
* Version 1.0                                                                 *
*                                                                             *
* Copyright 2001, Eberhard Funck                                              *
*                                                                             *
\*****************************************************************************/


#ifndef __stringx_h__
#define __stringx_h__


// Function for String manipulation

LPSTR  LeftStr(LPSTR, int);
LPSTR  RightStr(LPSTR, int);
LPSTR  MidStr(LPSTR, int, ...);
LPSTR  GetString(LPSTR, int);
LPSTR  Str(double, ...);
LPSTR  Format(int);
int    InStr(LPSTR, LPSTR);
int    RinStr(LPSTR, LPSTR);

double Round(double, ...);


#endif
