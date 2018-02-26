#include <windows.h>
#include <tchar.h>
#include "msghook.h"

// ---------------------------------------------------------------------------
// data structure slated for the memory-mapped file
typedef struct tagHookData {
	HWND  hwndNotify;
	HHOOK hHook;
}
HOOKDATA, *PHOOKDATA;

// ---------------------------------------------------------------------------
// handle to shared data
static HOOKDATA *g_pHookData = NULL;

// ---------------------------------------------------------------------------
// shared memory related globals
static LPCTSTR   g_pszMemoryMappedFileName = _T("MsgHookMappedData");
static HANDLE    g_hMemoryMappedFile = NULL;

// ---------------------------------------------------------------------------
// data structure passed to notify window via WM_COPYDATA
typedef struct tagMsgData {
	MSG msg;
	TCHAR szModName[MAX_PATH];
}
MsgData;

// ---------------------------------------------------------------------------
// per process globals

static HINSTANCE g_hDll = NULL; // needed to set the hook
static TCHAR   g_pszProcessFileName[MAX_PATH];

// ---------------------------------------------------------------------------
// local prototype
static LPTSTR _stdcall GetProcessFileName(LPTSTR pszFileName, DWORD nSize);


// ---------------------------------------------------------------------------
//
BOOL _stdcall LibMain(HINSTANCE hDll, DWORD fdwReason, PVOID pvReserved)
{

	switch ( fdwReason )
	{

		case DLL_PROCESS_ATTACH :

			g_hDll = hDll;

			// set up the per process data
			GetProcessFileName(g_pszProcessFileName, sizeof(g_pszProcessFileName));

			// set up the shared data in the memory mapped file

			// detemine if file mapping object already exists
			g_hMemoryMappedFile = OpenFileMapping(
                                    FILE_MAP_ALL_ACCESS,
                                    FALSE,
                                    g_pszMemoryMappedFileName
                                    );

            // file mapping object exists so map a view of it
			if ( NULL != g_hMemoryMappedFile ) {

				g_pHookData = (HOOKDATA*)MapViewOfFile(
                                    g_hMemoryMappedFile,
                                    FILE_MAP_ALL_ACCESS, 0, 0,
                                    sizeof(*g_pHookData)
                                    );

				if ( NULL == g_pHookData ) {
					// a major error occured so close up and get out
					CloseHandle(g_hMemoryMappedFile);
					g_hMemoryMappedFile = NULL;
					return FALSE;
				}

			}
			else {

				// file mapping object doesn't exist so create it

                SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, FALSE };

				g_hMemoryMappedFile = CreateFileMapping(
                                        INVALID_HANDLE_VALUE,
                                        &sa,
                                        PAGE_READWRITE,
                                        0,
                                        sizeof(*g_pHookData),
                                        g_pszMemoryMappedFileName
                                        );

				if ( NULL != g_hMemoryMappedFile ) {

    				g_pHookData = (HOOKDATA*)MapViewOfFile(
                                                g_hMemoryMappedFile,
                                                FILE_MAP_ALL_ACCESS, 0, 0,
                                                sizeof(*g_pHookData)
                                                );

					if ( NULL == g_pHookData ) {
						// a major error occured so close up and get out
						CloseHandle( g_hMemoryMappedFile );
    					g_hMemoryMappedFile = NULL;
						return FALSE;
					}

                    // zero the newly create file mapping 
					ZeroMemory(g_pHookData, sizeof(*g_pHookData));

				}

			}

			break;

		case DLL_PROCESS_DETACH :

			// unmap the memory mapped file and
			if ( NULL != g_pHookData ) {
				UnmapViewOfFile(g_pHookData);
				g_pHookData = NULL;
			}

			// close the file mapping handle
			if ( NULL != g_hMemoryMappedFile ) {
				CloseHandle( g_hMemoryMappedFile );
				g_hMemoryMappedFile = NULL;
			}

			break;

	}

	return TRUE;

}


