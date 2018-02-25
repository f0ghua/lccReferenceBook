/*
Crypto v2.0
Written by Ken
mailto: kayan_azimov@mail.ru
http://www.kayan76.f2s.com
Baku, Azerbaijan, Oct 2001.

Features:
This program will help you to encode
small texts using simple algorithm,
so do not relay to it too much as it
will be easy for hackesrs to encode it ;-)
*/

#include <windows.h>

#define ID_BUTTON1    1
#define ID_BUTTON2    2
#define ID_EDIT_TEXT  3
#define ID_EDIT_PASS  4


HINSTANCE hInst;
HWND hwndBtn1, hwndBtn2, hwndEditText, hwndEditPass;
HFONT hFont;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

void Encrypt()
{
	char text[10000], pass[100];
	int i, x, ltext, lpass;

	x = 0;

	ltext = GetWindowTextLength(hwndEditText)+1;
	lpass=GetWindowTextLength(hwndEditPass)+1;

	GetWindowText(hwndEditText,text,ltext);
	GetWindowText(hwndEditPass,pass,lpass);

	for (i=0; i <= ltext-2; ++i)
	{
	    text[i]=(text[i]+pass[x]);
	    x++;
	    if (x==(lpass-1))
		x=0;
	}
	SetWindowText(hwndEditText,text);
}

void Decrypt()
{
	char text[10000], pass[100];
	int i, x, ltext, lpass;

	x = 0;

	ltext = GetWindowTextLength(hwndEditText)+1;
	lpass = GetWindowTextLength(hwndEditPass)+1;

	GetWindowText(hwndEditText,text,ltext);
	GetWindowText(hwndEditPass,pass,lpass);

	for (i=0; i <= ltext-2; ++i)
	{
	    text[i]=(text[i]-pass[x]);
	    x++;
	    if (x==(lpass-1))
		x=0;
	}
	SetWindowText(hwndEditText,text);
}


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{

     char szAppName[] = "Crypto" ;
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;
	 hFont 		= GetStockObject(ANSI_VAR_FONT);

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) (COLOR_3DFACE+1);
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("Error while registering new class!"),
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }

     hInst  = hInstance;
     hwnd = CreateWindow (szAppName, TEXT ("Crypto v2.0        -==by Ken==-"),
                          WS_CAPTION |WS_MINIMIZEBOX|WS_SYSMENU	,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          318, 380,
                          NULL, NULL, hInstance, NULL) ;



     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     HDC         hdc ;
     PAINTSTRUCT ps ;

     switch (message)
     {
     case WM_CREATE:{

          hwndEditPass = CreateWindow ("edit", NULL,
                         WS_CHILD | WS_VISIBLE |
                                   WS_BORDER | ES_LEFT  | ES_PASSWORD	|
                                   ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                         180, 25, 120, 20, hwnd, (HMENU) ID_EDIT_PASS,
                         hInst, NULL) ;

		 SendMessage(hwndEditPass, WM_SETFONT, (WPARAM)hFont, 0);


         hwndEditText = CreateWindow ("edit", NULL,
                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                                   WS_BORDER | ES_LEFT | ES_MULTILINE |
                                   ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                         5, 50, 300, 300, hwnd, (HMENU) ID_EDIT_TEXT,
                         hInst, NULL) ;

         SendMessage(hwndEditText, WM_SETFONT, (WPARAM)hFont, 0);

		 hwndBtn1=CreateWindow("button",
                             "Encrypt",
                              WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                              10, 10, 60, 30,


							  hwnd, (HMENU)ID_BUTTON1,
                              hInst, NULL) ;

         SendMessage(hwndBtn1, WM_SETFONT, (WPARAM)hFont, 0);

		 hwndBtn2=CreateWindow("button",
                             "Decrypt",
                              WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                              80, 10, 60, 30,
                              hwnd, (HMENU)ID_BUTTON2,
                              hInst, NULL) ;

         SendMessage(hwndBtn2, WM_SETFONT, (WPARAM)hFont, 0);
		}
          return 0 ;

	 case WM_COMMAND:

		 if (LOWORD (wParam) == ID_BUTTON1)
			  if (HIWORD (wParam) == BN_CLICKED)
				  Encrypt();


		 if (LOWORD (wParam) == ID_BUTTON2)
			 if (HIWORD (wParam) == BN_CLICKED)
				 Decrypt();

     case WM_PAINT:
		 {
          hdc = BeginPaint (hwnd, &ps) ;
		  SetBkColor(hdc, GetSysColor(COLOR_3DFACE));

		  HANDLE hOldFont = SelectObject(hdc, hFont);

          TextOut (hdc, 205, 5, "Password", lstrlen ("Password")) ;

		  SelectObject(hdc, hOldFont);
          }

          EndPaint (hwnd, &ps) ;
          return 0 ;

     case WM_DESTROY:
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}

