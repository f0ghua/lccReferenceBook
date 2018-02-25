/*****************************************************************
	GenMDI -
		A very very very simple MDI example that uses edit controls.
	  There are alot of things that can be improved, added on.
*****************************************************************/
#include <windows.h>
#include <stdio.h>
#include "GenMDI.h"

LRESULT CALLBACK FrameWndProc (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK ChildWndProc (HWND, UINT, WPARAM, LPARAM) ;

char szFrameClass[] = "TestMDI" ;
char szChildClass[] = "MDIChild" ;
HINSTANCE hInst;
HMENU hmenu;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	HWND        hwndFrame, hwndClient;
	MSG         msg ;
	WNDCLASSEX  wc ;

	hInst = hInstance;

	wc.cbSize        = sizeof (wc) ;
	wc.style         = CS_HREDRAW | CS_VREDRAW ;
	wc.lpfnWndProc   = FrameWndProc ;
	wc.cbClsExtra    = 0 ;
	wc.cbWndExtra    = 0 ;
	wc.hInstance     = hInstance ;
	wc.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wc.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE ;
	wc.lpszMenuName  = NULL ;
	wc.lpszClassName = szFrameClass ;
	wc.hIconSm       = LoadIcon (NULL, IDI_APPLICATION) ;

	RegisterClassEx(&wc);

	wc.lpfnWndProc   = ChildWndProc;
    wc.hIcon	     = LoadIcon(NULL, IDI_APPLICATION);
    wc.lpszMenuName  = NULL;
/*
 *  The extra bytes will be used to hold the HWND of an edit control,
 *  and other info.
 */ wc.cbWndExtra    = CBWNDEXTRA;
    wc.lpszClassName = szChildClass;

	RegisterClassEx (&wc);

	hmenu = LoadMenu(hInstance, "GenMenu");

	hwndFrame = CreateWindow (szFrameClass, szFrameClass, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, NULL, hmenu, hInstance, NULL) ;

	hwndClient = GetWindow(hwndFrame, GW_CHILD);

	ShowWindow (hwndFrame, iCmdShow) ;
	UpdateWindow (hwndFrame) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	DestroyMenu(hmenu);
	return msg.wParam ;
}

LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND			hwndClient;
	CLIENTCREATESTRUCT	ccs;
	HWND				hwndChild;
	MDICREATESTRUCT		mcs;

	switch (iMsg)
	{
		case WM_CREATE :
			ccs.hWindowMenu  = GetSubMenu(hmenu, 2);
			ccs.idFirstChild = IDM_FIRSTCHILD;

			hwndClient = CreateWindow("MDICLIENT", NULL, WS_CHILD | WS_CLIPCHILDREN |
									  WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1, hInst,
									  (LPSTR)&ccs);
			return 0 ;

		case WM_COMMAND:
			switch(wParam)
			{	/*
				 * Create MDI child window.
			     */
				case IDM_OPEN:
				case IDM_NEW:
					mcs.szTitle = "Untitled";
 					mcs.szClass = szChildClass;
    				mcs.hOwner  = hInst;
					mcs.x       = CW_USEDEFAULT;
					mcs.cx      = CW_USEDEFAULT;
    				mcs.y       = CW_USEDEFAULT;
					mcs.cy      = CW_USEDEFAULT;
					mcs.style   = 0;

    				hwndChild = (HWND)SendMessage (hwndClient, WM_MDICREATE, 0,
			      					 (LPARAM)(LPMDICREATESTRUCT)&mcs);
					return 0;

				case IDM_TILE:
					SendMessage(hwndClient, WM_MDITILE, 0, 0);
					return 0;

				case IDM_CASCADE:
					SendMessage(hwndClient, WM_MDICASCADE, 0, 0);
					return 0;

				case IDM_ARRANGE:
					SendMessage(hwndClient, WM_MDIICONARRANGE, 0, 0);
					return 0;

				default:
				/* All unprocessed WM_COMMAND messages
				 * must be passed to DefFrameProc()
				 */
					break;
			}
			break;

		case WM_DESTROY :
			PostQuitMessage (0) ;
			return 0 ;
	}
	return DefFrameProc (hwnd, hwndClient, iMsg, wParam, lParam) ;
}

LRESULT CALLBACK ChildWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND 			hwndEdit;

	switch(msg)
	{
		case WM_CREATE:
	    	/* Create an edit control over the MDI child.
			 */
	    	hwndEdit = CreateWindow ("edit", NULL, WS_CHILD | WS_HSCROLL | WS_MAXIMIZE |
					     WS_VISIBLE | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL |
					     ES_MULTILINE, 0, 0, 0, 0, hwnd, (HMENU)ID_EDIT, hInst,
				    	 NULL);

	    	/* Remember the edit control's window handle,
			 * and set the edit changed flag to 0.
			 * You can use this to see if you can close
			 * the child window.
			 */
	    	SetWindowLong (hwnd, GWL_HWNDEDIT, (LONG)hwndEdit);
			SetWindowLong (hwnd, GWL_EDITCHANGED, 0);

			SetFocus (hwndEdit);
	    	return 0;

		case WM_SIZE:
			/* Move the edit control to MDI child's size.
			 */
			hwndEdit = (HWND)GetWindowLong (hwnd, GWL_HWNDEDIT);
	    	if(!MoveWindow (hwndEdit, 0, 0, LOWORD(lParam),
						    HIWORD(lParam), TRUE))
					MessageBox(NULL,"Could not move window", NULL, 0);
			break;/* DefMDIChildProc must process WM_SIZE.
			       */

		case WM_COMMAND:
			hwndEdit = (HWND)GetWindowLong (hwnd, GWL_HWNDEDIT);
			if(lParam && LOWORD (wParam) == ID_EDIT)
            {
            	switch (HIWORD(wParam))
                {
                	case EN_UPDATE:
					case EN_CHANGE:
						/*
						 * Edit control's contents changed.
						 */
						SetWindowLong (hwnd, GWL_EDITCHANGED, 1);
						return 0 ;
				}
			}
			/* Paste, cut, undo etc. here.
			 * SendMessage(hwndEdit, EM_xx, , );
			 */
			return 0;
		case WM_SETFOCUS:
			SetFocus((HWND)GetWindowLong (hwnd, GWL_HWNDEDIT));
			return 0;

	}
	return DefMDIChildProc(hwnd, msg, wParam, lParam);
}
