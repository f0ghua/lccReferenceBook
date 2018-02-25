/*********************************************************************
				Tools.c - Tools Source File for Small Edit

	This module contains the commonly used tools and options
	in Small Edit.  This file contains such features as new, open,
	save, saveas, print, etc....

	Almost all the functions contained in this module are global and called
	from the WndMainProc.
*********************************************************************/
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <winspool.h>
#include <richedit.h>

#include "global.h"

HDC static GetPrinterDC(void);

/*------------------------------------------------------------------------
 Procedure:     New ID:1
 Purpose:       Creates a new file.  If the current file needs to be
				saved a message box informs the user.
 Input:         The handle to the main window.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int New(HWND hwnd){
	TCHAR  Buffer[MAX_NAME];
	int Answer = 0;

	if(SendMessage(hEdit, EM_GETMODIFY, 0, 0)){
		_sntprintf(Buffer, MAX_NAME, TEXT("The text in %s has changed.\nWould you like to save it?"), Info.FileName);
		Answer = MessageBox(hwnd, Buffer, WND_TITLE, MB_YESNOCANCEL | MB_ICONQUESTION);
		switch(Answer){
			case IDYES:
				return FAILURE;
			case IDCANCEL:
				return FAILURE;
		}
	}

	lstrcpy(Info.FileName, NEW_FILE_NAME);
	SetWindowText(hEdit, TEXT(""));
	SendMessage(hEdit, EM_SETMODIFY, FALSE, 0);

	return SUCCESS;
}

/*------------------------------------------------------------------------
 Procedure:     Open ID:1
 Purpose:       Choose a new file to load.  Displays a message box if
				the current file needs to be saved.
 Input:         The handle to the main window.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int Open(HWND hwnd){
	TCHAR  Buffer[MAX_NAME];
	int Answer = 0;

	if(SendMessage(hEdit, EM_GETMODIFY, 0, 0)){
		_sntprintf(Buffer, MAX_NAME, TEXT("The text in %s has changed.\nWould you like to save it?"), Info.FileName);
		Answer = MessageBox(hwnd, Buffer, WND_TITLE, MB_YESNOCANCEL | MB_ICONQUESTION);
		switch(Answer){
			case IDYES:
				return FAILURE;
			case IDCANCEL:
				return FAILURE;
		}
	}

	OPENFILENAME ofn;
	TCHAR  szFileName[MAX_FILE] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = FILE_FILTER;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = TEXT("uni");

	if(GetOpenFileName(&ofn)){
		LoadFile(hwnd, szFileName);
		SendMessage(hEdit, EM_SETMODIFY, FALSE, 0);
		return SUCCESS;
	}

	return FAILURE;
}

/*------------------------------------------------------------------------
 Procedure:     LoadFile ID:1
 Purpose:       Loads a file into memory and then onto the edit
				window.
 Input:         The handle to the main window and file to load.
 Output:        Success or failure.
 Errors:        Validates malloc and a file handle.
------------------------------------------------------------------------*/
int LoadFile(HWND hwnd, TCHAR  FileName[]){
	HANDLE hFile;
	DWORD FileSize;
	DWORD HighBytes;
	DWORD Read;
	PBYTE pBuffer;
	int UniTest;
	WORD   wByteOrderMark;

	lstrcpy(Info.FileName, FileName);

	hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile!=NULL) {

		FileSize = GetFileSize(hFile, &HighBytes);


		if(FileSize!=0xFFFFFFFF&&GetLastError()==NO_ERROR){

			pBuffer = malloc(FileSize+2);
			if(pBuffer!=NULL){

				ReadFile(hFile, pBuffer, FileSize, &Read, NULL);
				CloseHandle(hFile);

				pBuffer[FileSize]='\0';
				pBuffer[FileSize+1]='\0';

		UniTest = IS_TEXT_UNICODE_SIGNATURE;

		if(IsTextUnicode(pBuffer, FileSize, &UniTest))
			{
				pBuffer = pBuffer+2;
				FileSize -=2;
		    }

				Info.Text = malloc(FileSize+2);

                lstrcpy((PTSTR) Info.Text, (PTSTR) pBuffer);
				SetWindowText(hEdit, Info.Text);
				free(pBuffer);
				free(Info.Text);
				Info.Text = NULL;
				pBuffer = NULL;

				SendMessage(hEdit, EM_SETMODIFY, FALSE, 0);
				return SUCCESS;
			}
		}
	}

	ERRBOX(hwnd, ERR_LOAD);
	return FAILURE;
}

