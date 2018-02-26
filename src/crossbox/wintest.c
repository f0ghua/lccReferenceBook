
#include <windows.h>
#include <stdio.h>
#include "crossbox.h"
//project files: crossbox.c, wintest.rc, wintest.c

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpszCmdLine,int nCmdShow )
{
	MSG  msg;
	HWND hWnd;
	WNDCLASS 	wndClass;


	wndClass.style         = CS_OWNDC;
	wndClass.lpfnWndProc   = WndProc;
	wndClass.cbClsExtra    = 0;
	wndClass.cbWndExtra    = 0;
	wndClass.hInstance     = hInstance;
	wndClass.hIcon         = NULL;
	wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName  = MAKEINTRESOURCE(1000);
	wndClass.lpszClassName = "wintest";
	if (!RegisterClass(&wndClass))	return FALSE;

	hWnd = CreateWindow("wintest",
		"CrossBox",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE
		    |	WS_CLIPSIBLINGS  | WS_CLIPCHILDREN,
		(GetSystemMetrics(SM_CXFULLSCREEN)-300)/2,
		(GetSystemMetrics(SM_CYFULLSCREEN)-300)/2,
		300,300,
		NULL,NULL,hInstance,NULL);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg );
		DispatchMessage(&msg );
	}
	return msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static PAINTSTRUCT ps;
	static HDC hdc;
	static int width, height;
	static DRAGBOX db;
	static CROSSHAIR ch;
	POINT pMouse;
	char buf[100];
	char lcc[]="lcc-win32, clickNdrag, lcc-win32, clickNdrag, lcc-win32";


	switch(message) {

	default:
		return DefWindowProc(hWnd,message,wParam,lParam);

	case WM_CREATE:
		hdc = GetDC(hWnd);
		init_crossbox(hdc, &ch, &db, RGB(0,0,200));
		return 0;


	case WM_DESTROY:
		cleanup_crossbox(&ch, &db);
		ReleaseDC(hdc, hWnd);
		PostQuitMessage(0);
		return 0;


	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		SelectObject(hdc,GetStockObject(ANSI_VAR_FONT));
		for(int i=0; i<13; i++)
			TextOut(ps.hdc,0,i*20,lcc,strlen(lcc));
		EndPaint(hWnd, &ps);
		return 0;



	case WM_LBUTTONDOWN:
		pMouse.x=LOWORD(lParam);
		pMouse.y=HIWORD(lParam);
		SetCapture(hWnd);
		lbutdown_crossbox(hdc, pMouse, &ch, &db);
		return 0;


	case WM_LBUTTONUP:
		pMouse.x=LOWORD(lParam);
		pMouse.y=HIWORD(lParam);
		lbutup_crossbox(hdc, pMouse, &db, &ch);
		ReleaseCapture();
		sprintf(buf,"%d,%d,%d,%d", db.pStart.x, db.pStart.y, db.pEnd.x, db.pEnd.y);
		SetWindowText(hWnd,buf);
		return 0;


	case WM_MOUSEMOVE:
		pMouse.x=LOWORD(lParam);
		pMouse.y=HIWORD(lParam);
		mousemove_crossbox(hdc, pMouse, &ch, &db, wParam);
		return 0;


	case WM_NCMOUSEMOVE:
		ncmousemove_crossbox(hdc, &ch);
		return 0;


	case WM_COMMAND:
		ch.showcross=!ch.showcross;
		InvalidateRect(hWnd,NULL,TRUE);
		return 0;


	case WM_SIZE:
		width	=	LOWORD(lParam);
		height	=	HIWORD(lParam);
		return 0;

	} // end of switch(message)

} // end of WndProc}

