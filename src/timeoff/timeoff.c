/*--------------------------------------------------------
   TIMEOFF.C -- Turn off computer at a given time.
--------------------------------------------------------*/

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include "resource.h"

// Link with shell32.lib

/* global variables */
HANDLE	g_hInst;

/* function definitions */
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

/****************************
* WinMain
****************************/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND					hWnd;
	MSG						msg;
	WNDCLASSEX				wndclassex;
	INITCOMMONCONTROLSEX	InitCtrls;

	g_hInst = hInstance;

	memset(&InitCtrls,0,sizeof(InitCtrls));
	InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrls.dwICC = ICC_DATE_CLASSES;

	InitCommonControlsEx(&InitCtrls);

	wndclassex.cbSize		= sizeof(WNDCLASSEX);
	wndclassex.style		= CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc	= (WNDPROC)WndProc;
	wndclassex.cbClsExtra	= 0;
	wndclassex.cbWndExtra	= DLGWINDOWEXTRA;
	wndclassex.hInstance	= hInstance;
	wndclassex.hIcon		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wndclassex.hIconSm		= LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	wndclassex.hCursor		= LoadCursor (NULL, IDC_ARROW);
	wndclassex.hbrBackground= (HBRUSH)(COLOR_BTNFACE + 1);
	wndclassex.lpszMenuName	= NULL;
	wndclassex.lpszClassName= TEXT("TIMEOFF");

	RegisterClassEx(&wndclassex);

	hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_TIMEOFF), (HWND)0, (DLGPROC)WndProc);

	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if(!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam ;
}

/****************************
* WndProc
****************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static FILETIME 	quitFTime;
	static HGDIOBJ		hOldTitleFont = NULL, hNewTitleFont;
	long				ctrlID;
	LOGFONT				lFont;
	FILETIME			currentFTime;
	SYSTEMTIME			tmpSTime, quitSTime;
	OSVERSIONINFO		versionInfo;
	HANDLE				hToken;
	TOKEN_PRIVILEGES	tkp;
	HMENU				hSysMenu;

	switch (message)
	{
		case WM_INITDIALOG:
			/* appends about... item to system menu */
			hSysMenu = GetSystemMenu(hWnd, FALSE);
			AppendMenu(hSysMenu, MF_SEPARATOR, 0, NULL);
			AppendMenu(hSysMenu, MF_STRING, IDC_ABOUT, TEXT("About..."));
			break;

		case WM_CTLCOLORSTATIC:
			/* draws application name with a bold font */
			ctrlID = GetWindowLong((HWND) lParam, GWL_ID);

			if(ctrlID == IDC_TITLE)
			{
				SetBkMode((HDC)wParam, TRANSPARENT);

				if(!hOldTitleFont)
				{
					ZeroMemory(&lFont, sizeof(LOGFONT));

					hOldTitleFont = GetCurrentObject((HDC) wParam, OBJ_FONT);
					GetObject(hOldTitleFont, sizeof(LOGFONT), &lFont);
					lFont.lfWeight = FW_BOLD ;
					hNewTitleFont = CreateFontIndirect(&lFont);
				}
				SelectObject((HDC) wParam, hNewTitleFont);

				return (long)GetStockObject(NULL_BRUSH);
			}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_EXIT:
					SendMessage(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
					break;

				case IDC_START:
					/* gets quit date */
					SendDlgItemMessage(hWnd, IDC_DATEPICKER, DTM_GETSYSTEMTIME, 0, (LPARAM)&tmpSTime);
					quitSTime.wYear = tmpSTime.wYear;
					quitSTime.wMonth = tmpSTime.wMonth;
					quitSTime.wDayOfWeek = tmpSTime.wDayOfWeek;
					quitSTime.wDay = tmpSTime.wDay;

					/* gets quit time */
					SendDlgItemMessage(hWnd, IDC_TIMEPICKER, DTM_GETSYSTEMTIME, 0, (LPARAM)&tmpSTime);
					quitSTime.wHour = tmpSTime.wHour;
					quitSTime.wMinute = tmpSTime.wMinute;
					quitSTime.wSecond = tmpSTime.wSecond;
					quitSTime.wMilliseconds = tmpSTime.wMilliseconds;

					SystemTimeToFileTime(&quitSTime, &quitFTime);

					/* gets current time */
					GetLocalTime(&tmpSTime);
					SystemTimeToFileTime(&tmpSTime, &currentFTime);

					/* starts a timer */
					if(SetTimer(hWnd, IDC_TIMER, 1000, NULL) == 0)
					{
						MessageBox(hWnd, TEXT("Error creating timer."), TEXT("Error"), MB_ICONERROR | MB_OK);
						break;
					}
					/* disables Start button and enables Stop button */
					EnableWindow(GetDlgItem(hWnd, IDC_START), FALSE);
					EnableWindow(GetDlgItem(hWnd, IDC_STOP), TRUE);
					break;

				case IDC_STOP:
					KillTimer(hWnd, IDC_TIMER);
					/* enables Start button and disables Stop button */
					EnableWindow(GetDlgItem(hWnd, IDC_START), TRUE);
					EnableWindow(GetDlgItem(hWnd, IDC_STOP), FALSE);
					break;
			}
			break;

		case WM_SYSCOMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_ABOUT:
					/* standard about box */
					ShellAbout(hWnd, "", TEXT("Time OFF v1.0 - Freeware\r\nCopyright (C) 2001 by Luca"), LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON)));
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_TIMER:
			/* gets current time */
			GetLocalTime(&tmpSTime);
			SystemTimeToFileTime(&tmpSTime, &currentFTime);

			/* checks if current time is greater than quit time */
			if(CompareFileTime(&currentFTime, &quitFTime) > 0)
			{
				/* stops timer */
				KillTimer(hWnd, IDC_TIMER);

				/* enables Start button and disables Stop button */
				EnableWindow(GetDlgItem(hWnd, IDC_START), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_STOP), FALSE);

				/* tests for Win9x or WinNT */
				versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
				GetVersionEx(&versionInfo);
				if(versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
				{
					/* snippet from Microsoft SDK documentation */
					/*------------------------------------------*/
					// Get a token for this process.
					if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
					{
						MessageBox(hWnd, TEXT("Error opening process tocken."), TEXT("Error"), MB_ICONERROR | MB_OK);
						break;
					}
					// Get the LUID for the shutdown privilege.
					LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

					tkp.PrivilegeCount = 1;  // one privilege to set
					tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

					// Get the shutdown privilege for this process.
					AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

					// Cannot test the return value of AdjustTokenPrivileges.
					if(GetLastError() != ERROR_SUCCESS)
					{
						MessageBox(hWnd, TEXT("Error adjusting tocken privileges."), TEXT("Error"), MB_ICONERROR | MB_OK);
						break;
					}

					// Shut down the system and force all applications to close.
					if(!ExitWindowsEx(EWX_SHUTDOWN, 0))
					{
						MessageBox(hWnd, TEXT("Error quitting Windows."), TEXT("Error"), MB_ICONERROR | MB_OK);
						break;
					}
					/*------------------------------------------*/
				}
				else
				{
					if(!ExitWindowsEx(EWX_SHUTDOWN, 0))
					{
						MessageBox(hWnd, TEXT("Error quitting Windows."), TEXT("Error"), MB_ICONERROR | MB_OK);
						break;
					}
				}
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

