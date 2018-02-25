/*********************************************************************
				Main.c - Main Source File for Small Edit

	This module contains the main source file for creating and running
	procedures for the main window GUI.  This module creates the
	class, main window, and child windows and then runs the
	appropiate window procedures.

	Re-writing for UNICODE (load, save, windowing)
	Bidi support for Hebrew and Arabic
	J.laporte 0ct 2002.

	Global Variables:
		hEdit - Handle to the rich edit control
		Font - Font option structure
		Info - Info information structure
		Find - Find and replace information strucutre

	Global Functions:
		WinMain - Main windows procedure
		WndMainProc - Main window procedure
*********************************************************************/
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include <richedit.h>
#include <direct.h>
#include <shellapi.h>
#include <stdio.h>


#pragma lib <shell32.lib>

#include "Resource.h"
#include "Main.h"
#include "Global.h"
#include "Menu.h"


static HINSTANCE ghLibrary = NULL;
static HINSTANCE ghInstance = NULL;
static HWND hwndMain = NULL;
static HANDLE hTool = NULL;
static HANDLE hStatus = NULL;
static WNDPROC DefaultRichEditProc;



INFO Info;
HANDLE hEdit = NULL;
FONT Font;
FONT FontEdit;
HANDLE hWndEdit;
FIND Find;


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	MSG Msg;
	HANDLE hAccel = NULL;

	ghInstance = hInstance;

	LoadOptions();

	ZeroMemory(&Info, sizeof(INFO));
	ZeroMemory(&Find, sizeof(FIND));

	ghLibrary = LoadLibrary(TEXT("Riched20.dll"));


	hAccel = LoadAccelerators (hInstance, MAKEINTRESOURCE(IDACCEL)) ;

	if(RegisterWndClass()==FAILURE){
		return 0;
	}

	if(CreateMainWnd(nCmdShow)==FAILURE){
		return 0;
	}

	while(GetMessage(&Msg, NULL, 0, 0) > 0){
		if(!TranslateAccelerator (hwndMain, hAccel, &Msg)){
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}

	UnRegisterWnd();

	return Msg.wParam;
}

/*------------------------------------------------------------------------
 Procedure:     RegisterWndClass ID:1
 Purpose:       Fills the window class and registers the class with
				windows.  If the class cannot be registered an error
				is displayed.
 Input:         None.
 Output:        Success or failure.
 Errors:        Failed window class registration results in an error
				box and termination of the program.
------------------------------------------------------------------------*/
int static RegisterWndClass(void){
	WNDCLASSEX wc;

	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc		= WndMainProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= ghInstance;
	wc.hIcon			= LoadIcon(ghInstance, MAKEINTRESOURCE(ICON_DEFAULT));
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground 	= (HBRUSH)(COLOR_WINDOW+11);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= WNDCLASSNAME;
	wc.hIconSm			= NULL;

	if(RegisterClassEx(&wc)==0){
		MSGBOX(NULL, ERR_REGCLASS);
		return FAILURE;
	}
	return SUCCESS;
}

/*------------------------------------------------------------------------
 Procedure:     CreateMainWnd ID:1
 Purpose:       Creates, updates, and shows the main window.  If an
				error in creation occurs an error message is shown
				and the program terminates.
 Input:         Integer specifying how the window should be shown.
 Output:        Success or failure.
 Errors:        If creation fails an error message is shown and the
				program terminates.
------------------------------------------------------------------------*/
int static CreateMainWnd(int nCmdShow){

	hwndMain = CreateWindow(WNDCLASSNAME,
							WND_TITLE,
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT, CW_USEDEFAULT, WND_WIDTH, WND_HEIGHT,
							NULL, NULL, ghInstance, NULL);

	if(hwndMain == NULL){
		ERRBOX(NULL, ERR_WND);
		return FAILURE;
	}

	ShowWindow(hwndMain, nCmdShow);
	UpdateWindow(hwndMain);

	InitCommonControls();

	return SUCCESS;
}

