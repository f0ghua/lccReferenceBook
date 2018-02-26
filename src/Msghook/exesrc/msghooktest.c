#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

#include "msghooktest.h"

// ------------------------------------------------------------------------------------------------
// WM_COPYDATA MESSAGE CRACKER - per Jeff Richter
//
#define HANDLE_WM_COPYDATA(hwnd, wParam, lParam, fn) \
	((fn)((hwnd), (HWND)(wParam), (PCOPYDATASTRUCT)lParam), 0L)

// ------------------------------------------------------------------------------------------------
// hooking module passes these as blocks of memory

typedef struct tagMsgData {
	MSG msg;
	TCHAR szModName[MAX_PATH];
}
MsgData;


// ------------------------------------------------------------------------------------------------
// for use with LoadLibrary and GetProcAddress
//
typedef void (_stdcall *UnsetMsgHookT)(void);
typedef int  (_stdcall *SetMsgHookT)(HWND);

UnsetMsgHookT pfnUnsetMsgHook;
SetMsgHookT pfnSetMsgHook;

// msghook.dll
static HINSTANCE g_hInstLib;

// ---------------------------------------------------------------------------
// window class name
static LPCTSTR g_pszWndClass = _T("MsgHookTestClass");

// global handle to listbox
static HWND g_hListBox;


// ---------------------------------------------------------------------------
// create the child listbox
static BOOL  _stdcall MainWndProc_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	// setup child windows

	RECT rc;
	GetClientRect(hwnd, &rc);

	g_hListBox = CreateWindowEx(
                        0x0L,
                        _T("ListBox"),
                        _T(""),
                        WS_CHILD | WS_VISIBLE | WS_VSCROLL,
						rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top,
						hwnd,
						(HMENU)ID_LIST,
						GetModuleHandle(NULL),
						NULL
                        );

	if ( NULL == g_hListBox ) {
		return TRUE;
	}

	SendMessage(g_hListBox, WM_SETFONT, (WPARAM)GetStockObject(ANSI_FIXED_FONT), FALSE);

	return FALSE;

}


// ---------------------------------------------------------------------------
// unset the hook and free the dll
static void _stdcall MainWndProc_OnClose(HWND hWnd)
{

    if ( NULL != pfnUnsetMsgHook ) {
        pfnUnsetMsgHook();
    }

	if ( NULL != g_hInstLib ) {
		FreeLibrary(g_hInstLib);
	}

	DestroyWindow(hWnd);

}

// ---------------------------------------------------------------------------
// Resize the listbox to fill the client area
static void _stdcall MainWndProc_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	if ( SIZE_RESTORED == state || SIZE_MAXIMIZED == state ) {

		SetWindowPos(
            GetDlgItem(hwnd, ID_LIST),
			NULL, 0, 0, cx, cy,
			SWP_NOACTIVATE | SWP_NOZORDER
            );

	}

}


// ---------------------------------------------------------------------------
// return BOOL - TRUE if handled, FALSE otherwise
// Handle the messages sent by the hook procedure by placing a message in the listbox
BOOL _stdcall MainWndProc_OnCopyData(HWND hwnd, HWND hSender, PCOPYDATASTRUCT pData)
{

	TCHAR szMessageText[512];
	ZeroMemory(&szMessageText, sizeof(szMessageText));

	if ( NULL != pData && NULL != pData->lpData ) {

		MsgData *msgdata = (MsgData *)pData->lpData;

		MSG *pMsg = &(msgdata->msg);

		TCHAR szWindowTitle[MAX_PATH];
		ZeroMemory(&szWindowTitle,sizeof(szWindowTitle));

		GetWindowText(
        	pMsg->hwnd,
            szWindowTitle,
            sizeof(szWindowTitle)/sizeof(TCHAR)
            );

		wsprintf(
            szMessageText,
			_T("%08X | %04X | %08X | %08X | %08X | %04d | %04d | %s - %s"),
			pMsg->hwnd,
			pMsg->message,
			pMsg->wParam,
			pMsg->lParam,
			pMsg->time,
			pMsg->pt.x,
			pMsg->pt.y,
			msgdata->szModName,
			szWindowTitle
            );

	}
	else {

		wsprintf( szMessageText, _T("Error in copydata message handler."));
		return FALSE;

	}

	RECT rc;
	GetClientRect(hwnd, &rc);

	// scale N to current size of the client area
	int N = (rc.bottom - rc.top)/ListBox_GetItemHeight(g_hListBox,0);

	int nCount = ListBox_GetCount(g_hListBox);

	// clear listbox every nth message
	if ( nCount >= N ) {
		ListBox_ResetContent(g_hListBox);
	}

	nCount = ListBox_GetCount(g_hListBox);

	// add lable when listbox is clear
	if ( 0 == nCount ) {

		LPCTSTR szLableText = _T("hwnd     | msg  | wParam   | lParam   | time     | x    | y    | modname - window");
		ListBox_AddString(g_hListBox, szLableText);

	}

	ListBox_AddString(g_hListBox, szMessageText);
	UpdateWindow(g_hListBox);

	return TRUE;

}


