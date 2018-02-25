#include <windows.h>
#include <windowsx.h>
#include <winver.h>
#include <commctrl.h>
#include <richedit.h>
#include "tinypadres.h"

HWND hWndStatusbar;				// Statusbar window handle

static SYSTEMTIME stime;
static LCID Locale;
static HWND hwndList;
extern HINSTANCE hInst;
extern HWND hEdit;
extern TCHAR szAppName[];
extern CHAR FindBuffer[MAX_PATH];
extern BOOL bFullScreen;
extern BOOL bWordWarp;
extern HWND hwndMain;
extern BOOL bNeedSave;
extern TCHAR szFileName[];

typedef struct _WORD_COUNT_DATA { // wcd
	DWORD	dwLines;
	DWORD	dwWords;
	DWORD	dwChars;
	BOOL    bState;
} WORD_COUNT_DATA, *LPWORD_COUNT_DATA;

//=============================================================================
// Function.....: UpdateStatusBar
// Description..: Updates the statusbar control with the appropiate text.
// Parameters...: lpszStatusString 	- Charactar string that will be shown.
//                partNumber 		- index of the status bar part number.
//                displayFlags 		- Decoration flags.
// Return values: Nonde
//=============================================================================
extern void UpdateStatusBar(LPSTR lpszStatusString, WORD partNumber, WORD displayFlags)
{
    SendMessage(hWndStatusbar,SB_SETTEXT, partNumber | displayFlags,
	(LPARAM)lpszStatusString);
}

//=============================================================================
// Function.....: MsgMenuSelect
// Description..: Shows in the status bar a descriptive explaation of the
//                purpose of each menu item.The message WM_MENUSELECT is sent
//                when the user starts browsing the menu for each menu item
//                where the mouse passes.
// Parameters...: Standard windows.
// Return values: Zero
//=============================================================================
#pragma argsused // No hwnd and uMessage are used.
extern LRESULT MsgMenuSelect(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    static char szBuffer[256];
    UINT   nStringID;
    UINT   fuFlags = GET_WM_MENUSELECT_FLAGS(wparam, lparam) & 0xffff;
    UINT   uCmd    = GET_WM_MENUSELECT_CMD(wparam, lparam);
    HMENU  hMenu   = GET_WM_MENUSELECT_HMENU(wparam, lparam);

    szBuffer[0] = 0;                            // First reset the buffer
    if (fuFlags == 0xffff && hMenu == NULL)     // Menu has been closed
        nStringID = 0;

    else if (fuFlags & MFT_SEPARATOR)           // Ignore separators
        nStringID = 0;

    else if (fuFlags & MF_POPUP)                // Popup menu
    {
        if (fuFlags & MF_SYSMENU)               // System menu
            nStringID = IDS_SYSMENU;
        else
            // Get string ID for popup menu from idPopup array.
            nStringID = 0;
    }  // for MF_POPUP
    else                                        // Must be a command item
        nStringID = uCmd;                       // String ID == Command ID

    // Load the string if we have an ID
    if (0 != nStringID)
        LoadString(hInst, nStringID, szBuffer, sizeof(szBuffer));
    // Finally... send the string to the status bar
    UpdateStatusBar(szBuffer, 0, 0);
    return 0;
}

//=============================================================================
// Function.....: InitializeStatusBar
// Description..: Initialize the status bar
// Parameters...: hwndParent: the parent window
//                nrOfParts: The status bar can contain more than one
//                part. What is difficult, is to figure out how this
//                should be drawn. So, for the time being only one is
//                being used...
// Return values: None
//=============================================================================
extern void InitializeStatusBar(HWND hwndParent,int nrOfParts)
{
//   const int cSpaceInBetween = 8;
    int   ptArray[40];   // Array defining the number of parts/sections
    RECT  rect;
    HDC   hDC;

   /* * Fill in the ptArray...  */

    hDC = GetDC(hwndParent);
    GetClientRect(hwndParent, &rect);

    ptArray[nrOfParts-1] = rect.right;
    //---TODO--- Add code to calculate the size of each part of the status
    // bar here.

    ReleaseDC(hwndParent, hDC);
    SendMessage(hWndStatusbar,
                SB_SETPARTS,
                nrOfParts,
                (LPARAM)(LPINT)ptArray);

    UpdateStatusBar("Ready", 0, 0);
    //---TODO--- Add code to update all fields of the status bar here.
    // As an example, look at the calls commented out below.

//    UpdateStatusBar("Cursor Pos:", 1, SBT_POPOUT);
//    UpdateStatusBar("Time:", 3, SBT_POPOUT);
}

