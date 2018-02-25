/**************************************************************

	MondPhase in TaskBar

	By Eberhard Funck	funck@hildesheim.sgh-net.de


***************************************************************/

#include <windows.h>
#include <shellapi.h>
#include <math.h>
#include "stringx.h"
#include "moon.h"

#if defined (WIN32)
	#define IS_WIN32 TRUE
#else
	#define IS_WIN32 FALSE
#endif

#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)
#define IS_WIN32S  IS_WIN32 && (BOOL)(!(IS_NT) && (LOBYTE(LOWORD(GetVersion()))<4))
#define IS_WIN95   (BOOL)(!(IS_NT) && !(IS_WIN32S)) && IS_WIN32

#define IDM_ABOUT  100
#define IDM_EXIT   110
#define ID_TASKBAR 2

UINT CONST WM_NOTIFYICON = WM_USER + 0x10;

typedef LONG (CALLBACK* GETRES)(int);
#define GFSR_SYSTEMRESOURCES	0
#define GFSR_GDIRESOURCES		1
#define GFSR_USERRESOURCES		2
#define HKEY_LOCAL_MASHINE   ((HKEY) 0x80000002)

BOOL    bLanguage = FALSE; // FALSE for englisch

char szTag[] = {"Sonntag\0Montag\0Dienstag\0Mittwoch\0"
				"Donnerstag\0Freitag\0Samstag\0\0"};

char szDay[] = {"Sunday\0Monday\0Tuesday\0Wednesday\0"
				"Thursday\0Friday\0Saturday\0\0"};

char szMonat[] = {"Januar\0Februar\0März\0April\0Mai\0Juni\0July\0"
				  "August\0September\0Oktober\0November\0Dezember\0\0"};

char szMonth[] = {"January\0February\0March\0April\0May\0June\0July\0"
				  "August\0September\0October\0November\0December\0\0"};
char szTip[128];

HINSTANCE hInst;   // current instance
HWND      hMainWnd;
HICON     hIcon;
HMENU     hMenu;

void      CenterAppDialog(HWND, int);
RECT      MapDialogControlRect(HWND, int, BOOL);
BOOL      TaskBarAddIcon(HWND, UINT, HICON, LPSTR);
BOOL      TaskBarModifyIcon(HWND, UINT, LPSTR);
BOOL      TaskBarDeleteIcon(HWND, UINT);
HICON     GetMoonIcon(VOID);
LPSTR     GetTip(VOID);

LRESULT CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HINSTANCE hInstance;     /* handle of current instance	*/
HINSTANCE hPrevInstance; /* handle of previous instance	*/
LPSTR     lpCmdLine;     /* address of command line	    */
int       nCmdShow;      /* show-window type (open/icon)*/
{
	MSG msg;

	hInst = hInstance;
	hIcon = GetMoonIcon();

	if (bLanguage == FALSE)
		hMenu = GetSubMenu(LoadMenu(hInst, MAKEINTRESOURCE(MAINMENU_E)), 0);
	else
		hMenu = GetSubMenu(LoadMenu(hInst, MAKEINTRESOURCE(MAINMENU_D)), 0);

	SetMenuDefaultItem(hMenu, 0, TRUE);

	if (bLanguage == FALSE)
		hMainWnd = CreateDialog(hInst, MAKEINTRESOURCE(ABOUT_E), NULL, (DLGPROC)MainDlgProc);
	else
		hMainWnd = CreateDialog(hInst, MAKEINTRESOURCE(ABOUT_D), NULL, (DLGPROC)MainDlgProc);

	if (!hMainWnd){
		return(FALSE);}

	ShowWindow(hMainWnd, SW_HIDE);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyMenu(hMenu);

	return(msg.wParam);
}



