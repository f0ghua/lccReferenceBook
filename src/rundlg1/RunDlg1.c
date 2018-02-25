//
// RunDlg1.c
//
// Demonstrates a dialog created at run time.
//
// John Findlay
//
#include <windows.h>
#include <windowsx.h>
#include "rundlg1res.h"
#include "msgbox.h"

HINSTANCE 	hInst;				// Instance handle
HWND 		hwndMain;			// Main window handle

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

static BOOL InitApplication(void)
{
	WNDCLASS wc;

	memset(&wc, 0, sizeof(WNDCLASS));
	wc.style 			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
	wc.lpfnWndProc 		= (WNDPROC)MainWndProc;
	wc.hInstance 		= hInst;
	wc.hbrBackground 	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName 	= "rundlgWndClass";
	wc.lpszMenuName 	= MAKEINTRESOURCE(IDMAINMENU);
	wc.hCursor 			= LoadCursor(NULL, IDC_ARROW);
	wc.hIcon 			= LoadIcon(hInst, MAKEINTRESOURCE(ID_ICON));

	if (!RegisterClass(&wc))
		return 0;

	return 1;
}

void MainWndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id) {
	case IDM_OPEN:{
		int ret;
		ret = MsgBox(hInst, hwnd, "Owner Created Dialog", "There were no reported errors!");
		}
		break;

	case IDM_EXIT:
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		break;
	}
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {

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

	hInst = hInstance;

	if (!InitApplication())
		return 0;

	hwndMain = CreateWindow("rundlgWndClass", "Rundlg1",
		WS_MINIMIZEBOX | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN| \
		WS_MAXIMIZEBOX | WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_THICKFRAME,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		NULL, NULL, hInst, NULL);

	if (hwndMain == (HWND)0)
		return 0;

	ShowWindow(hwndMain, SW_SHOW);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;

}
