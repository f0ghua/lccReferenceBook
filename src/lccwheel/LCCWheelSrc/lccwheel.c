/*--------------------------------------------------------
   LCCWHELL.C -- Add WM_MOUSEWHEEL functionality to wedit (LCC-Win32 ide)
                 (c) Luca, 2001
  --------------------------------------------------------*/

// Link with shell32.lib

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include "resource.h"

#define WM_NOTIFYICON	WM_USER+1

HANDLE 			g_hInst;
unsigned int	g_nLNumber = 5;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID APIENTRY DisplayContextMenu(HWND, POINT, HMENU);
BOOL CALLBACK ConfigProc(HWND, UINT, WPARAM, LPARAM);
BOOL TaskBarAddIcon(HWND, UINT, HICON, LPSTR);
BOOL TaskBarDeleteIcon(HWND, UINT);
BOOL TaskBarModifyIcon (HWND, UINT, HICON, LPSTR);

/************************************
* WinMain: window entry point
*************************************/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    static char				szAppName[] = "LCCWheel";
    HWND					hWnd;
    MSG						msg;
    WNDCLASSEX				wndclassex;
	INITCOMMONCONTROLSEX	InitCtrls;

	memset(&InitCtrls,0,sizeof(InitCtrls));
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_UPDOWN_CLASS;

	InitCommonControlsEx(&InitCtrls);

	wndclassex.cbSize        = sizeof(WNDCLASSEX);
	wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc   = (WNDPROC)WndProc;
	wndclassex.cbClsExtra    = 0;
	wndclassex.cbWndExtra    = 0;
	wndclassex.hInstance     = hInstance;
	wndclassex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOUSEON));
	wndclassex.hIconSm       = LoadImage(hInstance, MAKEINTRESOURCE(IDI_MOUSEON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	wndclassex.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wndclassex.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
	wndclassex.lpszMenuName  = NULL;
	wndclassex.lpszClassName = szAppName;

	RegisterClassEx (&wndclassex);

	g_hInst = hInstance;

	hWnd = CreateWindowEx (0,
				szAppName,
				szAppName,
				WS_BORDER | WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				320,
				200,
				NULL,
				NULL,
				hInstance,
				NULL);

	ShowWindow(hWnd, SW_HIDE);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

/************************************
* WndProc
*************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static FARPROC		StartHook, StopHook;
	static HINSTANCE	hDLL;
	static HMENU		hTopMenu;
	HICON				hIcon;
	HMENU				hSubMenu;
	POINT				point;
	UINT				uMenuState;
	char				cBuff[3];

	switch(message)
	{
		case WM_CREATE:

			hIcon = LoadImage(g_hInst, MAKEINTRESOURCE(IDI_MOUSEON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
			TaskBarAddIcon(hWnd, IDI_TNAICON, hIcon, "LCCWheel - Enabled");
			DestroyIcon(hIcon);

			hTopMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDM_POPUP));
			hSubMenu = GetSubMenu(hTopMenu, 0);
			SetMenuDefaultItem(hSubMenu, 0, TRUE);

			hDLL = LoadLibrary("wheeldll.dll");
			StartHook = GetProcAddress(hDLL, (LPCSTR)"StartHook");
			StopHook = GetProcAddress(hDLL, (LPCSTR)"StopHook");

			g_nLNumber = GetPrivateProfileInt("SETTINGS", "LinesToScroll", 5, "LCCWheel.ini");

			StartHook();

			return 0;

	    case WM_NOTIFYICON:

			switch (lParam)
			{
				case WM_RBUTTONUP:
				case WM_NCRBUTTONUP:

					GetCursorPos(&point);

					DisplayContextMenu(hWnd, point, hTopMenu);

					return 0;

				case WM_LBUTTONDBLCLK:

					hSubMenu = GetSubMenu(hTopMenu, 0);
					uMenuState = GetMenuState(hSubMenu, IDC_CONFIGURE, MF_BYCOMMAND);

					if (!(uMenuState & MF_GRAYED))
					{
						SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_CONFIGURE, 0), (LPARAM)NULL);
					}

					return 0;
			}
			return 0;

		case WM_COMMAND:

			switch (wParam)
			{
				case IDC_EXIT:

					SendMessage(hWnd, WM_CLOSE, (WPARAM)0, (LPARAM)0);
					return 0;

				case IDC_ABOUT:

					hSubMenu = GetSubMenu(hTopMenu, 0);
					EnableMenuItem(hSubMenu, IDC_ABOUT, MF_BYCOMMAND | MF_GRAYED);
					ShellAbout(hWnd, "", "LCCWheel v1.0 - Freeware\r\nCopyright (C) 2001 by Luca", LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MOUSEON)));
					EnableMenuItem(hSubMenu, IDC_ABOUT, MF_BYCOMMAND | MF_ENABLED);
					return 0;

				case IDC_ENABLED:

					hSubMenu = GetSubMenu(hTopMenu, 0);
					uMenuState = GetMenuState(hSubMenu, IDC_ENABLED, MF_BYCOMMAND);

					if (uMenuState & MF_CHECKED)
					{
						CheckMenuItem(hSubMenu, IDC_ENABLED, MF_BYCOMMAND | MF_UNCHECKED);

						hIcon = LoadImage(g_hInst, MAKEINTRESOURCE(IDI_MOUSEOFF), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
						TaskBarModifyIcon(hWnd, IDI_TNAICON, hIcon, "LCCWheel - Disabled");
						DestroyIcon(hIcon);

						StopHook();
					}
					else
					{
						CheckMenuItem(hSubMenu, IDC_ENABLED, MF_BYCOMMAND | MF_CHECKED);

						hIcon = LoadImage(g_hInst, MAKEINTRESOURCE(IDI_MOUSEON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
						TaskBarModifyIcon(hWnd, IDI_TNAICON, hIcon, "LCCWheel - Enabled");
						DestroyIcon(hIcon);

						StartHook();
					}
					return 0;

				case IDC_CONFIGURE:

					hSubMenu = GetSubMenu(hTopMenu, 0);
					EnableMenuItem(hSubMenu, IDC_CONFIGURE, MF_BYCOMMAND | MF_GRAYED);
					if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONFIG), hWnd, ConfigProc))
					{
						itoa(g_nLNumber, cBuff, 10);
						WritePrivateProfileString("SETTINGS", "LinesToScroll", cBuff, "LCCWheel.ini");
						/* flush cache? */
						WritePrivateProfileString(NULL, NULL, NULL, "LCCWheel.ini");
						StopHook();
						StartHook();
					}
					EnableMenuItem(hSubMenu, IDC_CONFIGURE, MF_BYCOMMAND | MF_ENABLED);
					return 0;
			}

			return 0;

		case WM_DESTROY:

			TaskBarDeleteIcon(hWnd, IDI_TNAICON);

			DestroyMenu(hTopMenu);

			StopHook();

			FreeLibrary(hDLL);
			PostQuitMessage(0);

			return 0;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