/*------------------------------------------------------------------------
 Procedure:     Save ID:1
 Purpose:       Saves the current file to disk.  If the file does not
				have a name, SaveAs() is called.
 Input:         The handle to the main window.
 Output:        Success or failure.
 Errors:        Validates malloc.
------------------------------------------------------------------------*/
int Save(HWND hwnd){
	UINT Size = 0;
	HANDLE hFile;
	ULONG Write = 0;
	WORD   wByteOrderMark = 0xFEFF ;

	if(lstrcmp(Info.FileName, NEW_FILE_NAME)==0){
		SaveAs(hwnd);
		return SUCCESS;
	}

	Size = GetWindowTextLength(hEdit);

	Info.Text = (PTSTR) malloc((Size + 1) * sizeof (TCHAR));

	if(Info.Text!=NULL){
		GetWindowText(hEdit, Info.Text, Size);
		Info.Text[Size] = '\0';
		hFile = CreateFile(Info.FileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile!=NULL){
			WriteFile (hFile, &wByteOrderMark, 2, &Write, NULL) ;
			WriteFile(hFile, Info.Text, Size * sizeof (TCHAR), &Write,NULL);
			SendMessage(hEdit, EM_SETMODIFY, FALSE, 0);
			free(Info.Text);
			Info.Text=NULL;
			return SUCCESS;
		}
	}
	if(Info.Text!=NULL){
		free(Info.Text);
		Info.Text = NULL;
	}
	ERRBOX(hwnd, ERR_SAVE);
	return FAILURE;
}