/*------------------------------------------------------------------------
 Procedure:     CreateChildWnd ID:1
 Purpose:       Create the edit control and status bar.  Calls the
				function to create the tool bar.  If one of these
				child window fails to be created an error message is
				displayed and the program terminates.
 Input:         Handle of the main window.
 Output:        None.
 Errors:        If creation fails an error message is displayed and
				the program terminates.
------------------------------------------------------------------------*/
void static CreateChildWnd(HWND hwnd){
	LONG lAlign;

	if(Font.Wrap==TRUE){
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, TEXT(""),
							WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL,
							0, 40, 100, 100, hwnd, NULL, ghInstance, NULL);
	}
	else{
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, TEXT(""),
							WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL | WS_HSCROLL | ES_AUTOHSCROLL,
							0, 40, 100, 100, hwnd, NULL, ghInstance, NULL);
	}

	hStatus = CreateWindowEx(0, STATUSCLASSNAME, TEXT(""),
							SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE,
							0, 0, 0, 0, hwnd, NULL, ghInstance, NULL);

	CreateToolBar(hwnd);


	if(hEdit==NULL||hStatus==NULL||hTool==NULL){
		ERRBOX(hwnd, ERR_CHILD);
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}

	Font.hFont = CreateFont(Font.Size, 0, 0, 0, (Font.Bold ? 700 : 0), Font.Italic, 0, 0, 0, 0, 0, 0, 0, Font.Name);
	if(Font.hFont==NULL){
		ERRBOX(hwnd, ERR_FONT);
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}
	SendMessage(hEdit, WM_SETFONT, (UINT)Font.hFont, MAKELPARAM(TRUE, 0));

	SendMessage(hEdit, EM_EXLIMITTEXT, 0, MAX_EDIT_CHAR);

	DefaultRichEditProc = (WNDPROC)SetWindowLong(hEdit, GWL_WNDPROC, (LONG)RichEditProc);
}

/*------------------------------------------------------------------------
 Procedure:     CreateToolBar ID:1
 Purpose:       Creates the tool bar and loads the bitmap and buttons
				for it.
 Input:         The handle of the main window.
 Output:        None.
 Errors:        None.
------------------------------------------------------------------------*/
void static CreateToolBar(HWND hwnd){
	TBADDBITMAP Bitmap;
	TBBUTTON Button[12];

	hTool = CreateWindowEx(0, TOOLBARCLASSNAME, TEXT(""),
						WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
						0, 0, 0, 0,	hwnd, NULL,	ghInstance,	NULL);

	SendMessage(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);

	Bitmap.hInst = HINST_COMMCTRL;
	Bitmap.nID = IDB_STD_SMALL_COLOR;

	SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM) &Bitmap);

	ZeroMemory(Button, sizeof(Button));

	Button[0].iBitmap   = STD_FILENEW;
	Button[0].fsState   = TBSTATE_ENABLED;
	Button[0].fsStyle   = TBSTYLE_BUTTON;
	Button[0].idCommand = MENU_FILE_NEW;

	Button[1].iBitmap   = STD_FILEOPEN;
	Button[1].fsState   = TBSTATE_ENABLED;
	Button[1].fsStyle   = TBSTYLE_BUTTON;
	Button[1].idCommand = MENU_FILE_OPEN;

	Button[2].iBitmap   = STD_FILESAVE;
	Button[2].fsState   = TBSTATE_ENABLED;
	Button[2].fsStyle   = TBSTYLE_BUTTON;
	Button[2].idCommand = MENU_FILE_SAVE;

	Button[3].iBitmap   = STD_PRINT;
	Button[3].fsState   = TBSTATE_ENABLED;
	Button[3].fsStyle   = TBSTYLE_BUTTON;
	Button[3].idCommand = MENU_FILE_PRINT;

	Button[4].fsStyle   = TBSTYLE_SEP;

	Button[5].iBitmap   = STD_CUT;
	Button[5].fsState   = TBSTATE_ENABLED;
	Button[5].fsStyle   = TBSTYLE_BUTTON;
	Button[5].idCommand = MENU_EDIT_CUT;

	Button[6].iBitmap   = STD_COPY;
	Button[6].fsState   = TBSTATE_ENABLED;
	Button[6].fsStyle   = TBSTYLE_BUTTON;
	Button[6].idCommand = MENU_EDIT_COPY;

	Button[7].iBitmap   = STD_PASTE;
	Button[7].fsState   = TBSTATE_ENABLED;
	Button[7].fsStyle   = TBSTYLE_BUTTON;
	Button[7].idCommand = MENU_EDIT_PASTE;

	Button[8].fsStyle   = TBSTYLE_SEP;

	Button[9].iBitmap   = STD_UNDO;
	Button[9].fsState   = TBSTATE_ENABLED;
	Button[9].fsStyle   = TBSTYLE_BUTTON;
	Button[9].idCommand = MENU_EDIT_UNDO;

	Button[10].iBitmap   = STD_FIND;
	Button[10].fsState   = TBSTATE_ENABLED;
	Button[10].fsStyle   = TBSTYLE_BUTTON;
	Button[10].idCommand = MENU_SEARCH_FIND;

	Button[11].iBitmap   = STD_REPLACE;
	Button[11].fsState   = TBSTATE_ENABLED;
	Button[11].fsStyle   = TBSTYLE_BUTTON;
	Button[11].idCommand = MENU_SEARCH_REPLACE;


	SendMessage(hTool, TB_ADDBUTTONS, sizeof(Button)/sizeof(Button[0]), (LPARAM) &Button);
}

