#include <windows.h>

HHOOK			g_hMsgHook = NULL;
HWND			g_hLCCWnd = NULL;
HINSTANCE		g_hInst = NULL;
unsigned int	g_nLNumber = 5;

/************************************
* LibMain: DLL entry point
*************************************/
BOOL WINAPI __declspec(dllexport) LibMain(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved)
{
	switch( fdwReason )
	{
		case DLL_PROCESS_ATTACH:

			OutputDebugString("Attached!\n");

			g_nLNumber = GetPrivateProfileInt("SETTINGS", "LinesToScroll", 5, "LCCWheel.ini");

			g_hInst = hDLLInst;
			break;

		case DLL_PROCESS_DETACH:

			OutputDebugString("Detached\n");
			break;
	}
	return TRUE;
}

/************************************
* MsgHookProc: hooked procedure
*************************************/
LRESULT CALLBACK MsgHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LPMSG			lpMsg;
	unsigned int	i;

	lpMsg = (LPMSG)lParam;

	if (lpMsg->message == WM_MOUSEWHEEL)
	{
		if ((short) HIWORD(lpMsg->wParam) > 0)
		{
			for (i = 0; i < g_nLNumber; i++)
			{
				SendMessage(lpMsg->hwnd, WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), (LPARAM)NULL);
			}
		}
		else
		{
			for (i = 0; i < g_nLNumber; i++)
			{
				SendMessage(lpMsg->hwnd, WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), (LPARAM)NULL);
			}
		}
	}

	return CallNextHookEx(g_hMsgHook, nCode, wParam, lParam) ;
}

/************************************
* StartHook: Starts WH_GETMESSAGE hook
*************************************/
BOOL WINAPI __declspec(dllexport) StartHook()
{
	DWORD			dwThreadID;
	char			cBuffer[64];

	/* looking for LCC main window handle */
	g_hLCCWnd = FindWindow("clMDIFrame", NULL);
	if (g_hLCCWnd == NULL)
		return FALSE ;
	wsprintf(cBuffer, "LCC window handle: 0x%x\n", g_hLCCWnd);
	OutputDebugString(cBuffer) ;

	/* get creator thread ID */
	dwThreadID = GetWindowThreadProcessId(g_hLCCWnd, NULL);
	wsprintf(cBuffer, "LCC main thread ID: 0x%x\n", dwThreadID);
	OutputDebugString(cBuffer);

	/* inject DLL */
	g_hMsgHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)MsgHookProc, g_hInst, dwThreadID);
	if (g_hMsgHook == NULL)
		return FALSE;
	wsprintf(cBuffer, "Message hook handle: 0x%x\n", g_hMsgHook);
	OutputDebugString(cBuffer);

	return TRUE;
}

/************************************
* StopHook: Stops WH_GETMESSAGE hook
*************************************/
BOOL WINAPI __declspec(dllexport) StopHook()
{
	return UnhookWindowsHookEx(g_hMsgHook);
}
