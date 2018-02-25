//=============================================================================
// TinyPad
//
// What's new in the 0.2 release:
//
// 0.2 18 Aug 2002
//
//     - New editor cmd in file menu.
//     - File properties in file menu.
//     - Date/Time cmd in edit menu.
//     - Word count cmd in edit menu.
//     - Full screen cmd in options menu.
//     - Always on top in options menu
//     - Line and column display.
//     - Splitted source code in the several files.
//     - Program can now load file as argument.
//	   + Fixed Drag and Drop bug.
//     + Fixed accelerator bug.
//
// 0.1 14 Aug 2002 - Initial release
//=============================================================================
#include <windows.h>
#include <richedit.h>
#include <commctrl.h>
#include <string.h>
#include <shlobj.h>
#include <shellapi.h>
#include "tinypadres.h"

//=============================================================================
// Global variables
//=============================================================================
HINSTANCE hInst;				// Instance handle
HWND hwndMain;					// Main window handle
HWND hEdit;						// Rich Edit control handle.
TCHAR szAppName[] = "Tinypad";	// Apps name used in globally.
CHAR szFileName[MAX_PATH]="";	// Loaded filename
BOOL bNeedSave = FALSE;			// File is changed = TRUE

extern CHAR FindBuffer[MAX_PATH];
extern FINDTEXTEX findtext;
extern HWND hWndStatusbar;

//=============================================================================
// Function prototypes
//=============================================================================
INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT);
static BOOL InitApplication(void);
static LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
static LRESULT CommandSelect(HWND hwnd, WPARAM wParam, LPARAM lParam);
void ErrorExit(LPCSTR msg);

// FILE.C
extern BOOL LoadFile(LPCSTR szFileName);
extern BOOL OpenDlg(LPCSTR buffer,INT buflen);
extern BOOL SaveDlg(LPSTR buffer,INT buflen);
extern BOOL SaveFile(LPCSTR szFileName);
extern BOOL Print(HWND hMain);
extern BOOL FileProperties(LPCSTR szFileName);