//=============================================================================
// Function.....: CreateSBar
// Description..: Calls CreateStatusWindow to create the status bar
// Parameters...: hwndParent: the parent window
//                initial text: the initial contents of the status bar
// Return values: True if success.
//=============================================================================
extern BOOL CreateSBar(HWND hwndParent,char *initialText,int nrOfParts)
{
    hWndStatusbar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_BORDER|SBARS_SIZEGRIP,
                                       initialText,
                                       hwndParent,
                                       IDM_STATUSBAR);
    if(hWndStatusbar)
    {
        InitializeStatusBar(hwndParent,nrOfParts);
        return TRUE;
    }

    return FALSE;
}

//=============================================================================
// Function.....: AskToSave
// Description..: Ask to user to save file.
// Parameters...: MainWindow handle and file title
// Return values: Return IDCANCEL or IDYES
//=============================================================================
extern SHORT AskToSave(HWND hwnd, TCHAR *szTitleName)
{
	TCHAR szBuffer[MAX_PATH + 64];
	int iReturn;

	wsprintf(szBuffer, TEXT("Save current changes in %s ?"),
		szTitleName[0] ? szTitleName : "Untitled");

	iReturn = MessageBox(hwnd, szBuffer, szAppName,
		MB_YESNOCANCEL | MB_ICONQUESTION);

	if(iReturn == IDYES)
		if(!SendMessage(hwnd, WM_COMMAND, IDM_SAVE, 0))
			iReturn = IDCANCEL;

	return iReturn;
}

//=============================================================================
// Function.....: DoCaption
// Description..: Make main window title -> Tinyedit - [file.txt]
// Parameters...: Main window handle and pointer to filename.
// Return values: None.
//=============================================================================
extern void DoCaption (HWND hwnd, TCHAR *szFileName)
{
	TCHAR szCaption[64 + MAX_PATH];
	TCHAR *p;

	p = strrchr(szFileName, '\\');
	if(p != NULL) p++;

	if(szFileName[0] == 0) p = szFileName;

	wsprintf(szCaption, TEXT("%s - [%s]"), szAppName,
		*p ? p : "Untitled");
	SetWindowText(hwnd, szCaption);
}

//=============================================================================
// Function.....: OkMessage
// Description..: Show message used in non critical errors.
// Parameters...: Win handle, message, and filename.
// Return values: None.
//=============================================================================
extern void OkMessage(HWND hwnd, TCHAR *szMessage, TCHAR *szFileName)
{
	TCHAR szBuffer[64 + MAX_PATH];
	TCHAR *p;

	p = strrchr(szFileName, '\\');
	if(p != NULL) p++;

	wsprintf(szBuffer, szMessage, p ? p : "Untitled");
	MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
}


