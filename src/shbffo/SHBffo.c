////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*

        shbffo.c

        The shbffo.dll exports two functions:

        BrowseForFolder: Front end to the Win32 SHBrowseForFolder() API for
                         picking a directory with various enhancements.
                         Specifically this function permits to add a "New
                         Folder" button for creating a new folder to the dialog
                         and can make the dialog resizable. Note that these
                         features are custom implementations and are available
                         on all versions of Windows (SHBrowseForFolder natively
                         supports this only with Shell32.dll v5.0 in Windows
                         ME/2000/XP)

        FileOperation:   Front end to the Win32 SHFileOperation() API (copying,
                         moving and deleting files with progress indicator).


        Notes to Lcc-Win32 users:
        =========================
        For information on parameters lists see shbffo.h. To use the DLL in your
        own projects, just include shbffo.h and link with shbffo.lib. For an
        example, see the project in the 'Testdll' directory.

        Special notes:

        Be careful when editing the resource file (.\res\resource.rc) in Weditres.
        I have made the following manual modifications to the resource file:

        - added conditional #define for DS_SHELLFONT
        - changed "IDD_MAINDIALOG DIALOG" to "IDD_MAINDIALOG DIALOGEX"
        - added DS_SHELLFONT to dialog style
        - changed font name to "MS Shell Dlg"

        These modification will be removed by Weditres, make sure to put them
        back in after exiting Weditres. This ensures that at runtime, the
        dialog uses the default system font for dialog boxes (currently,
        dialogs created in Weditres use the specified font and cannot be told
        to use a "System Default" font).

        - added #include "version.rc" to have version information compiled into
          the DLL.

          This also will be removed by Weditres. Make sure to put back in.

          A backup of the original resource files/headers exists in the
          'res\backup' directory.

	    - Whenever Wedit rebuilds the make file, you will have to fix it.
          Wedit does not correctly handle the resource file being in the 'res'
          subdirectory, you will have to insert $(SRCDIR)\ in front of every
          occurrence of 'res\resource.rc' in the makefile.



        Files
        =====
        |
        +----.\.        - the source and project files for the SHBffo project
        |
        +----.\Lcc      - compiler/linker output directory for the SHBffo
        |                 project. Contains the DLL.
        |
        +----.\Res      - resource files for the SHBffo project
        |
        +----.\TestDll  - Lcc project (console mode) demonstrating how to use
        |                 the DLL (also used to debug the DLL)
        |
        +----.\Wise     - scripts for the Wise Installation System demonstrating
                          how to use the DLL in Wise scripts.


        History
        =======
        16.12. 1999 - First release

        13.09. 2000 - Added ability to set an arbitrary file system path as the
                      root of the browse dialog box. Added support for
                      SHGetFolderLocation in WinME/2K.

        20.10. 2001 - Complete rework. Added ability to add a "New Folder"
                      button, make dialog resizable and include an edit box
                      where users can type in a path. Various other
                      enhancements.


        LICENSE
        =======
        Permission is hereby granted to use, modify and distribute this software
        for any and all purposes in both source and binary form. I only ask that
        you do not remove my name as the original author of this work.

        There is no charge for the original software.

        NOTE: Author takes no responsibility for any damage or losses resulting
        from the use of this software.

*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <shlobj.h>
//Shellapi.h that ships with LCC-Win32 does not byte-align structures as it should.
//Causes problems with SHFILEOPSTRUCT structure as FOF_FLAGS is defines as WORD.
#pragma pack (push, 1)
#include <shellapi.h>
#pragma pack (pop)
#include <string.h>
#include <stdio.h>
#include "shbffo.h"
#include "res\resource.h"

//Defines for various spacings
#define EDITBOX_VSPACING 5
#define BUTTON_VSPACING 15
#define BUTTONTEXT_SPACING 5
#define EDITBOX_HEIGHT 22

//Flags that control how a control is handled when the dialog is being resized
#define RESIZEHORZ 1
#define RESIZEVERT 2
#define ALIGNBOTTOM 4
#define ALIGNRIGHT 8
#define DONT_REDRAW 64

//Base number to add to the highest control ID found in the dialog when
//creating new child windows and assigning a menu ID.
#define CTRLIDINCREMENT 5

//Custom message sent to record the initial size of the dialog and do other initialization
#define WM_INITBFFWNDPROC WM_USER + 142

//Structure holding various parameters. A pointer to this
//structure is passed around in dialog functions
typedef struct tagPARAMETERS {
	BOOL bBffInitialized;
	BOOL bDisableOKButton;
	BOOL bDisableNewFolderButton;
	BOOL bUserEdit;
	UINT ulFlags;
	UINT ulUIFlags;
	HICON hIcon;
	LPMALLOC pMalloc;
	LPSHELLFOLDER pShellFolder;
	char *lpszBrowseDialogTitle;
	char *lpszNewFolderButtonText;
	char *lpszNewFolderDialogTitle;
	HWND hWndStatic;
	HWND hWndEditBox;
	HWND hWndTreeCtrl;
	HWND hWndOKButton;
	HWND hWndNewFolderButton;
	HFONT hFont;
	SIZE sizeOKButton;
	SIZE sizeCancelButton;
	int idEditBox;
	int idOKButton;
	int idNewFolderButton;
	WNDPROC OldWndProc;
	char szInitialFolder[MAX_PATH + 1];
	char szDialogSelectedFolder[MAX_PATH + 1];
	char szSelectedFolder[(MAX_PATH + 4) * 2];
	char *lpszAppendDir;
	char szOKButtonText[255];
	char szCancelButtonText[255];
	char szNewFolder[(MAX_PATH + 4) * 2];
} PARAMETERS, *LPPARAMETERS;

//Structure used in our WM_SIZE handler.
//It holds the delta between the old and
//new size of the dialog and it's window
//handle
typedef struct tagWMSIZEINFO {
	int dX;
	int dY;
	HWND hWndParent;
} WMSIZEINFO, *LPWMSIZEINFO;

//Function declarations
BOOL WINAPI LibMain(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved);
BOOL CALLBACK WmSizeProc (HWND hWndChild, LPWMSIZEINFO lpSizeInfo);
LRESULT CALLBACK PathEllipsesWndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK NewFolderDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK BFFWndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
BOOL CALLBACK GetFolderPIDL (HWND hWnd, LPSHELLFOLDER pShellFolder, int nCSIDL, char *lpszPath, LPITEMIDLIST *lpIDL);
BOOL CALLBACK ParsePIDL (LPMALLOC pMalloc, LPITEMIDLIST pidlFull, LPITEMIDLIST *pidlParent, LPITEMIDLIST *pidlChild);
BOOL CALLBACK GetInfoFromPIDL (LPMALLOC pMalloc, LPSHELLFOLDER pShellFolder, LPITEMIDLIST pidl, DWORD *dwType);
BOOL RemoveCRLF(char *lpszInput, char nReplacementChar, char **lpszOutput, int *nCRLF);

//Global instance of this module
HINSTANCE hInstance;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     LibMain

 Purpose:       Dll entry point.Called when a dll is loaded or
				unloaded by a process, and when new threads are
				created or destroyed.

