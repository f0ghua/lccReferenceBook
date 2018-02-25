//=============================================================================
// TinyPad - Commands under search menu.
//=============================================================================
#include <windows.h>
#include <richedit.h>
#include "tinypadres.h"


CHAR FindBuffer[MAX_PATH]="";	// Search parse
FINDTEXTEX findtext;			// Used by search.

extern HWND hEdit;

//=============================================================================
// Function.....: DlgFind
// Description..: Find dialog handle search.
// Parameters...: Standard
// Return values: Is message processed.
//=============================================================================
#pragma argsused // No lParam is used.
extern BOOL CALLBACK DlgFind(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT uFlags;

	switch(msg)
	{
		case WM_INITDIALOG:
			CheckRadioButton(hDlg, IDC_UP, IDC_DOWN, IDC_DOWN);
			SendDlgItemMessage(hDlg, IDC_FINDEDIT, WM_SETTEXT,0,  (LPARAM) FindBuffer);
		break;

		case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case IDOK:
				uFlags = 0;
				// No return value.
				SendMessage(hEdit, EM_EXGETSEL, 0,  (LPARAM) &findtext.chrg);
				if(GetDlgItemText(hDlg, IDC_FINDEDIT, (LPTSTR) &FindBuffer, sizeof(FindBuffer)))
				{
					if(IsDlgButtonChecked(hDlg, IDC_DOWN) == BST_CHECKED)
					{
						uFlags = FR_DOWN;
						if(findtext.chrg.cpMin)
						{
							LONG t;
							t = findtext.chrg.cpMax;
							findtext.chrg.cpMin = t;
						}else findtext.chrg.cpMax = -1;
					}else
						findtext.chrg.cpMax = 0;

					if(IsDlgButtonChecked(hDlg, IDC_MATCHCASE) == BST_CHECKED)
						uFlags = uFlags | FR_MATCHCASE;
					if(IsDlgButtonChecked(hDlg, IDC_WHOLEWORD) == BST_CHECKED)
						uFlags = uFlags | FR_WHOLEWORD;
					findtext.lpstrText = FindBuffer;
					if(-1 != SendMessage(hEdit, EM_FINDTEXTEX, uFlags, (LPARAM) (FINDTEXT FAR *) &findtext))
						SendMessage(hEdit, EM_EXSETSEL, 0, (LPARAM) (CHARRANGE FAR *) &findtext.chrgText);
				}
				EndDialog(hDlg, 1);
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

//=============================================================================
// Function.....: DlgReplace
// Description..: Replace command.
// Parameters...: Standard
// Return values: Is message processed.
//=============================================================================
extern BOOL CALLBACK DlgReplace(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT uFlags;
	static char szReplace[MAX_PATH];
	static SETTEXTEX settext;

	switch(msg)
	{
		case WM_INITDIALOG:
			CheckRadioButton(hDlg, IDC_UP, IDC_DOWN, IDC_DOWN);
			SendDlgItemMessage(hDlg, IDC_FINDEDIT, WM_SETTEXT,0,  (LPARAM) FindBuffer);
		break;

		case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case IDC_FINDEDIT:
				if(HIWORD(wParam) == EN_CHANGE)
					if(SendMessage( (HWND) lParam, WM_GETTEXTLENGTH, 0, 0))
					{
						EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
						EnableWindow(GetDlgItem(hDlg, IDC_REPLACEALL), TRUE);
					}
					else
					{
						EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
						EnableWindow(GetDlgItem(hDlg, IDC_REPLACEALL), FALSE);
					}
			break;

			case IDOK:
				uFlags = 0;
				// No return value.
				SendMessage(hEdit, EM_EXGETSEL, 0,  (LPARAM) &findtext.chrg);
				if(GetDlgItemText(hDlg, IDC_FINDEDIT, (LPTSTR) &FindBuffer, sizeof(FindBuffer)))
				{
					if(IsDlgButtonChecked(hDlg, IDC_DOWN) == BST_CHECKED)
					{
						uFlags = FR_DOWN;
						if(findtext.chrg.cpMin != findtext.chrg.cpMax)
						{
							findtext.chrg.cpMin = findtext.chrg.cpMax;
						}
						findtext.chrg.cpMax = -1;
					}else findtext.chrg.cpMax = 0;

					if(IsDlgButtonChecked(hDlg, IDC_MATCHCASE) == BST_CHECKED)
						uFlags = uFlags | FR_MATCHCASE;
					if(IsDlgButtonChecked(hDlg, IDC_WHOLEWORD) == BST_CHECKED)
						uFlags = uFlags | FR_WHOLEWORD;
					findtext.lpstrText = FindBuffer;
					if(-1 != SendMessage(hEdit, EM_FINDTEXTEX, uFlags, (LPARAM) (FINDTEXT FAR *) &findtext))
					{
						SendMessage(hEdit, EM_EXSETSEL, 0, (LPARAM) (CHARRANGE FAR *) &findtext.chrgText);
						EnableWindow(GetDlgItem(hDlg, IDC_REPLACE), TRUE);
						EnableWindow(GetDlgItem(hDlg, IDC_REPLACEALL), TRUE);
					}
					else
						MessageBeep(MB_OK);
				}
			break;

			case IDC_REPLACE:

				if(GetDlgItemText(hDlg, IDC_REDIT, (LPTSTR) &szReplace, sizeof(szReplace)) == 0)
					szReplace[0] = 0;
				settext.flags = ST_SELECTION;
				settext.codepage = CP_ACP;

				SendMessage(hEdit, EM_SETTEXTEX, (WPARAM) &settext, (LPARAM) &szReplace);
				EnableWindow(GetDlgItem(hDlg, IDC_REPLACE), FALSE);
			break;

			case IDC_REPLACEALL:
				uFlags = 0;
				if(GetDlgItemText(hDlg, IDC_REDIT, (LPTSTR) &szReplace, sizeof(szReplace)) == 0)
					szReplace[0] = 0;

				if(GetDlgItemText(hDlg, IDC_FINDEDIT, (LPTSTR) &FindBuffer, sizeof(FindBuffer)))
				{
					if(IsDlgButtonChecked(hDlg, IDC_DOWN) == BST_CHECKED)
					{
						uFlags = FR_DOWN;
						if(findtext.chrg.cpMin)
						{
							LONG t;
							t = findtext.chrg.cpMax;
							findtext.chrg.cpMin = t;
						}else findtext.chrg.cpMax = -1;
					}else findtext.chrg.cpMax = 0;

					//uFlags = FR_DOWN;
					findtext.chrg.cpMin = 0;
					findtext.chrg.cpMax = -1;
					if(IsDlgButtonChecked(hDlg, IDC_MATCHCASE) == BST_CHECKED)
						uFlags = uFlags | FR_MATCHCASE;
					if(IsDlgButtonChecked(hDlg, IDC_WHOLEWORD) == BST_CHECKED)
						uFlags = uFlags | FR_WHOLEWORD;
					findtext.lpstrText = FindBuffer;
					settext.flags = ST_SELECTION;
					settext.codepage = CP_ACP;

					while(1)
					{
						if(-1 == SendMessage(hEdit, EM_FINDTEXTEX, uFlags, (LPARAM) (FINDTEXT FAR *) &findtext))
							break;
						else
						{
							SendMessage(hEdit, EM_EXSETSEL, 0, (LPARAM) (CHARRANGE FAR *) &findtext.chrgText);
							SendMessage(hEdit, EM_SETTEXTEX, (WPARAM) &settext, (LPARAM) &szReplace);
						}
					}
				}
				EndDialog(hDlg, 1);
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

//=============================================================================
// Function.....: DlgGotoLine
// Description..: Jump to specifid line.
// Parameters...: Standard
// Return values: Is message processed.
//=============================================================================
#pragma argsused // No lParam is used.
extern BOOL CALLBACK DlgGotoLine(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT LineNo;
	CHARRANGE chrg;
	BOOL bSuccess;
	DWORD max;

	switch(msg)
	{
		case WM_INITDIALOG:
			SetFocus(GetDlgItem(hDlg, IDC_LINENO));
		break;

		case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case IDOK:
				LineNo = GetDlgItemInt(hDlg, IDC_LINENO, &bSuccess, FALSE);
				max = SendMessage(hEdit, EM_GETLINECOUNT, 0, 0);
				if(max > LineNo)
				{
					LineNo = SendMessage(hEdit, EM_LINEINDEX, LineNo, 0);
					LineNo--;
					SendMessage(hEdit, EM_SETSEL, LineNo, LineNo);
					SetFocus(hEdit);
				}
				EndDialog(hDlg, 1);
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
