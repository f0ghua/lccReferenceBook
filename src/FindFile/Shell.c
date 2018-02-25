// shell.c

#include <stdio.h>
#include "shell.h"
#include "dialogs.h"
#include "findfileres.h"

LPMALLOC    pMalloc;                // The OLE task allocator
LPSHELLFOLDER psfDesktop;           // The desktop folder

static SHFILEOPSTRUCT shf;

BOOL fexists(const char * szFile)
{
	FILE * file;
	file = fopen(szFile, "r");
	if (!file)
		return FALSE;
	else
	{
		fclose(file);
		return TRUE;
	}
}

char * extract_fname(char * full)
{
	char * p;
	if((p = strrchr(full, '\\')) != NULL)
		return p+1;
	else
	    return 0;
}

void extract_path(char * full, char * path)
{
	char * p;
	if((p = strrchr(full, '\\'))!= NULL)
	{
		strncpy(path, full, p - full + 1);
		path[p - full + 1] = 0;
	}else
		*path = 0;
}

int DoShellExecuteEx(HWND hwnd, char * p, char * progname)
{
	char full[MAX_PATH+MAX_PATH];
	while (*p != 0)
	{
		// add quotes around the path + filename [p]
		sprintf(full, "%s %c%s%c", progname, '"', p, '"');
		WinExec(full, SW_SHOW);
		p += strlen(p);
		p++;
	}
	return 0;
}

int SHDelete(HWND hListView, FINDFILE_OPTIONS * pfileopts)
{
	char s[MAX_PATH+100];
	int index = -1;
	int result, ret = 1;

	while ((-1) != (index = ListView_GetNextItem(hListView, index, LVNI_SELECTED)))
	{
		strcpy(s, pfileopts->ppfi[index]->pathname);
		strcat(s, pfileopts->ppfi[index]->filename);
		ret = DeleteFile(s);
		if(ret)
		{
			for(int i = index; i<pfileopts->count; i++)
				pfileopts->ppfi[i] = pfileopts->ppfi[i+1];

			pfileopts->count--;
			ListView_DeleteItem(hListView, index);
			index = -1;
		}
		else
		{
			result = ret;
		}
	}
	sprintf(s, "%u File(s)", pfileopts->count);
	SendDlgItemMessage(GetParent(hListView), IDC_STATUSBAR, SB_SETTEXT, 0, (LPARAM)s);
	return result;
}

int SHMoveTo(char ** buf)
{
	memset(&shf, 0, sizeof(SHFILEOPSTRUCT));
	shf.hwnd  = NULL;
	shf.wFunc = FO_MOVE;
	shf.pFrom = buf[0];
	shf.pTo   = buf[1];
	shf.fFlags = FOF_FILESONLY;
	return SHFileOperation(&shf);
}

int SHCopyTo(char ** buf)
{
	memset(&shf, 0, sizeof(SHFILEOPSTRUCT));
	shf.hwnd  = NULL;
	shf.wFunc = FO_COPY;
	shf.pFrom = buf[0];
	shf.pTo   = buf[1];
	shf.fFlags = FOF_FILESONLY;
	return SHFileOperation(&shf);
}

int SHRenameFile(char * fileFrom, char * fileTo)
{
	return rename(fileFrom, fileTo);
}

int SHRecycleBin(char ** buf)
{
	memset(&shf, 0, sizeof(SHFILEOPSTRUCT));
	shf.hwnd  = NULL;
	shf.wFunc = FO_DELETE;
	shf.pFrom = buf[0];
	shf.fFlags = FOF_ALLOWUNDO | FOF_FILESONLY | FOF_NOCONFIRMATION;
	return SHFileOperation(&shf);
}

int SHProperties(char * filename)
{
	SHELLEXECUTEINFO shi;
	memset(&shi, 0, sizeof(SHELLEXECUTEINFO));
	shi.cbSize 		 = sizeof(SHELLEXECUTEINFO);
	shi.fMask 		 = SEE_MASK_INVOKEIDLIST ;
	shi.hwnd 		 = NULL;
	shi.lpVerb 		 = "properties";
	shi.lpFile 		 = filename;
	shi.lpParameters = "";
	shi.lpDirectory  = NULL;
	shi.nShow 		 = SW_SHOW;
	shi.hInstApp 	 = NULL;
	return ShellExecuteEx(&shi);
}

