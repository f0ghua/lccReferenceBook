// find.c

#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "find.h"
#include "dialogs.h"
#include "findfileres.h"

static char filter[MAX_PATH + 100];		//
static FINDFILE_OPTIONS * pfileopts;//
static FILEINFO * pInfoArr;			//
static FILEINFO ** infoBuf;			//
static int error = ERR_OK;			//
static HMEM hMem = NULL;			// memory handler
static unsigned long hThread;		// this threads handle
static BOOL bTerminate = FALSE;		// if thread is terminated
static int memCapacity;				// initial storage size for FILEINFO's

#define WC_PATHNAME   (1 << 0) 		// No wildcard can ever match `/'.
#define WC_ESCAPE     (1 << 1)		// Backslashes don't quote special chars.
#define WC_PERIOD     (1 << 2) 		// Leading `.' is matched only explicitly.

#define WC_FILENAME   WC_PATHNAME   // Preferred GNU name.
#define WC_LEADINGDIR (1 << 3) 		// Ignore `/...' after a match.
#define WC_NOCASE     (1 << 4) 		// Compare without regard to case.

// value returned by `IsFilterMatch' if STRING does not match PATTERN.
#define FNM_MATCH       0			//
#define FNM_NOMATCH     1			//

// this evaluates a char many times
#define FOLD(c) ((flags & WC_NOCASE) ? tolower (c) : (c))

#define MSIZE 0x40000				// buffer size for read file in text search

static char * pBuf = 0;				// main buffer for reading file in text search
static size_t pos[MAX_NUMBER];		// max number of text matches for pos[]

int strSearchi(const char * filename, const char * pattern, unsigned int filesize)
{
	FILE * in;
	int number = 0, thiscount = 0;
	int patsize = strlen(pattern);
	char * pTxtbuf;
	unsigned int totalcount = 0, readsize;
	const char *str1, *str2;
	BOOL eof = FALSE;
	
	if ( !*pattern || !*filename || pattern == NULL || filename == NULL)
		return 0;
	
	in = fopen(filename, "rb");
	if(!in)
	{
		return 0;
	}
	
	readsize = fread(pBuf, 1,  MSIZE, in);
	pTxtbuf = pBuf;
	
	int cnt = 0, offset = 0;
	
	while (totalcount < filesize)
	{
		str1 = pTxtbuf;
		str2 = pattern;
		
		// if the last match is at the end of file and there are no more
		// characters after the match (CR/LF) the loop can go one past the count,
		// so end when the pattern end. [ check && *str2 ].
		//
		// if 'eof' == TRUE no more bytes are to be read but the
		// remaining readsize bytes still need to be processed
		while ( (*str1 & 0xdf) == (*str2 & 0xdf) && *str2 )
		{
			str1++, str2++;	cnt++;
			if(thiscount + cnt == readsize)
			{		// read new block
				if(!eof)
				{
					readsize = fread(pBuf, 1,  MSIZE, in);
					if(readsize != MSIZE)
						eof = TRUE;
					str1 = pTxtbuf = pBuf;			// reset pointers
					thiscount = 0; offset = cnt;	// offset updates totalcount later
				}
			}
		}
		
		if(cnt == patsize)
		{							// we have a match
			pos[number] = totalcount;				// store position
			number++;
			if (pfileopts->iFileCounts == 1)		// end if "file count only"
				goto ende;
			if (number >= MAX_NUMBER)
			{
				goto ende;
			}
			pTxtbuf    += patsize - 1 - offset;		// update pointers
			thiscount  += patsize - 1 - offset;		//
			totalcount += patsize - 1;				// and totalcount
		}
		else if(cnt)
		{
			totalcount += offset;					// only 'offset' for this case
		}											// pTxtbuf & thiscount are incremented next
		// offset is how many cnt was when allocating
		// new block
		pTxtbuf++; totalcount++; thiscount++;
		
		if(thiscount == readsize)
		{
			if(!eof)
			{
				readsize = fread(pBuf, 1,  MSIZE, in);
				if(readsize != MSIZE)
					eof = TRUE;
				str1 = pTxtbuf = pBuf;
				thiscount = 0;
			}
		}
		
		cnt = 0; offset = 0;
	} // while (totalcount < filesize)
	ende:
	fclose(in);
	return number;
}