/*------------------------------------------------------------------------
 Procedure:     CreateMenuSystem ID:1
 Purpose:       Creates the main windows menus.
 Input:         The handle to the main window.
 Output:        None.
 Errors:        None.
------------------------------------------------------------------------*/
void static CreateMenuSystem(HWND hwnd){
	HMENU hMenu;
	HMENU hSubMenu;
	HMENU hSubSub;

	hMenu = CreateMenu();

	hSubMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&File"));
	AppendMenu(hSubMenu, MF_STRING, MENU_FILE_NEW, TEXT("&New\tCtrl+N"));
	AppendMenu(hSubMenu, MF_STRING, MENU_FILE_OPEN, TEXT("&Open...\tCtrl+O"));
	AppendMenu(hSubMenu, MF_SEPARATOR, 0, TEXT(""));
	AppendMenu(hSubMenu, MF_STRING, MENU_FILE_SAVE, TEXT("&Save\tCtrl+S"));
	AppendMenu(hSubMenu, MF_STRING, MENU_FILE_SAVEAS, TEXT("Save &as..."));
	AppendMenu(hSubMenu, MF_SEPARATOR, 0, TEXT(""));
	AppendMenu(hSubMenu, MF_STRING, MENU_FILE_PRINT, TEXT("&Print\tCtrl+P"));
	AppendMenu(hSubMenu, MF_SEPARATOR, 0, TEXT(""));
	AppendMenu(hSubMenu, MF_STRING, MENU_FILE_EXIT, TEXT("E&xit\tCtrl+Q"));


	hSubMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&Edit"));
	AppendMenu(hSubMenu, MF_STRING, MENU_EDIT_UNDO, TEXT("&Undo\tCtrl+Z"));
	AppendMenu(hSubMenu, MF_SEPARATOR, 0, TEXT(""));
	AppendMenu(hSubMenu, MF_STRING, MENU_EDIT_CUT, TEXT("Cu&t\tCtrl+X"));
	AppendMenu(hSubMenu, MF_STRING, MENU_EDIT_COPY, TEXT("&Copy\tCtrl+C"));
	AppendMenu(hSubMenu, MF_STRING, MENU_EDIT_PASTE, TEXT("&Paste\tCtrl+V"));
	AppendMenu(hSubMenu, MF_STRING, MENU_EDIT_DEL, TEXT("&Delete\tDel"));
	AppendMenu(hSubMenu, MF_SEPARATOR, 0, TEXT(""));
	AppendMenu(hSubMenu, MF_STRING, MENU_EDIT_SEL, TEXT("Select &All\tCtrl+A"));


	hSubMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&Search"));
	AppendMenu(hSubMenu, MF_STRING, MENU_SEARCH_FIND, TEXT("&Find\tAlt+F3"));
	AppendMenu(hSubMenu, MF_STRING, MENU_SEARCH_NEXT, TEXT("Find &Next\tF3"));
	AppendMenu(hSubMenu, MF_SEPARATOR, 0, TEXT(""));
	AppendMenu(hSubMenu, MF_STRING, MENU_SEARCH_REPLACE, TEXT("&Replace"));

	hSubSub = CreatePopupMenu();

	AppendMenu(hSubMenu, MF_POPUP, (UINT)hSubSub, TEXT("&Goto"));
	AppendMenu(hSubSub, MF_STRING, MENU_SEARCH_TOP, TEXT("&Top\tCtrl+Home"));
	AppendMenu(hSubSub, MF_STRING, MENU_SEARCH_END, TEXT("&End\tCtrl+End"));
	AppendMenu(hSubSub, MF_STRING, MENU_SEARCH_LINE, TEXT("&Line\tCtrl+G"));


	hSubMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&Format"));
	AppendMenu(hSubMenu, MF_STRING, MENU_FMAT_WRAP, TEXT("&Word Wrap"));
	AppendMenu(hSubMenu, MF_STRING, MENU_FMAT_FONT, TEXT("&Font"));

	hSubMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&Help"));
	AppendMenu(hSubMenu, MF_STRING, MENU_HELP_TOPIC, TEXT("&Help Topics\tF1"));

	AppendMenu(hSubMenu, MF_STRING, MENU_HELP_SITE, TEXT("&Web Site"));

	AppendMenu(hSubMenu, MF_SEPARATOR, 0, TEXT(""));
	AppendMenu(hSubMenu, MF_STRING, MENU_HELP_ABOUT, TEXT("&About"));


	AppendMenu(hSubMenu, MF_STRING, MENU_HELP_VERSION, TEXT("&Version"));

	SetMenu(hwnd, hMenu);
}