LRESULT CALLBACK BrowseForFolderDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		char szPath[30];
		strcpy(szPath, "c:\\");
		SendMessage(hDlg, BFFM_SETSELECTION, 1, (LPARAM)szPath);
	}

	return (FALSE);
}

int SHBrowseDir(char *Title, char *result)
{
	LPMALLOC pMalloc;
	BROWSEINFO browseInfo;
	LPITEMIDLIST ItemIDList;
	int r = 0;

	if (S_OK != SHGetMalloc(&pMalloc))
		return 0;

	memset(&browseInfo,0,sizeof(BROWSEINFO));
	browseInfo.hwndOwner 		= GetActiveWindow();
	browseInfo.pszDisplayName 	= result;
	browseInfo.lpszTitle 		= Title;
	browseInfo.ulFlags 			= BIF_NEWDIALOGSTYLE | BIF_UAHINT;
	browseInfo.ulFlags 			= BIF_NEWDIALOGSTYLE | BIF_UAHINT;
	browseInfo.lpfn             = (BFFCALLBACK)BrowseForFolderDlgProc;
	browseInfo.lParam           = 0;

	ItemIDList = SHBrowseForFolder(&browseInfo);

	if (ItemIDList != NULL)
	{
		*result = 0;
		if (SHGetPathFromIDList(ItemIDList, result))
		{
			if (result[0])
				r = 1;
			pMalloc->lpVtbl->Free(pMalloc,ItemIDList);
		}
	}
	pMalloc->lpVtbl->Release(pMalloc);
	return r;
}

void ShellFree(LPVOID pv)
{
	pMalloc->lpVtbl->Free(pMalloc, pv);
}

LPITEMIDLIST PidlFromPath(IShellFolder * pShellFolder, HWND hwnd, LPCSTR pszPath)
{
	LPITEMIDLIST pidl;
	ULONG ulEaten;
	DWORD dwAttributes;
	HRESULT hres;
	WCHAR wszName[MAX_PATH+100];

	if (!MultiByteToWideChar(CP_ACP, 0, pszPath, -1, wszName, MAX_PATH+100))
		return NULL;

	hres = pShellFolder->lpVtbl->ParseDisplayName(pShellFolder, hwnd,
							NULL, wszName, &ulEaten, &pidl, &dwAttributes);
	if (FAILED(hres))
		return NULL;

	return pidl;
}

void StrRetToStr(LPSTRRET pstr, LPITEMIDLIST pidl, char * string)
{
	switch (pstr->uType)
	{

	case STRRET_WSTR:
		// The pOleStr points to a WCHAR string that we convert
		// to ANSI.  We need to stash pstr->pOleStr into a safe
		// place, because pstr->cStr lives in the same place.
		WideCharToMultiByte(CP_ACP, 0, pstr->pOleStr, -1, string, MAX_PATH+100, NULL, NULL);
		break;

	case STRRET_OFFSET:
		// The string lives inside the pidl, so copy it out.
		lstrcpyn(string, (LPSTR)((LPBYTE)pidl + pstr->uOffset), MAX_PATH+100);
		break;

	case STRRET_CSTR:
		// The string already is in the right place.
		break;
	}
}

LPSHELLFOLDER GetSpecialFolder(IShellFolder * pSFDesktop, HWND hwnd, int idFolder)
{
	HRESULT hres;
	LPSHELLFOLDER psf;
	LPITEMIDLIST pidl;

	hres = SHGetSpecialFolderLocation(hwnd, idFolder, &pidl);
	if (SUCCEEDED(hres))
	{
		hres = pSFDesktop->lpVtbl->BindToObject(pSFDesktop, pidl, NULL,	&IID_IShellFolder, (LPVOID *)&psf);
		if (SUCCEEDED(hres))
		{
			// done
		}
		else
		{
			psf = NULL;
		}
		ShellFree(pidl);
	}
	else
	{
		psf = NULL;
	}
	return psf;
}

/*
	Resolving a Shortcut
	An application may need to access and manipulate a shortcut that was previously
	created. This operation is referred to as resolving the shortcut.

	The application-defined ResolveIt function in the following example resolves
	a shortcut. Its parameters include a window handle, a pointer to the path of
	the shortcut, and the address of a buffer that receives the new path to the
	object. The window handle identifies the parent window for any message boxes
	that the Shell may need to display. For example, the Shell can display a
	message box if the link is on unshared media, if network problems occur, if
	the user needs to insert a floppy disk, and so on.

	The ResolveIt function calls the CoCreateInstance function and assumes that
	the CoInitialize function has already been called. Note that ResolveIt needs
	to use the IPersistFile interface to store the link information. IPersistFile
	is implemented by the IShellLink object. The link information must be loaded
	before the path information is retrieved, which is shown later in the example.
	Failing to load the link information causes the calls to the IShellLink::GetPath
	and IShellLink::GetDescription member functions to fail.
*/

