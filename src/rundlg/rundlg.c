//
// Demonstrates a dialog created at run time.
//
// John Findlay
//
//
#include <windows.h>
#include <windowsx.h>
#include "rundlgres.h"
#include "rundlg.h"

HINSTANCE hInst;		// Instance handle
HWND hwndMain;			// Main window handle

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

BOOL CALLBACK OwnerDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg){
	case WM_INITDIALOG:
		// Set various text's in controls
		SetWindowText(GetDlgItem(hwnd, IDOK), "Try Again");	// Set button text
		SetWindowText(GetDlgItem(hwnd, IDCANCEL), "Cancel");	// Set button text
		SetWindowText(hwnd, (LPSTR)lParam);				// Set Dialog title
		SetWindowText(GetDlgItem(hwnd, IDSTAT), "The Quick Brwon Fox, etc.");// Set button text
		return TRUE;

    case WM_COMMAND:{
		switch (LOWORD(wParam))
		case IDOK:
		case IDCANCEL:{
			return EndDialog(hwnd, LOWORD(wParam));

		}
		}
	}
	return FALSE;
}

int CreateOwnerDialog(HWND hwnd, LPSTR szTit)
{
	int ret;

	// Make dialog template containing three controls
 	DLGDATA dtp={

		// The dialog itself (n = number of controls)
		// style  ex  n  x  y  cx   cy   m  c  title[8]        pt  font[14]
		{DLGSTYLE, 0, 4, 0, 0, 178, 96}, 0, 0, L"Owner..", 8, L"MS Sans Serif",

		// The Pushbutton
		{{BUTSTYLE, 0, 113, 75, 50, 14, IDOK}, 0xFFFF, 0x0080, 0, 0, 0},

		// The Pushbutton
		{{BUTSTYLE, 0, 13, 75, 50, 14, IDCANCEL}, 0xFFFF, 0x0080, 0, 0, 0},

		// Big      ex  x   y   cx  cy id
		{{STATSTYLE, 0, 7, 7, 164, 52, IDSTAT}, 0xFFFF, 0x0082, 0, 0, 0},

		// Small     ex  x   y   cx  cy id
		{{STATSTYLE1, 0, 7, 62, 164, 1, 2}, 0xFFFF, 0x0082, 0, 0, 0}};

	ret = DialogBoxIndirectParam(hInst,(DLGTEMPLATE*)&dtp, hwnd,
	  							OwnerDialogProc, (LONG)szTit);
	if (ret == IDOK)
		MessageBox(0, "[Try Again] was selected", "Message", MB_OK);
	else if(ret == IDCANCEL)
		MessageBox(0, "[Cancel] was selected", "Message", MB_OK);

	return ret;

}

static BOOL InitApplication(void)
{
	WNDCLASS wc;

	memset(&wc,0,sizeof(WNDCLASS));
	wc.style 			= CS_HREDRAW|CS_VREDRAW |CS_DBLCLKS ;
	wc.lpfnWndProc 		= (WNDPROC)MainWndProc;
	wc.hInstance 		= hInst;
	wc.hbrBackground 	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName 	= "rundlgWndClass";
	wc.lpszMenuName 	= MAKEINTRESOURCE(IDMAINMENU);
	wc.hCursor 			= LoadCursor(NULL,IDC_ARROW);
	wc.hIcon 			= LoadIcon(NULL,IDI_APPLICATION);
	if (!RegisterClass(&wc))
		return 0;

	return 1;
}

void MainWndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id) {
	case IDM_OPEN:
		CreateOwnerDialog(hwnd, "Owner Created Dialog");
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
	HANDLE hAccelTable;

	hInst = hInstance;

	if (!InitApplication())
		return 0;

	hwndMain = CreateWindow("rundlgWndClass", "rundlg",
		WS_MINIMIZEBOX | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN| \
		WS_MAXIMIZEBOX | WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_THICKFRAME,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		NULL,
		NULL,
		hInst,
		NULL);

	if (hwndMain == (HWND)0)
		return 0;

	ShowWindow(hwndMain,SW_SHOW);

	while (GetMessage(&msg,NULL,0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;

}
