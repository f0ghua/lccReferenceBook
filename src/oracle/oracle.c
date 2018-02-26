
// JEP
// NAMEWARE
// Before using: Send a mail with your name, country and city to: jep-nware@altern.org

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include "oracleres.h"
#include <stdlib.h>
#include <time.h>

char *txt[]={
"YES",
"AS I SEE IT, YES",
"VERY DOUBTFUL",
"ASK AGAIN LATER",
"MY REPLY IS NO",
"REPLY HAZY",
"TRY AGAIN",
"OUTLOOK GOOD",
"MY SOURCES SAY NO",
"YOU MAY RELY ON IT",
"WITHOUT A DOUBT",
"CANNOT PREDICT NOW",
"MOST LIKELY",
"SIGNS POINT TO YES",
"CONCENTRATE AND ASK AGAIN",
"OUTLOOK NOT SO GOOD",
"IT IS DECIDEDLY SO",
"BETTER NOT TELL YOU NOW",
"YES - DEFINITELY",
"IT IS CERTAIN",
"DON'T COUNT ON IT"
};

static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

HBITMAP hBMP;
HINSTANCE hInst;
int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;

	MessageBox(NULL,"Every day, a hundred decisions...\t\nHere's something that might help.","JwH",MB_OK|MB_ICONEXCLAMATION);
	hInst=hinst;
	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc = DefDlgProc;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hinst;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszClassName = "oracle";
	RegisterClass(&wc);

	return DialogBox(hinst, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC) DialogFunc);
}

static int InitializeApp(HWND hDlg,WPARAM wParam, LPARAM lParam)
{
HRGN rgn;

    srand(time(NULL));
	rgn = CreateEllipticRgn(62,50,350,350);
	SetWindowRgn(hDlg, rgn, TRUE);
	hBMP = LoadBitmap(hInst,MAKEINTRESOURCE(IDD_BMP0));
    SendMessage(CreateWindow("static", "",
                WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_BITMAP , //| SS_NOTIFY,
                1,1,0,0,
                hDlg, (HMENU)555,
                hInst, NULL), STM_SETIMAGE, IMAGE_BITMAP, (LONG)hBMP);
	SetWindowText(hDlg,"Oracle");
	ShowWindow(GetDlgItem(hDlg,IDOK),SW_SHOW);
	return 1;
}


static BOOL CALLBACK DialogFunc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
#define NB_REP 21
#define ICONx1 193
#define ICONx2 223
#define ICONy1 318
#define ICONy2 331
#define SEUIL_MOVE 5
static int xPos = 0;
static int yPos = 0;
static unsigned long iMoving=0;
static unsigned long iLastVal=NB_REP+1;
RECT rect;

  switch(msg) {

	case WM_INITDIALOG:
		 InitializeApp(hDlg,wParam,lParam);
		 return TRUE;


	case WM_LBUTTONUP:
		 if(iMoving>SEUIL_MOVE) break;
 		 if((xPos>=ICONx1) && (xPos<=ICONx2) && (yPos>=ICONy1) && (yPos<=ICONy2)) {
		   if(xPos>=ICONx1+16) {
 		     EndDialog(hDlg,1);
		     DeleteObject(hBMP);
		     break;
		   } else MessageBox(hDlg,"Oracle 0.01\rSeptember 2000\rIdea from: www.quartus.net\r\rSpeak your question, then tap the ball to\nhave the Oracle provide words of wisdom !\t\r ","JwH",MB_OK|MB_ICONQUESTION);
		 } else {
		   unsigned long iVal;
		   SetWindowText(GetDlgItem(hDlg,IDTEXT),"...thinking");
		   Sleep(800);
		   SetWindowText(GetDlgItem(hDlg,IDTEXT),"sink in...");
		   Sleep(1000);
		   while((iVal=rand() % NB_REP)==iLastVal) ;
		   SetWindowText(GetDlgItem(hDlg,IDTEXT),txt[iVal]);
		   iLastVal=iVal;
		 }
         return TRUE;

    case WM_MOUSEMOVE:
         if(wParam & MK_LBUTTON) {
           GetWindowRect(hDlg, &rect);
           MoveWindow(hDlg, rect.left + LOWORD(lParam) - xPos, rect.top + HIWORD(lParam) - yPos , rect.right-rect.left, rect.bottom-rect.top, TRUE);
		   iMoving++;
         } else {
		   iMoving=0;
           xPos = LOWORD(lParam);
           yPos = HIWORD(lParam);
		   if((xPos>=ICONx1) && (xPos<=ICONx2) && (yPos>=ICONy1) && (yPos<=ICONy2)) {
		     SetCursor(LoadCursor(NULL,IDC_UPARROW));
		   } else {
		     SetCursor(LoadCursor(NULL,IDC_ARROW));
		   }
         }
         break;

	case WM_CLOSE:
    	 DeleteObject(hBMP);
		 EndDialog(hDlg,0);
		 return TRUE;

  }
  return FALSE;
}