HRESULT ResolveLnk(HWND hwnd, LPCSTR lpszLinkFile, LPSTR lpszPath)
{
    HRESULT hres;
    IShellLink* psl;
    char szGotPath[MAX_PATH+100];
    char szDescription[MAX_PATH+100];
    WIN32_FIND_DATA wfd;

    *lpszPath = 0; // assume failure

    // Get a pointer to the IShellLink interface.
    hres = CoCreateInstance((REFCLSID)&CLSID_ShellLink, NULL,
        CLSCTX_INPROC_SERVER, (REFIID)&IID_IShellLink, (LPVOID *) &psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;

        // Get a pointer to the IPersistFile interface.
        hres = psl->lpVtbl->QueryInterface(psl, (REFIID)&IID_IPersistFile, (void**)&ppf);
        if (SUCCEEDED(hres))
        {
            WCHAR wsz[MAX_PATH+100];

            // Ensure that the string is Unicode.
            MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH+100);

            // Load the shortcut.
            hres = ppf->lpVtbl->Load(ppf, wsz, STGM_READ);
            if (SUCCEEDED(hres))
            {
                // Resolve the link.
                hres = psl->lpVtbl->Resolve(psl, hwnd, SLR_NO_UI);
                if (0 == hres)
                {
                    // Get the path to the link target.
                    hres = psl->lpVtbl->GetPath(psl, szGotPath,
                        MAX_PATH+100, (WIN32_FIND_DATA *)&wfd, SLGP_SHORTPATH );
                    if (FAILED(hres))
                        return (hres); // application-defined function

                    // Get the description of the target.
                    hres = psl->lpVtbl->GetDescription(psl, szDescription, MAX_PATH+100);
                    if (FAILED(hres))
                        return (hres);
                    lstrcpy(lpszPath, szGotPath);
                }
                else
                {
					return hres;
				}
            }
        // Release the pointer to the IPersistFile interface.
        ppf->lpVtbl->Release(ppf);
        }
    // Release the pointer to the IShellLink interface.
    psl->lpVtbl->Release(psl);
    }
    return hres;
}


// CreateLink - uses the Shell's IShellLink and IPersistFile interfaces
//			    to create and store a shortcut to the specified object.
//
// Returns the result of calling the member functions of the interfaces.
//
// lpszPathObj - address of a buffer containing the path of the object.
//
// lpszPathLink - address of a buffer containing the path where the
//   			  Shell link is to be stored.
//
// lpszDesc - address of a buffer containing the description of the Shell link.

HRESULT CreateShortCutLink(char * szPathObj, char * szPathLink, char * szDesc)
{
	HRESULT hres;
	IShellLink* psl;

	// Get a pointer to the IShellLink interface.
	hres = CoCreateInstance((CLSID*)&CLSID_ShellLink, NULL,
		CLSCTX_INPROC_SERVER, (REFIID)&IID_IShellLink, (LPVOID *) &psl);

	if (SUCCEEDED(hres))
	{
		IPersistFile * ppf;

		// Set the path to the shortcut target and add the
		// description.
		psl->lpVtbl->SetPath(psl, szPathObj);
		psl->lpVtbl->SetDescription(psl, szDesc);

		// Query IShellLink for the IPersistFile interface for saving the
		// shortcut in persistent storage.
		hres = psl->lpVtbl->QueryInterface(psl, (REFIID)&IID_IPersistFile, (LPVOID*)&ppf);

		if (SUCCEEDED(hres))
		{
			WCHAR wsz[MAX_PATH+100];

			// Ensure that the string is Unicode.
			MultiByteToWideChar(CP_ACP, 0, szPathLink, -1, wsz, MAX_PATH+100);

			// Save the link by calling IPersistFile::Save.
			hres = ppf->lpVtbl->Save(ppf, wsz, TRUE);
			ppf->lpVtbl->Release(ppf);
		}
		psl->lpVtbl->Release(psl);
	}
	return hres;
}


