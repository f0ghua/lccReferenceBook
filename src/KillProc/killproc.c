// killproc.c
//
// Microsoft helper functions used. See common.c
//
// Lists running processes and kills selected items.
//
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "killprocres.h"
#include "common.h"

#define MAX_TASKS   256
DWORD   numTasks;
TASK_LIST tlist[MAX_TASKS];

// forward prototypes
int    APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow);
void   InitListView(HWND hwndLV);
static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
int    KillProc( HWND hDlg );
int    GetProcesses(HWND hDlg);
void   FillListView(HWND hWndList, int numItems );

int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
	InitCommonControls();

	WNDCLASS wc;

	memset( &wc, 0, sizeof(wc) );
	wc.lpfnWndProc 		= DefDlgProc;
	wc.cbWndExtra 		= DLGWINDOWEXTRA;
	wc.hInstance 		= hinst;
	wc.hCursor 			= LoadCursor( NULL, IDC_ARROW );
	wc.hIcon 			= LoadIcon( hinst, MAKEINTRESOURCE( ID_ICON ) );
	wc.hbrBackground 	= (HBRUSH) ( COLOR_BTNFACE + 1 );
	wc.lpszClassName 	= "killprocclass";

	RegisterClass( &wc );

	return DialogBox( hinst, MAKEINTRESOURCE( IDD_MAINDIALOG ), NULL, (DLGPROC) DialogFunc );

}

void InitListView( HWND hwndLV )
{
	LV_COLUMN lvc;
	long baseUnits = GetDialogBaseUnits();

	// make the selction the whole row
	SendMessage( hwndLV, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT );

	memset( &lvc, 0, sizeof(LV_COLUMN) );
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;

	// first column
	int ColWidth = (INT)(40 * LOWORD(baseUnits)) / 4;
	lvc.iSubItem   = 0;
	lvc.cx         = ColWidth;
	lvc.cchTextMax = 12;
	lvc.pszText    = "Proc ID";
	ListView_InsertColumn( hwndLV, 0, &lvc );

	// second column
	ColWidth = (INT)(40 * LOWORD(baseUnits)) / 3;
	lvc.iSubItem   = 1;
	lvc.cx         = ColWidth;
	lvc.cchTextMax = 80;
	lvc.pszText    = "Proc Name";
	ListView_InsertColumn( hwndLV, 1, &lvc );

	// third column
	ColWidth = (INT)(40 * LOWORD(baseUnits)) / 2;
	lvc.iSubItem   = 2;
	lvc.cx         = ColWidth;
	lvc.cchTextMax = 80;
	lvc.pszText    = "Window Name";
	ListView_InsertColumn( hwndLV, 2, &lvc );

}

static BOOL CALLBACK DialogFunc( HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{

	switch (msg) {
	case WM_INITDIALOG:
		InitListView( GetDlgItem( hwndDlg, IDD_LISTVIEW ) );
		GetProcesses( hwndDlg );
		return 1;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDD_KILL:
			KillProc( hwndDlg );
			break;

		case IDD_REFRESH:
			GetProcesses( hwndDlg );
			break;

		case IDD_QUIT:
			EndDialog( hwndDlg, 0 );
			return 1;
		}
		break;

	case WM_CLOSE:
		EndDialog( hwndDlg, 0 );
		return 1;

	}
	return FALSE;
}

int KillProc( HWND hDlg )
{
	HWND hListview = GetDlgItem( hDlg, IDD_LISTVIEW );
	int i;
	int cnt = ListView_GetItemCount( hListview );

	// find which item has been selected
	for (i = 0; i<cnt; i++){
		if ( LVIS_SELECTED == ListView_GetItemState( hListview, i, LVIS_SELECTED ) )
			break;
	}

	// nothing selected
	if (i>=cnt){
		MessageBox( hDlg, "Nothing selected!", "Kill Process", MB_OK );
		return 0;
	}

	// items in a list box are '0' based, the same as our array tlist[]
	// so just pass tlist[i]
	if (!KillProcess( &tlist[i] ))
		MessageBox( hDlg, "Could not Kill Process!", "Kill Process", MB_OK );

	// wait for 1 second before refreshing the list box
	sleep(1000);
	GetProcesses( hDlg );
	return 1;
}

int GetProcesses( HWND hDlg )
{

    TASK_LIST_ENUM    te;
    OSVERSIONINFO     verInfo = {0};
    LPGetTaskList     GetTaskList;
    LPEnableDebugPriv EnableDebugPriv;

    // Determine what system we're on and do the right thing
    verInfo.dwOSVersionInfoSize = sizeof(verInfo);
    GetVersionEx(&verInfo);

    switch (verInfo.dwPlatformId){

    case VER_PLATFORM_WIN32_NT:
       GetTaskList     = GetTaskListNT;
       EnableDebugPriv = EnableDebugPrivNT;
       break;

    case VER_PLATFORM_WIN32_WINDOWS:
       GetTaskList = GetTaskList95;
       EnableDebugPriv = EnableDebugPriv95;
       break;

    default:
		// O dear!
       return 0;
    }

    // Obtain the ability to manipulate other processes
    EnableDebugPriv();

    // get the task list for the system
    numTasks = GetTaskList( tlist, MAX_TASKS );

    // enumerate all windows and try to get the window
    // titles for each task
    te.tlist = tlist;
    te.numtasks = numTasks;
    GetWindowTitles( &te );

	FillListView( GetDlgItem( hDlg, IDD_LISTVIEW ), numTasks );

    return 1;
}

void FillListView( HWND hWndList, int numItems )
{
	LV_ITEM	lvI;
	lvI.mask = LVIF_TEXT;
	char txtbuf[1024];

	ListView_DeleteAllItems( hWndList );

	for (int i = 0; i<numItems; i++ ) {

		lvI.iItem = i;
		lvI.iSubItem = 0;

		sprintf( txtbuf, "%u", tlist[i].dwProcessId );
		lvI.pszText = txtbuf;
		ListView_InsertItem( hWndList, &lvI );

		sprintf( txtbuf, "%s", tlist[i].ProcessName );
		lvI.pszText = txtbuf;
		ListView_SetItemText( hWndList, i, 1, txtbuf );

		if (tlist[i].WindowTitle[0] != '0'){
			sprintf( txtbuf, "%s", tlist[i].WindowTitle );
			lvI.pszText = txtbuf;
			ListView_SetItemText( hWndList, i, 2, txtbuf );
		}
	}
}