int strSearch(const char * filename, const char * pattern, unsigned int filesize)
{
	FILE * in;
	int number = 0, thiscount = 0;
	int patsize = strlen(pattern);
	char * pTxtbuf;
	unsigned int totalcount = 0, readsize;
	char *str1, *str2;
	BOOL eof = FALSE;
	
	if ( !*pattern || !*filename || pattern == NULL || filename == NULL)
		return 0;
	
	in = fopen(filename, "rb");
	if(!in)
	{
		return 0;
	}
	
	readsize = fread(pBuf, 1,  MSIZE, in);
	pTxtbuf = pBuf;
	
	int cnt = 0, offset = 0;
	
	while (totalcount < filesize)
	{
		str1 = pTxtbuf;
		str2 = (char*)pattern;
		
		// if the last match is at the end of file and there are no more
		// characters after the match (CR/LF) the loop can go one past the count,
		// so end when the pattern end. [ check && *str2 ].
		//
		// if 'eof' == TRUE no more bytes are to be read but the
		// remaining readsize bytes still need to be processed
		//
		while ( *str1 == *str2 && *str2)
		{
			str1++, str2++;	cnt++;
			if(thiscount + cnt == readsize)
			{		// read new block
				if(!eof)
				{
					readsize = fread(pBuf, 1,  MSIZE, in);
					if(readsize != MSIZE)
						eof = TRUE;
					str1 = pTxtbuf = pBuf;			// reset pointers
					thiscount = 0; offset = cnt;	// offset updates totalcount later
				}
			}
		}
		
		if(cnt == patsize)
		{							// we have a match
			pos[number] = totalcount;				// store position
			number++;
			if (pfileopts->iFileCounts == 1)		// end if "file count only"
				goto ende;
			if (number >= MAX_NUMBER)
			{
				goto ende;
			}
			pTxtbuf    += patsize - 1 - offset;		// update pointers
			thiscount  += patsize - 1 - offset;		//
			totalcount += patsize - 1;				// and totalcount
		}
		else if(cnt)
		{
			totalcount += offset;					// only 'offset' for this case
		}											// pTxtbuf & thiscount are incremented next
		// offset is how many cnt was when allocating
		// new block
		pTxtbuf++; totalcount++; thiscount++;
		
		if(thiscount == readsize)
		{
			if(!eof)
			{
				readsize = fread(pBuf, 1,  MSIZE, in);
				if(readsize != MSIZE)
					eof = TRUE;
				str1 = pTxtbuf = pBuf;
				thiscount = 0;
			}
		}
		
		cnt = 0; offset = 0;
	} // end while (totalcount < filesize)
	
	ende:
	fclose(in);
	return number;
	
}

void ViewSearchResults(FINDFILE_OPTIONS * fileopts, HWND hwndHead, int index)
{
	char * pBuffer = calloc(1, MSIZE + 2);
	if(!pBuffer) 
		return;
	int num;
	FILEINFO * pfi;				// used for the fileopts.fi[]
	pfi = fileopts->ppfi[index];
	size_t size = (size_t)MAKELONGLONG(pfi->nFileSizeLow, pfi->nFileSizeHigh);
	int ii, tmp;
	char pathname[MAX_PATH + 100];
	strcpy(pathname, pfi->pathname);
	strcat(pathname, pfi->filename);
	
	// strSearch returns the number of occurences of the text searched in the file.
	if(fileopts->iCase == 1)
		num = strSearch(pathname, fileopts->txtsearch, size);
	else
		num = strSearchi(pathname, fileopts->txtsearch, size);
	
	FILE * f = fopen(pathname, "rb");
	size_t npos, txtlen, read;
	
	SetWindowText(hwndHead, "");
	
	for(int i = 0; i<num; i++)
	{
		
		if(pos[i] < 20)
			npos = 0;
		else
			npos = pos[i] - 20;
		
		fseek(f, npos, SEEK_SET);
		if(num>1)
		{
			sprintf(pBuffer, "[%d] ", i+1);
			tmp = strlen(pBuffer);
		}
		else
			tmp = 0;
			
		read = fread(pBuffer + tmp, 1,  80 + strlen(fileopts->txtsearch), f);
		for(ii = 0; ii<(read+tmp); ii++)
		{
			if(pBuffer[ii] == 0)
				pBuffer[ii] = 27;
			if(pBuffer[ii] == 10 || pBuffer[ii] == 13) // take out any LFCR's that in the actual text
				pBuffer[ii] = ' ';
		}
		
		pBuffer[ii++] = 13; pBuffer[ii++] = 10; 	// CRLF
		pBuffer[ii++] = 13; pBuffer[ii++] = 10; 	// CRLF
		pBuffer[ii++] = 0;  					// terminate it
		txtlen = (size_t)SendMessage(hwndHead, WM_GETTEXTLENGTH, 0, 0);
		SendMessage(hwndHead, EM_SETSEL, txtlen + 1, txtlen + 1);
		SendMessage(hwndHead, EM_REPLACESEL, TRUE, (LPARAM)pBuffer);
	}
	
	fclose(f);
	free(pBuffer);
}

