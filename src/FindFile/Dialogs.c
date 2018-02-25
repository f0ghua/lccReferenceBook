// Dialogs.c

#include <windows.h>
#include <stdio.h>
#include <winver.h>
#include "findfileres.h"
#include "dialogs.h"
#include "shell.h"
#include "findfile.h"

#define JOHNSWEBSITE "http://www.btinternet.com/~john.findlay1/"

static WNDPROC pSubClassEditProc;
static char g_sResult[MAX_PATH+100];
extern char g_strSendto[6][MAX_PATH+100];
extern char g_strDisp[6][MAX_DISP];

static char g_strSendto_tmp[6][MAX_PATH+100];
static char g_strDisp_tmp[6][MAX_DISP];
static HWND hwndDisplay;

extern HINSTANCE g_hInst;
SYSTEMTIME st;
FILETIME ft;
static EXTENDED Ext;

extern HBRUSH g_hBrLite;

static void DrawBitmap(HWND hwnd, HDC hdc, HBITMAP hBmp)
{
    RECT rc;
    HDC hdcMem;
    HBITMAP hbmOld;
	BITMAP bm;

	// Get the size of the bitmap
	GetObject(hBmp, sizeof(BITMAP), &bm);

    // Get the size of the client rectangle.
    GetClientRect(hwnd, &rc);

    // Create a compatible DC.
    hdcMem = CreateCompatibleDC(hdc);

    // Select the bitmap into the off-screen DC.
    hbmOld = SelectObject(hdcMem, hBmp);

    // Blt to the window DC.
    StretchBlt(hdc, rc.left, rc.top, rc.right, rc.bottom, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

    // Cleanup
    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);
}
// Stephan Zorn
int SetVersionInfo(HWND hwndDlg, int controlid)
{

	#define BUFSIZE	2047
	char *szmodn = malloc(BUFSIZE * sizeof(char));
	if(szmodn == NULL)
		return 0;

	char *szvers = malloc(BUFSIZE * sizeof(char));
	if(szvers == NULL)
		return 0;

	char *sztext = malloc(BUFSIZE * sizeof(char));
	if(sztext == NULL)
		return 0;

	VS_FIXEDFILEINFO *ffi;

	GetModuleFileName(
		NULL,	// handle to module to find filename for
		szmodn,	// pointer to buffer for module path
		BUFSIZE 	// size of buffer, in characters
	);

	DWORD dwdum;
	GetFileVersionInfo(
		szmodn,	// pointer to filename string
		dwdum,	// ignored
		BUFSIZE,  // size of buffer
		szvers 	// pointer to buffer to receive file-version info.
	);

	DWORD dwr;
	unsigned int uilen;
	dwr = VerQueryValue(szvers, "\\", (void**)&ffi, &uilen);
	if (!dwr) {
		free(szmodn);
		free(szvers);
		free(sztext);
		return -1;
	}

	sprintf(sztext,"Version %1d.%1d.%1d.%1d\0",
		(ffi->dwFileVersionMS >> 16),
		(ffi->dwFileVersionMS & 0xffff),
		(ffi->dwFileVersionLS >> 16),
		(ffi->dwFileVersionLS & 0xffff) );

	dwr = SetDlgItemText(hwndDlg, controlid, sztext);
	free(szmodn);
	free(szvers);
	free(sztext);
	return dwr;
}

BOOL CALLBACK AboutDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBmp = NULL;
	switch (msg)
	{
	case WM_INITDIALOG:
		hBmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDM_BANNER));
		SetVersionInfo(hwndDlg,IDS_VERSION);
		return FALSE;

	case WM_DRAWITEM:
	{
		DRAWITEMSTRUCT * di = (DRAWITEMSTRUCT *)lParam;
		if(wParam == 108)
			DrawBitmap(di->hwndItem, di->hDC, hBmp);
		return 0;
	}

	case WM_CTLCOLORDLG:
		return (BOOL)g_hBrLite;

	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, RGB(234, 232, 227));
		return (BOOL)g_hBrLite;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_LNK:
			ShellExecute(NULL, "open", JOHNSWEBSITE , NULL, NULL, SW_SHOW);
			return 0;
		case IDOK:
			return EndDialog(hwndDlg, 0);
		}
		break;

	case WM_CLOSE:
		if(hBmp) DeleteObject(hBmp);
		return EndDialog(hwndDlg, 0);
	}
	return 0;
}

