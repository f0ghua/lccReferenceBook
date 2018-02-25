//=============================================================================
// TinyPad - Stuff under options menu.
//=============================================================================
#include <windows.h>
#include <richedit.h>
#include "tinypadres.h"

BOOL bFullScreen = FALSE;
BOOL bWordWarp = TRUE;
static COLORREF TextColor;				// Text color
static COLORREF BackgroundColor;		// RE backround color
extern HWND hwndMain;
extern HWND hEdit;
extern HINSTANCE hInst;

extern void UpdateStatusBar(LPSTR lpszStatusString, WORD partNumber, WORD displayFlags);

//=============================================================================
// Function.....: DlgOptions
// Description..: Litle options, not saved eny way.
// Parameters...: Standard
// Return values: Is message processed.
//=============================================================================
extern BOOL CALLBACK DlgOptions(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static COLORREF Colors[16];
	CHOOSECOLOR cc;
	BOOL bModify;
	CHARFORMAT cfm;

	switch(msg)
	{
		case WM_CTLCOLORSTATIC:
			if((HWND) lParam == GetDlgItem(hDlg, IDC_BACKCOLORBOX))
				return  (INT)  CreateSolidBrush(BackgroundColor);
			else
				if((HWND) lParam == GetDlgItem(hDlg, IDC_TEXTCOLORBOX))
				return (INT)  CreateSolidBrush(TextColor);
		return FALSE;

		case WM_COMMAND:
		switch(LOWORD(wParam))
		{

			case IDC_BACKCOLORBOX:
				ZeroMemory(&cc, sizeof(CHOOSECOLOR));
				cc.lStructSize = sizeof(CHOOSECOLOR);
				cc.hwndOwner   = hwndMain;
				cc.lpCustColors= Colors;
				if(ChooseColor(&cc))
				{
					BackgroundColor = cc.rgbResult;
					InvalidateRect( GetDlgItem(hDlg, IDC_BACKCOLORBOX), 0, TRUE);
				}
			break;

			case IDC_TEXTCOLORBOX:
				ZeroMemory(&cc, sizeof(CHOOSECOLOR));
				cc.lStructSize = sizeof(CHOOSECOLOR);
				cc.hwndOwner   = hwndMain;
				cc.lpCustColors= Colors;
				if(ChooseColor(&cc))
				{
					TextColor = cc.rgbResult;
					InvalidateRect( GetDlgItem(hDlg, IDC_TEXTCOLORBOX), 0, TRUE);
				}
			break;

			case IDOK:
				bModify = SendMessage(hEdit, EM_GETMODIFY, 0, 0);
				SendMessage(hEdit, EM_SETBKGNDCOLOR, 0, BackgroundColor);
				ZeroMemory(&cfm, sizeof(CHARFORMAT));
				cfm.cbSize = sizeof(CHARFORMAT);
				cfm.dwMask = CFM_COLOR;
				cfm.crTextColor = TextColor;
				SendMessage(hEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM) &cfm);
				SendMessage(hEdit, EM_SETMODIFY, bModify, 1);
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
// Function.....: LoadSettings
// Description..: Don't actually load setting only look default values.
// Parameters...: None.
// Return values: None.
//=============================================================================
extern void LoadSettings(void)
{
	// Use default system colors.
	TextColor = GetSysColor(COLOR_WINDOWTEXT);
   	BackgroundColor = GetSysColor(COLOR_WINDOW);
}

//=============================================================================
// Function.....: FullScreen
// Description..: Toggle full screen mode.
// Parameters...: Main window handle.
// Return values: TRUE if success.
//=============================================================================
extern BOOL FullScreen(HWND hwnd)
{
	static WINDOWPLACEMENT wndpl;
	LONG style;
	RECT rect;

	wndpl.length = sizeof(WINDOWPLACEMENT);

	if(!bFullScreen)
	{
		GetWindowPlacement(hwnd, &wndpl);
		SetWindowLong(hwnd, GWL_STYLE, WS_POPUPWINDOW);
		ShowWindow(hwnd, SW_SHOWMAXIMIZED);
		bFullScreen = TRUE;
	}
	else
	{
		SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		ShowWindow(hwnd, SW_RESTORE);
		SetWindowPlacement(hwnd, &wndpl);
		bFullScreen = FALSE;
	}
	return TRUE;
}

//=============================================================================
// Function.....: WordWarp
// Description..: Toggle word warp mode in rich edit control.
// Parameters...: None use global hEdit.
// Return values: TRUE if success.
//=============================================================================
BOOL WordWarp(void)
{
	RECT r;


	if(bWordWarp == TRUE)
	{
		if(SendMessage(hEdit,EM_SETTARGETDEVICE, 0, 0) == 0)
			return FALSE;
		bWordWarp = FALSE;
	}else
	{
		if(GetClientRect(hEdit, &r) == 0)
			return FALSE;
		if(SendMessage(hEdit,EM_SETTARGETDEVICE, 0, r.right) == 0)
			return FALSE;
		bWordWarp = TRUE;
	}
	return TRUE;
}