LRESULT CALLBACK MainDlgProc(hDlg, msg, wParam, lParam)
HWND   hDlg;    /* handle of window       */
UINT   msg;     /* type of message        */
WPARAM wParam;  /* additional information */
LPARAM lParam;  /* additional information */
{
	POINTL p;
	int    mIndex;
	static LPBITMAPINFO lpBitmap;
	static LPTSTR       lpBits;
	static RECT         rd;
	HANDLE              hRes;
	HANDLE              hModule;
	GETRES              pGetRes;
	long                lSysRes;
	char                szTemp[128];
	char                szText[128];
	LPCTSTR             subkey;
	HKEY			    hKey;
	DWORD		        dwBytes;
	DWORD		        dwType;
	MEMORYSTATUS        mst;

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		TaskBarAddIcon(hDlg, ID_TASKBAR, hIcon, GetTip());
		CenterAppDialog(hDlg, 0);

		if (bLanguage == FALSE)
			SetWindowText(hDlg, "About");
		else
			SetWindowText(hDlg, "Über");

		if (HIWORD(GetDialogBaseUnits()) == 16)
		{
			rd = MapDialogControlRect(hDlg, 0, FALSE);
			SetWindowPos(hDlg, 0, 0, 0, rd.right + 20, rd.bottom, SWP_NOMOVE);
			rd = MapDialogControlRect(hDlg, 30, FALSE);
			SetWindowPos(GetDlgItem(hDlg, 30), 0, rd.left - 8,
						 rd.top - 5, 0, 0, SWP_NOSIZE);
			rd = MapDialogControlRect(hDlg, 40, FALSE);
			SetWindowPos(GetDlgItem(hDlg, 40), 0, rd.left + 5,
						 rd.top, 0, 0, SWP_NOSIZE);
		}

		SendDlgItemMessage(hDlg, IDMYAPPICON, STM_SETICON,
						  (WPARAM) LoadImage(hInst,
						 					 MAKEINTRESOURCE(IDMYAPPICON),
											 IMAGE_ICON,
											 64, 64,
											 LR_DEFAULTCOLOR), 0);

		LoadString(hInst, 1, szText, 128);
		SetDlgItemText(hDlg, 11, szText);

		if (IS_NT)
			subkey = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";
		else
			subkey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion";

		RegOpenKeyEx(HKEY_LOCAL_MASHINE, subkey, 0, KEY_ALL_ACCESS, &hKey);
		dwType  = REG_SZ;
		dwBytes = 128;
		RegQueryValueEx(hKey, "RegisteredOwner", 0,
						&dwType, szText, &dwBytes);
		SetDlgItemText(hDlg, 15, szText);
		RegQueryValueEx(hKey, "RegisteredOrganization", NULL,
	   					&dwType, szText, &dwBytes);
		SetDlgItemText(hDlg, 16, szText);
		RegCloseKey(hKey);

		// Free memory in MByte

		memset(&mst, 0, sizeof(mst));
		mst.dwLength = sizeof(mst);
		GlobalMemoryStatus(&mst);

		wsprintf(szText, "%i", (mst.dwTotalPhys / 1024 / 1000));
		lstrcat(szText, " MByte");
		SetDlgItemText(hDlg, 18, szText);

		if (IS_WIN95)
		{
			SetDlgItemText(hDlg, 19, "Systemresourcen:");
			hModule = LoadLibrary("RSRC32.dll");
			if (hModule != NULL)
			{
				pGetRes = (GETRES) GetProcAddress(hModule,
										   "_MyGetFreeSystemResources32@4");
				if (pGetRes)
					lSysRes = (*pGetRes)(GFSR_SYSTEMRESOURCES);

				wsprintf(szText, "%d", lSysRes);
				lstrcat(szText, " % frei");
				SetDlgItemText(hDlg, 20, szText);
				FreeLibrary(hModule);
			}
		}

		hRes = LoadResource(hInst,
					FindResource(hInst, "#200", RT_BITMAP));
		lpBitmap = (LPBITMAPINFO) LockResource(hRes);
		lpBits   = (LPTSTR) lpBitmap;
		lpBits   += lpBitmap->bmiHeader.biSize +
					(256 * sizeof(RGBQUAD));

		}
		return (TRUE);

	case WM_CTLCOLORDLG:
		return (HRESULT) GetStockObject(NULL_BRUSH);

	case WM_ACTIVATE:
		SendMessage(GetParent(hDlg), WM_NCACTIVATE, TRUE, 0);
		return (FALSE);

	case WM_PAINT:
	{
		HDC     hdc;
		HFONT   hFont;
		LOGFONT lf;
		int     id;

		hdc = GetDC(hDlg);

		SetStretchBltMode(hdc, 4);
		rd = MapDialogControlRect(hDlg, 0, FALSE);
		StretchDIBits(hdc, 0, 0,
	   				 	   rd.right, rd.bottom,
						   0, 0,
						   lpBitmap->bmiHeader.biWidth,
						   lpBitmap->bmiHeader.biHeight,
						   lpBits, lpBitmap, DIB_RGB_COLORS, SRCCOPY);

		SetBkMode(hdc, TRANSPARENT);

		memset(&lf, 0, sizeof(lf));
		lstrcpy(lf.lfFaceName, "Arial");
		lf.lfWeight   = 900;
		lf.lfHeight   = HIWORD(GetDialogBaseUnits());
		hFont         = CreateFontIndirect(&lf);

		SelectObject(hdc, hFont);
		GetDlgItemText(hDlg, 10, szText, sizeof(szText));
		wsprintf(szTemp, szText, "1.0");

		SetTextColor(hdc, RGB(0, 0, 0));
		rd = MapDialogControlRect(hDlg, 10, TRUE);
		rd.left += 2; rd.top += 2;
		DrawText(hdc, szTemp, -1, &rd, DT_LEFT);
		SetTextColor(hdc, RGB(255, 0, 0));
		rd = MapDialogControlRect(hDlg, 10, TRUE);
		DrawText(hdc, szTemp, -1, &rd, DT_LEFT);

		DeleteObject(hFont);
		SelectObject(hdc, (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0));
		SetTextColor(hdc, RGB(0, 255, 0));

		for (id = 11; id < 21; id++)
		{
			if (IS_NT && (id == 19 || id == 20))
				continue;

			GetDlgItemText(hDlg, id, szTemp, 218);
			rd = MapDialogControlRect(hDlg, id, TRUE);
			DrawText(hdc, szTemp, -1, &rd, DT_LEFT);
		}

		ReleaseDC(hDlg, hdc);
	}
	return (FALSE);

	case WM_NOTIFYICON:
	{
		// wParam: Icon's ID, as declared when calling AddIcon
		// lParam: Mouse event for the Icon
		switch ((UINT)lParam)
		{
			case WM_LBUTTONDBLCLK:
				ShowWindow(hDlg, SW_SHOW);
				SetForegroundWindow(hDlg);
				break;

			case WM_RBUTTONDOWN:
				if ((UINT)wParam == ID_TASKBAR)
				{
					GetCursorPos(&p);

					SetForegroundWindow(hDlg);
					mIndex = TrackPopupMenuEx(hMenu, TPM_RETURNCMD,
											p.x, p.y, hDlg, NULL);

					//MessageBox(NULL, Str(mIndex), "Debug", MB_OK);
					SendMessage(hDlg, WM_COMMAND, mIndex, 0);
				}
		}

		return (FALSE);
	}

	case WM_TIMECHANGE:
		hIcon = GetMoonIcon();
		TaskBarModifyIcon(hDlg, ID_TASKBAR, GetTip());
		return (FALSE);

	case WM_DESTROY:
		PostQuitMessage(0);
		return (FALSE);

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDOK:
				ShowWindow(hDlg, SW_HIDE);
				break;

			case IDM_ABOUT:
				ShowWindow(hDlg, SW_SHOW);
				SetForegroundWindow(hDlg);
				break;

			case IDM_EXIT:
				TaskBarDeleteIcon(hDlg, ID_TASKBAR);
				DestroyWindow(hDlg);
				break;
		}

	}

	return (0L);
}