/*------------------------------------------------------------------------
 Procedure:     ResizeWnd ID:1
 Purpose:       Resizes the controls in the main window each time the
				WM_SIZE message is recieved.
 Input:         The handle to the main window.
 Output:        None.
 Errors:        None.
------------------------------------------------------------------------*/
void static ResizeWnd(HWND hwnd){
	RECT rcClient;
	RECT rcTool;
	int iToolHeight;
	RECT rcStatus;
	int iStatusHeight;
	int iEditHeight;

	SendMessage(hTool, TB_AUTOSIZE, 0, 0);

	GetWindowRect(hTool, &rcTool);
	iToolHeight = rcTool.bottom - rcTool.top;

	SendMessage(hStatus, WM_SIZE, 0, 0);

	GetWindowRect(hStatus, &rcStatus);
	iStatusHeight = rcStatus.bottom - rcStatus.top;

	int statwidths[] = {200, -1};
	statwidths[0]=rcStatus.right-rcStatus.left-100;
	SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);

	GetClientRect(hwnd, &rcClient);

	iEditHeight = rcClient.bottom - iToolHeight - iStatusHeight;

	SetWindowPos(hEdit, NULL, 0, iToolHeight, rcClient.right, iEditHeight, SWP_NOZORDER);
}

void static UnRegisterWnd(void){
	UnregisterClass(WNDCLASSNAME, ghInstance);
	FreeLibrary(ghLibrary);
	DeleteObject(Font.hFont);
	SaveOptions();
}

/*------------------------------------------------------------------------
 Procedure:     CloseWnd ID:1
 Purpose:       Called when the main window recieves the WM_CLOSE
				command.  Validates the file is saved and then
				destroys the main window.
 Input:         Handle to the main window.
 Output:        None.
 Errors:        None.
------------------------------------------------------------------------*/
void static CloseWnd(HWND hwnd){
	TCHAR  Buffer[MAX_NAME];
	int Answer = 0;

	if(SendMessage(hEdit, EM_GETMODIFY, 0, 0)){
		_sntprintf(Buffer, MAX_NAME, TEXT("The text in %s has changed.\nWould you like to save it?"), Info.FileName);
		Answer = MessageBox(hwnd, Buffer, WND_TITLE, MB_YESNOCANCEL | MB_ICONQUESTION);
		switch(Answer){
			case IDYES:
				Save(hwnd);
				break;
			case IDCANCEL:
				return;
		}
	}
	WinHelp(hwnd, HELP_FILE, HELP_QUIT, 0);
	DestroyWindow(hwnd);
}