// ---------------------------------------------------------------------------

LRESULT _stdcall MainWndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{

	switch( id ) {

	case IDM_SETHOOK:

	    if ( NULL != pfnSetMsgHook ) {
			pfnSetMsgHook(hwnd);
	    }
		break;

	case IDM_UNSETHOOK:

	    if ( NULL != pfnUnsetMsgHook ) {
	        pfnUnsetMsgHook();
	    }
		break;

	}

	return 0;
}


// ---------------------------------------------------------------------------
// set the background and text color of the list box

LRESULT _stdcall MainWndProc_OnCtlColorListBox(HWND hwnd, HDC hdc, HWND hListBox, UINT flag)
{
	// black background, green text
	SetBkColor(hdc, RGB(0,0,0));
	SetTextColor(hdc, RGB(0,204,0));

	// return stock object to let the system deal with clean up
	return (LRESULT)GetStockObject(BLACK_BRUSH);

}

// ---------------------------------------------------------------------------
// Main window procedure.
static LRESULT CALLBACK MainWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
	case WM_CREATE :
		HANDLE_WM_CREATE(hwnd, wParam, lParam, MainWndProc_OnCreate);
		break;

	case WM_SIZE :
		HANDLE_WM_SIZE(hwnd, wParam, lParam, MainWndProc_OnSize);
		break;

	case WM_CTLCOLORLISTBOX :
		return HANDLE_WM_CTLCOLORLISTBOX(hwnd, wParam, lParam, MainWndProc_OnCtlColorListBox);

	case WM_COMMAND :
		HANDLE_WM_COMMAND(hwnd, wParam, lParam, MainWndProc_OnCommand);
		break;

	case WM_COPYDATA :
		return HANDLE_WM_COPYDATA(hwnd, wParam, lParam, MainWndProc_OnCopyData);

	case WM_CLOSE :
		HANDLE_WM_CLOSE(hwnd, wParam, lParam, MainWndProc_OnClose);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);

}


// ---------------------------------------------------------------------------
static BOOL _stdcall InitApplication(void)
{
	HMODULE hMod = GetModuleHandle(NULL);
	WNDCLASSEX wcex;
	ZeroMemory(&wcex,sizeof(WNDCLASSEX));
    wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = (WNDPROC)MainWndProc;
	wcex.hInstance = hMod;
	wcex.hIcon = LoadIcon(hMod, MAKEINTRESOURCE(IDI_SUN));
	wcex.hCursor = LoadCursor(NULL,IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME+1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDMAINMENU);
	wcex.lpszClassName = g_pszWndClass;
    wcex.hIconSm = LoadIcon(hMod, MAKEINTRESOURCE(IDI_SUN));

	if ( !RegisterClassEx(&wcex) ) {
		return FALSE;
	}

	// obtain msghook library functions

	g_hInstLib = LoadLibrary(_T("msghook.dll"));
	if( NULL == g_hInstLib ) {
		return FALSE;
	}

	pfnUnsetMsgHook = (UnsetMsgHookT) GetProcAddress(g_hInstLib, "UnsetMsgHook");
	if ( NULL == pfnUnsetMsgHook ) {
        FreeLibrary(g_hInstLib);
		return FALSE;
	}

 	pfnSetMsgHook = (SetMsgHookT) GetProcAddress(g_hInstLib, "SetMsgHook");
 	if ( NULL == pfnSetMsgHook ) {
        FreeLibrary(g_hInstLib);
		return FALSE;
	}

	return TRUE;

}


// ---------------------------------------------------------------------------

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{

	MSG msg;

    if ( !InitApplication() ) {
        return -1;
    }

	HWND hwnd = CreateWindowEx(0L,
                    g_pszWndClass,
                    _T("Msg Hook Test Rig"),
					WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT, CW_USEDEFAULT,
					CW_USEDEFAULT, CW_USEDEFAULT,
					NULL, NULL, hInstance, NULL
                    );

	if ( NULL == hwnd ) {
        FreeLibrary(g_hInstLib);
        return -1;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	int loop = 0;

    while ( ( loop = GetMessage(&msg, NULL, 0, 0) ) )
	{
		if ( -1 == loop ) {
			break;
		} else {

			TranslateMessage(&msg);	// translates virtual-key codes
			DispatchMessage(&msg);	// dispatches message to window

		}

	}

	return 0;

}


// ---------------------------------------------------------------------------
// EOF