/************************************
* ConfigProc: dialog proc for configure dialog box
*************************************/
BOOL CALLBACK ConfigProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	BOOL	bRes;

	switch (uMsg)
	{
		case WM_INITDIALOG:

			SendDlgItemMessage(hDlg, IDC_SPIN, UDM_SETBUDDY, (WPARAM)(HWND)GetDlgItem(hDlg, IDC_LNUMBER), (LPARAM)0);
			SendDlgItemMessage(hDlg, IDC_SPIN, UDM_SETRANGE, (WPARAM)0, (LPARAM)MAKELONG((short)15, (short)1));
			SendDlgItemMessage(hDlg, IDC_SPIN, UDM_SETPOS, (WPARAM)0, (LPARAM)MAKELONG((short)g_nLNumber, 0));

			SetForegroundWindow(hDlg);
			return TRUE;

		case WM_DESTROY:

			return FALSE;

		case WM_SYSCOMMAND:

			if (wParam == SC_CLOSE)
			{
				EndDialog(hDlg,0);
				return FALSE;
			}
			return FALSE;

		case WM_COMMAND:

			switch (LOWORD(wParam))
			{
				case IDOK:

					g_nLNumber = GetDlgItemInt(hDlg, IDC_LNUMBER, &bRes, FALSE);
					if (!bRes)
						g_nLNumber = 5;

					EndDialog(hDlg,1);
					return FALSE;

				case IDCANCEL:

					EndDialog(hDlg,0);
					return FALSE;
			}
	}
	return FALSE;
}

/************************************
* DisplayContextMenu: cut'n'paste from WIN32 SDK docs
*************************************/
VOID APIENTRY DisplayContextMenu(HWND hWnd, POINT pt, HMENU hTopMenu)
{
	HMENU hPopup;

	hPopup = GetSubMenu(hTopMenu, 0);

	SetForegroundWindow(hWnd);

	TrackPopupMenu(hPopup, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);

	PostMessage(hWnd, WM_USER, 0, 0);

}

/************************************
* TaskBarAddIcon: adds an icon to the TNA
*************************************/
BOOL TaskBarAddIcon(HWND hWnd, UINT uID, HICON hIcon, LPSTR lpszTip)
{
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = hWnd;
	tnid.uID = uID;
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnid.uCallbackMessage = WM_NOTIFYICON;
	tnid.hIcon = hIcon;
	if (lpszTip)
		lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
	else
		tnid.szTip[0] = '\0';

	return Shell_NotifyIcon(NIM_ADD, &tnid);
}

/************************************
* TaskBarDeleteIcon: deletes an icon from the taskbar
*************************************/
BOOL TaskBarDeleteIcon(HWND hWnd, UINT uID)
{
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = hWnd;
	tnid.uID = uID;

	return Shell_NotifyIcon(NIM_DELETE, &tnid);
}

/************************************
* TaskBarModifyIcon: modify the icon in the TNA
*************************************/
BOOL TaskBarModifyIcon (HWND hWnd, UINT uID, HICON hIcon, LPSTR lpszTip)
{
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = hWnd;
	tnid.uID = uID;
	tnid.uFlags = 0;
	if (hIcon)
	{
		tnid.uFlags |= NIF_ICON;
		tnid.hIcon = hIcon;
	}
	if (lpszTip)
	{
		tnid.uFlags |= NIF_TIP;
		lstrcpyn (tnid.szTip, lpszTip, sizeof(tnid.szTip));
	}
	return Shell_NotifyIcon (NIM_MODIFY, &tnid);
}
