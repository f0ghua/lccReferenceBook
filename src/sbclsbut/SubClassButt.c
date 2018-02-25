/*--------------------------------------------------------

	SubClassButton.c

	Subclass a button and change mouse pointer when
	mouse is over button.

	John Findlay

---------------------------------------------------------*/
#include <windows.h>
#include "SubClassButton.h"

HCURSOR		hHandCur = NULL;
WNDPROC		pOldBtnProc = NULL;

LRESULT CALLBACK ButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_SETCURSOR: //MOUSEMOVE
			SetCursor(hHandCur);
			return 0;

		case WM_LBUTTONDOWN:
			MessageBeep(MB_ICONHAND);
			// do CallWindowProc() so that button is depressed when clicked
			return CallWindowProc(pOldBtnProc, hWnd, message, wParam, lParam) ;

		default:
			return CallWindowProc(pOldBtnProc, hWnd, message, wParam, lParam) ;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			// Get the original proc address
			// Set the callback for subclass
			pOldBtnProc = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, ID_BTN),
								GWL_WNDPROC, (DWORD)ButtonProc) ;
			return 0;

		case WM_DESTROY:
			// Set the original proc address back
			SetWindowLong(GetDlgItem(hWnd, ID_BTN), GWL_WNDPROC, (DWORD)pOldBtnProc) ;
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	static char szAppName[] = "SUBCLASSBUT";
	HWND        hWnd;
	MSG         msg;
	WNDCLASSEX  wndclassex;

	wndclassex.cbSize        = sizeof(WNDCLASSEX);
	wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc   = (WNDPROC)WndProc;
	wndclassex.cbClsExtra    = 0;
	wndclassex.cbWndExtra    = DLGWINDOWEXTRA;
	wndclassex.hInstance     = hInstance;
	wndclassex.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
	wndclassex.hIconSm       = LoadIcon (NULL, IDI_APPLICATION);
	wndclassex.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wndclassex.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
	wndclassex.lpszMenuName  = NULL;
	wndclassex.lpszClassName = szAppName;

	RegisterClassEx (&wndclassex);

	hHandCur = LoadCursor(hInstance, "hand");

	// Make the dialog that will be main window
	hWnd = CreateDialog (hInstance, MAKEINTRESOURCE(ID_WND), 0, WndProc);

	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