BOOL CALLBACK ExtenedDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i;
	char s[40];
	static EXTENDED * pOrigExt;
	switch (msg)
	{

	case WM_CTLCOLORDLG:
		return (BOOL)g_hBrLite;

	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, RGB(234, 232, 227));
		return (BOOL)g_hBrLite;

	case WM_INITDIALOG:
		GetSystemTime(&st);
		pOrigExt = (EXTENDED *)lParam;
		for(i = 1; i<32; i++)
		{
			sprintf(s, "%02d", i);
			SendDlgItemMessage(hwndDlg, IDC_DAY_AFTER, CB_ADDSTRING, 0,(LPARAM)(LPSTR)s) ;
			SendDlgItemMessage(hwndDlg, IDC_DAY_BEFORE, CB_ADDSTRING, 0,(LPARAM)(LPSTR)s) ;
		}
		for(i = 1; i<13; i++)
		{
			sprintf(s, "%02d", i);
			SendDlgItemMessage(hwndDlg, IDC_MONTH_AFTER, CB_ADDSTRING, 0,(LPARAM)(LPSTR)s) ;
			SendDlgItemMessage(hwndDlg, IDC_MONTH_BEFORE, CB_ADDSTRING, 0,(LPARAM)(LPSTR)s) ;
		}
		// from 1980 to current year
		for(i = 1980; i<st.wYear+1; i++)
		{
			sprintf(s, "%d", i);
			SendDlgItemMessage(hwndDlg, IDC_YEAR_AFTER, CB_ADDSTRING, 0,(LPARAM)(LPSTR)s) ;
			SendDlgItemMessage(hwndDlg, IDC_YEAR_BEFORE, CB_ADDSTRING, 0,(LPARAM)(LPSTR)s) ;
		}
		// limit text length to 9 chars in both edit box's
		SendDlgItemMessage(hwndDlg, IDE_SIZE_GREATER, EM_LIMITTEXT, 9, 0);
		SendDlgItemMessage(hwndDlg, IDE_SIZE_LESS, EM_LIMITTEXT, 9, 0);

		// put values back in Edit Box's
		if(pOrigExt->dwMoreThanSize)
		{
			sprintf(s, "%d", pOrigExt->dwMoreThanSize);
			SendDlgItemMessage(hwndDlg, IDE_SIZE_GREATER, WM_SETTEXT, 0, (LPARAM)s);
		}
		if(pOrigExt->dwLessThanSize)
		{
			sprintf(s, "%d", pOrigExt->dwLessThanSize);
			SendDlgItemMessage(hwndDlg, IDE_SIZE_LESS, WM_SETTEXT, 0, (LPARAM)s);
		}
		// if these have values, put them back in the Combo Box's
		if(pOrigExt->after[0] > -1)
			SendDlgItemMessage(hwndDlg, IDC_DAY_AFTER, CB_SETCURSEL, pOrigExt->after[0], 0);
		if(pOrigExt->after[1] > -1)
			SendDlgItemMessage(hwndDlg, IDC_MONTH_AFTER, CB_SETCURSEL, pOrigExt->after[1], 0);
		if(pOrigExt->after[2] > -1)
			SendDlgItemMessage(hwndDlg, IDC_YEAR_AFTER, CB_SETCURSEL, pOrigExt->after[2], 0);
		if(pOrigExt->before[0] > -1)
			SendDlgItemMessage(hwndDlg, IDC_DAY_BEFORE, CB_SETCURSEL, pOrigExt->before[0], 0);
		if(pOrigExt->before[1] > -1)
			SendDlgItemMessage(hwndDlg, IDC_MONTH_BEFORE, CB_SETCURSEL, pOrigExt->before[1], 0);
		if(pOrigExt->before[2] > -1)
			SendDlgItemMessage(hwndDlg, IDC_YEAR_BEFORE, CB_SETCURSEL, pOrigExt->before[2], 0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDB_RESET:
			// all dialog controls back to no selection
			SendDlgItemMessage(hwndDlg, IDC_DAY_AFTER, CB_SETCURSEL, -1, 0);
			SendDlgItemMessage(hwndDlg, IDC_MONTH_AFTER, CB_SETCURSEL, -1, 0);
			SendDlgItemMessage(hwndDlg, IDC_YEAR_AFTER, CB_SETCURSEL, -1, 0);
			SendDlgItemMessage(hwndDlg, IDC_DAY_BEFORE, CB_SETCURSEL, -1, 0);
			SendDlgItemMessage(hwndDlg, IDC_MONTH_BEFORE, CB_SETCURSEL, -1, 0);
			SendDlgItemMessage(hwndDlg, IDC_YEAR_BEFORE, CB_SETCURSEL, -1, 0);
			SendDlgItemMessage(hwndDlg, IDE_SIZE_GREATER, WM_SETTEXT, 0, 0);
			SendDlgItemMessage(hwndDlg, IDE_SIZE_LESS, WM_SETTEXT, 0, 0);
			pOrigExt->fExtEnabled = FALSE;
			AnimateWindow(GetDlgItem(GetParent(hwndDlg), IDS_EXTBACK), 400, AW_HIDE | AW_VER_NEGATIVE | AW_SLIDE);
			CheckDlgButton(GetParent(hwndDlg), IDCHK_ENABLE_EXT, BST_UNCHECKED);
			return 0;

		case IDOK:
		{
			int j[3] = {0,0,0};
			memset(&Ext, 0, sizeof(EXTENDED));
			// do sizes
			SendDlgItemMessage(hwndDlg, IDE_SIZE_GREATER, WM_GETTEXT, 10, (LPARAM)s);
			Ext.dwMoreThanSize = atoi(s);

			SendDlgItemMessage(hwndDlg, IDE_SIZE_LESS, WM_GETTEXT, 10, (LPARAM)s);
			Ext.dwLessThanSize = atoi(s);

			// do ftAfterTime
			memset(&st, 0, sizeof(SYSTEMTIME));

			j[0] = SendDlgItemMessage(hwndDlg, IDC_DAY_AFTER, CB_GETCURSEL, 0, 0);
			j[1] = SendDlgItemMessage(hwndDlg, IDC_MONTH_AFTER, CB_GETCURSEL, 0, 0);
			j[2] = SendDlgItemMessage(hwndDlg, IDC_YEAR_AFTER, CB_GETCURSEL, 0, 0);
			if(j[0] > -1 && j[1] > -1 && j[2] > -1)
			{
				// miss these out if not
				Ext.after[0] = j[0];
				SendDlgItemMessage(hwndDlg, IDC_DAY_AFTER, CB_GETLBTEXT, j[0], (LPARAM)s);
				st.wDay   = atoi(s);

				Ext.after[1] = j[1];
				SendDlgItemMessage(hwndDlg, IDC_MONTH_AFTER, CB_GETLBTEXT, j[1], (LPARAM)s);
				st.wMonth =  atoi(s);

				Ext.after[2] = j[2];
				SendDlgItemMessage(hwndDlg, IDC_YEAR_AFTER, CB_GETLBTEXT, j[2], (LPARAM)s);
				st.wYear  =  atoi(s);

				SystemTimeToFileTime(&st, &Ext.ftAfterTime);
			}
			else
			{
				// -1 here means that the values will not be put back in in WM_INITDIALOG
				Ext.after[0] = -1;
				Ext.after[1] = -1;
				Ext.after[2] = -1;
			}
			// do ftBeforeTime
			memset(&st, 0, sizeof(SYSTEMTIME));

			j[0] = SendDlgItemMessage(hwndDlg, IDC_DAY_BEFORE, CB_GETCURSEL, 0, 0);
			j[1] = SendDlgItemMessage(hwndDlg, IDC_MONTH_BEFORE, CB_GETCURSEL, 0, 0);
			j[2] = SendDlgItemMessage(hwndDlg, IDC_YEAR_BEFORE, CB_GETCURSEL, 0, 0);
			if(j[0] > -1 && j[1] > -1 && j[2] > -1)
			{
				Ext.before[0] = j[0];
				SendDlgItemMessage(hwndDlg, IDC_DAY_BEFORE, CB_GETLBTEXT, j[0], (LPARAM)s);
				st.wDay   = atoi(s);

				Ext.before[1] = j[1];
				SendDlgItemMessage(hwndDlg, IDC_MONTH_BEFORE, CB_GETLBTEXT, j[1], (LPARAM)s);
				st.wMonth =  atoi(s);

				Ext.before[2] = j[2];
				SendDlgItemMessage(hwndDlg, IDC_YEAR_BEFORE, CB_GETLBTEXT, j[2], (LPARAM)s);
				st.wYear  =  atoi(s);
				SystemTimeToFileTime(&st, &Ext.ftBeforeTime);
			}
			else
			{
				Ext.before[0] = -1;
				Ext.before[1] = -1;
				Ext.before[2] = -1;
			}
			return EndDialog(hwndDlg, IDOK);
		}
		break;
		case IDCANCEL:
			return EndDialog(hwndDlg, IDCANCEL);
		default:
			return 0;
		}
	case WM_CLOSE:
		return EndDialog(hwndDlg, IDB_CONFIRMCANCEL);
	}
	return 0;
}

