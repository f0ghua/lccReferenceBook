// shell.h

#ifndef _SHELL_H_
#define _SHELL_H_

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include "find.h"

int SHDelete(HWND hListView, FINDFILE_OPTIONS * pffo);
int SHCopyTo(char ** buf);
int SHMoveTo(char ** buf);
LPSHELLFOLDER GetSpecialFolder(IShellFolder * pSFDesktop, HWND hwnd, int idFolder);
int SHBrowseDir(char *Title, char *result);
int SHRecycleBin(char ** buf);
int SHProperties(char * filename);
int SHRenameFile(char * fileFrom, char * fileTo);
void StrRetToStr(LPSTRRET pstr, LPITEMIDLIST pidl, char * string);
int DoCopyFiles(HWND h, char * p, char * dir);
int DoShellExecuteEx(HWND hwnd, char * p, char * progname);
HRESULT CreateLink(char * szPathObj, char * szPathLink, char * szDesc);
BOOL fexists(const char * szFile);
#endif // _SHELL_H_
