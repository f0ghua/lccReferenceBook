#include <windows.h>
#include <stdio.h>
#include "CoolMenu.h"
#include "resource.h"

char szAppName[] = "CoolMenu";
HINSTANCE hInstance;

VOID FontRotator(HWND hwnd);

VOID FontRotator(HWND hwnd)
{
	if ( !IsWindow(hwnd) ) { return; }
	HDC hdc = GetDC(hwnd);

	RECT rc;
	int angle;
	HFONT hfnt, hfntPrev;
	LPSTR lpszRotate = "String to be rotated...";

	TCHAR szString[MAX_PATH];

	// Allocate memory for a LOGFONT structure.
	PLOGFONT plf = (PLOGFONT) LocalAlloc(LPTR, sizeof(LOGFONT));

	// Specify a font typeface name and weight.
	lstrcpy(plf->lfFaceName, "Arial");
	plf->lfWeight = FW_NORMAL;

	// Retrieve the client-rectangle dimensions.
	GetClientRect(hwnd, &rc);

	// Set the background mode to transparent for the text-output operation.
	SetBkMode(hdc, TRANSPARENT);

	// Draw the string 36 times,
	// rotating 10 degrees counter-clockwise each time.

	for (angle = 0; angle < 3600; angle += 100)
	{
    	plf->lfEscapement = angle;

	    hfnt = CreateFontIndirect(plf);
	    hfntPrev = SelectObject(hdc, hfnt);

	    TextOut(hdc
				, rc.right / 2
				, rc.bottom / 2
				, szString
				, sprintf(szString,"%s - %d", lpszRotate, angle));

	    SelectObject(hdc, hfntPrev);
	    DeleteObject(hfnt);

	}

	// Reset the background mode to its default.
 	SetBkMode(hdc, OPAQUE);

	// Free the memory allocated for the LOGFONT structure.
	LocalFree((LOCALHANDLE) plf);

	ReleaseDC(hwnd, hdc);

}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	HMENU hmenu, hsub;

	switch(msg)
	{
	case WM_CREATE:

		hmenu = GetMenu(hwnd);

		hsub = GetSubMenu(hmenu, 0);
		CoolMenu_Create(hsub, szAppName);

		hsub = GetSubMenu(hmenu, 1);
		CoolMenu_Create(hsub, szAppName);

		return 0;

	case WM_MEASUREITEM :

		return CoolMenu_MeasureItem(wParam, lParam);

	case WM_DRAWITEM :

		return CoolMenu_DrawItem(wParam, lParam);

	case WM_COMMAND :
		{
			switch( LOWORD(wParam) )
			{
				case IDM_FILE_NEW :
				case IDM_FILE_OPEN :

					CoolMenu_SetEnabled(hwnd, IDM_FILE_PRINT);
					CoolMenu_SetEnabled(hwnd, IDM_FILE_PREVIEW);
					CoolMenu_SetEnabled(hwnd, IDM_FILE_SETUP);

					break;

				case IDM_TRACK_DOCK :

					CoolMenu_SetCheck(hwnd, IDM_TRACK_DOCK);

					break;

				case IDM_TRACK_AUTOHIDE :

					CoolMenu_SetCheck(hwnd, IDM_TRACK_AUTOHIDE);

					break;

				case IDM_MODE_COMPUTER :

					CoolMenu_SetCheck(hwnd, IDM_MODE_COMPUTER);

					break;

				case IDM_STD_INFO :
					CoolMenu_SetCheck(hwnd, IDM_STD_INFO);
					break;

				case IDM_TRACK_OPTIONS :
					// successful experiment
					FontRotator(hwnd);
					break;

				case IDM_TRACK_EXIT :
				case IDM_FILE_EXIT :
					SendMessage(hwnd, WM_DESTROY, 0, 0);
					break;

				default:
					break;
			}
		}
		break;

	case WM_CLOSE:

		DestroyWindow(hwnd);

		return 0;

	case WM_CONTEXTMENU :

		CoolMenu_PopupContext(hwnd, lParam, 1);

		return 0;

	case WM_DESTROY:

		CoolMenu_Cleanup(hwnd);

		PostQuitMessage(0);

		return 0;

	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);

}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{

	HWND		hwndMain;
	MSG			msg;

	hInstance = hInst;

	//
	//	Register our main window class
	//
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));

	wc.cbSize			= sizeof(wc);
	wc.style			= 0;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= LoadIcon   (NULL, IDI_APPLICATION);
	wc.hCursor			= LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE+1);

	wc.lpszMenuName		= MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName	= szAppName;
	wc.hIconSm			= LoadIcon (NULL, IDI_APPLICATION);

	RegisterClassEx(&wc);

	//
	//	Create the main window.
	//  This window will host two child controls.
	//
	hwndMain = CreateWindowEx(WS_EX_CLIENTEDGE,
		szAppName,				// window class name
		szAppName,				// window caption
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,		// window style
		CW_USEDEFAULT,			// initial x position
		CW_USEDEFAULT,			// initial y position
		CW_USEDEFAULT,			// initial x size
		CW_USEDEFAULT,			// initial y size
		NULL,					// parent window handle
		NULL,					// use window class menu
		hInst,					// program instance handle
		NULL);					// creation parameters

	ShowWindow(hwndMain, nShowCmd);

	while( GetMessage(&msg, NULL, 0, 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