/*------------------------------------------------------------------------
 Procedure:     SaveAs ID:1
 Purpose:       Opens a common dialog to save a file under a
				different name.  Then calls Save() to save the file
				to disk.
 Input:         The handle of the main window.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int SaveAs(HWND hwnd){
	OPENFILENAME ofn;
	TCHAR  szFileName[MAX_FILE] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = FILE_FILTER;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_FILE;
	ofn.lpstrTitle = TEXT("Save File as...");
	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = TEXT("txt");

	if(GetSaveFileName(&ofn)){
		lstrcpy(Info.FileName, szFileName);
		Save(hwnd);
		return SUCCESS;
	}

	return FAILURE;
}

/*------------------------------------------------------------------------
 Procedure:     InsertDT ID:1
 Purpose:       Inserts the date and time into the editor window.  If
				Value is true the date and time are inserted.  If
				false the time and date are inserted.
 Input:         Handle to the main window.  Value determining if
				time/date or date/time.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int InsertDT(HWND hwnd, int Value){
	return SUCCESS;
}

/*------------------------------------------------------------------------
 Procedure:     LineCount ID:1
 Purpose:       Counts the number of lines in the file and displays
				the results in a message box.
 Input:         The handle to the main window.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int LineCount(HWND hwnd){
return SUCCESS;
}


/*------------------------------------------------------------------------
 Procedure:     CharChange ID:1
 Purpose:       Changes the current selection to upper or lowercase
				depending on Value.
 Input:         Handle to the main window and Value.  If Value is
				true then upper if value is false then lower.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int CharChange(HWND hwnd, int Value){
return 1;
}


/*------------------------------------------------------------------------
 Procedure:     Print ID:1
 Purpose:       Prints the current file.  This function calls upon
                the windows GDI to display the print dialog and then
                send the data to the appropiate printer.
 Input:         The handle to the main window.
 Output:        Success or failure.
 Errors:        If an error occurs the function exits.
------------------------------------------------------------------------*/
int Print(HWND hwnd){
	static DOCINFO  di = {sizeof (DOCINFO)};
	static PRINTDLG pd ;
	BOOL         	bSuccess ;
	int             yChar, iCharsPerLine, iLinesPerPage, iTotalLines,
					iTotalPages, iPage, iLine, iLineNum, iChar;
	PTSTR           pstrBuffer;
	TCHAR           szJobName [64 + MAX_PATH] ;
	TEXTMETRIC      tm ;
	WORD            iColCopy, iNoiColCopy ;

		// Invoke Print common dialog box

	pd.lStructSize         = sizeof (PRINTDLG);
	pd.hwndOwner           = hwnd;
	pd.hDevMode            = NULL;
	pd.hDevNames           = NULL;
	pd.hDC                 = NULL;
	pd.Flags               = PD_ALLPAGES | PD_COLLATE |
							PD_RETURNDC | PD_NOSELECTION;
	pd.nFromPage           = 0;
	pd.nToPage             = 0;
	pd.nMinPage            = 0;
	pd.nMaxPage            = 0;
	pd.nCopies             = 1;
	pd.hInstance           = NULL;
	pd.lCustData           = 0L;
	pd.lpfnPrintHook       = NULL;
	pd.lpfnSetupHook       = NULL;
	pd.lpPrintTemplateName = NULL;
	pd.lpSetupTemplateName = NULL;
	pd.hPrintTemplate      = NULL;
	pd.hSetupTemplate      = NULL;

	if(!PrintDlg (&pd))
		return TRUE;

	if(0 == (iTotalLines = SendMessage (hEdit, EM_GETLINECOUNT, 0, 0)))
		return TRUE;

		// Calculate necessary metrics for file

	GetTextMetrics(pd.hDC, &tm);
	yChar = tm.tmHeight + tm.tmExternalLeading;

	iCharsPerLine = GetDeviceCaps (pd.hDC, HORZRES) / tm.tmAveCharWidth;
	iLinesPerPage = (GetDeviceCaps (pd.hDC, VERTRES) / yChar) - PRINTER_PAGE_LESS_LINES;
	iTotalPages   = (iTotalLines + iLinesPerPage - 1) / iLinesPerPage;

		// Allocate a buffer for each line of text

	pstrBuffer = (PTSTR) malloc(sizeof (TCHAR) * (iCharsPerLine + 1));

		// Display the printing dialog box

	EnableWindow(hwnd, FALSE);
	bSuccess   = TRUE;

		// Start the document

	GetWindowText(hwnd, szJobName, sizeof (szJobName));
	di.lpszDocName = szJobName;

	if (StartDoc (pd.hDC, &di) > 0){
		for (iColCopy = 0 ;
			iColCopy < ((WORD) pd.Flags & PD_COLLATE ? pd.nCopies : 1) ;
			iColCopy++)
		{
			for (iPage = 0 ; iPage < iTotalPages ; iPage++)
			{
					for (iNoiColCopy = 0 ;
						iNoiColCopy < (pd.Flags & PD_COLLATE ? 1 : pd.nCopies);
						iNoiColCopy++)
					{
							// Start the page

						if (StartPage (pd.hDC) < 0)
						{
							bSuccess = FALSE;
							break;
						}

							// For each page, print the lines

						for (iLine = 0 ; iLine < iLinesPerPage ; iLine++)
						{
							iLineNum = iLinesPerPage * iPage + iLine ;

							if (iLineNum > iTotalLines)
								break ;

							*(int *) pstrBuffer = iCharsPerLine;

							iChar = (int) SendMessage (hEdit, EM_GETLINE,
									(WPARAM) iLineNum, (LPARAM) pstrBuffer);

							if(pstrBuffer[lstrlen(pstrBuffer)-1]<14){
								pstrBuffer[lstrlen(pstrBuffer)-1] = ' ';
							}
							pstrBuffer[lstrlen(pstrBuffer)] = ' ';

							TextOut(pd.hDC, PRINTER_INDENT, yChar * iLine + PRINTER_HEADER, pstrBuffer, iChar);
						}

						if(EndPage (pd.hDC) < 0){
							bSuccess = FALSE;
							break ;
						}
					}

					if(!bSuccess)
						break;
			}

			if(!bSuccess)
					break;
		}
	}
	else
		bSuccess = FALSE;

	if (bSuccess)
		EndDoc(pd.hDC);

	free(pstrBuffer);
	DeleteDC(pd.hDC);

	EnableWindow(hwnd, TRUE);

	return bSuccess;

}