// ----------------------------------------------------------------------------
static int private_IsFilterMatch( const char *szPattern, const char *szString, int no_leading_period, int flags)
{
	register const char *pPat = szPattern, *pStr = szString;
	register unsigned char c;
	
	while ((c = *pPat++) != '\0')
	{
		c = FOLD (c);
		
		switch (c)
		{
		case '?':
			if (*pStr == '\0')
			{
				return FNM_NOMATCH;
			}
			else if (*pStr == '\\' && (flags & WC_FILENAME))
			{
				return FNM_NOMATCH;
			}
			else if (*pStr == '.' && no_leading_period && (pStr == szString || (pStr[-1] == '\\' && (flags & WC_FILENAME))))
			{
				return FNM_NOMATCH;
			}
			break;
			
		case '\\':
			if (!(flags & WC_ESCAPE))
			{
				c = *pPat++;
				if (c == '\0')
				{
					// Trailing \ loses.
					return FNM_NOMATCH;
				}
				c = FOLD (c);
			}
			if (FOLD ((unsigned char) *pStr) != c)
			{
				return FNM_NOMATCH;
			}
			break;
			
		case '*':
			if (*pStr == '.' && no_leading_period
				&& (pStr == szString || (pStr[-1] == '\\' && (flags & WC_FILENAME))))
			{
				return FNM_NOMATCH;
			}
			
			for (c = *pPat++; c == '?' || c == '*'; c = *pPat++)
			{
				if (*pStr == '\\' && (flags & WC_FILENAME))
				{
					return FNM_NOMATCH;		// A slash does not match a wildcard under WC_FILENAME.
				}
				else if (c == '?')
				{
					if (*pStr == '\0')
					{	// A ? needs to match one character.
						return FNM_NOMATCH; // There isn't another character; no match.
					}
					else
					{
						//  One character of the szString is consumed in matching
						//  this ? wildcard, so *??? won't match if there are
						//  less than three characters.
						++pStr;
					}
				}
			}
			
			if (c == '\0')
			{
				// The wildcard(s) is/are the last element of the szPattern.
				// If the name is a file name and contains another slash
				// this does mean it cannot match.
				return ((flags & WC_FILENAME) && strchr (pStr, '\\') != NULL ? FNM_NOMATCH : 0);
			}
			else
			{
				const char *endp;
				// replace call to private glibc function with equivalent
				if (!(flags & WC_FILENAME) || ((endp = strchr(pStr, '\\')) == NULL))
				{
					endp = pStr + strlen(pStr);
				}
				
				if (c == '[')
				{
					int flags2 = ((flags & WC_FILENAME) ? flags : (flags & ~WC_PERIOD));
					for (--pPat; pStr < endp; ++pStr)
					{
						if ( private_IsFilterMatch(pPat, pStr, (no_leading_period && ( pStr == szString ||
							( pStr[-1] == '\\' && (flags & WC_FILENAME) ) ) ),flags2 )== 0 )
						{
							return 0; // Match
						}
					}
				}
				else if ( c == '\\' && (flags & WC_FILENAME) )
				{
					
					while (*pStr != '\0' && *pStr != '\\')
					{
						++pStr;
					}
					if ( *pStr == '\\' &&
						(private_IsFilterMatch(pPat, pStr + 1, flags & WC_PERIOD, flags) == 0))
					{
						return FNM_MATCH;
					}
				}
				else
				{
					int flags2 = ( (flags & WC_FILENAME) ? flags : (flags & ~WC_PERIOD) );
					
					if ( c == '\\' && !(flags & WC_ESCAPE) )
					{
						c = *pPat;
					}
					c = FOLD (c);
					for (--pPat; pStr < endp; ++pStr)
					{
						if ( FOLD ((unsigned char) *pStr) == c
							&& (private_IsFilterMatch(pPat, pStr, (no_leading_period
										&& (pStr == szString || ( pStr[-1] == '\\'
												&& (flags & WC_FILENAME)))), flags2) == 0))
						{
							return FNM_MATCH;
						}
					}
				}
			}
			
			// If we come here no match is possible with the wildcard.
			return FNM_NOMATCH;
			break;
			
		default:
			if (c != FOLD ((unsigned char) *pStr)){
				return FNM_NOMATCH;
			}
		} // end switch
		++pStr;
	} // end while
	
	if (*pStr == '\0')
	{
		return FNM_MATCH;
	}
	
	if ( ( flags & WC_LEADINGDIR) && *pStr == '\\' )
	{
		// The WC_LEADINGDIR flag says that "foo*" matches "foobar/frobozz".
		return FNM_MATCH;
	}
	
	return FNM_NOMATCH;
	
}

