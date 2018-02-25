/*
  EVALDEMO.C version 1.1w for LCCwin32 compiler.
  Program for evaluating arithmetic expressions at runtime.

  It simply reads a file into a string in memory and
  calls evaluateExpression to evaluate the expressions
  in the string and store the results in a simple symbol table.
  It then prints out the contents of the symbol table on Woutput.
  If there was an error, it prints a diagnostic message into same window.

  This program is a modified version of Parsifal EvaluateExpression demo.
  EvaluateExpression is an example program for evaluating C-style expressions
  which demonstrates some of the capabilities of the AnaGram parser generator.
  Copyright (c) Parsifal Software. All Rights Reserved.

  EVALDEMO is freeware, distributed under Parsifal license agreements.
  Please read license agreement within "readme.htm" document.
  This version has been released by Davide U. (udmbox@tin.it)
*/

#include <windows.h>
#include <stdio.h>
#include "evaldemo.h"   //header for wedit resource
#include "evaldefs.h"   //header for evaldemo library

// global const
#define  I64_W  20      //size in chars for a 64bit-number
#define  MAXSTR 65536   //size in chars for a edit-control

// global vars
UCHAR    errormsg[100];
HWND     Wconsole, Woutput;
UCHAR    *filename = "test.txt";
UCHAR    *expressionString = NULL;
UCHAR    *CRLF = "\r\n";     //msdos newline == chr(13)+chr(10)
int      slen, errnum = 0;

// Pototype for functions and the callback func
static   BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void     free_symbol_table(void);
void     eval_text(UCHAR *expressionString, HWND houtput);
int      load_text(UCHAR *filename);
void     printCR (HWND hwnd, UCHAR *this, int crlf);
UCHAR    *numStr( __int64 i);


// Entry point
int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
    return DialogBox(hinst, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC) DialogFunc);
}


// The Dialog callback
static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
 switch (msg) {

 case WM_INITDIALOG:
  /* Allocate storage for input string */
  expressionString = (char*)malloc(MAXSTR);
  Wconsole = GetDlgItem( hwndDlg, IDC_INPUT);
  Woutput = GetDlgItem( hwndDlg, IDC_OUTPUT);
  return TRUE;

 case WM_COMMAND:
  switch (LOWORD(wParam)) {

  case IDC_EVAL:
   SetWindowText( Woutput, "");  //clear edit control
   slen = GetWindowText( Wconsole, (LPTSTR)expressionString, MAXSTR);
   expressionString[slen] = '\0';
   eval_text(expressionString, Woutput);
   SetFocus(Wconsole);
   return 1;

  case IDC_LOAD:
   SetWindowText( Woutput, "");  //clear edit control
   errnum = load_text(filename);
   if (errnum)
        SetWindowText( Woutput, strupr(errormsg));
   else SetWindowText( Wconsole, expressionString);
   return 1;

  case IDC_QUIT:
   EndDialog(hwndDlg,0);
   return 1;
  }
  break;

 case WM_CLOSE:
  free_symbol_table();
  free(expressionString);
  EndDialog(hwndDlg,0);
  return TRUE;

 }
 return FALSE;
}

  //free symbol table array
void free_symbol_table(void) {
  int    i;
  nVariables=0;  //reset global var
  for (i = 0; i <= N_VARIABLES; i++) {
      if (variable[i].name) free(variable[i].name);
      variable[i].name  = NULL;
      variable[i].value = 0;
  }
}

  /* evaluate expressions */