LRESULT CALLBACK WndMainProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){


	TCHAR  Buffer[MAX_NAME];

	Update(hwnd);


	switch(uMsg)
	{

	    case WM_CREATE:
			hwndMain = hwnd;
			CreateChildWnd(hwnd);
			CreateMenuSystem(hwnd);
			New(hwnd);
			break;
		case WM_SETFOCUS:
			SetFocus(hEdit);
			break;
		case WM_CLOSE:
			CloseWnd(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)){

				case MENU_FILE_NEW:
					New(hwnd);
					break;
				case MENU_FILE_OPEN:
					Open(hwnd);
					break;
				case MENU_FILE_SAVE:
					Save(hwnd);
					break;
				case MENU_FILE_SAVEAS:
					SaveAs(hwnd);
					break;
				case MENU_FILE_PRINT:
					Print(hwnd);
					break;
				case MENU_FILE_EXIT:
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				case MENU_EDIT_UNDO:
					SendMessage(hEdit, WM_UNDO, 0, 0);
					break;
				case MENU_EDIT_CUT:
					SendMessage(hEdit, WM_CUT, 0, 0);
					break;
				case MENU_EDIT_COPY:
					SendMessage(hEdit, WM_COPY, 0, 0);
					break;
				case MENU_EDIT_PASTE:
					SendMessage(hEdit, WM_PASTE, 0, 0);
					break;
				case MENU_EDIT_DEL:
					SendMessage(hEdit, WM_CLEAR, 0, 0);
					break;
				case MENU_EDIT_SEL:
					SelectAll(hwnd);
					break;
				case MENU_SEARCH_FIND:
					DialogBox(ghInstance, MAKEINTRESOURCE(DLG_FIND), hwnd, DlgFindProc);
					break;
				case MENU_SEARCH_NEXT:
					FindNext(hwnd);
					break;
				case MENU_SEARCH_REPLACE:
					DialogBox(ghInstance, MAKEINTRESOURCE(DLG_REPLACE), hwnd, DlgReplaceProc);
					break;
				case MENU_SEARCH_TOP:
					ScrollTop(hwnd, TRUE);
					break;
				case MENU_SEARCH_END:
					ScrollTop(hwnd, FALSE);
					break;
				case MENU_SEARCH_LINE:
					DialogBox(ghInstance, MAKEINTRESOURCE(DLG_LINE), hwnd, DlgLineProc);
					break;
				case MENU_FMAT_WRAP:
					if(WordWrap(hwnd)==FAILURE){
						break;
					}
					DefaultRichEditProc = (WNDPROC)SetWindowLong(hEdit, GWL_WNDPROC, (LONG)RichEditProc);
					ResizeWnd(hwnd);
					SetFocus(hEdit);
					Update(hwnd);
					break;
				case MENU_FMAT_FONT:
					SelectFontDlg(hwnd);
					break;
				case MENU_HELP_TOPIC:
					WinHelp(hwnd, HELP_FILE, HELP_FINDER, 0);
					break;
				case MENU_HELP_SITE:
					ShellExecute(hwnd, TEXT("open"), WEB_SITE, NULL, NULL, SW_SHOWNORMAL);
					break;
				case MENU_HELP_ABOUT:
					DialogBox(ghInstance, MAKEINTRESOURCE(DLG_ABOUT), hwnd, DlgAboutProc);
					break;
				case MENU_HELP_VERSION:
					_sntprintf(Buffer, MAX_NAME, TEXT("%s\n"), VERSION_INFO);
					MSGBOX(hwnd, Buffer);
					break;
			}
			break;
		case WM_SIZE:
			ResizeWnd(hwnd);
			break;
		case WM_NOTIFY:
			switch(((LPNMHDR) lParam)->code){
				case TTN_GETDISPINFO:
					{
						LPTOOLTIPTEXT lpttt;

						lpttt = (LPTOOLTIPTEXT) lParam;
						lpttt->hinst = ghInstance;

							switch (lpttt->hdr.idFrom){
								case MENU_FILE_NEW:
									lpttt->lpszText = MAKEINTRESOURCE(IDM_NEW);
									break;
								case MENU_FILE_OPEN:
									lpttt->lpszText = MAKEINTRESOURCE(IDM_OPEN);
									break;
								case MENU_FILE_SAVE:
									lpttt->lpszText = MAKEINTRESOURCE(IDM_SAVE);
									break;
								case MENU_FILE_PRINT:
									lpttt->lpszText = MAKEINTRESOURCE(IDM_PRINT);
									break;
								case MENU_EDIT_CUT:
									lpttt->lpszText = MAKEINTRESOURCE(IDM_CUT);
									break;
								case MENU_EDIT_COPY:
									lpttt->lpszText = MAKEINTRESOURCE(IDM_COPY);
									break;
								case MENU_EDIT_PASTE:
									lpttt->lpszText = MAKEINTRESOURCE(IDM_PASTE);
									break;
								case MENU_EDIT_UNDO:
									lpttt->lpszText = MAKEINTRESOURCE(IDM_UNDO);
									break;
								case MENU_SEARCH_FIND:
									lpttt->lpszText = MAKEINTRESOURCE(IDM_FIND);
									break;
								case MENU_SEARCH_REPLACE:
									lpttt->lpszText = MAKEINTRESOURCE(IDM_REPLACE);
									break;
								case MENU_HELP_TOPIC:
									lpttt->lpszText = MAKEINTRESOURCE(IDM_HELP);
									break;
							}
					break;
				}
			default:
				break;

		}
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK RichEditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

static PARAFORMAT * lpFmt = NULL;

UINT Size_Fmt;

	switch(uMsg){

		case WM_INPUTLANGCHANGE:
			CharSet = wParam;
			Size_Fmt = sizeof(PARAFORMAT);
			lpFmt = malloc(Size_Fmt);
            lpFmt -> cbSize = Size_Fmt;

			SendMessage(hwnd, EM_GETPARAFORMAT,(WPARAM) 0, (LPARAM) (PARAFORMAT FAR *) lpFmt);

			if (CharSet==HEBREW_CHARSET || CharSet==ARABIC_CHARSET) {
				lpFmt -> dwMask = PFM_ALIGNMENT;
				lpFmt -> wAlignment = PFA_RIGHT;
			}
			else {
				lpFmt -> dwMask = PFM_ALIGNMENT;
				lpFmt -> wAlignment = PFA_LEFT;
			}

			SendMessage(hwnd, EM_SETPARAFORMAT,(WPARAM) 0, (LPARAM) (PARAFORMAT FAR *) lpFmt);

		break;
		case WM_RBUTTONUP:
			PopUpMenu(hwnd, LOWORD(lParam), HIWORD(lParam));
			return 0;
	}
	return CallWindowProc(DefaultRichEditProc, hwnd, uMsg, wParam, lParam) ;
}