int IsFilterMatch( const char *szPattern,  const char *szString, int flags)
{
	return private_IsFilterMatch(szPattern, szString, flags & WC_PERIOD, flags);
}

void AbortFindFile(void)
{
	bTerminate = TRUE;
	error = ERR_ABORT;
}

void FindFileStop(FINDFILE_OPTIONS * fileopts)
{
	bTerminate = TRUE;
	error = ERR_ABORT;
	pfileopts->CallBackProc(FF_FINISHED, (FILEINFO*)hMem);
}

int FindFileLastErr(void)
{
	return error;
}

static char * CombinePath(char * str, char * str1)
{
	static char p[MAX_PATH + 100];
	strcpy(p, str);
	p[(strstr(p, "\\*") - p)] = '\0';
	strcat(p, "\\");
	strcat(p, str1);
	strcat(p, "\\*");
	return p;
}

void Cleanup(int err)
{
	error = err;
	hThread = 0;
	_endthread();
}

extern HINSTANCE g_hInst;
extern HWND g_hwndMain;
static BOOL FindFile_getFiles(HANDLE * phFindFile, WIN32_FIND_DATA * pfileData, char * pPath)
{
	HANDLE h = NULL;
	DWORD tmp;
	if (*phFindFile == NULL)
	{
		tmp = SetErrorMode(SEM_FAILCRITICALERRORS);
		h = FindFirstFile(pPath, pfileData);
		SetErrorMode(tmp);
		if( h == INVALID_HANDLE_VALUE)
		{
			// i = strlen(pPath);
			//	DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDDIALOG_MESBOX), g_hwndMain,
			//	(DLGPROC)MessageDialogProc, (LPARAM)"Could not open directory - no disk?");
			
			return FALSE;
		}
		else
		{
			*phFindFile = h;
			if (strcmp(pfileData->cFileName, ".") != 0 && strcmp(pfileData->cFileName, "..") != 0)
				return TRUE;
		}
	}
	
	while (*phFindFile != INVALID_HANDLE_VALUE && FindNextFile(*phFindFile, pfileData))
	{
		// if it is not . or .., it's OK
		if (strcmp(pfileData->cFileName, ".") != 0 && strcmp(pfileData->cFileName, "..") != 0)
			return TRUE;
	}
	
	FindClose(*phFindFile);
	*phFindFile = NULL;
	
	return FALSE;
}

