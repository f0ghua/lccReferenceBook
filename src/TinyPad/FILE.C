//=============================================================================
// TinyPad - File functions.
//=============================================================================
#include <windows.h>
#include <richedit.h>
#include <shellapi.h>

extern HWND hEdit;

//=============================================================================
// Function.....: LoadFileCallBack
// Description..: RE load file callback function.
// Parameters...: Standard.
// Return values: -1 if error.
//=============================================================================
static DWORD CALLBACK LoadFileCallBack(DWORD dwCookie, LPBYTE lpBuffer, LONG lSize, LONG *plRead)
{
	if(!lSize) return 1;

	// BCC 5.5 needs (LPDWORD) conversion.
	if(!ReadFile((HANDLE) dwCookie,lpBuffer, lSize, (LPDWORD) plRead, NULL))
		return -1;
	return 0;
}

//=============================================================================
// Function.....: LoadFile
// Description..: Load file to RE.
// Parameters...: Filename
// Return values: False if error.
//=============================================================================
extern BOOL LoadFile(LPCSTR szFileName)
{
	HANDLE hFile;
	EDITSTREAM es;

	hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	es.dwCookie = (DWORD) hFile;
	es.dwError = 0;
	es.pfnCallback = LoadFileCallBack;

	SendMessage(hEdit, EM_STREAMIN, SF_TEXT, (LPARAM) &es);
	CloseHandle(hFile);
	return ( es.dwError ? FALSE : TRUE );
}

//=============================================================================
// Function.....: OpenDlg
// Description..: Show open filename dialog.
// Parameters...: buffer - to store filename
//                buffer lengt - MAX_PATH
// Return values: True if success.
//=============================================================================
extern BOOL OpenDlg(LPSTR buffer,INT buflen)
{
	char tmpfilter[120];
	int i = 0;
	OPENFILENAME ofn;
	BOOL bSuccess;

	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hInstance = GetModuleHandle(NULL);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = buflen;
	ofn.lpstrTitle = "Open";
	ofn.nFilterIndex = 3;
	ofn.nFilterIndex = 2;
	buffer[0] = 0;
	strcpy(tmpfilter,"All files (*.*),*.*,Text Files (*.txt),*.txt,Text Files - DOS format (*.txt),*.txt");
	while(tmpfilter[i]) {
		if (tmpfilter[i] == ',')
			tmpfilter[i] = 0;
		i++;
	}
	i++;
	tmpfilter[i] = 0;
	i++;
	tmpfilter[i] = 0;
	ofn.Flags = 539678;
	ofn.lpstrFilter = tmpfilter;
	bSuccess = GetOpenFileName(&ofn);
	return bSuccess;

}

//=============================================================================
// Function.....: SaveDlg
// Description..: Show common save file dialog.
// Parameters...: Buffer to store filename and buffer lenght,
// Return values: False if error.
//=============================================================================
extern BOOL SaveDlg(LPSTR buffer,INT buflen)
{
	char tmpfilter[40];
	int i = 0;
	OPENFILENAME ofn;

	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hInstance = GetModuleHandle(NULL);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = buflen;
	ofn.lpstrTitle = "Save";
	ofn.nFilterIndex = 2;
	ofn.lpstrDefExt = "";
	strcpy(buffer,"*.txt");
	strcpy(tmpfilter,"All files,*.*,Text Files,*.txt");
	while(tmpfilter[i]) {
		if (tmpfilter[i] == ',')
			tmpfilter[i] = 0;
		i++;
	}
	i++;
	tmpfilter[i] = 0;
	i++;
	tmpfilter[i] = 0;
	ofn.Flags = 539678;
	ofn.lpstrFilter = tmpfilter;
	return GetSaveFileName(&ofn);

}

//=============================================================================
// Function.....: SaveFileCallBack
// Description..: RE save file callback function.
// Parameters...: Standard.
// Return values: -1 if error.
//=============================================================================
static DWORD CALLBACK SaveFileCallBack(DWORD dwCookie, LPBYTE lpBuffer, LONG lSize, LONG *plWriten)
{
	if(!lSize) return 1;

	// BCC 5.5 need (LPDWORD) conversion.
	if(!WriteFile((HANDLE) dwCookie, lpBuffer, lSize, (LPDWORD) plWriten, NULL))
		return -1;
	return 0;
}

//=============================================================================
// Function.....: SaveFile
// Description..: Save the file.
// Parameters...: Filename
// Return values: False if error.
	//=============================================================================
extern BOOL SaveFile(LPCSTR szFileName)
{
	HANDLE hFile;
	EDITSTREAM es;

	hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	es.dwCookie = (DWORD) hFile;
	es.dwError = 0;
	es.pfnCallback = SaveFileCallBack;

	SendMessage(hEdit, EM_STREAMOUT, SF_TEXT, (LPARAM) &es);
	CloseHandle(hFile);
	return ( es.dwError ? FALSE : TRUE );
}

//=============================================================================
// Function.....: Print
// Description..: Wery lazy printing.
// Parameters...: RE handle.
// Return values: True if success.
//=============================================================================
extern BOOL Print(HWND hMain)
{
	CHAR szTemp[MAX_PATH];

	if( GetTempPath(MAX_PATH, szTemp) == 0)
		return FALSE;

	if( lstrcat(szTemp, "tinypad.txt") == NULL)
		return FALSE;

	if( SaveFile(szTemp) == FALSE)
		return FALSE;

 	if( (int) ShellExecute(hMain, "print", szTemp, NULL, NULL, SW_SHOWNORMAL) < 33)
		return FALSE;

	return DeleteFile(szTemp);
}

extern BOOL FileProperties(LPCSTR szFileName)
{
	SHELLEXECUTEINFO se;

	ZeroMemory(&se, sizeof(SHELLEXECUTEINFO));
	se.cbSize = sizeof(SHELLEXECUTEINFO);
	se.fMask = SEE_MASK_INVOKEIDLIST;
	se.hwnd = NULL;
	se.lpVerb = "properties";
	se.lpFile = szFileName;
	se.lpDirectory = NULL;
	se.nShow = 0;
	se.hInstApp = NULL;

	return ShellExecuteEx(&se);
}