*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI LibMain (HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved)
{
	hInstance = hDLLInst;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     WmSizeProc

 Purpose:       Callback function for the EnumChildWindows() API used
 				in our WM_SIZE handler for the dialog, below. This routine
				handles repositioning and sizing the child windows of the
				dialog when it is resized.
*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK WmSizeProc (HWND hWndChild, LPWMSIZEINFO lpSizeInfo)
{
	UINT ulFlags;
	RECT rc;

	//Know how to handle this control
	ulFlags = GetWindowLong(hWndChild, GWL_USERDATA);

	if (ulFlags != 0)
	{
		//Get child window extents
		GetWindowRect(hWndChild, &rc);
		//Convert to client coordinates
		ScreenToClient(lpSizeInfo->hWndParent, (POINT*)&rc);
		ScreenToClient(lpSizeInfo->hWndParent, (POINT*)&rc + 1);

		//SetWindowPos wants a width and hight
		rc.right = rc.right - rc.left;
		rc.bottom = rc.bottom - rc.top;

		//Calculate new child window positions/dimensions
		if (ulFlags & ALIGNBOTTOM)
			rc.top = rc.top + lpSizeInfo->dY;
		if (ulFlags & ALIGNRIGHT)
			rc.left = rc.left + lpSizeInfo->dX;
		if (ulFlags & RESIZEHORZ)
			rc.right = rc.right + lpSizeInfo->dX;
		if (ulFlags & RESIZEVERT)
			rc.bottom = rc.bottom + lpSizeInfo->dY;

		//Set it
		if (!(ulFlags & DONT_REDRAW))
			SendMessage(hWndChild, WM_SETREDRAW, FALSE, 0);
		SetWindowPos(hWndChild,
					 NULL,
					 rc.left,
					 rc.top,
					 rc.right,
					 rc.bottom,
					 SWP_NOZORDER | SWP_NOOWNERZORDER
					 );
		//Certain child windows do not need to be
		//redrawn after resizing/moving (prevents flicker)
		if (!(ulFlags & DONT_REDRAW))
		{
			SendMessage(hWndChild, WM_SETREDRAW, TRUE, 0);
	 		InvalidateRect(hWndChild, NULL, TRUE);
		}
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     PathEllipsesWndProc

 Purpose:       Custom window procedure for the "static" edit box control
 				and the static control in the "New Folder" dialog.
 				This draws the full path name of the currently selected
				folder using the "path ellipsis" style. Unfortunately, the
				SS_PATHELLIPSIS style for static controls is only available
				in Windows NT/2000/XP, that's why we do it manually.
*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK PathEllipsesWndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	RECT rc;
	PAINTSTRUCT ps;
	LPPARAMETERS lpParameters = (LPPARAMETERS)GetWindowLong(GetParent(hWnd), GWL_USERDATA);

	if (lpParameters != NULL)
	{
		switch (uMsg)
		{
			case WM_PAINT:
			{
				GetClientRect(hWnd, &rc);
				hDC = BeginPaint(hWnd, &ps);
				SetBkMode(hDC, TRANSPARENT);
				FillRect(hDC, &rc, GetSysColorBrush(COLOR_BTNFACE));
				SelectObject(hDC, lpParameters->hFont);
				InflateRect(&rc, -2, 0);
				DrawText(hDC,
						 hWnd == lpParameters->hWndEditBox ? lpParameters->szSelectedFolder : lpParameters->szDialogSelectedFolder,
						 -1,
						 &rc,
						 (hWnd == lpParameters->hWndEditBox ? DT_LEFT : DT_RIGHT) | DT_NOPREFIX | DT_PATH_ELLIPSIS | DT_SINGLELINE | DT_VCENTER
						 );
				EndPaint(hWnd, &ps);
			}
			break;
		}
	}
	//Let the system handle the rest of the drawing and all other messages for this control
	return CallWindowProc(DefWindowProc, hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     NewFolderDialogProc

 Purpose:       Dialog box procedure for the "New Folder" dialog.

*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK NewFolderDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static LPPARAMETERS lpParameters;
	static char szNewFolder[MAX_PATH + 1];
	RECT rcEdit, rcOK;
	void *lpMsgBuf;

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			//Save pointer to structure
			lpParameters = (LPPARAMETERS)lParam;
			//Also store in private window data (PathEllipsesWndProc gets it from there).
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)lpParameters);
			//Subclass static control to have path name printed with path ellipses
			SetWindowLong(GetDlgItem(hWnd, IDD_STATIC), GWL_WNDPROC, (LONG)PathEllipsesWndProc);
			if (lpParameters->lpszNewFolderDialogTitle)
				SetWindowText(hWnd, lpParameters->lpszNewFolderDialogTitle);
			else
				SetWindowText(hWnd, lpParameters->lpszNewFolderButtonText);
			//Set buttons text
			SetWindowText(GetDlgItem(hWnd, IDOK), lpParameters->szOKButtonText);
			SetWindowText(GetDlgItem(hWnd, IDCANCEL), lpParameters->szCancelButtonText);
			SendDlgItemMessage(hWnd, IDD_EDIT, EM_LIMITTEXT, MAX_PATH, 0);
			//OK button is initially disabled
			EnableWindow(GetDlgItem(hWnd, IDOK), FALSE);

			//Make buttons same size as buttons in main dialog
			GetWindowRect(GetDlgItem(hWnd, IDD_EDIT), &rcEdit);
			ScreenToClient(hWnd, (POINT*)&rcEdit);
			ScreenToClient(hWnd, (POINT*)&rcEdit + 1);
			GetWindowRect(GetDlgItem(hWnd, IDOK), &rcOK);
			ScreenToClient(hWnd, (POINT*)&rcOK);
			ScreenToClient(hWnd, (POINT*)&rcOK + 1);
			rcOK.left = rcEdit.left;
			SetWindowPos(GetDlgItem(hWnd, IDOK),
						 NULL,
						 rcOK.left,
						 rcOK.top,
						 lpParameters->sizeOKButton.cx,
						 lpParameters->sizeOKButton.cy,
						 SWP_NOZORDER
						 );
			rcOK.right = rcOK.left + lpParameters->sizeOKButton.cx;
			if ((rcOK.right + 5 + lpParameters->sizeCancelButton.cx) > rcEdit.right)
				lpParameters->sizeCancelButton.cx = rcEdit.right - rcOK.right - 5;
			SetWindowPos(GetDlgItem(hWnd, IDCANCEL),
						 NULL,
						 rcOK.right + 5,
						 rcOK.top,
						 lpParameters->sizeCancelButton.cx,
						 lpParameters->sizeCancelButton.cy,
						 SWP_NOZORDER
						 );

			return TRUE;
		}
		break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					//Create the directory
					strcpy(lpParameters->szNewFolder, lpParameters->szDialogSelectedFolder);
					//Append traling backslash if necessary
					if ( (*lpParameters->szNewFolder != '\0') &&
						 (*(lpParameters->szNewFolder + (strlen(lpParameters->szNewFolder) - 1)) != '\\')
					   )
						strcat(lpParameters->szNewFolder, "\\");
					SendDlgItemMessage(hWnd, IDD_EDIT, WM_GETTEXT, (WPARAM)sizeof(szNewFolder), (LPARAM)szNewFolder);
					strcat(lpParameters->szNewFolder, szNewFolder);
					if (CreateDirectory(lpParameters->szNewFolder, NULL))
						EndDialog(hWnd, TRUE);
					else
					{
						FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    								  NULL,
    								  GetLastError(),
    								  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    								  (LPTSTR)&lpMsgBuf,
    								  0,
    								  NULL
									  );
						if (lpParameters->lpszNewFolderDialogTitle)
							MessageBox(hWnd, lpMsgBuf, lpParameters->lpszNewFolderDialogTitle, MB_OK | MB_ICONSTOP);
						else
							MessageBox(hWnd, lpMsgBuf, lpParameters->lpszNewFolderButtonText, MB_OK | MB_ICONSTOP);

						LocalFree(lpMsgBuf);
						return TRUE;
					}
				}
				break;

				case IDCANCEL:
				{
					EndDialog(hWnd, FALSE);
				}
				break;

				case IDD_EDIT:
				{
					//Enable/disable OK button
					if (SendDlgItemMessage(hWnd, IDD_EDIT, WM_GETTEXTLENGTH, 0, 0))
						EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
					else
						EnableWindow(GetDlgItem(hWnd, IDOK), FALSE);
				}
				break;
			}
		}
		break;

		case WM_CLOSE:
		{
			EndDialog(hWnd, FALSE);
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     BFFWndProc

 Purpose:       Subclassing dialog procedure for the browse for folder dialog,
*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK BFFWndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static LPPARAMETERS lpParameters;
	RECT rc;
	HTREEITEM hti, hchi;
	TVITEM tvi;
	static POINT ptMinTrackSize;
	static WMSIZEINFO sizeInfo;
	static int nOldX;
	static int nOldY;
	static BOOL bEditBoxHasFocus;

	switch (uMsg)
	{
		case WM_INITBFFWNDPROC:
		{
			//Store away initial size of dialog for later
			//responding to WM_GETMINMAXINFO and WM_SIZE
			GetWindowRect(hWnd, &rc);
			ptMinTrackSize.x = rc.right - rc.left;
			ptMinTrackSize.y = rc.bottom - rc.top;
			GetClientRect(hWnd, &rc);
			nOldX = rc.right;
			nOldY = rc.bottom;
			//Set state of edit box focus flag
			bEditBoxHasFocus = FALSE;
			//Save pointer to PARAMETERS structure
			lpParameters = (LPPARAMETERS)wParam;
			return 0;
		}
		break;

		case WM_GETMINMAXINFO:
		{
			//Tell the system about the minimum extents of our window
			LPMINMAXINFO lpMinMaxInfo = (LPMINMAXINFO)lParam;
			lpMinMaxInfo->ptMinTrackSize = ptMinTrackSize;
			return 0;
		}
		break;

		case WM_SIZE:
		{
			if ((wParam != SIZE_MINIMIZED) && (lpParameters->bBffInitialized))
			{
				//Compute delta between old and new dialog size
				int nNewX = LOWORD(lParam);
				int nNewY = HIWORD(lParam);
				sizeInfo.dX = nNewX - nOldX;
				sizeInfo.dY = nNewY - nOldY;
				sizeInfo.hWndParent = hWnd;
				//Resize/reposition child windows
				EnumChildWindows(hWnd, WmSizeProc, (LPARAM)&sizeInfo);
				nOldX = nNewX;
				nOldY = nNewY;
				InvalidateRgn(hWnd, NULL, TRUE);
			}
			return 0;
		}
		break;

		case WM_COMMAND:
		{
			if ((lpParameters->idEditBox) && (LOWORD(wParam) == lpParameters->idEditBox))
			{
				switch (HIWORD(wParam))
				{
					case EN_SETFOCUS:
						bEditBoxHasFocus = TRUE;
						break;
					case EN_KILLFOCUS:
						bEditBoxHasFocus = FALSE;
						break;
					case EN_CHANGE:
					{
						//If the user types something in the edit box, make sure the OK button gets enabled
						if (bEditBoxHasFocus)
						{
							lpParameters->bUserEdit = TRUE;

							if (SendMessage(lpParameters->hWndEditBox, WM_GETTEXTLENGTH, 0, 0))
								EnableWindow(lpParameters->hWndOKButton, TRUE);
							else
								EnableWindow(lpParameters->hWndOKButton, FALSE);
						}
					}
					break;

					default:
						break;
				}
				return 0;
			}

			if ((lpParameters->idNewFolderButton) && (LOWORD(wParam) == lpParameters->idNewFolderButton))
			{
				//"New Folder" dialog
				if (DialogBoxParam(hInstance,
								   MAKEINTRESOURCE(IDD_MAINDIALOG),
								   hWnd,
								   NewFolderDialogProc,
								   (LPARAM)lpParameters)
				   )
				{
					//Update tree control to recognize the newly created folder
					if (lpParameters->hWndTreeCtrl != NULL)
					{
						hti = TreeView_GetSelection(lpParameters->hWndTreeCtrl);
						if (hti != NULL)
						{
							//Disable redrawing while updating tree control
							SendMessage(lpParameters->hWndTreeCtrl, WM_SETREDRAW, FALSE, 0);

							//This is the actual trick which I learned from Leonid Kunin
							//who posted this at CodeGuru (in MFC speak, though). Many thanks!
							//
							//So, to have the tree control recognize the newly created folder,
							//we delete all child items (subdirectories) of the currently selected folder
							//(which is the parent directory of the newly created folder) after collapsing it
							//and then tell the control that the selected folder has child items and
						    //was never expanded before.
							TreeView_Expand(lpParameters->hWndTreeCtrl, hti, TVE_COLLAPSE);
							while ((hchi = TreeView_GetChild(lpParameters->hWndTreeCtrl, hti)) != NULL)
						    {
								//There is a small chance that deleting the child items
								//leaks a little memory. I have found that the lParam
								//associated with each child item is a pointer to a small
								//block of memory that contains the long and short name
								//of the folder and binary stuff (looks like an undocumented
								//structure or a PIDL). I tried to free this pointer in various ways
								//(GlobalFree, LocalFree and through IMalloc) before actually
								//deleting the child item but this always resulted in a crash.
								//So I guess that the default dialog box procedure frees this
								//memory when it recieves the TVN_DELETEITEM notification message.
								TreeView_DeleteItem(lpParameters->hWndTreeCtrl, hchi);
							}
							ZeroMemory(&tvi, sizeof(TVITEM));
							tvi.hItem = hti;
							tvi.mask = TVIF_HANDLE|TVIF_CHILDREN;
							tvi.cChildren = I_CHILDRENCALLBACK;
							TreeView_SetItem(lpParameters->hWndTreeCtrl, &tvi);
							TreeView_Expand(lpParameters->hWndTreeCtrl, hti, TVE_COLLAPSERESET);

							//And this is REALLY straightforward...
							SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (WPARAM)lpParameters->szNewFolder);

							//Make sure new folder is visible
							SetFocus(lpParameters->hWndTreeCtrl);
							hti = TreeView_GetSelection(lpParameters->hWndTreeCtrl);
							if (hti != NULL)
								TreeView_EnsureVisible(lpParameters->hWndTreeCtrl, hti);

							//Permit redrawing and tell the system that the control
							//needs to be redrawn
							SendMessage(lpParameters->hWndTreeCtrl, WM_SETREDRAW, TRUE, 0);
							InvalidateRect(lpParameters->hWndTreeCtrl, NULL, TRUE);
						}
					}
				}
				return 0;
			}

			//If there is a non-readonly edit box, return it's
			//contents for the selected folder.
			if ((lpParameters->idOKButton) && (LOWORD(wParam) == lpParameters->idOKButton))
			{
				if ( (lpParameters->hWndEditBox !=NULL) &&
					!(lpParameters->ulUIFlags & BFFUI_EDITBOXREADONLY)
				   )
				{
					GetWindowText(lpParameters->hWndEditBox,
								  lpParameters->szSelectedFolder,
								  sizeof(lpParameters->szSelectedFolder)
								  );
				}
			}
			break;
		}
		break;

	}
	return CallWindowProc(lpParameters->OldWndProc, hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
Procedure:     BrowseCallbackProc

Purpose:       Callback procedure used with the SHBrowseForFolder API
*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	LPPARAMETERS lpParameters = (LPPARAMETERS)lpData;

	//Needed for comparison in BFFM_SELCHANGED
	static char szTemp[MAX_PATH + 1];

	switch (uMsg)
	{
		//The dialog has initialized. We perform any modifications
		//to the dialog here.
		case BFFM_INITIALIZED:
		{
			int maxCtrlID = 0;
			int CtrlID = 0;
			int nButtonSpacing = 0;
			HWND hWndChild = NULL;
			HWND hWndButton = NULL;
			HWND hWndOKButton = NULL;
			HWND hWndCancelButton = NULL;
			HWND hWndNewFolderButton = NULL;
			HWND hWndStatic = NULL;
			HWND hWndTreeCtrl = NULL;
			HWND hWndEditBox = NULL;
			HWND hWndSizeGrip = NULL;
			char szClassName[255];
			HFONT hFont;
			HDC hDC;
			SIZE size;
			TEXTMETRIC tm;
			RECT rcButton, rcChild, rcOKButton, rcCancelButton, rcTreeCtrl, rcMain, rc;
			LPITEMIDLIST pidlSelected;

			//Store pointer to parameters structure in the dialog
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)lpParameters);

			//Set dialog title
			if ((lpParameters->lpszBrowseDialogTitle) &&
				(*lpParameters->lpszBrowseDialogTitle != '\0'))
				SetWindowText(hWnd, lpParameters->lpszBrowseDialogTitle);

			//The following hack is neccessary to be able to make the dialog resizable.
			//Basically, it needs a minimize box (which is not compatible with WS_EX_CONTEXTHELP)
			//and a "Minimize" entry in the system menu. We always do this to have consistent
			//appearance of the dialog, whether or not it is resizable.
			SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) & !WS_EX_CONTEXTHELP);
			//WS_CLIPCHILDREN prevents child windows from flickering when resizing/repositioning them
			SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_MINIMIZEBOX | WS_CLIPCHILDREN);
			InsertMenu(GetSystemMenu(hWnd,FALSE), 1, MF_BYPOSITION | MF_STRING, SC_ICON, "Minimize");
			//This localizes the system menu in the system language
			GetSystemMenu(hWnd, TRUE);

			//Set icon
			if (lpParameters->hIcon != NULL)
				SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)lpParameters->hIcon);
			else
			{
				if (lpParameters->ulFlags & BIF_BROWSEFORCOMPUTER)
					SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)(HICON)LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON_COMPUTER)));
				else if (lpParameters->ulFlags & BIF_BROWSEFORPRINTER)
					SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)(HICON)LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON_PRINTER)));
				else
					SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)(HICON)LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON_FOLDER)));
			}

			//Get and store font used in the dialog
			hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
			lpParameters->hFont = hFont;

			//Find highest control ID used in the dialog,
			//get handles to buttons, tree view and static text control
			//and other common stuff
			hWndChild = GetWindow(hWnd, GW_CHILD);
			while (hWndChild != NULL)
			{
				CtrlID = GetDlgCtrlID(hWndChild);
				if (maxCtrlID == 0)
					maxCtrlID = CtrlID;
				else if (CtrlID > maxCtrlID)
					maxCtrlID = CtrlID;

				if (GetClassName(hWndChild, szClassName, sizeof(szClassName)))
				{
					if (!stricmp(szClassName, WC_TREEVIEW))
					{
						//Found the TreeView control
						hWndTreeCtrl = hWndChild;
						lpParameters->hWndTreeCtrl = hWndChild;
						//Get TreeView rectangle, convert to client coordinates
						GetWindowRect(hWndTreeCtrl, &rcTreeCtrl);
						ScreenToClient(hWnd, (POINT*)&rcTreeCtrl);
						ScreenToClient(hWnd, (POINT*)&rcTreeCtrl + 1);
						//Define how this control should be handled
						//during resizing the dialog
						SetWindowLong(hWndChild, GWL_USERDATA, RESIZEHORZ | RESIZEVERT | DONT_REDRAW);
					}
					else if (!stricmp(szClassName, "BUTTON"))
					{
						//Assume there are two buttons in the dialog,
						//the OK and the Cancel button
						if (hWndButton == NULL)
							hWndButton = hWndChild;
						else
						{
							GetWindowRect(hWndChild, &rcChild);
							GetWindowRect(hWndButton, &rcButton);
							//Rightmost button is Cancel button,
							//leftmost button is OK button.
							if (rcChild.left > rcButton.left)
							{
								hWndOKButton = hWndButton;
								hWndCancelButton = hWndChild;
							}
							else
							{
								hWndOKButton = hWndChild;
								hWndCancelButton = hWndButton;
							}
							//Get button rectangles, convert to client coordinates
							GetWindowRect(hWndOKButton, &rcOKButton);
							ScreenToClient(hWnd, (POINT*)&rcOKButton);
							ScreenToClient(hWnd, (POINT*)&rcOKButton + 1);

							GetWindowRect(hWndCancelButton, &rcCancelButton);
							ScreenToClient(hWnd, (POINT*)&rcCancelButton);
							ScreenToClient(hWnd, (POINT*)&rcCancelButton + 1);

							//Get buttons text and dimensions for usage in "New Folder" dialog
							GetWindowText(hWndOKButton, lpParameters->szOKButtonText, sizeof(lpParameters->szOKButtonText));
							GetWindowText(hWndCancelButton, lpParameters->szCancelButtonText, sizeof(lpParameters->szCancelButtonText));
							lpParameters->sizeOKButton.cx = rcOKButton.right - rcOKButton.left;
							lpParameters->sizeOKButton.cy = rcOKButton.bottom - rcOKButton.top;
							lpParameters->sizeCancelButton.cx = rcCancelButton.right - rcCancelButton.left;
							lpParameters->sizeCancelButton.cy = rcCancelButton.bottom - rcCancelButton.top;

							//Store control ID of OK button. If this button is clicked and we have a
							//non-static edit control in the dialog, return the contents of the edit control
							lpParameters->idOKButton = GetDlgCtrlID(hWndOKButton);
							lpParameters->hWndOKButton = hWndOKButton;
						}
						//Define how this control should be handled
						//during resizing the dialog
						SetWindowLong(hWndChild, GWL_USERDATA, ALIGNBOTTOM | ALIGNRIGHT);
					}
					else if (!stricmp(szClassName, "STATIC"))
					{
						if (hWndStatic == NULL)
						{
							//Found the first static text control
							//We don't care for any other static controls
							hWndStatic = hWndChild;
							lpParameters->hWndStatic = hWndStatic;
							//Fix up style
							SetWindowLong(hWndStatic,
										  GWL_STYLE,
										  GetWindowLong(hWndStatic, GWL_STYLE) | SS_NOPREFIX
										  );
							//Define how this control should be handled
							//during resizing the dialog
							SetWindowLong(hWndStatic, GWL_USERDATA, RESIZEHORZ);
						}
					}
				}
				hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
			} /* while (hWndChild != NULL) */

			//Make sure we have everything we need
			if ((hWndStatic != NULL) 	&&
				(hWndTreeCtrl != NULL)	&&
				(hWndOKButton != NULL)	&&
				(hWndCancelButton != NULL)
			   )
			{
				//Resize dialog vertically to have the same vertical alignment
				//of the buttons below the tree control on all OS environments
				GetWindowRect(hWndTreeCtrl, &rc);
				GetWindowRect(hWnd, &rcMain);
				SetWindowPos(hWnd,
				   			 NULL,
							 0,
							 0,
							 rcMain.right - rcMain.left,
							 rc.bottom - rcMain.top + (rcOKButton.bottom - rcOKButton.top) + 2 * BUTTON_VSPACING,
							 SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER
							 );
				//Resize the static text control for proper spacing between
				//it and the tree view control
				GetWindowRect(hWndStatic, &rc);
				ScreenToClient(hWnd, (POINT*)&rc);
				ScreenToClient(hWnd, (POINT*)&rc + 1);
				SetWindowPos(hWndStatic,
		   					 NULL,
							 0,
							 0,
							 rc.right - rc.left,
							 rcTreeCtrl.top - EDITBOX_VSPACING - rc.top,
							 SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER
							 );

				rcOKButton.bottom = rcTreeCtrl.bottom + BUTTON_VSPACING + (rcOKButton.bottom - rcOKButton.top);
				rcOKButton.top = rcTreeCtrl.bottom + BUTTON_VSPACING;
				rcCancelButton.top = rcOKButton.top;
				rcCancelButton.bottom = rcOKButton.bottom;
				//Also, make sure buttons are aligned with
				//right edge of tree view. Just in case.
				nButtonSpacing = rcCancelButton.left - rcOKButton.right;
				rcCancelButton.left = rcTreeCtrl.right - (rcCancelButton.right - rcCancelButton.left);
				rcCancelButton.right = rcTreeCtrl.right;
				rcOKButton.left = rcCancelButton.left - (rcOKButton.right - rcOKButton.left) - nButtonSpacing;
				rcOKButton.right = rcCancelButton.left - nButtonSpacing;
				SetWindowPos(hWndOKButton,
							 NULL,
							 rcOKButton.left,
							 rcOKButton.top,
							 0,0,
							 SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER
							 );
				SetWindowPos(hWndCancelButton,
							 NULL,
							 rcCancelButton.left,
							 rcCancelButton.top,
							 0,0,
							 SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER
							 );

				//////////////////////////////////////////////////////////////////////////
				//
				// Create the (static) edit box
				//
				if (lpParameters->ulUIFlags & BFFUI_EDITBOX)
				{
					//Increase height of dialog to make room
					//for the edit box, move down tree view and
					//buttons
					int nMove = EDITBOX_HEIGHT + EDITBOX_VSPACING;
					int idEditBox;

					rcTreeCtrl.top = rcTreeCtrl.top + nMove;
					rcTreeCtrl.bottom = rcTreeCtrl.bottom + nMove;
					rcOKButton.top = rcOKButton.top + nMove;
					rcOKButton.bottom = rcOKButton.bottom + nMove;
					rcCancelButton.top = rcCancelButton.top + nMove;
					rcCancelButton.bottom = rcCancelButton.bottom + nMove;

					GetWindowRect(hWnd, &rc);
					SetWindowPos(hWnd,
				   				 NULL,
								 0,
								 0,
								 rc.right - rc.left,
								 rc.bottom - rc.top + nMove,
								 SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER
								 );
					SetWindowPos(hWndOKButton,
				   				 NULL,
								 rcOKButton.left,
								 rcOKButton.top,
								 0,
								 0,
								 SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER
								 );
					SetWindowPos(hWndCancelButton,
				   				 NULL,
								 rcCancelButton.left,
								 rcCancelButton.top,
								 0,
								 0,
								 SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER
								 );
					SetWindowPos(hWndTreeCtrl,
				   				 NULL,
								 rcTreeCtrl.left,
								 rcTreeCtrl.top,
								 0,
								 0,
								 SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER
								 );

					//Create the Edit Box
					idEditBox = maxCtrlID + CTRLIDINCREMENT;
					hWndEditBox = CreateWindowEx(WS_EX_CLIENTEDGE,
												 lpParameters->ulUIFlags & BFFUI_EDITBOXREADONLY ? "STATIC" : "EDIT",
												 lpParameters->ulUIFlags & BFFUI_EDITBOXREADONLY ? "" : lpParameters->szSelectedFolder,
												 lpParameters->ulUIFlags & BFFUI_EDITBOXREADONLY ?
												 WS_CHILD | WS_BORDER | WS_VISIBLE
												 :
												 ES_AUTOHSCROLL | ES_LEFT | WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE,
												 rcTreeCtrl.left,
												 rcTreeCtrl.top - EDITBOX_VSPACING - EDITBOX_HEIGHT,
												 rcTreeCtrl.right - rcTreeCtrl.left,
												 EDITBOX_HEIGHT,
												 hWnd,
												 (HMENU)idEditBox,
												 hInstance,
												 NULL
												 );
					if (hWndEditBox != NULL)
					{
						//Set Font
						SendMessage (hWndEditBox, WM_SETFONT, (WPARAM)hFont, 0);
						//Store handle and ID in parameters structure
						lpParameters->hWndEditBox = hWndEditBox;
						lpParameters->idEditBox = idEditBox;
						//Insert in Z order after static control for proper tab order
						SetWindowPos(hWndEditBox,
									 hWndStatic,
									 0,0,0,0,
									 SWP_NOMOVE | SWP_NOSIZE
									 );
						//Define how this control should be handled
						//during resizing the dialog
						SetWindowLong(hWndEditBox, GWL_USERDATA, RESIZEHORZ);

						if (lpParameters->ulUIFlags & BFFUI_EDITBOXREADONLY)
						{
							//In order to have "path ellipses" inserted in the static
							//control's text, we need to draw it's text during WM_PAINT.
							SetWindowLong(hWndEditBox, GWL_WNDPROC, (LONG)PathEllipsesWndProc);
							//Make sure current folder is displayed
							InvalidateRgn(hWndEditBox, NULL, TRUE);
						}
						else
							//Limit number of characters to MAX_PATH
							SendMessage(hWndEditBox, EM_LIMITTEXT, MAX_PATH, 0);
					}

				} /* if (lpParameters->ulUIFlags & BFFUI_EDITBOX) */
				//
				//////////////////////////////////////////////////////////////////////////


				//////////////////////////////////////////////////////////////////////////
				//
				// Create the "New Folder" button
				//
				if (lpParameters->ulUIFlags & BFFUI_NEWFOLDER)
				{
					//Move buttons left, insert "New Folder" button
					int nButtonWidth;
					int idNewFolderButton;
					int nStrlen = strlen(lpParameters->lpszNewFolderButtonText);

					//Calculate width of "New Folder" button based on string length
					hDC = GetDC(hWnd);
					GetTextExtentPoint32(hDC,
				    					 lpParameters->lpszNewFolderButtonText,
										 nStrlen,
										 &size
										 );
					nButtonWidth = size.cx;
					ReleaseDC(hWnd, hDC);

					if ((rcOKButton.left - nButtonSpacing - nButtonWidth) < rcTreeCtrl.left)
					{
						//Make sure the buttons don't exceed the left edge
						//of the Tree View
						nButtonWidth = rcOKButton.left - rcTreeCtrl.left - nButtonSpacing;
					}
					SetWindowPos(hWndOKButton,
				   				 NULL,
								 rcOKButton.left - nButtonSpacing - nButtonWidth,
								 rcOKButton.top,
								 0,
								 0,
								 SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER
								 );
					SetWindowPos(hWndCancelButton,
				   				 NULL,
								 rcCancelButton.left - nButtonSpacing - nButtonWidth,
								 rcCancelButton.top,
								 0,
								 0,
								 SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER
								 );

					//Create button
					idNewFolderButton = maxCtrlID + CTRLIDINCREMENT + 1;
					hWndNewFolderButton = CreateWindow("BUTTON",
		   				 								lpParameters->lpszNewFolderButtonText,
						 								BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						 								rcTreeCtrl.right - nButtonWidth,
						 								rcCancelButton.top,
						 								nButtonWidth,
						 								rcCancelButton.bottom - rcCancelButton.top,
						 								hWnd,
						 								(HMENU)idNewFolderButton,
						 								hInstance,
						 								NULL
														);
					if (hWndNewFolderButton != NULL)
					{
						//Set Font
						SendMessage (hWndNewFolderButton, WM_SETFONT, (WPARAM)hFont, 0);
						//Store handle and menu ID parameters structure
						lpParameters->hWndNewFolderButton = hWndNewFolderButton;
						lpParameters->idNewFolderButton = idNewFolderButton;
						//Insert in Z order after Cancel button for proper tab order
						SetWindowPos(hWndNewFolderButton,
									 hWndCancelButton,
									 0,0,0,0,
									 SWP_NOMOVE | SWP_NOSIZE);
						//Define how this control should be handled
						//during resizing the dialog
						SetWindowLong(hWndNewFolderButton, GWL_USERDATA, ALIGNBOTTOM | ALIGNRIGHT);

						//Use button text for "New Folder" dialog title, remove any '&' characters
						lpParameters->lpszNewFolderDialogTitle = calloc(strlen(lpParameters->lpszNewFolderButtonText) + 1, sizeof(char));
						if (lpParameters->lpszNewFolderDialogTitle != NULL)
						{
							char *lpszSave = lpParameters->lpszNewFolderDialogTitle;
							while (TRUE)
							{
								if (*lpParameters->lpszNewFolderButtonText == '\0')
									break;
								if (*lpParameters->lpszNewFolderButtonText != '&')
								{
									*lpParameters->lpszNewFolderDialogTitle = *lpParameters->lpszNewFolderButtonText;
									lpParameters->lpszNewFolderDialogTitle++;
								}
								lpParameters->lpszNewFolderButtonText++;
							}
							lpParameters->lpszNewFolderDialogTitle = lpszSave;
						}
					}
				} /* if (lpParameters->ulUIFlags & BFFUI_NEWFOLDER) */
				//
				//////////////////////////////////////////////////////////////////////////


				//////////////////////////////////////////////////////////////////////////
				//
				// Create the size gripper control
				//
				if (lpParameters->ulUIFlags & BFFUI_RESIZABLE)
				{
					GetClientRect(hWnd, &rc);
					//Actually, it's a scrollbar without a scrollbar
					hWndSizeGrip = CreateWindow("SCROLLBAR",
		   				 						"",
						 						SBS_SIZEGRIP | SBS_SIZEBOX | SBS_SIZEBOXBOTTOMRIGHTALIGN | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
						 						rc.right - GetSystemMetrics(SM_CXVSCROLL) - GetSystemMetrics(SM_CXEDGE),
						 						rc.bottom - GetSystemMetrics(SM_CYHSCROLL) - GetSystemMetrics(SM_CYEDGE),
						 						GetSystemMetrics(SM_CXVSCROLL),
						 						GetSystemMetrics(SM_CYHSCROLL),
						 						hWnd,
						 						NULL,
						 						hInstance,
						 						NULL
												);
					//Define how this control should be handled
					//during resizing the dialog
					SetWindowLong(hWndSizeGrip, GWL_USERDATA, ALIGNBOTTOM | ALIGNRIGHT);

					//Make dialog resizable
					SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_THICKFRAME);
					SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
				   				 SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE | SWP_FRAMECHANGED);
				} /* if (lpParameters->ulUIFlags & BFFUI_RESIZABLE) */
				//
				//////////////////////////////////////////////////////////////////////////

				//Finished initializing
				lpParameters->bBffInitialized = TRUE;

				//Subclass the dialog, allowing us to process the
				//"New Folder" button, resizing managing the
				//non-readonly edit box
				if ( (lpParameters->ulUIFlags & BFFUI_RESIZABLE)									||
					 ((hWndEditBox != NULL) && !(lpParameters->ulUIFlags & BFFUI_EDITBOXREADONLY))	||
					 (hWndNewFolderButton != NULL)
				   )
				{
					WNDPROC OldWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
					if (OldWndProc != NULL)
					{
						lpParameters->OldWndProc = OldWndProc;

						//Call custom dialog proc directly to initialize static variables
						BFFWndProc(hWnd, WM_INITBFFWNDPROC, (WPARAM)lpParameters, 0);

						SetWindowLong(hWnd, GWL_WNDPROC, (LONG)BFFWndProc);
					}
				}

				//Disable "New Folder" button if necessary (see BFFM_SELCHANGED)
				if (lpParameters->bDisableNewFolderButton && (hWndNewFolderButton != NULL))
					EnableWindow(hWndNewFolderButton, FALSE);

			} /* if ((hWndStatic != NULL) 	&&
					 (hWndTreeCtrl != NULL)	&&
					 (hWndOKButton != NULL)	&&
					 (hWndCancelButton != NULL)
			   ) */

			//Expand passed-in path if necessary
			if (!(lpParameters->ulUIFlags & BFFUI_DONTEXPAND))
			{
				//Send selection as PIDL
				if (GetFolderPIDL(hWnd, lpParameters->pShellFolder, 0, lpParameters->szSelectedFolder, &pidlSelected))
				{
					SendMessage(hWnd, BFFM_SETSELECTION, FALSE, (LPARAM)pidlSelected);
					//Free PIDL
					lpParameters->pMalloc->lpVtbl->Free(lpParameters->pMalloc, pidlSelected);
				}
				//Send selection as file system path anyway. Otherwise, BFFM_SETSELECTION may
				//fail for network resources on Windows 95/NT4. It doesn't harm for other
				//operating systems.
				SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpParameters->szSelectedFolder);
			}

			//If passed-in path is not empty, make sure OK button is initially
			//enabled so that user's dont have to click "Cancel" to get out.
			if ((*lpParameters->szInitialFolder != '\0') && !lpParameters->bDisableOKButton)
				EnableWindow(hWndOKButton, TRUE);

			return TRUE;

		} /* case BFFM_INITIALIZED */
		break;

		case BFFM_SELCHANGED:
		{
			HTREEITEM hti = NULL;
			TVITEM tvi;
			DWORD dwType = BFF_OBJECTTYPE_UNKNOWN;

			//BrowseCallbackProc gets passed the lParam passed in
			//the BROWSEINFO structure in lpData.
			LPPARAMETERS lpParameters = (LPPARAMETERS)lpData;

			//BFFM_SELCHANGED is called before BFFM_INITIALIZED.
			//
			//Update currently selected folder only if
			//lpParameters is not NULL
			if 	(lpParameters != NULL)
			{
				//... except when the dialog has not
				//initialized yet and the passed-in path is
				//not empty (because BFFM_SETSELECTION can be
				//called no earlier than in BFFM_INITIALIZED but
				//BFFM_SELCHANGED is always called at least once
				//before BFFM_INITIALIZED, this would overwrite
				//the passed-in folder with the default selection
				//in the Tree View (My Computer)).
				if (!lpParameters->bBffInitialized && (*lpParameters->szInitialFolder != '\0'))
				{
					//In this case, test only if what the tree control selects is a file system
					//path and if not, mark "New Folder" button as disabled.
					if (! ( ((LPITEMIDLIST)lParam != NULL) &&
					        SHGetPathFromIDList((LPITEMIDLIST)lParam, szTemp)
						  )
					   )
					{
						lpParameters->bDisableNewFolderButton = TRUE;

						//Fix OK button not disabled for computers if
						//BIF_RETURNONLYFSDIRS is specified
						if (lpParameters->ulFlags & BIF_RETURNONLYFSDIRS)
						{
							//Get type of selected object
							GetInfoFromPIDL (lpParameters->pMalloc, lpParameters->pShellFolder, (LPITEMIDLIST)lParam, &dwType);
							if (dwType == BFF_OBJECTTYPE_COMPUTER)
								lpParameters->bDisableOKButton = TRUE;
						}
					}
					else
					{
						//Check if what has been selected is actually a directory
						if ( !(GetFileAttributes(szTemp) & FILE_ATTRIBUTE_DIRECTORY))
							lpParameters->bDisableNewFolderButton = TRUE;
						strcpy(lpParameters->szDialogSelectedFolder, szTemp);
					}
				}
				else
				{
					//Get selected path from PIDL passed in lParam. The documentation on
					//SHBrowseForFolder is confused here, it says the PIDL is passed in lpData,
					//but it's actually passed in lParam.
					if (! ( ((LPITEMIDLIST)lParam != NULL) &&
					        SHGetPathFromIDList((LPITEMIDLIST)lParam, lpParameters->szDialogSelectedFolder)
						  )
					   )
					{
						//Failed, get display name from tree control
						//
						//Because BFFM_SELCHANGED is called before BFFM_INITIALIZED,
						//where lpParameters->hWndTreeCtrl is initialized,
						//lpParameters->hWndTreeCtrl may be NULL
						if (lpParameters->hWndTreeCtrl == NULL)
							lpParameters->hWndTreeCtrl = FindWindowEx(hWnd, NULL, WC_TREEVIEW, NULL);

						hti = TreeView_GetSelection(lpParameters->hWndTreeCtrl);
						if (hti != NULL)
						{
							ZeroMemory(&tvi, sizeof(TVITEM));
							tvi.mask = TVIF_HANDLE | TVIF_TEXT;
							tvi.hItem = hti;
							tvi.pszText = lpParameters->szDialogSelectedFolder;
							tvi.cchTextMax = sizeof(lpParameters->szDialogSelectedFolder);
							TreeView_GetItem(lpParameters->hWndTreeCtrl, &tvi);
						}

						//The selected object is not a file system path -> disable "New Folder" button
						if (lpParameters->hWndNewFolderButton != NULL)
							EnableWindow(lpParameters->hWndNewFolderButton, FALSE);
						else
							//The button might not as yet be there
							lpParameters->bDisableNewFolderButton = TRUE;

						//Get type of selected object
						GetInfoFromPIDL (lpParameters->pMalloc, lpParameters->pShellFolder, (LPITEMIDLIST)lParam, &dwType);

						//Fix OK button not disabled for computers if
						//BIF_RETURNONLYFSDIRS is specified
						if (lpParameters->ulFlags & BIF_RETURNONLYFSDIRS)
						{
							if (dwType == BFF_OBJECTTYPE_COMPUTER)
							{
								if (lpParameters->hWndOKButton != NULL)
									EnableWindow(lpParameters->hWndOKButton, FALSE);
								else
									//The button might not as yet be there
									lpParameters->bDisableOKButton = TRUE;
							}
						}
					}
					else
					{
						//The selected object is a file system path, check that it is actually a directory
						if (lpParameters->hWndNewFolderButton != NULL)
						{
							if (GetFileAttributes(lpParameters->szDialogSelectedFolder) & FILE_ATTRIBUTE_DIRECTORY)
								EnableWindow(lpParameters->hWndNewFolderButton, TRUE);
							else
								EnableWindow(lpParameters->hWndNewFolderButton, FALSE);
						}
					}

					//Prepend double backslashes if it is a computer name...
					if (dwType == BFF_OBJECTTYPE_COMPUTER)
					{
						strcpy(lpParameters->szSelectedFolder, "\\\\");
						strcat(lpParameters->szSelectedFolder, lpParameters->szDialogSelectedFolder);
					}
					else
						//else simply copy selected object into return buffer
						strcpy(lpParameters->szSelectedFolder, lpParameters->szDialogSelectedFolder);

					if ( !(lpParameters->ulUIFlags & BFFUI_DONTAPPEND)
						 && (lpParameters->lpszAppendDir != NULL)
						 && (*lpParameters->lpszAppendDir != '\0')
						 //Only append if dialog is initialized and the
						 //currently selected directory does not match
						 //the one intitially passed in.
						 && (lpParameters->bBffInitialized && stricmp(lpParameters->szInitialFolder, lpParameters->szSelectedFolder))
					   )
					{
						if ( (*lpParameters->szSelectedFolder != '\0') &&
							 (*(lpParameters->szSelectedFolder + (strlen(lpParameters->szSelectedFolder) - 1)) != '\\')
						   )
							strcat(lpParameters->szSelectedFolder, "\\");
						strcat(lpParameters->szSelectedFolder, lpParameters->lpszAppendDir);
					}

					if (lpParameters->hWndEditBox != NULL)
					{
						if ( !(lpParameters->ulUIFlags & BFFUI_EDITBOXREADONLY))
						{
							//Update edit control when a new folder is selected.
							//
							//Send a fake notification message that the edit control has lost the focus
							//Reason:
							//Clearly, when we are in BFFM_SELCHANGED, the tree control has the keyboard focus.
							//However, the main window receives the edit control's EN_KILLFOCUS notification message
							//_after_ the tree control selection has changed (TVN_SELCHANGED). SetWindowText()
							//on the edit control generates an EN_CHANGE message. This means that when the edit control
							//is focussed and the user uses the mouse to select a different item in the tree control,
							//the handler for WM_COMMAND messages from the edit control in BFFWndProc would
							//unconditionally enable the OK button (because the EN_KILLFOCUS message is still pending),
							//although SHBrowseForFolder may have just disabled it (BIF_RETURNONLYFSDIRS
							//specified in ulFlags and selected item is not a file system directory).
							SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(lpParameters->idEditBox, EN_KILLFOCUS), (LPARAM)lpParameters->hWndEditBox);
							SetWindowText(lpParameters->hWndEditBox, lpParameters->szSelectedFolder);
							//Reset "Edited by user" flag
							lpParameters->bUserEdit = FALSE;
						}
						else
							//In case it's a "read only" edit box, cause a WM_PAINT for the control
							//to be issued. Our handler for WM_PAINT for this control will
							//draw the text with "path ellipses" if neccessary.
							InvalidateRgn(lpParameters->hWndEditBox, NULL, TRUE);
					}

				} /* if (!lpParameters->bBffInitialized && (*lpParameters->szSelectedFolder != '\0')) */

			} /* if	(lpParameters != NULL) */

		} /* case BFFM_SELCHANGED */
		break;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     ParsePIDL

 Purpose:		Parse a fully qualified PIDL into a parent and a child PIDL. The child PIDL will be
 				the last level of the fully qualified PIDL. The parent PIDL can then be used
				in pShellFolder->BindToObject to get the IShellFolder interface of the object the parent
				PIDL describes. The child PIDL can then be used in SHGetDataFromIDList() to obtain display
				information about the child object the child PIDL describes.

 Parameters:
				pMalloc:		pointer to the Shell's IMalloc interface
								(use SHGetMalloc to obtain it).

				pShellFolder:	pointer to the Shell's IShellFolder interface
								(use SHGetDesktopFolder to obtain it).

				pidlFull:		pointer to the fully qualified item ID list
								(that is, relative to the desktop folder) for
								the shell object

				pidlParent		address of a pointer that, on return, points
								to the parent PIDL. Caller is responsible for
								freeing this pointer through the shell's IMalloc
								interface.

				pidlChild		address of a pointer that, on return, points
								to the child PIDL. Caller is responsible for
								freeing this pointer through the shell's IMalloc
								interface