void eval_text(UCHAR *expressionString, HWND houtput) {

  int    i;
  int    errorFlag;
  UCHAR  valuemsg[1000];
  UCHAR *defaultvar = "Expr=";

  free_symbol_table();
  errorFlag = evaluateExpression(expressionString);     // Call parser function

        /* single expression without assignments */
  if (nVariables==0) {

      if (strstr(expressionString,defaultvar)==0) {
         sprintf( valuemsg, "%s", defaultvar);        //use temp var
         strcat( valuemsg, expressionString);
         strcpy( expressionString, valuemsg);
         SetWindowText( Wconsole, expressionString);  //refresh edit control
      }

      free_symbol_table();
      errorFlag = evaluateExpression(expressionString);     // Call parser function

      printCR( houtput, defaultvar, 0);
      sprintf( valuemsg, "%g", variable[0].value);
      printCR( houtput, valuemsg, 1);

  }
  else {
     /* print values of variables with assignments */
     for (i = 0; i < nVariables; i++) {

         printCR( houtput, variable[i].name, 0);
         printCR( houtput, "=", 0);
         sprintf( valuemsg, "%g", variable[i].value);
         printCR( houtput, valuemsg, 1);

     }
  }

  /* check for error */
  if (errorFlag) {
     printCR( houtput, "------------------------------",1);
     printCR( houtput, "error: ", 0);
     printCR( houtput, errorRecord.message, 1);
     printCR( houtput, "line: ", 0);
     printCR( houtput, numStr(errorRecord.line)  , 1);
     printCR( houtput, "col: ", 0);
     printCR( houtput, numStr(errorRecord.column), 1);
  }

}

  /* read test file */
int load_text(UCHAR *filename) {

  FILE   *input;
  long   fileLength;
  size_t stringLength;

  /* Open input file */
  input = fopen(filename,"r");
  if (input == NULL) {
     strcpy(errormsg, "Cannot open file");
     return 2;
  }

  /* find out how big the file is */
  if (fseek(input, SEEK_SET, SEEK_END)) {
     strcpy(errormsg, "Strange problems with SEEK_END");
     return 3;
  }

  fileLength = ftell(input);
  if (fileLength < 0 ) {    // -1L is error return
     strcpy(errormsg, "Error getting file length");
     return 4;
  }

  /* fseek to beginning of file */
  if (fseek(input, 0, SEEK_SET)) {
     strcpy(errormsg, "Strange problems with SEEK_SET");
     return 5;
  }

  /* read file */
  stringLength = fread(expressionString, 1, (unsigned)fileLength, input);
  if (stringLength == 0) {
     strcpy(errormsg, "Unable to read file");
     fclose(input);
     return 7;
  }

  expressionString[stringLength] = '\0';             // Terminate string with null
  fclose(input);
  return 0;

}

// PRINTf emulation
void printCR (HWND hwnd, UCHAR *this, int crlf) {

  static int lpos;

  if (hwnd != NULL) {

     lpos = SendMessage(hwnd, (UINT)WM_GETTEXTLENGTH, 0, 0);
     SendMessage(hwnd, (UINT)EM_SETSEL,     (WPARAM)lpos,  (LPARAM)lpos);
     SendMessage(hwnd, (UINT)EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)this);
     SendMessage(hwnd, (UINT)EM_SCROLLCARET, 0, 0);

     while ( crlf-- ) {
            lpos = SendMessage(hwnd, (UINT)WM_GETTEXTLENGTH, 0, 0);
            SendMessage(hwnd, (UINT)EM_SETSEL,     (WPARAM)lpos,  (LPARAM)lpos);
            SendMessage(hwnd, (UINT)EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)CRLF);
            SendMessage(hwnd, (UINT)EM_SCROLLCARET, 0, 0);
     }
  }
}

// number to string conversion
UCHAR *numStr( __int64 i) {

   static UCHAR a[I64_W + 1], *p, f;
   static UCHAR buf[100];

   f = 0;
   if (i < 0) { i = -i; f = 1; }
   a[I64_W] = '\0';
   p = a + I64_W - 1;

   if (i) {
            while (i) {
               *p-- = (UCHAR) (i % 10 + '0');
               i /= 10;
            }
            if (f)	*p = '-'; else	p++;
            memmove(buf, p, strlen(p) + 1);
   }
   else { buf[0] = '0'; buf[1]='\0'; }

   return buf;

}