int DoExtendedParams(HWND hwnd, EXTENDED * pExt)
{
	HWND hExtBack;
	SYSTEMTIME st;
	char s[40];
	int fEnable = FALSE;

	if(IDOK == DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDDIALOG_EXTENDED), hwnd,(DLGPROC)ExtenedDialogProc, (LPARAM)pExt))
	{
		memcpy(pExt, &Ext, sizeof(EXTENDED));

		hExtBack = GetDlgItem(hwnd, IDS_EXTBACK);
		FileTimeToSystemTime(&pExt->ftAfterTime, &st);

		if(st.wYear == 1601)
			sprintf(s, "%S", "");
		else
		{
			sprintf(s, "%02d/%02d/%d", st.wDay, st.wMonth, st.wYear);
			fEnable = TRUE;
		}
		SetWindowText(GetDlgItem(hExtBack, IDS_DATE_AFTER_TXT), s);

		FileTimeToSystemTime(&pExt->ftBeforeTime, &st);
		if(st.wYear == 1601)
			sprintf(s, "%S", "");
		else
		{
			sprintf(s, "%02d/%02d/%d", st.wDay, st.wMonth, st.wYear);
			fEnable = TRUE;
		}
		SetWindowText(GetDlgItem(hExtBack, IDS_DATE_BEFORE_TXT), s);

		if(pExt->dwMoreThanSize == 0)
			sprintf(s, "%s", "");
		else
		{
			sprintf(s, "%d", pExt->dwMoreThanSize);
			fEnable = TRUE;
		}
		SetWindowText(GetDlgItem(hExtBack, IDS_SIZE_MORE_TXT), s);

		if(pExt->dwLessThanSize == 0)
			sprintf(s, "%s", "");
		else
		{
			sprintf(s, "%d", pExt->dwLessThanSize);
			fEnable = TRUE;
		}
		SetWindowText(GetDlgItem(hExtBack, IDS_SIZE_LESS_TXT), s);

		if(fEnable == TRUE)
		{

			ShowWindow(GetDlgItem(hwnd, IDCHK_ENABLE_EXT), SW_SHOW);

			pExt->fExtEnabled = TRUE;
			PostMessage(hwnd, WM_COMMAND, MAKELONG(IDCHK_ENABLE_EXT, 0), TRUE);
		}
		else
		{
			pExt->fExtEnabled = FALSE;
			PostMessage(hwnd, WM_COMMAND, MAKELONG(IDCHK_ENABLE_EXT, 0), FALSE);
		}
	}
	else
	{
		pExt->fExtEnabled = FALSE;
		PostMessage(hwnd, WM_COMMAND, MAKELONG(IDCHK_ENABLE_EXT, 0), FALSE);
	}

	return 0;
}

