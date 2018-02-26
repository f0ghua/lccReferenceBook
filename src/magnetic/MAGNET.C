/*------------------------------------------------------------------------
 Module:        magnet.c
 Author:        tuukka.lehtinen@koti.soon.fi
 Project:       Magnet windows 1.0
 State:         Ready
 Creation Date: 1 Nov 2001
 Description:   I like to WinAmp style to move window. When
                you move window near of edge, it will snap.
------------------------------------------------------------------------*/
#include <windows.h>

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow);
LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
int MagnetWindow(LPARAM *lParam);

// #pragma argsused, will stop BCC to warning never used arguments
#pragma argsused
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	MSG msg;
	WNDCLASS wc;
	HWND hwnd;
	CONST CHAR szAppName[]="Magnet";

	memset(&wc,0,sizeof(WNDCLASS));
	wc.style = CS_HREDRAW|CS_VREDRAW |CS_DBLCLKS ;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = szAppName;
	wc.lpszMenuName = NULL;;
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	if (!RegisterClass(&wc))
		return 0;

	hwnd = CreateWindow(szAppName,szAppName, WS_OVERLAPPEDWINDOW,
		200,200,200,200,
		NULL,NULL,hInstance, NULL);
	if(hwnd == NULL) return 0;

	ShowWindow(hwnd,SW_SHOW);

	while (GetMessage(&msg,NULL,0,0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg) {

	case WM_MOVING:
		return MagnetWindow(&lParam);

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}

/*------------------------------------------------------------------------
 Procedure:     MagnetWindow ID:1
 Purpose:       Make windows magnetic
 Input:         case WM_MOVING:
                return MagnetWindow(wParam,lParam);
 Output:        Change windows coordinates.
 Errors:        If the function succeeds, the return value is true.
                If the function fails, the return value is false.
 Notes:			This function can only handle single window.
------------------------------------------------------------------------*/
BOOL MagnetWindow(LPARAM *lParam)
{
	RECT *pRect = (LPRECT) *lParam;	// screen coordinates
	static RECT rcOld = { 0, 0, 0, 0 };
	RECT rcDesktop;					// dimension of desktop work area
	INT	 iPower = 20;	// how long distance windows snap (in pixels)
	SIZE szSize;		// size of window

	// get the desktop work area dimension
	if(FALSE == SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0))
		return FALSE;

	szSize.cx = pRect->right -  pRect->left;
	szSize.cy = pRect->bottom - pRect->top;


	if(pRect->top < iPower && rcOld.top > pRect->top)
	{
		pRect->top = 0;
		pRect->bottom = szSize.cy;
	}
	else
	if(pRect->bottom > (rcDesktop.bottom - iPower) &&
	   rcOld.bottom  < pRect->bottom)
	{
		pRect->bottom = rcDesktop.bottom;
		pRect->top = rcDesktop.bottom - szSize.cy;
	}

	if(pRect->left < iPower && rcOld.left > pRect->left)
	{
		pRect->left = 0;
		pRect->right = szSize.cx;
	}
	else
	if(pRect->right > (rcDesktop.right - iPower) &&
		rcOld.right < pRect-> right)
	{
		pRect->right = rcDesktop.right;
		pRect->left =  rcDesktop.right - szSize.cx;
	}

	rcOld.top = pRect -> top;
	rcOld.bottom = pRect -> bottom;
	rcOld.left = pRect -> left;
	rcOld.right = pRect -> right;

	return TRUE;
}