// ---------------------------------------------------------------------------
// MsgHook procedure used with WH_GETMESSAGE hook
//
static LRESULT CALLBACK MsgHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	// required checking
	if ( NULL != g_pHookData ) {

		// call CallNextHookEx and return immediately if nCode < 0
		if ( nCode < 0 ) {
			return CallNextHookEx(g_pHookData->hHook, nCode, wParam, lParam);
		}

		// verify relevant g_pHookData members
		if ( !IsWindow(g_pHookData->hwndNotify)  ) {
			return CallNextHookEx(g_pHookData->hHook, nCode, wParam, lParam);
		}

		MSG *pMsg = (MSG *) lParam;

        // alternative messages to capture
		// if ( pMsg->message == WM_RBUTTONDOWN ) {
		// if ( pMsg->message == WM_LBUTTONDOWN ) {
        // if ( pMsg->message == WM_TIMER ) {

		if ( pMsg->message == WM_PAINT ) {

			// prepare for copying the data
			MsgData msgdata;
            ZeroMemory(&msgdata, sizeof(msgdata));

            // copy captured message
			CopyMemory(&(msgdata.msg), pMsg, sizeof(*pMsg));
            // copy process module name
			CopyMemory(msgdata.szModName, g_pszProcessFileName, sizeof(g_pszProcessFileName));

            // prepare the copydata struct
			COPYDATASTRUCT cds;
			cds.dwData = (DWORD)nCode;
			cds.cbData = sizeof(msgdata);
			cds.lpData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(msgdata));

            // copy msgdata to copydata struct
			CopyMemory(cds.lpData, &msgdata, sizeof(msgdata));

			// pass the copied data to the receiving window
			SendMessage(
                    g_pHookData->hwndNotify,
                    WM_COPYDATA,
                    (WPARAM)pMsg->hwnd,
                    (LPARAM)&cds
                    );

            // free memory used by copydata struct
			HeapFree(GetProcessHeap(), 0, cds.lpData);

		}

        // pass message along to the current hook chain
		return CallNextHookEx(g_pHookData->hHook, nCode, wParam, lParam);

	}

	// if the hook procedure does not call CallNextHookEx,
	// the return value should be zero.
	return 0;

}

// ---------------------------------------------------------------------------
// Set the hook.
// If it is already set, unset it first.
// stash handle to window slated to receive copydata
//
BOOL __declspec(dllexport)  _stdcall SetMsgHook(HWND hwndNotify)
{

	// verify the hook is not set and all the global data is initialized
	UnsetMsgHook();

	if ( NULL != g_pHookData ) {

		// set the hook
		g_pHookData->hHook = SetWindowsHookEx(
                                    WH_GETMESSAGE,
                                    (HOOKPROC)MsgHookProc,
								    g_hDll, // dll containing the hook proc
                                    0);     // 0 indicates hook all threads in the desktop 

        // confirm the hook
		if ( NULL != g_pHookData->hHook ) {
			g_pHookData->hwndNotify = hwndNotify;
		}

		return ( NULL != g_pHookData->hHook );

	}

	return FALSE;

}


// ---------------------------------------------------------------------------
// Unset the hook
//
void __declspec(dllexport)  _stdcall UnsetMsgHook(void)
{
	if ( NULL != g_pHookData ) {

		if ( NULL != g_pHookData->hHook ) {
			UnhookWindowsHookEx(g_pHookData->hHook);
		}

		g_pHookData->hHook = NULL;
		g_pHookData->hwndNotify = NULL;
        // g_pHookData = NULL;

	}

}


// ----------------------------------------------------------------------------
// HMODULE hModule - adapt for dll name...
//
static LPTSTR _stdcall GetProcessFileName(LPTSTR pszFileName, DWORD nSize)
{
	if ( NULL == pszFileName ) {
		return NULL;
	}
	ZeroMemory( pszFileName, nSize);
	GetModuleFileName(
		NULL,               		// handle to current process module
		pszFileName,        		// buffer to store the module file name
		nSize                       // size of buffer in TCHARS
		);
	return pszFileName;
}

// ---------------------------------------------------------------------------
// EOF

