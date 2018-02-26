/*------------------------------------------------------------------------
Module:        DJ.C
Author:        tuukka.lehtinen@koti.soon.fi
Project:       Drive Jump
State:         Ready
Creation Date: 29 Oct 2001
Description:   Drive Jump is pop-up drive browser. Right click the
			   program icon in the system tray and browse
               your drives. Required Windows 95, OEM Service Release 2
			   or higer OS.
------------------------------------------------------------------------*/
#include <windows.h>  // basic windows stuff
#include <shellapi.h> // for access to system tray
#include "dj.h"

/*------------------------------------------------------------------------
* Defines and consts
*----------------------------------------------------------------------*/
#define WM_SYSTEMTRAY WM_USER
#define ID_EXIT 	50

const char szAppName[]="Drive Jump";

/*------------------------------------------------------------------------
* Globlas
*----------------------------------------------------------------------*/
HINSTANCE gInst;

/*------------------------------------------------------------------------
* Function prototypes
*----------------------------------------------------------------------*/
INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT);
LRESULT CALLBACK wndProc(HWND,UINT , WPARAM, LPARAM);
HMENU BuildMenu(VOID);
DWORD MegFromInt64(__int64 i);
VOID Error(VOID);

// #pragma argsused, will stop BCC to warning never used arguments
#pragma argsused
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
{
	HWND hwnd;
	WNDCLASSEX wc;
	MSG msg;
	NOTIFYICONDATA nid;

	// Create the window class for the main window.
	ZeroMemory(&wc,sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = wndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
		return FALSE;

	gInst = hInstance;

	// Create the hidden window.
	hwnd = CreateWindowEx(0, szAppName, szAppName, WS_POPUP, CW_USEDEFAULT,
		0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if(hwnd == NULL)
		return FALSE;

	// Fill out NOTIFYICONDATA structure for system tray icon
	ZeroMemory(&nid,sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = WM_SYSTEMTRAY; // message sent to nid.hWnd
	nid.hIcon = LoadImage(hInstance, MAKEINTRESOURCE(IDICONSM), IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	lstrcpy(nid.szTip, szAppName);
	if(!Shell_NotifyIcon(NIM_ADD, &nid))
		return FALSE;

	// Start the main message loop.
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HMENU menu;
	POINT pt;
	CHAR  drive[]="C:\\";
	BOOL  bool;
	NOTIFYICONDATA nid;
	WORD  item;

	switch (message) {

	case WM_SYSTEMTRAY:
		switch (lParam) {
		case WM_RBUTTONUP:
			bool = GetCursorPos(&pt);
			if(bool == FALSE) Error();
			bool = SetForegroundWindow(hwnd);
			if(bool == FALSE) Error();
			menu = BuildMenu();
			if(menu == NULL) Error();
			TrackPopupMenu(menu,TPM_CENTERALIGN, pt.x, pt.y,0, hwnd, NULL);
			if(menu == FALSE) Error();
			bool = PostMessage(hwnd, 0, 0, 0);
			if(bool == FALSE) Error();
			bool = DestroyMenu(menu);
			if(bool == FALSE) Error();
			break;
		}
		return TRUE;

	case WM_COMMAND:
		item = LOWORD(wParam);
		// If item identifier is between 100 - 126 user is clicked drive
		// letter.
		if(item > 99 && item < 127)
		{
			drive[0] = (CHAR) (item - 35);
			ShellExecute(hwnd, "open", drive, NULL, NULL,SW_SHOWDEFAULT);
		}
		if(item !=  ID_EXIT) break;

	case WM_DESTROY:
		// Windows don't remove icon from system tray if application
		// quit, it only check if app exist when mouse is over icon.
		ZeroMemory(&nid,sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		nid.uID = 1;
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		return TRUE;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


/*------------------------------------------------------------------------
 Procedure:     BuildMenu ID:1
 Purpose:       Create pop-up menu. Create menu item's by drives. If
                drive is fixed (HD) or ramdrive its show how many MB
                it available. Else it only show drive letter.
 Input:         None
 Output:        Handle of menu
 Errors:        Return NULL if someting fanny happend.
------------------------------------------------------------------------*/
HMENU BuildMenu(VOID)
{
	HMENU menu;
	DWORD drives;
	CHAR counter;
	char buf[20];
	__int64 lpFreeBytesAvailableToCaller;
	__int64 lpTotalNumberOfBytes;
	__int64 lpTotalNumberOfFreeBytes;
	char drive[]="X:\\";
	unsigned int type;
	BOOL bool;

	menu = CreatePopupMenu();

	// returns a bitmask
	drives = GetLogicalDrives();

	for(counter=0; counter<26; ++counter)
	{
		if(drives & 1)
		{
			drive[0] = (CHAR) (65+counter);
			type = GetDriveType(drive);

			if(type == DRIVE_FIXED || type == DRIVE_RAMDISK)
			{
#ifdef __LCC__
				if(FALSE == GetDiskFreeSpaceEx(drive, &lpFreeBytesAvailableToCaller,
					&lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes))
				return NULL;
#else
				if(FALSE == GetDiskFreeSpaceEx(drive, (PULARGE_INTEGER) &lpFreeBytesAvailableToCaller,
					(PULARGE_INTEGER) &lpTotalNumberOfBytes,(PULARGE_INTEGER) &lpTotalNumberOfFreeBytes))
				return NULL;
#endif
				wsprintf(buf,"%c:\t%d MB",
					65+counter, MegFromInt64(lpFreeBytesAvailableToCaller));
			}
			else
			{
				wsprintf(buf,"%c:",65+counter);
			}

			bool = InsertMenu(menu, 0xFFFFFFFF,MF_BYPOSITION | MF_STRING, 100+counter, buf);
			if(bool == FALSE) return NULL;

		}
		drives=drives>>1;
	}

	bool = InsertMenu(menu, 0xFFFFFFFF,MF_BYPOSITION | MF_SEPARATOR, 0, "");
	if(bool == FALSE) return NULL;
	bool = InsertMenu(menu, 200,MF_BYPOSITION | MF_STRING, ID_EXIT, "Exit");
	if(bool == FALSE) return NULL;
	return menu;
}

DWORD MegFromInt64(__int64 i)
{
	return ((double) i) / ((__int64) 1024 * 1024);
}

VOID Error(VOID)
{
	MessageBox(NULL,"Error during running program.",szAppName,MB_OK);
	ExitProcess(0);
}