// OPTIONS.C
extern BOOL CALLBACK DlgOptions(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern void LoadSettings(void);
extern BOOL FullScreen(HWND hwnd);
extern BOOL WordWarp(void);

// SEARCH.C
extern BOOL CALLBACK DlgFind(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK DlgReplace(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK DlgGotoLine(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

// MISC.C
extern void UpdateStatusBar(LPSTR lpszStatusString, WORD partNumber, WORD displayFlags);
extern LRESULT MsgMenuSelect(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam);
extern void InitializeStatusBar(HWND hwndParent,int nrOfParts);
extern BOOL CreateSBar(HWND hwndParent,char *initialText,int nrOfParts);
extern SHORT AskToSave(HWND hwnd, TCHAR *szTitleName);
extern void DoCaption (HWND hwnd, TCHAR *szFileName);
extern void OkMessage(HWND hwnd, TCHAR *szMessage, TCHAR *szFileName);
extern LRESULT InitMenuPopup(WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK DlgDateTime(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern int RichEditVersion(void);
extern void ShowPositionInSBAR(HWND hwndRE);
extern void WordCount(HWND hwndMain, HWND hwndRE);

//=============================================================================
// Function.....: WinMain
// Description..: StartUp program.
// Parameters...: Standard.
// Return values: Zero if error.
//=============================================================================
#pragma argsused // No old parameters are used.
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, INT nCmdShow)
{
	MSG msg;
	HANDLE hAccelTable;
	BOOL bRet;

	hInst = hInstance;

	if (InitApplication() == FALSE)
		return 0;

	hAccelTable = LoadAccelerators(hInst,MAKEINTRESOURCE(IDACCEL));
	if(hAccelTable == NULL)
		ErrorExit("Application initialision error.");

	hwndMain = CreateWindowEx(WS_EX_ACCEPTFILES,"tinypadWndClass","tinypad",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,0,CW_USEDEFAULT,0, NULL, NULL, hInst, NULL);
	if(hwndMain == NULL) return 0;

	if(*lpCmdLine)
	{
		strncpy(szFileName, lpCmdLine, MAX_PATH);
		if(!LoadFile(szFileName))
		{
			OkMessage(hwndMain, TEXT("Could not read file %s!"), szFileName);
			szFileName[0] = '\0';
		}

		bNeedSave = FALSE;
	}
	CreateSBar(hwndMain,"Ready",1);
	DoCaption(hwndMain, szFileName);
	ShowWindow(hwndMain,SW_SHOW);

	while ( (bRet = GetMessage(&msg,NULL,0,0)) != 0)
	{
		// Get Message has fail
		if(bRet == -1) return 0;
		else
		{
			// Check for accelerator keystrokes.
			if (!TranslateAccelerator(hwndMain,hAccelTable,&msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return msg.wParam;
}

//=============================================================================
// Function.....: InitApplication
// Description..: Fill WNDCLASSEX structure with required variables
// Parameters...: None.
// Return values: True if success.
//=============================================================================
static BOOL InitApplication(void)
{
	WNDCLASS wc;

	memset(&wc,0,sizeof(WNDCLASS));
	wc.style = CS_HREDRAW|CS_VREDRAW |CS_DBLCLKS ;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.hInstance = hInst;
	wc.hbrBackground = (HBRUSH)(COLOR_MENU+1);
	wc.lpszClassName = "tinypadWndClass";
	wc.lpszMenuName = MAKEINTRESOURCE(IDMAINMENU);
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(ID_ICON));
	if (!RegisterClass(&wc))
		return 0;

	LoadSettings();
	return 1;
}

//=============================================================================
// Function.....: MainWndProc
// Description..: Handle all messages.
// Parameters...: Standard.
// Return values: Depends message.
//=============================================================================
static LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	static HANDLE hRichDLL;

	switch (msg) {

	case WM_NOTIFY:
	{
			NMHDR *pnmh = (NMHDR *) lParam;
			if(pnmh->idFrom == EDITID && pnmh->code == EN_SELCHANGE)
				ShowPositionInSBAR(hEdit);
	}
	break;

	case WM_CREATE:
		hRichDLL = LoadLibrary("Riched20.dll");
		hEdit = CreateWindowEx(0, RICHEDIT_CLASS, 0, WS_VISIBLE | ES_SUNKEN |
			WS_CHILDWINDOW | WS_CLIPSIBLINGS | ES_MULTILINE | WS_CHILD |
		    ES_NOHIDESEL  |  WS_VSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_HSCROLL ,
			0, 0, 0, 0, hwnd, (HMENU) EDITID, hInst, NULL);
		if(hEdit == NULL) MessageBox(hwnd, "Rick edit null", "Error", MB_OK);
		DragAcceptFiles(hwnd, TRUE);
		DoCaption(hwnd, szFileName);
		SendMessage(hEdit, EM_SETLIMITTEXT, 0, 0);
		SendMessage(hEdit, EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE | EN_MSGFILTER);
	break;

	case WM_SIZE:
	{
		RECT rect;
		INT  nHeight = HIWORD(lParam);

		SendMessage(hWndStatusbar,msg,wParam,lParam);
		InitializeStatusBar(hWndStatusbar,1);
		GetClientRect(hWndStatusbar, &rect);
		nHeight = nHeight - rect.bottom;
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), nHeight, TRUE);
	}
	break;

	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP) wParam;
		TCHAR szNewFile[MAX_PATH];
		DragQueryFile(hDrop, 0, szNewFile, MAX_PATH);
		DragFinish(hDrop);
		MessageBox(hwnd, szNewFile,"DEBUG", MB_OK);
		if(!LoadFile(szNewFile))
		{
			OkMessage(hwnd, TEXT("Could not read file %s!"), szNewFile);
			szNewFile[0] = '\0';
		}else
			strcpy(szFileName, szNewFile);
		DoCaption(hwnd, szFileName);
		bNeedSave = FALSE;
	}
	break;

	case WM_INITMENUPOPUP:
	return InitMenuPopup(wParam, lParam);

	case WM_MENUSELECT:
	return MsgMenuSelect(hwnd,msg,wParam,lParam);

	case WM_COMMAND:
	return CommandSelect(hwnd, wParam, lParam);

	case WM_SETFOCUS:
		SetFocus(hEdit);
	break;

	case WM_CLOSE:
		if(!bNeedSave || IDCANCEL != AskToSave(hwnd, szFileName))
			DestroyWindow(hwnd);
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
	break;

	default:
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return 0;
}

//=============================================================================
// Function.....: CommandSelect
// Description..: Handle all commands.
// Parameters...: Win handle and WPARAM and LPARAM
// Return values: 0
//=============================================================================
static LRESULT CommandSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	char buffer[MAX_PATH];

	if(lParam && LOWORD(wParam) == EDITID)
	{
		switch(HIWORD(wParam))
		{
			case EN_CHANGE:
				bNeedSave = TRUE;
			break;
		}
	}

	switch(LOWORD(wParam)) {

		case IDM_NEW:
			if(bNeedSave && IDCANCEL == AskToSave(hwnd, szFileName))
				return 0;

			SendMessage(hEdit, WM_SETTEXT, 0, 0);
			szFileName[0] = 0;
			SendMessage(hEdit, EM_SETMODIFY, 0, 0);
			DoCaption(hwnd, szFileName);
			bNeedSave = FALSE;
		break;

		case IDM_NEWEDITOR:
		{
			TCHAR szModule[MAX_PATH];

			GetModuleFileName(NULL, szModule, MAX_PATH);
			WinExec(szModule, SW_SHOW);
		}
		break;

		case IDM_PROPERTIES:
			FileProperties(szFileName);
		break;

		case IDM_DATETIME:
			DialogBox(hInst, MAKEINTRESOURCE(DLG_DATETIME), hwnd, DlgDateTime);
		break;

		case IDM_FULLSCREEN:
			FullScreen(hwnd);
		break;

		case IDM_ALWAYSONTOP:
		{
			LONG style;

			style = GetWindowLong(hwndMain, GWL_EXSTYLE);
			if(style & WS_EX_TOPMOST)
			{
				SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			else
			{
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE	| SWP_NOSIZE);
			}
		}
		break;

		case IDM_OPEN:
			if(bNeedSave && IDCANCEL == AskToSave(hwnd, szFileName))
				return 0;
			if(OpenDlg(buffer,sizeof(buffer)))
				if(!LoadFile(buffer))
				{
					OkMessage(hwnd, TEXT("Could not read file %s!"),
					szFileName);
					szFileName[0] = '\0';
				}
				else
				{
					strcpy(szFileName, buffer);
					SHAddToRecentDocs(SHARD_PATH, szFileName);
				}
			DoCaption(hwnd, szFileName);
			bNeedSave = FALSE;
		break;

		case IDM_SAVE:
			if(szFileName[0])
			{
				if(SaveFile(buffer))
				{
					bNeedSave = FALSE;
					return 0;
				}
				else
				{
					OkMessage(hwnd, TEXT("Could not write file %s"), szFileName);
					return 0;
				}
			}

		// Fall throut
		case IDM_SAVEAS:
			if(SaveDlg(szFileName, MAX_PATH))
			{
				DoCaption(hwnd, szFileName);

				if(SaveFile(szFileName))
				{
					bNeedSave = FALSE;
					return 0;
				}
				else
				{
					OkMessage(hwnd, TEXT("Could not save file %s!"), szFileName);
					return 0;
				}
			}
		break;

		case IDM_PRINT:
			Print(hwnd);
		break;

		case IDM_WORDWARP:
			WordWarp();
		break;

		case IDM_UNDO:
			SendMessage(hEdit, WM_UNDO, 0, 0);
		break;

		case IDM_CUT:
			SendMessage(hEdit, WM_CUT, 0, 0);
		break;

		case IDM_COPY:
			SendMessage(hEdit, WM_COPY, 0, 0);
		break;

		case IDM_PASTE:
			SendMessage(hEdit, EM_PASTESPECIAL, CF_TEXT, 0);
		break;

		case IDM_DELETE:
			SendMessage(hEdit, WM_CLEAR, 0, 0);
		break;

		case IDM_SELECTALL:
			SendMessage(hEdit, EM_SETSEL, 0, -1);
		break;

		case IDM_WORDCOUNT:
			WordCount(hwnd, hEdit);
		break;

		case IDM_FIND:
			DialogBox(hInst, MAKEINTRESOURCE(DLG_FIND), hwnd, DlgFind);
		break;

		case IDM_REPLACE:
			DialogBox(hInst, MAKEINTRESOURCE(DLG_REPLACE), hwnd, DlgReplace);
		break;

		case IDM_FINDNEXT:
			if(lstrlen(FindBuffer))
			{
				SendMessage(hEdit, EM_EXGETSEL, 0, (LPARAM) &findtext.chrg);
				if(findtext.chrg.cpMin != findtext.chrg.cpMax)
					findtext.chrg.cpMin = findtext.chrg.cpMax;
				findtext.chrg.cpMax = -1;
				findtext.lpstrText = FindBuffer;
				if(-1 != SendMessage(hEdit, EM_FINDTEXTEX, FR_DOWN, (LPARAM) &findtext))
					SendMessage(hEdit, EM_EXSETSEL, 0, (LPARAM) &findtext.chrgText);
			}
		break;

		case IDM_GOTOLINE:
			DialogBox(hInst, MAKEINTRESOURCE(DLG_GOTOLINE), hwnd, DlgGotoLine);
		break;

		case IDM_OPTIONS:
			DialogBox(hInst, MAKEINTRESOURCE(DLG_OPTIONS), hwnd, DlgOptions);
		break;

		case IDM_EXIT:
			SendMessage(hwnd,WM_CLOSE,0,0);
		break;

		case IDM_ABOUT:
			MessageBox(hwnd, "Tinypad - Version 0.2\n", "About", MB_OK);
		break;
	}
	return 0;
}

//=============================================================================
// Function.....: ErrorExit
// Description..: Show error msg and shutdown the program.
// Parameters...: Message, NULL if no msg.
// Return values: None
//=============================================================================
void ErrorExit(LPCSTR msg)
{
	if(msg != NULL)
		MessageBox(hwndMain, msg, "Tinypad - Error", MB_OK | MB_ICONERROR);
	ExitProcess(1);
}