*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK ParsePIDL (LPMALLOC pMalloc, LPITEMIDLIST pidlFull, LPITEMIDLIST *pidlParent, LPITEMIDLIST *pidlChild)
{
	BOOL bParsed = FALSE;
	LPITEMIDLIST pidlNext, pidl;
	int cb = 0, cbParent = 0, cbChild = 0;

	if (!(pidlParent && pidlChild))
		return FALSE;

	*pidlParent = NULL;
	*pidlChild = NULL;

	pidl = pidlFull;
	if (pidl && (pidl->mkid.cb) && pMalloc)
	{
		pidlNext = pidl;
		//Find trailing Item ID
		while (pidlNext)
		{
			pidl = pidlNext;
			cb = pidl->mkid.cb;
			if (cb == 0)
				pidlNext = NULL;
			else
			{
				pidlNext = (LPITEMIDLIST) (((LPBYTE) pidl) + cb);
				if (pidlNext->mkid.cb == 0)
					pidlNext = NULL;
				else
					cbParent += cb;
			}
		}
		//Copy pidls
		cbChild = pidl->mkid.cb;

		if (cbChild)
		{
			//Allocate memory for PIDL plus NULL terminator
			*pidlParent = pMalloc->lpVtbl->Alloc(pMalloc, cbParent + sizeof(pidlFull->mkid.cb));
			*pidlChild = pMalloc->lpVtbl->Alloc(pMalloc, cbChild + sizeof(pidl->mkid.cb));

			if ((*pidlParent != NULL) && (*pidlChild != NULL))
			{
				ZeroMemory(*pidlParent, cbParent + sizeof(pidlFull->mkid.cb));
				ZeroMemory(*pidlChild, cbChild + sizeof(pidl->mkid.cb));
				CopyMemory(*pidlParent, pidlFull, cbParent);
				CopyMemory(*pidlChild, pidl, cbChild);
				bParsed = TRUE;
			}
		}
	}
	return bParsed;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     GetInfoFromPIDL

 Purpose:		Uses SHGetDataFromIDList to get display information for the object a PIDL describes

 Parameters:
				pMalloc:		pointer to the Shell's IMalloc interface
								(use SHGetMalloc to obtain it).

				pShellFolder:	pointer to the Shell's IShellFolder interface
								(use SHGetDesktopFolder to obtain it).

				pidl:			pointer to the fully qualified item ID list
								(that is, relative to the desktop folder) for
								the shell object

				dwType:			Address of a variable that receives a constant decribing
								the type of the shell object.

*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK GetInfoFromPIDL (LPMALLOC pMalloc, LPSHELLFOLDER pShellFolder, LPITEMIDLIST pidl, DWORD *dwType)
{
	LPITEMIDLIST	pidlParent = NULL, pidlChild = NULL;
	static BYTE		buffer[4096];
	SHDESCRIPTIONID shdid;
	NETRESOURCE		&nsr = *((NETRESOURCE*)buffer);
	WIN32_FIND_DATA	wfd;
	IShellFolder	*pParentFolder = NULL;
	BOOL			bSuccess = FALSE;

	if (dwType)
		*dwType = BFF_OBJECTTYPE_UNKNOWN;
	else
		return FALSE;

	if (pidl && pMalloc && pShellFolder)
	{
		//Parse PIDL into "parent" and "child" PIDL
		ParsePIDL(pMalloc, pidl, &pidlParent, &pidlChild);
		if (pidlChild)
		{
			//Get the IShellFolder interface for the "parent folder" the parent PIDL refers to
			if(pShellFolder->lpVtbl->BindToObject(pShellFolder, pidlParent, NULL, &IID_IShellFolder, &pParentFolder) == NOERROR)
			{
				//Get type of object from ID list. The pidl passed here must be an immediate child of
				//the parent folder.
				//
				//SHGDFIL_DESCRIPTIONID is only supported on Windows 98/ME/2000/XP
				if (SHGetDataFromIDList(pParentFolder, pidlChild, SHGDFIL_DESCRIPTIONID, &shdid, sizeof(SHDESCRIPTIONID)) == NOERROR)
				{
					switch (shdid.dwDescriptionId)
					{
						case SHDID_FS_FILE:
							*dwType = BFF_OBJECTTYPE_FILE;
							break;
						case SHDID_FS_DIRECTORY:
						case SHDID_COMPUTER_DRIVE35:
						case SHDID_COMPUTER_DRIVE525:
						case SHDID_COMPUTER_REMOVABLE:
						case SHDID_COMPUTER_FIXED:
						case SHDID_COMPUTER_NETDRIVE:
						case SHDID_COMPUTER_CDROM:
						case SHDID_COMPUTER_RAMDISK:
							*dwType = BFF_OBJECTTYPE_DIRECTORY;
							break;
						case SHDID_NET_DOMAIN:
							*dwType = BFF_OBJECTTYPE_DOMAIN;
							break;
						case SHDID_NET_SERVER:
							*dwType = BFF_OBJECTTYPE_COMPUTER;
							break;
						case SHDID_NET_SHARE:
							*dwType = BFF_OBJECTTYPE_SHARE;
							break;
						case SHDID_NET_OTHER:
							*dwType = BFF_OBJECTTYPE_NETGENERIC;
							break;
						default:
							*dwType = BFF_OBJECTTYPE_UNKNOWN;
							break;
					}

					bSuccess = TRUE;
				}
				//Is it a file system object? (Note: this fails for root directories (C:\) and network shares)
				else if (SHGetDataFromIDList(pParentFolder, pidlChild, SHGDFIL_FINDDATA, &wfd, sizeof(WIN32_FIND_DATA)) == NOERROR)
				{
					if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						*dwType = BFF_OBJECTTYPE_DIRECTORY;
					else
						*dwType = BFF_OBJECTTYPE_FILE;

					bSuccess = TRUE;
				}
				//Is it a network resource?
				else if (SHGetDataFromIDList(pParentFolder, pidlChild, SHGDFIL_NETRESOURCE, &nsr, sizeof(buffer)) == NOERROR)
				{
					switch (nsr.dwDisplayType)
					{
						case RESOURCEDISPLAYTYPE_DOMAIN:
							*dwType = BFF_OBJECTTYPE_DOMAIN;
							break;
						case RESOURCEDISPLAYTYPE_SERVER:
							*dwType = BFF_OBJECTTYPE_COMPUTER;
							break;
						case RESOURCEDISPLAYTYPE_SHARE:
							*dwType = BFF_OBJECTTYPE_SHARE;
							break;
						default:
							*dwType = BFF_OBJECTTYPE_NETGENERIC;
							break;
					}

					bSuccess = TRUE;
				}
				else
					bSuccess = FALSE;

				pParentFolder->lpVtbl->Release(pParentFolder);
			}
		}

		if (pidlParent)
			pMalloc->lpVtbl->Free(pMalloc, pidlParent);
		if (pidlChild)
			pMalloc->lpVtbl->Free(pMalloc, pidlChild);
	}
	return bSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     GetFolderPIDL

 Purpose:       Retrieves the item ID list for a file system path or a
 				special folder CSIDL.

 Parameters:

				hWnd:			window handle the ParseDisplayName method may
								use as the parent for any dialog boxes it may
								need to display.

				pShellFolder:	pointer to the Shell's IShellFolder interface
								(use SHGetDesktopFolder to obtain it).

				nCSIDL:			CSIDL for a special file system path. If this
 								parameter is non-NUL, the function ignores the
					    		lpszPath parameter and returns the item ID list
						    	for the	CSIDL in lpIDL. Otherwise, it returns
						    	the item ID list for the path specified in the
								lpszPath parameter.

				lpszPath:		Pointer to a NULL terminated string specifying
								a file system path for which the PIDL to return.

				lpIDL:			Address of a pointer to the item ID list for
								lpszPath or nCSIDL. The caller is responsible
								for freeing this pointer through the Shell's
								task allocator interface (IMalloc).

*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK GetFolderPIDL (HWND hWnd, LPSHELLFOLDER pShellFolder, int nCSIDL, char *lpszPath, LPITEMIDLIST *lpIDL)
{
	HINSTANCE       hDllInstance;
	HRESULT			nRet;
	HRESULT			(_stdcall *W2KME_SHGetFolderLocation)(HWND, INT, HANDLE, DWORD, LPITEMIDLIST *);
	ULONG			chUsed;
	int				nStrlen;
	BOOL			bSuccess = FALSE;

	if (!(pShellFolder && lpIDL))
		return FALSE;

	*lpIDL = NULL;

	if(nCSIDL)
	{
		// In Windows 2000/ME, SHGetFolderLocation superseds SHGetSpecialFolderLocation.
		// So try this function first.
		hDllInstance = LoadLibrary ("SHELL32.DLL");
		if (hDllInstance != NULL)
		{
			W2KME_SHGetFolderLocation = GetProcAddress (hDllInstance, "SHGetFolderLocation");
			if (W2KME_SHGetFolderLocation != NULL)
			{
				nRet = W2KME_SHGetFolderLocation(hWnd, nCSIDL, NULL, 0, lpIDL);
				if (FAILED(nRet))
					*lpIDL = NULL;
				else
					bSuccess = TRUE;
			}
			FreeLibrary (hDllInstance);
		}
		if (!bSuccess)
		{
			nRet = SHGetSpecialFolderLocation(hWnd, nCSIDL, lpIDL);
			if (FAILED(nRet))
				*lpIDL = NULL;
			else
				bSuccess = TRUE;
		}
	}
	else
	{
		if (lpszPath && (*lpszPath != '\0'))
		{
			nStrlen = strlen(lpszPath);
			// ParseDisplayname wants the folder name in UNICODE format.
			// Allocate sufficient memory for the UNICODE string.
			LPWSTR lpszPathW = malloc(nStrlen * 2 + 2);

			if (lpszPathW)
			{
				// Convert to UNICODE
				if (MultiByteToWideChar (CP_ACP, 0, lpszPath, -1, lpszPathW, nStrlen * 2 + 2))
				{
					nRet = pShellFolder->lpVtbl->ParseDisplayName(
					pShellFolder,
		         	hWnd,			//Owner window
		          	NULL,			//Reserved (must be NULL)
		          	lpszPathW,		//Folder name in UNICODE format
		          	&chUsed,		//Number of chars parsed. Always comes back as zero (??)
		          	lpIDL,			//pointer to LPITEMIDLIST
		          	NULL			//Attributes (can be NULL)
					);

					if (FAILED(nRet))
						*lpIDL = NULL;
					else
						bSuccess = TRUE;
				}
				free (lpszPathW);
			}
		}
	}

	return bSuccess;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     BrowseForFolder

 Purpose:       Function exported from DLL that displays the (customized) dialog for browsing for
 				a folder. For parameters, see shbffo.h
*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI BrowseForFolder (HWND hWnd,
							  HICON hIcon,
  							  UINT ulFlags,
							  UINT ulUIFlags,
							  char *lpszTitle,
							  char *lpszMsg,
							  char *lpszButtonText,
							  int nCSIDL,
							  char *lpszRoot,
							  char *lpszFolder,
							  DWORD *dwType)
{
	char			szDisplayName[MAX_PATH + 1];
	char			szFullPath[MAX_PATH + 1];
	BROWSEINFO		bi;
	DWORD			dwReturn, dwAttributes;
	int				nImage = 0;
	LPMALLOC		pMalloc = NULL;
	LPSHELLFOLDER	pShellFolder = NULL;
	HRESULT			hResult;
	LPITEMIDLIST	pidlRoot = NULL;
    LPITEMIDLIST	pidlSelected = NULL;
	PARAMETERS		parameters;

	if ( (lpszFolder == NULL) ||
		 ((ulUIFlags & BFFUI_NEWFOLDER) && ((lpszButtonText == NULL) || (*lpszButtonText == '\0')))
       )
		return BFFRET_INVALIDPARAM;

	if (strlen(lpszFolder) > MAX_PATH)
		return BFFRET_PATHTOOLONG;

	//Get the pointer to the shell's IMalloc interface
	hResult = SHGetMalloc(&pMalloc);
	if (hResult != NOERROR)
		return BFFRET_IMALLOCFAIL;

	//Get the pointer to the desktop folders IShellFolder interface
	//It is used in various routines
	hResult = SHGetDesktopFolder(&pShellFolder);
	if (hResult != NOERROR)
		return BFFRET_ISHELLFAIL;


	//Initialize COM libraries, in case they aren't already initialized...
	hResult = CoInitialize(NULL);
	if (FAILED(hResult))
		return BFFRET_COMINITFAIL;

	//Get the PIDL for the dialog's root folder, if any.
	GetFolderPIDL(hWnd, pShellFolder, nCSIDL, lpszRoot, &pidlRoot);

	//Drop incompatible flags from ulFlags
	ulFlags &= ~(BIF_USENEWUI | BIF_VALIDATE | BIF_UAHINT | BIF_NONEWFOLDERBUTTON | BIF_STATUSTEXT);

	//Fill in our PARAMETERS structure
	ZeroMemory(&parameters, sizeof(PARAMETERS));
	parameters.bBffInitialized = FALSE;
	parameters.bDisableNewFolderButton = FALSE;
	parameters.bDisableOKButton = FALSE;
	parameters.bUserEdit = FALSE;
	parameters.pMalloc = pMalloc;
	parameters.pShellFolder = pShellFolder;
	parameters.ulFlags = ulFlags;
	parameters.ulUIFlags = ulUIFlags;
	parameters.hIcon = hIcon;
	parameters.lpszBrowseDialogTitle = lpszTitle;
	parameters.lpszNewFolderButtonText = lpszButtonText;
	//Needed for comparison in BFFM_SELCHANGED
	strcpy(parameters.szInitialFolder, lpszFolder);

	if ((*lpszFolder != '\0') && (!(ulUIFlags & BFFUI_DONTAPPEND)))
	{
		//Get pointer to the trailing directory. It is the string past the last backslash.
		parameters.lpszAppendDir = lpszFolder;

		//First, make sure we don't treat network server names (\\SERVER) as trailing directories
		if (strstr(parameters.lpszAppendDir, "\\\\") == parameters.lpszAppendDir)
		{
			parameters.lpszAppendDir += strlen("\\\\");
			if (strrchr(parameters.lpszAppendDir, '\\'))
				parameters.lpszAppendDir = strrchr(parameters.lpszAppendDir, '\\') + 1;
			else
				parameters.lpszAppendDir = NULL;
		}
		else if (strrchr(parameters.lpszAppendDir, '\\'))
			parameters.lpszAppendDir = strrchr(parameters.lpszAppendDir, '\\') + 1;
	}

	strcpy(parameters.szSelectedFolder, lpszFolder);
	strcpy(parameters.szDialogSelectedFolder, lpszFolder);

	// Fill in the BROWSEINFO structure
	bi.hwndOwner		= hWnd;
	bi.pidlRoot			= pidlRoot;
	bi.pszDisplayName	= szDisplayName;
	bi.lpszTitle		= lpszMsg;
	bi.ulFlags			= ulFlags;
	bi.lpfn				= BrowseCallbackProc;
	bi.lParam			= (LPARAM)&parameters;
	bi.iImage			= nImage;

	//Browse for Folder.
	pidlSelected = SHBrowseForFolder(&bi);

	if (pidlSelected == NULL)
		//Cancel button was clicked
		dwReturn = BFFRET_CANCEL;
	else
	{
		dwReturn = BFFRET_OK;
		//Return selected object in lpszFolder
		strcpy(lpszFolder, parameters.szSelectedFolder);

		if (dwType)
		{
			//If the user didn't alter the selected folder and it was not modified
			//through appending a trailing directory, we can get type information
			//directly from the PIDL returned by SHBrowseForFolder, otherwise, we
			//must build a PIDL manually
			if ( parameters.bUserEdit ||
			     ( (parameters.lpszAppendDir != NULL) && (*parameters.lpszAppendDir != '\0') )
			   )
			{
				//Free PIDL returned
				pMalloc->lpVtbl->Free(pMalloc, pidlSelected);
				GetFolderPIDL(hWnd, pShellFolder, 0, parameters.szSelectedFolder, &pidlSelected);
			}
			if (!GetInfoFromPIDL (pMalloc, pShellFolder, pidlSelected, dwType))
			{
				//Hmm, shouldn't happen. Test for file system object, anyway.
				dwAttributes = GetFileAttributes(parameters.szSelectedFolder);
				if (dwAttributes != 0xFFFFFFFF)
				{
					if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
						*dwType = BFF_OBJECTTYPE_DIRECTORY;
					else
						*dwType = BFF_OBJECTTYPE_FILE;
				}
			}
		}
		//Free PIDL, if any
		if (pidlSelected)
			pMalloc->lpVtbl->Free(pMalloc, pidlSelected);
	}

	if (parameters.lpszNewFolderDialogTitle)
		//Free dynamically allocated string
		free (parameters.lpszNewFolderDialogTitle);

	if (pidlRoot)
		//Free the root PIDL
		pMalloc->lpVtbl->Free(pMalloc, pidlRoot);

	//Release IShellFolder interface
	pShellFolder->lpVtbl->Release(pShellFolder);
	//Release IMalloc interface
	pMalloc->lpVtbl->Release(pMalloc);

	// Uninitialize COM libraries
	CoUninitialize();

return dwReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     RemoveCRLF

 Purpose:       This routine replaces CR/LFs in a string with a single
				replacement char and double NUL-terminates the resulting
				string
*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL RemoveCRLF(char *lpszInput, char nReplacementChar, char **lpszOutput, int *nCRLF)
{

	BOOL bSuccess = FALSE;

	if(lpszInput && lpszOutput && nCRLF)
	{
		int		len_input = strlen(lpszInput);
		char    *sz_output = (char*)malloc(len_input+2);
		char    *p_a_sz_input = lpszInput;
		char    *p_sz_output  = sz_output;

		*nCRLF = 0;

		if (sz_output != NULL)
		{
			for(;;p_a_sz_input++)
			{
				if((p_a_sz_input - lpszInput) <= len_input)
				{
					if(*p_a_sz_input == '\r')
					{
						if(*(p_a_sz_input+1) == '\n')
						{
							//We have detected a \r\n.
							//Replace and advance the output pointer
							*p_sz_output++ = nReplacementChar;
							//Advance because we ate two characters
							p_a_sz_input++;
							(*nCRLF)++;
							continue;
						} // if(*(p_a_sz_input+1) == '\n')
					} // if(*p_a_sz_input == '\r')
					else
					if(*p_a_sz_input == '\n')
					{
						if(*(p_a_sz_input+1) == '\r')
						{
							//We have detected a \n\r
							//Replace and advance the output pointer
							*p_sz_output++ = nReplacementChar;
							//Advance because we ate two characters
							p_a_sz_input++;
							(*nCRLF)++;
							continue;
						} // if(*(p_a_sz_input+1) == '\n')
					} // if(*p_a_sz_input == '\r')
					else
					{
						*p_sz_output++ = *p_a_sz_input;
					}
				} // if((p_a_sz_input - lpszInput) <= len_input)
				else
				{
					//We're done.
					bSuccess = TRUE;
					*p_sz_output = '\0';
					//Double-Null terminate the string
					*p_sz_output++;
					*p_sz_output = '\0';
					break;
				} // else/if( (p_a_sz_input - lpszInput) <= len_input)
			} // for(;;p_a_sz_input++)
			*lpszOutput = sz_output;
		}
	} // if(lpszInput)
	else
	{
		bSuccess = TRUE;
		*lpszOutput = NULL;
	} // else/if(lpszInput)

	return bSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     FileOperation

 Purpose:       Function exported from DLL that provides access to the Win32
				"Explorer like" file operations. Copies, moves and deletes
			    files. Tuned for use with the Wise Installation System because
			    multiple source/destination files are passed as a CR/LF separated
			    list of files. For parameters, see shbffo.h
*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI FileOperation(HWND hWnd,
						   char *lpszFiles,
						   char *lpszDest,
						   UINT ulFunc,
						   WORD wFlags)
{
	SHFILEOPSTRUCT	sfo;
	char            *lpszSource = NULL;
	char            *lpszTarget = NULL;
	int				nRet, nCRLF1 = 0, nCRLF2 = 0;
	DWORD			dwReturn;

	//Convert CR/LF to single NULL characters in both source and destination file list
	//and double-null terminate the string. This format of the file list is required for
	//SHFileOperation.
	if (!RemoveCRLF (lpszFiles, '\0', &lpszSource, &nCRLF1) ||
		!RemoveCRLF (lpszDest, '\0', &lpszTarget, &nCRLF2)
	   )
	   //If this fails, we are out of memory
	   dwReturn = FORET_NOMEMORY;
	else
	{
		// Check source and destination file list consistency
		if ((nCRLF2 > nCRLF1)																	||
			( (ulFunc != FO_DELETE) &&((wFlags & FOF_MULTIDESTFILES) && (nCRLF1 != nCRLF2))	)	||
			( !(wFlags & FOF_MULTIDESTFILES) && (nCRLF2 != 0) )									||
			( lpszSource == NULL )																||
			( (ulFunc != FO_DELETE) && (lpszTarget == NULL) )
		   )
			// The file lists are not consistent
			dwReturn = FORET_INVALID;
		else
		{
			//Fill in the SHFILEOPSTRUCT structure
			sfo.hwnd					= hWnd;
			sfo.wFunc					= ulFunc;
			sfo.pFrom					= lpszSource;
			sfo.pTo						= lpszTarget;
			sfo.fFlags					= wFlags;
			sfo.fAnyOperationsAborted	= FALSE;
			sfo.hNameMappings			= NULL;
			sfo.lpszProgressTitle		= "";

			//Do file operation
			nRet = SHFileOperation(&sfo);

			if (sfo.fAnyOperationsAborted != FALSE)
				//The user aborted the file operation for some reason.
				dwReturn = FORET_USERABORT;
			else if	(nRet != 0)
				//The system aborted the file operation for some reason.
				dwReturn = FORET_SYSABORT;
			else
			//Success!
			dwReturn = FORET_SUCCESS;
		}
	}

	if (lpszSource)
		free (lpszSource);
	if (lpszTarget)
		free (lpszTarget);

	return dwReturn;
}