BOOL IsExtendedSearch(WIN32_FIND_DATA * pWFD)
{
    BOOL fOK = FALSE;

    DWORD ftLastWriteTime = pWFD->ftLastWriteTime.dwHighDateTime;
    long long llsize = MAKELONGLONG(pWFD->nFileSizeLow, pWFD->nFileSizeHigh);

    // look for both high and low dates being set
    if(pfileopts->ext.ftAfterTime.dwHighDateTime && pfileopts->ext.ftBeforeTime.dwHighDateTime)
    {
        if(ftLastWriteTime >= pfileopts->ext.ftAfterTime.dwHighDateTime && ftLastWriteTime <= pfileopts->ext.ftBeforeTime.dwHighDateTime)
            fOK = TRUE;
        else
            return FALSE;

        goto lsize;
    }

    // just one
    if(pfileopts->ext.ftAfterTime.dwHighDateTime)
    {
        if(ftLastWriteTime >= pfileopts->ext.ftAfterTime.dwHighDateTime)
            fOK = TRUE;
        else
            return FALSE;
    }

    // or the other
    if(pfileopts->ext.ftBeforeTime.dwHighDateTime)
    {
        if(ftLastWriteTime <= pfileopts->ext.ftBeforeTime.dwHighDateTime)
            fOK = TRUE;
        else
            return FALSE;
    }

lsize:

    // both sizes are active
    if(pfileopts->ext.dwMoreThanSize > 0 && pfileopts->ext.dwLessThanSize > 0)
    {
        if(llsize >= pfileopts->ext.dwMoreThanSize && llsize <= pfileopts->ext.dwLessThanSize)
            fOK = TRUE;
        else
            return FALSE;

        goto exit;
    }

    // only more than is active
    if(pfileopts->ext.dwMoreThanSize > 0)
    {
        if(llsize >= pfileopts->ext.dwMoreThanSize)
            fOK = TRUE;
        else
            return FALSE;
    }

    // only less than is active
    if(pfileopts->ext.dwLessThanSize > 0)
    {
        if(llsize <= pfileopts->ext.dwLessThanSize)
            fOK = TRUE;
        else
            return FALSE;
    }

exit:
    return fOK;
}

char filename[MAX_PATH + 100];
void FindFile_scanPath(char * path)
{
	size_t result = 0;
	FILEINFO * info = NULL;
	FILEINFO ** tmpinfo = NULL;
	HANDLE hFindFile = NULL;
	WIN32_FIND_DATA WFD;
	long int p;
	
	char pPath[MAX_PATH + 100]; // for the recusion, pPath carries THIS stack's string
	strcpy(pPath, path);
	
	if(pfileopts->CallBackProc)
	{
		// take off the "\*" filter
		p = strstr(pPath, "\\*")-pPath;
		pPath[p] = '\0';
		pfileopts->CallBackProc(FF_READ_DIR, (FILEINFO*)pPath);
		// put it back
		pPath[p] = '\\';
		pPath[p+1] = '*';
	}
	
	while (FindFile_getFiles(&hFindFile, &WFD, pPath))
	{
		// To include folders in the search, take out this IF
		//
		//if(!(WFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		//{
			
			// Abort on termination signal
			if (bTerminate){
				hThread = 0;
				_endthread();
			}
		
			if(FNM_NOMATCH == IsFilterMatch(filter, WFD.cFileName, WC_NOCASE | WC_ESCAPE))
				goto dir;//continue;
			
			if(pfileopts->ext.fExtEnabled)
			{
				if(!IsExtendedSearch(&WFD))
					goto dir;//continue;
			}
			
			if(pfileopts->txtsearch[0])
			{
				result = (size_t)MAKELONGLONG(WFD.nFileSizeLow, WFD.nFileSizeHigh);
				strcpy(filename, pPath);
				strcpy(filename+strlen(pPath)-1, WFD.cFileName);
				if(pfileopts->iCase == 1)
					result = strSearch(filename, pfileopts->txtsearch, result);
				else
					result = strSearchi(filename, pfileopts->txtsearch, result);
				
				if(!result)
					goto dir;//continue;
			}
			
			pInfoArr = pfileopts->ppfi[pfileopts->count];
			
			memcpy(pInfoArr, &WFD, 36); // partial copy of WIN32_FIND_DATA to FILEINFO
			
			pInfoArr->pathname = jmalloc( hMem, strlen(pPath) + 1);
			pInfoArr->filename = jmalloc( hMem, strlen(WFD.cFileName) + 1);
			pInfoArr->numsearch = result;
			if(pInfoArr->pathname == NULL)
			{
				Cleanup(ERR_MEM);
			}
			
			// take off filter
			p = strstr(pPath, "\\*")-pPath;
			strcpy(pInfoArr->pathname, pPath);
			pInfoArr->pathname[p+1] = '\0';
			strcpy(pInfoArr->filename, WFD.cFileName);
			if(pfileopts->CallBackProc){
				pfileopts->CallBackProc(FF_READ_FILE, pInfoArr);
			}
			
			pfileopts->count++;
			// allocate another 80000 bytes for another 20000 files
			if(pfileopts->count*4 >= memCapacity-8)
			{
				memCapacity += 80000;
				tmpinfo = realloc( infoBuf, memCapacity );
				if(tmpinfo == NULL)
				{
					Cleanup(ERR_MEM);
				}
				pfileopts->ppfi = (FILEINFO**)tmpinfo;
				infoBuf = tmpinfo;
			}
			
			// allocate memory for the next FILEINFO
			info = jmalloc( hMem, sizeof(FILEINFO) );
			if(info == NULL)
			{
				Cleanup(ERR_MEM);
			}
			
			pfileopts->ppfi[pfileopts->count] = info;
			
		//}
dir:
		// Skip this file/directory if not matching criteria
		if (!pfileopts->iSubFolder)
			continue;

		// If recursive option is set and this is a directory, then search in there too
		if (pfileopts->subdirs && WFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			FindFile_scanPath(CombinePath(pPath, WFD.cFileName));
		}
	}
	
}