int ChooseFile(HWND hParent, char * sResult)
{
	sResult[0] = 0;
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize	= sizeof(OPENFILENAME);
	ofn.hInstance   = g_hInst;
	ofn.Flags		= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	ofn.hwndOwner	= hParent;
	ofn.lpstrFilter	= ("Files Types(*.*)\0*.*\0All Types (*.*)\0\0");
	ofn.lpstrTitle	= ("Choose Program");
	ofn.lpstrFile			= sResult;
	ofn.nMaxFile			= MAX_PATH+100;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0L;
	if (IDOK == GetOpenFileName(&ofn))
	{
		return 1;
	}
	return 0;
}

LRESULT CALLBACK SubClassEditBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:		// stop editing in the text box
		return 0;

	case WM_SETFOCUS:
		if (HIWORD(wParam) == EN_SETFOCUS)
		{
			HideCaret(hwnd);
		}
		return 0;

	default:
		return CallWindowProc(pSubClassEditProc, hwnd, message, wParam, lParam) ;
	}
}

void SetEnables(HWND hwndDlg)
{
	 // first one will always be enabled so start at second control
	 // and set the text for the edit box's
	int i, set;
	for(i = 0; i<5; i++)
	{
		set = i + 302;
		EnableWindow(GetCtrl(hwndDlg, set + 0 ), g_strDisp_tmp[i][0] ? TRUE : FALSE);	// EDIT BOX's
		EnableWindow(GetCtrl(hwndDlg, set + 10), g_strDisp_tmp[i][0] ? TRUE : FALSE);	// EDIT BUTS
		SetWindowText(GetCtrl(hwndDlg, IDE_1 + i), g_strDisp_tmp[i]);
	}
}