/*------------------------------------------------------------------------
 Procedure:     SelectFontDlg ID:1
 Purpose:       Loads the font common dialog and allows the user to
				choose a font, boldness, italics, and size.
 Input:         The handle to the main window.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int SelectFontDlg(HWND hwnd){
	CHOOSEFONT cf;
	LOGFONT lf;

	GetObject(Font.hFont, sizeof(LOGFONT), &lf);

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.Flags = CF_FORCEFONTEXIST | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
	cf.hwndOwner = hwnd;
	cf.lpLogFont = &lf;

	if(ChooseFont(&cf)){
		HFONT hf = CreateFontIndirect(&lf);
		if(hf){
			DeleteObject(Font.hFont);
			Font.hFont = hf;
			SendMessage(hEdit, WM_SETFONT, (WPARAM)Font.hFont, MAKELPARAM(TRUE, 0));
			lstrcpy(Font.Name,lf.lfFaceName);
			Font.Size = lf.lfHeight;
			Font.Bold = (lf.lfWeight>=700) ? TRUE : FALSE;
			Font.Italic = lf.lfItalic;
			return SUCCESS;
		}
	}
	return FAILURE;
}

/*------------------------------------------------------------------------
 Procedure:     ScrollTop ID:1
 Purpose:       Scrolls to the top or bottom of the edit control
				depending Value.  True is top, false is bottom.
 Input:         The handle to the main wndow and a bool specifying
				top or bottom.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int ScrollTop(HWND hwnd, int Value){
	return SUCCESS;
}

/*------------------------------------------------------------------------
 Procedure:     SelectAll ID:1
 Purpose:       Selects all text in the edit control.
 Input:         The handle to the main window.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int SelectAll(HWND hwnd){
	CHARRANGE cr = {0, -1};
	SendMessage(hEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
	return SUCCESS;
}

/*------------------------------------------------------------------------
 Procedure:     WordWrap ID:1
 Purpose:       Change word wrap to true or false and set the
				appropiate richedit flags.
 Input:         The handle to the main window.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int WordWrap(HWND hwnd){

	ULONG Size = GetWindowTextLength(hEdit);
	int Change = SendMessage(hEdit, EM_GETMODIFY, 0, 0);

	Info.Text = (PTSTR) malloc(Size+1 * sizeof (TCHAR));
	if(Info.Text==NULL){
		ERRBOX(hwnd, ERR_MEM);
		return FAILURE;
	}

	GetWindowText(hEdit, Info.Text, Size);
	Info.Text[Size] = '\0';

	Font.Wrap = Font.Wrap==TRUE ? FALSE : TRUE;
	DestroyWindow(hEdit);

	if(Font.Wrap==TRUE){
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, TEXT(""),
							WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL,
							0, 40, 100, 100, hwnd, NULL, GetModuleHandle(NULL), NULL);
	}
	else{
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, TEXT(""),
							WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL | WS_HSCROLL | ES_AUTOHSCROLL,
							0, 40, 100, 100, hwnd, NULL, GetModuleHandle(NULL), NULL);
	}

	if(hEdit==NULL){
		ERRBOX(hwnd, ERR_CHILD);
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}

	SendMessage(hEdit, WM_SETFONT, (UINT)Font.hFont, MAKELPARAM(TRUE, 0));
	SendMessage(hEdit, EM_EXLIMITTEXT, 0, MAX_EDIT_CHAR);

	SetWindowText(hEdit, Info.Text);
	free(Info.Text);
	Info.Text=NULL;

	SendMessage(hEdit, EM_SETMODIFY, Change, 0);

	return SUCCESS;

}