BOOL TaskBarAddIcon(HWND hWnd, UINT uID, HICON hIcon, LPSTR lpszTip)
{
	BOOL res;
	NOTIFYICONDATA tnid;

	tnid.cbSize           = sizeof(NOTIFYICONDATA);
	tnid.hWnd             = hWnd;
	tnid.uID              = uID;
	tnid.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnid.uCallbackMessage = WM_NOTIFYICON;
	tnid.hIcon            = hIcon;
	if (lpszTip)
		lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
	else
		tnid.szTip[0]     = '\0';

	res = Shell_NotifyIcon(NIM_ADD, &tnid);

	if (hIcon)
		DestroyIcon(hIcon);

	return (res);
}


BOOL TaskBarModifyIcon(HWND hWnd, UINT uID, LPSTR lpszTip)
{
	BOOL res;
	NOTIFYICONDATA tnid;

	tnid.cbSize           = sizeof(NOTIFYICONDATA);
	tnid.hWnd             = hWnd;
	tnid.uID              = uID;
	tnid.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnid.uCallbackMessage = WM_NOTIFYICON;
	tnid.hIcon            = hIcon;
	if (lpszTip)
		lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
	else
		tnid.szTip[0]     = '\0';

	res = Shell_NotifyIcon(NIM_MODIFY, &tnid);

	if (hIcon)
		DestroyIcon(hIcon);

	return (res);
}


