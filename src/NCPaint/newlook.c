#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <commctrl.h>
#include <string.h>
#include "newlookres.h"

int InitNewLook(HWND hwnd); // in ncpaint.c


HINSTANCE hInst;		// Instance handle
HWND hwndMain;		    // Main window handle

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

static BOOL InitApplication(void)
{
	WNDCLASS wc;

	memset(&wc,0,sizeof(WNDCLASS));
	wc.style 			= CS_HREDRAW|CS_VREDRAW |CS_DBLCLKS ;
	wc.lpfnWndProc 		= (WNDPROC)MainWndProc;
	wc.hInstance 		= hInst;
	wc.hbrBackground 	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName 	= "newlookWndClass";
	wc.lpszMenuName 	= MAKEINTRESOURCE(IDMAINMENU);
	wc.hCursor 			= LoadCursor(NULL,IDC_ARROW);
	wc.hIcon 			= LoadIcon(hInst,MAKEINTRESOURCE(1));

	if (!RegisterClass(&wc))
		return 0;

	return 1;
}

HWND CreatenewlookWndClassWnd(void)
{
	return CreateWindow("newlookWndClass","NewLook",
		WS_MINIMIZEBOX|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME,
		//WS_OVERLAPPEDWINDOW,
		40, 40, 500, 300,
		NULL,
		NULL,
		hInst,
		NULL);
}

void MainWndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id) {
	case IDM_EXIT:
		PostMessage(hwnd,WM_CLOSE,0,0);
		break;
	}
}

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE:
		// InitNewLook() subclasses this window and takes over the
		// handling of various messages. The messages that are not
		// handled are passed on to this PROC.
		InitNewLook(hwnd);
		return 0;

	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd, wParam, lParam, MainWndProc_OnCommand);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	MSG msg;
	HANDLE hAccelTable;

	hInst = hInstance;
	if (!InitApplication())
		return 0;

	hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDACCEL));

	if ((hwndMain = CreatenewlookWndClassWnd()) == (HWND)0)
		return 0;

	ShowWindow(hwndMain, SW_SHOW);

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}
