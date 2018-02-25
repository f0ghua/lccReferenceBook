//################################################################
//### MemStat, by Shervin Emami (the_pope@OldSchoolPunk.com)
//###	(http://www.geocities.com/emami_s/Draw3D.html)
//### created using LCC-Win32 on 6th Feb 2001
//################################################################
//### MemStat is a small Windows 95/98/NT utility that will print
//###  the current memory available.
//### It does this every 400ms when the window is active,
//###  or once per second when a different window is active.
//### Click in the window to close it.
//################################################################

#include <windows.h>
#include <stdio.h>
/*<---------------------------------------------------------------------->*/
HINSTANCE hInst;		// Instance handle
HWND hwndMain;		//Main window handle

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

// My declarations:
BOOL initialized = FALSE;	// whether timer has started
int timeOut = 500;			// milliseconds between WM_TIMER messages
void ShowMemory(void);
void FormatBytes(TCHAR *szMsg, int bytes);


/*<---------------------------------------------------------------------->*/
static BOOL InitApplication(void)
{
	WNDCLASS wc;
	memset(&wc,0,sizeof(WNDCLASS));
	wc.style = CS_BYTEALIGNCLIENT |CS_BYTEALIGNWINDOW |CS_HREDRAW|CS_VREDRAW ;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.hInstance = hInst;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "memorystatusWndClass";
	wc.lpszMenuName = NULL;
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	if (!RegisterClass(&wc))
		return 0;
	return 1;
}

/*<---------------------------------------------------------------------->*/
HWND CreatememoryusageWndClassWnd(void)
{
	return CreateWindowEx(WS_EX_TOPMOST|WS_EX_TOOLWINDOW, "memorystatusWndClass","Memory Status",
		WS_VISIBLE|WS_CAPTION|WS_SIZEBOX,
		CW_USEDEFAULT,0, 280, 88,
		NULL,NULL,hInst,NULL);
}



/*<---------------------------------------------------------------------->*/
LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg) {
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
			// A different window has become active
			timeOut = 1000;
		else
			timeOut = 400;
		break;
	case WM_SIZE:
		if (!initialized) {
			ShowMemory();
			// Send a WM_TIMER message repeatedly
			SetTimer(hwndMain, 0x3000, timeOut, NULL);
		}
		initialized = TRUE;
		break;
	case WM_TIMER:
		ShowMemory();
		break;
	// Quit the program:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_DESTROY:
		KillTimer(hwndMain, 0x3000);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}

/*<---------------------------------------------------------------------->*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	MSG msg;

	hInst = hInstance;
	if (!InitApplication())
		return 0;
	if ((hwndMain = CreatememoryusageWndClassWnd()) == (HWND)0)
		return 0;
	ShowWindow(hwndMain,SW_SHOW);
	while (GetMessage(&msg,NULL,0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}


/*<---------------------------------------------------------------------->*/
// Convert the number of bytes into a readable form
// eg:	70534  ->   "68.8 KB"
void FormatBytes(TCHAR *szMsg, int bytes)
{
	if (bytes < 1000)
		wsprintf(szMsg, "%d bytes", bytes);
	else if (bytes < 1000000)
		sprintf(szMsg, "%.4g KB", (float)bytes / 1024.0 );
	else if (bytes < 1000000000)
		wsprintf(szMsg, "%d,%03d KB", bytes / 1024000, (bytes / 1024) % 1000);
	else
		sprintf(szMsg, "%.4g GB", (float)bytes / (1024.0*1024.0*1024.0) );
}

/*<---------------------------------------------------------------------->*/
// Show available memory
void ShowMemory(void)
{
	MEMORYSTATUS mem;
	HDC hDC;
	HFONT hfnt, hOldFont;
	int x, y;
	RECT rect;
	TCHAR szMsg[256];
	TCHAR szMsg2[16], szMsg3[16];

	// Get the available memory info
	mem.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&mem);

	// Display it
	hDC = GetDC(hwndMain);
    hfnt = GetStockObject(ANSI_VAR_FONT);	// small font
    hOldFont = SelectObject(hDC, hfnt);
	SetBkMode(hDC, TRANSPARENT);	// Transparent text

	x = 5;
	y = 5;
	// Clear the window background with light Yellow
	GetClientRect(hwndMain, &rect);
	FillRect(hDC, &rect, (HBRUSH)(COLOR_INFOBK+1) );
	// Show Physical memory
	FormatBytes(szMsg2, mem.dwAvailPhys);
	FormatBytes(szMsg3, mem.dwTotalPhys - mem.dwAvailPhys);
	wsprintf(szMsg, "RAM:  %s avail, %s used", szMsg2, szMsg3);
	TextOut(hDC, x, (y), szMsg, lstrlen(szMsg));
	// Show Page File
	FormatBytes(szMsg2, mem.dwTotalPageFile - mem.dwAvailPageFile);
	FormatBytes(szMsg3, mem.dwAvailPageFile);
	wsprintf(szMsg, "Paging:  %s used, %s avail", szMsg2, szMsg3);
	TextOut(hDC, x, (y+=15), szMsg, lstrlen(szMsg));
	// Show Virtual memory
	FormatBytes(szMsg2, mem.dwTotalVirtual - mem.dwAvailVirtual);
	FormatBytes(szMsg3, mem.dwAvailVirtual);
	wsprintf(szMsg, "Virtual mem: %s used, %s avail", szMsg2, szMsg3);
	TextOut(hDC, x, (y+=15), szMsg, lstrlen(szMsg));

	SelectObject(hDC, hOldFont);
	ReleaseDC(hwndMain, hDC);
}