BOOL CALLBACK EditSendToDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int which;

	switch (msg)
	{

	case WM_CTLCOLORDLG:
		return (BOOL)g_hBrLite;

	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, RGB(234, 232, 227));
		return (BOOL)g_hBrLite;

	case WM_INITDIALOG:
		which = lParam - IDB_ED1; 	// which control
		// subclass edit box - stop editing and caret for the "Folder or Program" box
		SetWindowLong(GetCtrl(hwndDlg, IDE_1),GWL_WNDPROC, (DWORD)SubClassEditBoxProc);
		SndMsg(GetCtrl(hwndDlg, IDE_DISP), EM_LIMITTEXT, MAX_DISP-1, 0);
		SetWindowText(GetCtrl(hwndDlg, IDE_DISP), g_strDisp_tmp[which]);	// Display Name
		SetWindowText(GetCtrl(hwndDlg, IDE_1), g_strSendto_tmp[which]);	// Folder/Program
		SetFocus(GetCtrl(hwndDlg, IDE_DISP));
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetWindowText(GetCtrl(hwndDlg, IDE_DISP), g_sResult, MAX_PATH+100);
			if(!strlen(g_sResult))
			{
				MsgBox(hwndDlg, "You need to enter a Display Name!");
				SetFocus(GetCtrl(hwndDlg, IDE_DISP));
				return 0;
			}
			GetWindowText(GetCtrl(hwndDlg, IDE_1), g_sResult, MAX_PATH+100);
			if(!strlen(g_sResult))
			{
				MsgBox(hwndDlg, "You need to select either a Folder or Promgram!");
				return 0;
			}
			GetWindowText(GetCtrl(hwndDlg, IDE_DISP), g_sResult, MAX_PATH+100);
			strcpy(g_strDisp_tmp[which] , g_sResult);
			GetWindowText(GetCtrl(hwndDlg, IDE_1), g_sResult, MAX_PATH+100);
			strcpy(g_strSendto_tmp[which] , g_sResult);
			SetEnables(hwndDisplay);
			return EndDialog(hwndDlg, IDOK);

		case IDCANCEL:
			return EndDialog(hwndDlg, IDCANCEL);

		case IDB_FOLD_1:
			if(SHBrowseDir("", g_sResult))
			{
				strcat(g_sResult, "\\");
				SetWindowText(GetCtrl(hwndDlg, IDE_1), g_sResult);
			}
			break;

		case IDB_PROG_1:
			if(ChooseFile(hwndDlg, g_sResult))
			{
				SetWindowText(GetCtrl(hwndDlg, IDE_1), g_sResult);
			}
			break;
		}
		return 0;

	case WM_CLOSE:
		return EndDialog(hwndDlg, IDCANCEL);

	case WM_DESTROY:
		// set control's wndproc's back to original
		SetWindowLong(GetCtrl(hwndDlg, IDE_1),GWL_WNDPROC, (DWORD)pSubClassEditProc);
		break;
	}
	return 0;
}

