// Dialogs.h

#ifndef _DIALOGS_H_
#define _DIALOGS_H_

#include <windows.h>
#include "find.h"

#define SndMsg SendMessage
#define SetWndTxt SetWindowText
#define GetCtrl GetDlgItem

int DoExtendedParams(HWND hwnd, EXTENDED * pext);

BOOL CALLBACK AboutDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK MessageDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
#define MsgBox(hwndDlg, s) 	DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDDIALOG_MESBOX), hwndDlg,(DLGPROC)MessageDialogProc, (LPARAM)s)

BOOL CALLBACK ConfirmDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
#define MsgBoxYesNo(hwndDlg, s)	DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDDIALOG_CONFIRM), hwndDlg,(DLGPROC)ConfirmDialogProc, (LPARAM)s)

BOOL CALLBACK ConfirmAllDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
#define MsgBoxYesAll(hwndDlg, s)	DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDDIALOG_CONFIRMALL), hwndDlg,(DLGPROC)ConfirmAllDialogProc, (LPARAM)s)

BOOL CALLBACK DisplaySendToDialogProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // _DIALOGS_H_