//=============================================================================
// Function.....: InitMenuPopup
// Description..: Disable or enable menu items used under WM_INITMENUPOPUP msg.
// Parameters...: Wparam and lParam
// Return values: 0
//=============================================================================
extern LRESULT InitMenuPopup(WPARAM wParam, LPARAM lParam)
{
	INT iSelBeg;
	INT iSelEnd;
	INT iEnable;

	switch(lParam)
	{
		case 0: // File menu
			EnableMenuItem ((HMENU) wParam, IDM_SAVE,
				(bNeedSave) ? MF_ENABLED : MF_GRAYED);
			EnableMenuItem ((HMENU) wParam, IDM_PROPERTIES,
				(szFileName[0]) ? MF_ENABLED : MF_GRAYED);
		break;
		case 1: // Edit Menu
			EnableMenuItem ((HMENU) wParam, IDM_UNDO,
				SendMessage (hEdit, EM_CANUNDO, 0, 0L) ? MF_ENABLED : MF_GRAYED);

			EnableMenuItem ((HMENU) wParam, IDM_PASTE,
                    IsClipboardFormatAvailable (CF_TEXT) ? MF_ENABLED : MF_GRAYED);

			SendMessage (hEdit, EM_GETSEL, (WPARAM) &iSelBeg, (LPARAM) &iSelEnd) ;
            iEnable = iSelBeg != iSelEnd ? MF_ENABLED : MF_GRAYED ;

            EnableMenuItem ((HMENU) wParam, IDM_CUT,   iEnable) ;
			EnableMenuItem ((HMENU) wParam, IDM_COPY,  iEnable) ;
			EnableMenuItem ((HMENU) wParam, IDM_DELETE, iEnable) ;
		break;

		case 2: // Search menu
			EnableMenuItem ((HMENU) wParam, IDM_FINDNEXT,
		    (lstrlen(FindBuffer)) ? MF_ENABLED : MF_GRAYED) ;
		break;

		case 3: // Options menu
		{
			UINT state = MF_BYCOMMAND;
			LONG style;

			if(bWordWarp)
				state |= MF_CHECKED;
			else
				state |= MF_UNCHECKED;

			CheckMenuItem((HMENU) wParam, IDM_WORDWARP, state);

			CheckMenuItem((HMENU) wParam, IDM_FULLSCREEN,
				(bFullScreen) ? MF_CHECKED : MF_UNCHECKED);

			style = GetWindowLong(hwndMain, GWL_EXSTYLE);
			if(style & WS_EX_TOPMOST)
				CheckMenuItem((HMENU) wParam, IDM_ALWAYSONTOP, MF_CHECKED);
			else
				CheckMenuItem((HMENU) wParam, IDM_ALWAYSONTOP, MF_UNCHECKED);

		}
		break;
	}
	return 0;
}


static BOOL CALLBACK DateFmtEnumProc(LPTSTR lpszFormatString)
{
	TCHAR buf[256];

	GetDateFormat(Locale, 0, &stime, lpszFormatString, buf, 256);
	if(SendMessage(hwndList, LB_FINDSTRINGEXACT, -1, (LPARAM) buf) == LB_ERR)
		SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) buf);
	return TRUE;
}

static BOOL CALLBACK TimeFmtEnumProc(LPTSTR lpszFormatString)
{
	TCHAR buf[256];

	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &stime, lpszFormatString, buf, 256);
	 if(SendMessage(hwndList, LB_FINDSTRINGEXACT, -1, (LPARAM) buf) == LB_ERR)
		SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) buf);
	return TRUE;
}

static VOID WriteRE(LPCSTR szString)
{
	const TCHAR *p = szString;

	while(*p)
	{
		SendMessage(hEdit, WM_CHAR, *p, 0);
		p++;
	}
}

//=============================================================================
// Function.....: DlgDatetime
// Description..: Jump to specifid line.
// Parameters...: Standard
// Return values: Is message processed.
//=============================================================================
#pragma argsused // No lParam is used.
extern BOOL CALLBACK DlgDateTime(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
				hwndList = GetDlgItem(hDlg, IDC_DATEDIALOG_LIST);
				Locale = GetUserDefaultLCID();
				GetLocalTime(&stime);
				EnumDateFormats(DateFmtEnumProc, Locale, DATE_SHORTDATE);
				EnumDateFormats(DateFmtEnumProc, Locale, DATE_LONGDATE);
				EnumTimeFormats(TimeFmtEnumProc, LOCALE_USER_DEFAULT, 0);
				SendMessage(hwndList, LB_SETCURSEL, 0, 0);
				SetFocus(hwndList);
		break;

		case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case IDOK:
			{
				LONG lResult;
				TCHAR buf[MAX_PATH];

				lResult = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
				if(lResult != LB_ERR)
				{
					SendMessage(hwndList, LB_GETTEXT, lResult,(LPARAM) &buf);
					WriteRE(buf);
				}
				EndDialog(hDlg, 1);
			}
			return 1;

			case IDCANCEL:
				EndDialog(hDlg, 0);
			return 1;
		}
		break;

		case WM_CLOSE:
			EndDialog(hDlg, 0);
		break;
	}
	return FALSE;
}