BOOL TaskBarDeleteIcon(HWND hWnd, UINT uID)
{
	BOOL res;
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd   = hWnd;
	tnid.uID    = uID;

	res = Shell_NotifyIcon(NIM_DELETE, &tnid);

	return (res);
}


LPSTR GetTip()
{
	SYSTEMTIME st;

	GetLocalTime(&st);
	if (bLanguage == FALSE)
	{
		lstrcpy(szTip, "Phase of moon for ");
		lstrcat(szTip, GetString(szDay, st.wDayOfWeek));
		wsprintf(szTip, "%s, %s %02d %d", szTip,
				GetString(szMonth, st.wMonth - 1), st.wDay, st.wYear);
	}
	else
	{
		lstrcpy(szTip, "Mondphase für ");
		lstrcat(szTip, GetString(szTag, st.wDayOfWeek));
		wsprintf(szTip, "%s, %02d. %s %d", szTip, st.wDay,
				GetString(szMonat, st.wMonth - 1), st.wYear);
	}


	return szTip;
}


HICON GetMoonIcon()
{
	SYSTEMTIME st;
	double     dx;
	int        x;
	char       szIcon[10];

	GetLocalTime(&st);

	if (st.wYear < 2000)
		dx = fmod((st.wYear - 1900), 19);
	else
		dx = fmod((st.wYear - 2000), 19);

	if (dx > 19)
		dx = dx - 19;

	dx = dx * 11;
	dx = fmod(dx, 30);

	if (st.wYear < 2000)
		dx = dx + st.wDay + st.wMonth - 4;
	else
		dx = dx + st.wDay + st.wMonth - 8.3;

	x = fmod(dx, 29) + 100;

	return LoadImage(hInst,
					 MAKEINTRESOURCE(x),
					 IMAGE_ICON,
					 32, 32,
					 LR_DEFAULTCOLOR);
}


void CenterAppDialog(HWND hWnd, int h)
{
	RECT r, rc, rect;
	HWND hParent;

	if (GetParent(hWnd) == NULL)
	{
		GetWindowRect(GetDesktopWindow(), &rect);
		GetClientRect(GetDesktopWindow(), &rc);
		GetClientRect(hWnd, &r);
	}
	else
	{
		GetWindowRect(GetParent(hWnd), &rect);
		GetClientRect(GetParent(hWnd), &rc);
		GetClientRect(hWnd, &r);
	}

	SetWindowPos(hWnd, HWND_TOP,
   				 rect.left + (rc.right - r.right) / 2,
				 rect.top + (rc.bottom - r.bottom) / 2 + h,
				 0, 0, SWP_NOSIZE);
}



RECT MapDialogControlRect(HWND hWnd, int id, BOOL flag)
{
	int w, h;
	RECT rw, rc, rect;

	if (GetWindowLong(hWnd, GWL_STYLE) & WS_SYSMENU)
	{
		w = GetSystemMetrics(SM_CXDLGFRAME);
		h = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME);
	}
	else if (GetWindowLong(hWnd, GWL_STYLE) & WS_DLGFRAME)
	{
		w = GetSystemMetrics(SM_CXDLGFRAME);
		h = GetSystemMetrics(SM_CYDLGFRAME);
	}
	else  if (GetWindowLong(hWnd, GWL_STYLE) & WS_BORDER)
	{
		w = GetSystemMetrics(SM_CXBORDER);
		h = GetSystemMetrics(SM_CYBORDER);
	}

	GetWindowRect(hWnd, &rw);
	GetWindowRect(GetDlgItem(hWnd, id), &rc);

	if (id == 0)
	{
		GetClientRect(hWnd, &rect);
		rect.left = 0;
	    rect.top  = 0;
	}
	else
	{
		GetClientRect(GetDlgItem(hWnd, id), &rect);
		rect.left = rc.left - rw.left - w;
		rect.top  = rc.top - rw.top - h;
		if (flag)
		{
			rect.right  = rect.left + rect.right;
			rect.bottom = rect.top + rect.bottom;
		}
	}

	return (rect);
}