BOOL CALLBACK DisplaySendToDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i;
	switch (msg)
	{

	case WM_CTLCOLORDLG:
		return (BOOL)g_hBrLite;

	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, RGB(234, 232, 227));
		return (BOOL)g_hBrLite;

	case WM_INITDIALOG:
		hwndDisplay = hwndDlg;
		// subclass edit box's so as to stop editing and the caret
		pSubClassEditProc = (WNDPROC)SetWindowLong(GetCtrl(hwndDlg, IDE_1),GWL_WNDPROC, (DWORD)SubClassEditBoxProc);
		SetWindowLong(GetCtrl(hwndDlg, IDE_2),GWL_WNDPROC, (DWORD)SubClassEditBoxProc);
		SetWindowLong(GetCtrl(hwndDlg, IDE_3),GWL_WNDPROC, (DWORD)SubClassEditBoxProc);
		SetWindowLong(GetCtrl(hwndDlg, IDE_4),GWL_WNDPROC, (DWORD)SubClassEditBoxProc);
		SetWindowLong(GetCtrl(hwndDlg, IDE_5),GWL_WNDPROC, (DWORD)SubClassEditBoxProc);
		SetWindowLong(GetCtrl(hwndDlg, IDE_6),GWL_WNDPROC, (DWORD)SubClassEditBoxProc);
		for(int i = 0; i<6; i++)
		{
			strcpy(g_strSendto_tmp[i], g_strSendto[i]);
			strcpy(g_strDisp_tmp[i], g_strDisp[i]);
		}
		SetEnables(hwndDlg);
		SetFocus(GetCtrl(hwndDlg, IDOK));
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDB_CLEAR:
			for(i = 0; i<5; i++)
			{
				SetWindowText(GetCtrl(hwndDlg, IDE_1 + i), "");
				g_strSendto_tmp[i][0] = 0;
				g_strDisp_tmp[i][0]   = 0;
			}

			return 0;

		case IDOK:
			for(int i = 0; i<6; i++)
			{
				strcpy(g_strSendto[i], g_strSendto_tmp[i]);
				strcpy(g_strDisp[i], g_strDisp_tmp[i]);
			}
			return EndDialog(hwndDlg, IDOK);

		case IDCANCEL:
			return EndDialog(hwndDlg, IDCANCEL);

		case IDB_ED1: case IDB_ED2:	case IDB_ED3: case IDB_ED4:	case IDB_ED5: case IDB_ED6:
			DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDDIALOG_EDITSENDTO), hwndDlg,
				(DLGPROC)EditSendToDialogProc, (LPARAM)LOWORD(wParam));
			break;
		}
		return 0;

	case WM_CLOSE:
		return EndDialog(hwndDlg, IDCANCEL);

	case WM_DESTROY:
		// set controls wndproc's back to original
		SetWindowLong(GetCtrl(hwndDlg, IDE_1),GWL_WNDPROC, (DWORD)pSubClassEditProc);
		SetWindowLong(GetCtrl(hwndDlg, IDE_2),GWL_WNDPROC, (DWORD)pSubClassEditProc);
		SetWindowLong(GetCtrl(hwndDlg, IDE_3),GWL_WNDPROC, (DWORD)pSubClassEditProc);
		SetWindowLong(GetCtrl(hwndDlg, IDE_4),GWL_WNDPROC, (DWORD)pSubClassEditProc);
		SetWindowLong(GetCtrl(hwndDlg, IDE_5),GWL_WNDPROC, (DWORD)pSubClassEditProc);
		SetWindowLong(GetCtrl(hwndDlg, IDE_6),GWL_WNDPROC, (DWORD)pSubClassEditProc);
	}
	return 0;
}

BOOL CALLBACK MessageDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		SetWndTxt(GetCtrl(hwndDlg, IDS_MESSAGETEXT), (char*)lParam);
		SetFocus(GetCtrl(hwndDlg, IDB_SEARCH));
		return FALSE;

	case WM_CTLCOLORDLG:
		return (BOOL)g_hBrLite;

	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, RGB(234, 232, 227));
		return (BOOL)g_hBrLite;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			return EndDialog(hwndDlg, IDOK);
		}
		break;
	case WM_CLOSE:
		return EndDialog(hwndDlg, 0);
	}
	return 0;
}

BOOL CALLBACK ConfirmDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Two functions use this dialog, DELETE and MOVETO
	// lParam points to the text for the static text control
	switch (msg)
	{
	case WM_INITDIALOG:
		// both icon windows are made containing their icons, hide one of them!
		SetWndTxt(GetCtrl(hwndDlg, IDS_TEXTCONFIRM), (char*)lParam);
		if(strstr((char*)lParam, "Delete"))
		{
			ShowWindow(GetCtrl(hwndDlg, IDI_CONFIRMICONMOV), SW_HIDE);
		}
		else if(strstr((char*)lParam, "Move"))
		{
			ShowWindow(GetCtrl(hwndDlg, IDI_CONFIRMICONDEL), SW_HIDE);
		}
		SetFocus(GetCtrl(hwndDlg, IDB_CONFIRMCANCEL));
		return FALSE;

	case WM_CTLCOLORDLG:
		return (BOOL)g_hBrLite;

	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, RGB(234, 232, 227));
		return (BOOL)g_hBrLite;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDB_CONFIRMYES:
			return EndDialog(hwndDlg, IDB_CONFIRMYES);
		case IDB_CONFIRMCANCEL:
			return EndDialog(hwndDlg, IDB_CONFIRMCANCEL);
		}
		break;

	case WM_CLOSE:
		return EndDialog(hwndDlg, IDB_CONFIRMCANCEL);
	}
	return 0;
}

