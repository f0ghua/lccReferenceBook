/*********************************************************************
			Dialog.c - Dialog Windows for Small Edit

	This module contains the dialog window procedures for the about,
	find, replace, and goto line dialogs.  This file also contains
	the functions to search and replace text in the edit window.

	Unicode CharSet support (J.Laporte Oct 2002)

	Global Functions:
		DlgAboutProc	- About dialog procedure
		DlgFindProc	- Find text dialog procedure
		DlgReplaceProc	- Replace text dialog procedure
		DlgLineProc	- Goto a line dialog procedure
		FindNext	- Find the next occurance of text in edit
*********************************************************************/
#include <windows.h>
#include <string.h>
#include <richedit.h>

#include "Global.h"
#include "Dialog.h"
#include "Resource.h"

FONT FontEdit;

HANDLE hWndEdit;
HANDLE hWndEditRF;
HANDLE hWndEditRR;

LONG lAlign;



BOOL CALLBACK DlgAboutProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam){
	switch(Message){
		case WM_INITDIALOG:
			SetDlgItemText(hwndDlg, ABOUT_INFO, (LPCTSTR) ABOUT);
			return TRUE;
		case WM_CLOSE:
			EndDialog(hwndDlg, FALSE);
			return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK DlgFindProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam){
	TCHAR  Buffer[MAX_NAME];

   	GetDlgItemText(hwndDlg, FIND_FIND, Buffer, MAX_NAME);

	EnableWindow(GetDlgItem(hwndDlg, FIND_SEARCH), lstrcmp((LPCTSTR)Buffer, TEXT(""))==0 ? FALSE : TRUE);

	switch(Message){
		case WM_INITDIALOG:

			hWndEdit = GetDlgItem(hwndDlg, FIND_FIND);

			if (CharSet==HEBREW_CHARSET || CharSet==ARABIC_CHARSET) {

				lAlign = GetWindowLong(hWndEdit, GWL_EXSTYLE);
				lAlign ^= WS_EX_RIGHT;
				lAlign ^= WS_EX_RTLREADING;
				lAlign ^= WS_EX_LEFTSCROLLBAR;
				SetWindowLong(hWndEdit, GWL_EXSTYLE, lAlign);
				InvalidateRect(hWndEdit, NULL, FALSE);

			}

			SetDlgItemText(hwndDlg, FIND_FIND, Find.Find);
			if(Find.Direction==BACKWARD){
				SendDlgItemMessage(hwndDlg, FIND_BACKWARD, BM_SETCHECK, BST_CHECKED, 0);
			}
			else{
				SendDlgItemMessage(hwndDlg, FIND_FOWARD, BM_SETCHECK, BST_CHECKED, 0);
			}
			if(Find.bCase==TRUE){
				SendDlgItemMessage(hwndDlg, FIND_CASE, BM_SETCHECK, BST_CHECKED, 0);
			}
			if(Find.bWord==TRUE){
				SendDlgItemMessage(hwndDlg, FIND_WORD, BM_SETCHECK, BST_CHECKED, 0);
			}
			return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case FIND_SEARCH:
					if(RunSearch(hwndDlg)!=FAILURE){
						EndDialog(hwndDlg, FALSE);
					}
					break;
				case FIND_CANCEL:
					EndDialog(hwndDlg, FALSE);
					break;
			}
			break;

		case WM_CLOSE:
			EndDialog(hwndDlg, FALSE);
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------------------------------
 Procedure:     RunSearch ID:1
 Purpose:       Fills in Find.cr strucuture with the charectors to
                search and determines which options to enable.
 Input:         The handle to the current dialog window.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int static RunSearch(HWND hwndDlg){
	GetDlgItemText(hwndDlg, FIND_FIND, Find.Find, MAX_NAME);

	if(lstrcmp(Find.Find, TEXT(""))==0){
		MSGBOX(hwndDlg, MSG_NTFIND);
		return FAILURE;
	}

	Find.bCase = FALSE;
	Find.bWord = FALSE;
	Find.bReplace = FALSE;
	SendMessage(hEdit, EM_EXGETSEL, 0, (LPARAM)&Find.cr);

	if(SendMessage(GetDlgItem(hwndDlg, FIND_CASE), BM_GETCHECK, 0, 0)==BST_CHECKED){
		Find.bCase = TRUE;
	}

	if(SendMessage(GetDlgItem(hwndDlg, FIND_WORD), BM_GETCHECK, 0, 0)==BST_CHECKED){
		Find.bWord = TRUE;
	}

	if(SendMessage(GetDlgItem(hwndDlg, FIND_FOWARD), BM_GETCHECK, 0, 0)==BST_CHECKED){
		Find.Direction = FOWARD;
	}
	else{
		Find.Direction = BACKWARD;
	}
	Find.cr.cpMin = 0;
	Find.cr.cpMax = GetWindowTextLength(hEdit);

	FindNext(hwndDlg);
	return SUCCESS;
}

/*------------------------------------------------------------------------
 Procedure:     FindNext ID:1
 Purpose:       Takes the current char range from Find.cr and checks
                to see if the string Find.Find is in that range.  If
                it is it is highlighted if not a message explains
                what happened.
 Input:         Handle to the main window.
 Output:        Success or failure.
 Errors:        Validates that Find.Find contains text and if the
                word is contained int the document.
------------------------------------------------------------------------*/
int FindNext(HWND hwnd){
	FINDTEXTEX ft;
	CHARRANGE CurPosition;
	int Pos = 0;
	UINT Flags = 0;

	SendMessage(hEdit, EM_EXGETSEL, 0, (LPARAM)&CurPosition);
	Find.cr.cpMin = CurPosition.cpMin + 1;

	if(lstrcmp(Find.Find, TEXT(""))==0){
		MSGBOX(hwnd, MSG_NTFIND);
		return FAILURE;
	}

	if(Find.bCase==TRUE){
		Flags = Flags | FR_MATCHCASE;
	}
	if(Find.Direction==FOWARD){
		Flags = Flags | FR_DOWN;
	}
	if(Find.bWord==TRUE){
		Flags = Flags | FR_WHOLEWORD;
	}


	ft.lpstrText = Find.Find;
	ft.chrg.cpMin = Find.cr.cpMin;
	ft.chrg.cpMax = Find.cr.cpMax;

	Pos = SendMessage(hEdit, EM_FINDTEXTEX, Flags, (LPARAM)&ft);
	Find.cr.cpMin = Pos+1;

	if(Pos==-1){
		MSGBOX(hwnd, MSG_NOFIND);
		return FAILURE;
	}

	SendMessage(hEdit, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
	return SUCCESS;
}

BOOL CALLBACK DlgReplaceProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam){
	TCHAR  lpFind[MAX_NAME];
	TCHAR  lpReplace[MAX_NAME];

	GetDlgItemText(hwndDlg, REPLACE_FIND, lpFind, MAX_NAME);
	GetDlgItemText(hwndDlg, REPLACE_REPLACE, lpReplace, MAX_NAME);
	EnableWindow(GetDlgItem(hwndDlg, REPLACE_GO), (lstrcmp(lpFind, TEXT(""))==0||lstrcmp(lpReplace, TEXT(""))==0 ? FALSE : TRUE));

	switch(Message){
		case WM_INITDIALOG:

			hWndEditRF = GetDlgItem(hwndDlg, REPLACE_FIND);
    		hWndEditRR = GetDlgItem(hwndDlg, REPLACE_REPLACE);

			if (CharSet==HEBREW_CHARSET || CharSet==ARABIC_CHARSET) {

				lAlign = GetWindowLong(hWndEditRF, GWL_EXSTYLE);
				lAlign ^= WS_EX_RIGHT;
				lAlign ^= WS_EX_RTLREADING;
				lAlign ^= WS_EX_LEFTSCROLLBAR;
				SetWindowLong(hWndEditRF, GWL_EXSTYLE, lAlign);
				InvalidateRect(hWndEditRF, NULL, FALSE);

				lAlign = GetWindowLong(hWndEditRR, GWL_EXSTYLE);
				lAlign ^= WS_EX_RIGHT;
				lAlign ^= WS_EX_RTLREADING;
				lAlign ^= WS_EX_LEFTSCROLLBAR;
				SetWindowLong(hWndEditRR, GWL_EXSTYLE, lAlign);
				InvalidateRect(hWndEditRR, NULL, FALSE);

			}


			SetDlgItemText(hwndDlg, REPLACE_FIND, Find.Find);
			SetDlgItemText(hwndDlg, REPLACE_REPLACE, Find.Replace);
			if(Find.Direction==BACKWARD){
				SendDlgItemMessage(hwndDlg, REPLACE_BCKWRD, BM_SETCHECK, BST_CHECKED, 0);
			}
			else{
				SendDlgItemMessage(hwndDlg, REPLACE_FOWARD, BM_SETCHECK, BST_CHECKED, 0);
			}
			if(Find.bCase==TRUE){
				SendDlgItemMessage(hwndDlg, REPLACE_CASE, BM_SETCHECK, BST_CHECKED, 0);
			}
			if(Find.bWord==TRUE){
				SendDlgItemMessage(hwndDlg, REPLACE_WORD, BM_SETCHECK, BST_CHECKED, 0);
			}

			SendDlgItemMessage(hwndDlg, REPLACE_CONFIRM, BM_SETCHECK, BST_CHECKED, 0);

			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case REPLACE_GO:
					RunReplace(hwndDlg);
					return TRUE;
				case REPLACE_CANCEL:
					EndDialog(hwndDlg, FALSE);
					return TRUE;
			}
			break;
		case WM_CLOSE:
			EndDialog(hwndDlg, FALSE);
			return TRUE;
	}
	return FALSE;
}

int static RunReplace(HWND hwndDlg){
	int Answer = IDYES;

	GetDlgItemText(hwndDlg, REPLACE_FIND, Find.Find, MAX_NAME);
	GetDlgItemText(hwndDlg, REPLACE_REPLACE, Find.Replace, MAX_NAME);

	if(lstrcmp(Find.Find, TEXT(""))==0||lstrcmp(Find.Replace,TEXT(""))==0){
		MSGBOX(hwndDlg, MSG_NTFIND);
		return FAILURE;
	}

	Find.bConfirm = FALSE;
	Find.bWord = FALSE;
	Find.bCase = FALSE;
	Find.Direction = BACKWARD;
	Find.bReplace = TRUE;

	if(SendDlgItemMessage(hwndDlg, REPLACE_CONFIRM, BM_GETCHECK, 0, 0)==BST_CHECKED){
		Find.bConfirm = TRUE;
	}
	if(SendDlgItemMessage(hwndDlg, REPLACE_WORD, BM_GETCHECK, 0, 0)==BST_CHECKED){
		Find.bWord = TRUE;
	}
	if(SendDlgItemMessage(hwndDlg, REPLACE_CASE, BM_GETCHECK, 0, 0)==BST_CHECKED){
		Find.bCase = TRUE;
	}
	if(SendDlgItemMessage(hwndDlg, REPLACE_FOWARD, BM_GETCHECK, 0, 0)==BST_CHECKED){
		Find.Direction = TRUE;
	}

	Find.cr.cpMin = 0;
	Find.cr.cpMax = GetWindowTextLength(hEdit);

	EndDialog(hwndDlg, FALSE);

	while(Answer!=IDCANCEL&&FindNext(hwndDlg)!=FAILURE){
		if(Find.bConfirm==TRUE){
			Answer = MessageBox(hwndDlg, TEXT("Would you like to change the selected text?"), TEXT("Find and Replace"), MB_YESNOCANCEL | MB_ICONQUESTION);
		}
		if(Answer==IDYES){
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)Find.Replace);
		}
	}
	return SUCCESS;
}

BOOL CALLBACK DlgLineProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam){
	TCHAR  Buffer[MAX_NAME];
	ULONG LineNum = 0;
	CHARRANGE cr;

	GetDlgItemText(hwndDlg, LINE_NUMBER, Buffer, MAX_NAME);

	EnableWindow(GetDlgItem(hwndDlg, LINE_GO), (lstrcmp(Buffer, TEXT(""))==0 ? FALSE : TRUE));

	switch(Message){
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case LINE_GO:
					LineNum = _ttoi((TCHAR *)Buffer);
					cr.cpMin = SendMessage(hEdit, EM_LINEINDEX, LineNum-1, 0);
					cr.cpMax = cr.cpMin;
					if(cr.cpMin==-1||LineNum==0){
						MSGBOX(hwndDlg, MSG_NFLINE);
						return TRUE;
					}
					SendMessage(hEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
					EndDialog(hwndDlg, FALSE);
					return TRUE;
				case LINE_CANCEL:
					EndDialog(hwndDlg, FALSE);
					return TRUE;
			}
			break;
		case WM_CLOSE:
			EndDialog(hwndDlg, FALSE);
			return TRUE;
	}
	return FALSE;
}
