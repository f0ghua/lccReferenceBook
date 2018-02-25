/******************************************************************
				Parse.h - Expression Parser Header File

	Parse.h contains basic definitions for the expression parser.

	If the library is compiled with DEBUG defined, when an error
	occurs an appropriate message will be displayed to the console
	window.  If you do not want this to happen or there is no
	console window then you can comment out the DEBUG definition
	line.

	Designed and Programmed by Sean O'Rourke.
	http://backfrog.digitalrice.com
******************************************************************/
#ifndef _PARSE_HEADER_
#define _PARSE_HEADER_

#define TRUE	1
#define FALSE	0

//P_ERR is the value Evaluate returns when it encounters an error.
#define P_ERR	0xDEADBEEF

//P_FACTOR is the multiplication factor used to determine how big the flagged string
//should be.  It is the strlen of the source string multiplied by P_FACTOR.
#define P_FACTOR	7

//Max Digit is the longest a number can be inorder to fit when the string is replaced
//in ReplaceString function.
#define MAX_DIGIT	32

//Macro to free memory
#define FreeMem(x)	free(x)

//If DEBUG is defined then debug mode is active.
//#define DEBUG
#endif