void __cdecl Private_FindFile(void * fileopts)
{
	pfileopts = (FINDFILE_OPTIONS *)fileopts;
	char path[MAX_PATH + 100];
	
	// check for '\' at end of initialdir
	if( strlen(pfileopts->initialdir)-2 != (strrchr(pfileopts->initialdir, '\\')-pfileopts->initialdir ))
	{
		error = ERR_WRONGDIR;
		hThread = 0;
		_endthread();
		return;
	}
	
	// make a local copy of filter as used throughout this translation unit
	// clear filter of previous search
	memset(filter, 0, MAX_PATH + 100);
	strcpy(filter, pfileopts->filter);
	
	// if filter == *.* or *. or zero
	if(filter[0] == 0 || !strcmp(filter, "*.*") || !strcmp(filter, "*."))
		strcpy(filter, "*");
	
	// if filter has '.' at the end, change to '*'
	int len = strlen(filter)-1;
	if(filter[len] == '.')
	{
		strcpy(&filter[len+1], "*");
	}
	
	extern int g_iAutoWildcards;
	if (g_iAutoWildcards) 
	{
		// if filter does not have any wildcards at beginning add one
		if(filter[0] != '*')
		{
			for(int i = strlen(filter)+1; i>=0; i--)
			{
				filter[i] = filter[i-1];
			}
			filter[0] = '*';
		}
		
		// if no wild cards at end
		if(filter[strlen(filter)-1] != '*')
		{
			filter[strlen(filter)] = '*';
		}
	}
	
	memCapacity = 80000;
	
	// 600 1meg blocks should be enough
	hMem = InitJMem( 1000000, 600 );
	if(hMem == NULL)
	{
		Cleanup(ERR_MEM);
		return;
	}
	
	// allocate memory for the array of pointers to FILEINFO's
	// enough for 200000 files.
	infoBuf = malloc( memCapacity );
	if(infoBuf == NULL)
	{
		Cleanup(ERR_MEM);
		return;
	}
	
	// use malloc here for infoBuf as it's easier to re-allocate later
	// all other memory uses jmalloc()
	pfileopts->ppfi = infoBuf;
	
	// allocate memory for the first FILEINFO
	pInfoArr = jmalloc( hMem, sizeof(FILEINFO) );
	if(pInfoArr == NULL)
	{
		Cleanup(ERR_MEM);
		return;
	}
	pfileopts->ppfi[0] = pInfoArr;
	
	strcpy(path, pfileopts->initialdir);
	char * pSep = strchr(pfileopts->initialdir, ';');
	char * pStr = pfileopts->initialdir;
	
	pBuf = calloc(1, MSIZE + 1);
	while(pSep!=NULL)
	{
		memset(path, 0, MAX_PATH + 100);
		lstrcpyn(path, pStr, (pSep - pStr)+1);
		path[strlen(path)] = '*';
		FindFile_scanPath(path);
		pSep += 1;
		pSep = strchr(pSep, ';' );
		pStr += 4;
	}
	pfileopts->CallBackProc(FF_FINISHED, (FILEINFO*)hMem);
	bTerminate = TRUE;
	hThread = 0;
}

void FreeFindFile(void)
{
	if(hMem)
		jfree(hMem); 

	hMem = NULL;

	if(infoBuf)
		free(infoBuf); 
		
	infoBuf = NULL;

	if(pBuf)
	{
		free(pBuf);
		pBuf = 0;
	}
}

int FindFile(FINDFILE_OPTIONS * fileopts)
{
	if(	hThread == 0)
	{
		error      = ERR_OK;
		bTerminate = FALSE;
		hThread    = _beginthread(Private_FindFile, 0, fileopts);
	}
	else
	{
		error = ERR_ALREADYWORKING;
	}
	return error;
}