/*------------------------------------------------------------------------
 Procedure:     Update ID:1
 Purpose:       Updates the status bar and menu system each time a
				message is recieved by the main window.
 Input:         The handle to the main window.
 Output:        None.
 Errors:        None.
------------------------------------------------------------------------*/
void static Update(HWND hwnd){
	TCHAR  Buffer[MAX_FILE];
	TCHAR  CurText[MAX_FILE];
	CHARRANGE cr;
	UINT Line = 0;
	HMENU hMenu;
	HMENU hSubMenu;

	SendMessage(hStatus, SB_GETTEXT, 0, (WPARAM)(LPSTR)CurText);
	if(lstrcmp(CurText, Info.FileName)!=0){
		SendMessage(hStatus, SB_SETTEXT, 0, (WPARAM) Info.FileName);
	}

	SendMessage(hEdit, EM_EXGETSEL, 0, (WPARAM)&cr);
	Line = SendMessage(hEdit, EM_EXLINEFROMCHAR, 0, cr.cpMin)+1;
	_sntprintf(Buffer, MAX_FILE, TEXT("%d"), Line);


	SendMessage(hStatus, SB_GETTEXT, 1, (WPARAM) (LPSTR) CurText);
	if(lstrcmp(Buffer, CurText)!=0){
		SendMessage(hStatus, SB_SETTEXT, 1, (WPARAM) (LPSTR)Buffer);
	}

	hMenu = GetMenu(hwnd);
	hSubMenu = GetSubMenu(hMenu, 3);
	switch(GetMenuState(hSubMenu, MENU_FMAT_WRAP, MF_BYCOMMAND)){
		case MF_CHECKED:
			switch(Font.Wrap){
				case TRUE:
					return;
				case FALSE:
					CheckMenuItem(hSubMenu, MENU_FMAT_WRAP, MF_UNCHECKED);
					return;
			}
			break;
		default:
			switch(Font.Wrap){
				case TRUE:
					CheckMenuItem(hSubMenu, MENU_FMAT_WRAP, MF_CHECKED);
					return;
				case FALSE:
					return;
			}
			break;
	}
}

/*------------------------------------------------------------------------
 Procedure:     PopUpMenu ID:1
 Purpose:       Creates a popup menu when the user right clicks the
				richedit control.
 Input:         The handle to the main window and the x and y
				coordinates to create the menu at.
 Output:        None.
 Errors:        None.
------------------------------------------------------------------------*/
void static PopUpMenu(HWND hwnd, int X, int Y){
	POINT point;
	HMENU hMenu = GetMenu(hwndMain);
	HMENU hSubMenu = GetSubMenu(hMenu, 1);

	point.x = X;
	point.y = Y;

	if(hMenu==NULL||hSubMenu==NULL){
		return;
	}

	ClientToScreen(hwnd, &point);
	TrackPopupMenu(hSubMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwndMain, NULL);
}