extern int RichEditVersion(void)
{
	HMODULE hDll;
	DWORD dummy;
	TCHAR modulePath[MAX_PATH];
	DWORD result;
	DWORD size;
	BYTE* buffer;
	VS_FIXEDFILEINFO* vsInfo;
	UINT vsInfoSize;
	int FileVersionMinor;

	// try RE 1.0 DLL
	hDll = GetModuleHandle("riched32.dll");
	if (hDll) return 1;

	// try RE 2.0/3.0 DLL
	hDll = GetModuleHandle("riched20.dll");
	if (!hDll) return 0; // neither version is loaded

	// get the path to the DLL
	result = GetModuleFileName(hDll, modulePath,
		sizeof(modulePath) / sizeof(TCHAR));

	// if could not get module filename, fail
	if(!result) return -1;  // unexpected failure

	// get size of version information
	size = GetFileVersionInfoSize(modulePath, &dummy);
	if (!size) return -1;  // unexpected failure

	// allocate a buffer for version information
	buffer = HeapAlloc(GetProcessHeap(), 0, size);

	// get the version information
	if (!GetFileVersionInfo(modulePath, 0, size, buffer))
	{
		// delete[] buffer;
		return -1;  // unexpected faiure
	}

	// get the static version information
	if (!VerQueryValue(buffer, "\\", (LPVOID*) &vsInfo, &vsInfoSize)) {
		// delete[] buffer;
		return -1;  // unexpected failure
	}


	FileVersionMinor = LOWORD(vsInfo->dwFileVersionMS);

	// free the buffer
 	HeapFree(GetProcessHeap(), 0, buffer);

	return FileVersionMinor ? 3 : 2;
}

//=============================================================================
// Function.....: ShowPositionInSBAR
// Description..: Show current line and column in statusbar
// Parameters...: hwndRE   - Rich edit control handle.
// Return values: None
//=============================================================================
extern void ShowPositionInSBAR(HWND hwndRE)
{
	CHARRANGE chrg;
	LRESULT Line;
	LRESULT Column;
	CHAR	szString[30];

	// Get the line number.
	SendMessage(hwndRE, EM_EXGETSEL, 0, (LPARAM) &chrg);
	Line = SendMessage(hwndRE, EM_EXLINEFROMCHAR, 0, (LPARAM) chrg.cpMin);

	// Get the column
	Column = chrg.cpMin - SendMessage(hwndRE, EM_LINEINDEX, (WPARAM) Line, 0);
	wsprintf(szString, "Line: %d Column: %d", Line+1, Column+1);
	UpdateStatusBar(szString, 0, 0);
}

static DWORD CALLBACK WordCountCallBack(DWORD dwCookie, LPBYTE lpBuffer, LONG lSize, LONG *plRead)
{
	LPWORD_COUNT_DATA lpwcd;
	LONG c;
	TCHAR *p = lpBuffer;


	lpwcd = (LPWORD_COUNT_DATA) dwCookie;

	//if(!lSize) return -1;

	for(c = 0; c < lSize; c++)
	{
		lpwcd->dwChars++;
		if('\n' == *p)
			lpwcd->dwLines++;
		if(isspace(*p))
			lpwcd->bState = FALSE;
		else if(lpwcd->bState == FALSE)
		{
			lpwcd->bState = TRUE;
			lpwcd->dwWords++;
		}
		p++;
	}

	*plRead = c;
	return 0;
}

extern void WordCount(HWND hwndMain, HWND hwndRE)
{
	EDITSTREAM es;
	WORD_COUNT_DATA wcd;
	TCHAR szBuffer[50];

	wcd.dwLines = 1;
	wcd.dwWords = 0;
	wcd.dwChars = 0;
	wcd.bState  = 0;

	es.dwCookie = (DWORD) &wcd;
	es.dwError = 0;
	es.pfnCallback = WordCountCallBack;

	SendMessage(hwndRE, EM_STREAMOUT, SF_TEXT, (LPARAM) &es);
	wsprintf(szBuffer, "Lines: %d Words: %d Chars: %d", wcd.dwLines, wcd.dwWords, wcd.dwChars);
	MessageBox(hwndMain, szBuffer, "Word Count", MB_OK | MB_ICONINFORMATION);
}
