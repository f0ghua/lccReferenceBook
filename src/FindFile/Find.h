// find.h

#ifndef _FIND_H_
#define _FIND_H_

#include <windows.h>
#include "jmem.h"

#define FF_READ_DIR	 		0x100
#define FF_READ_FILE	 	0x101
#define ERR_OK		 		0
#define ERR_WRONGDIR 		-1
#define ERR_MEM      		-2
#define ERR_ABORT    		-3
#define ERR_ALREADYWORKING  -4

#define FF_FINISHED 		WM_APP+10

typedef struct tagR{
	int l;
	int t;
	int r;
	int b;
	int f;
}RD;

typedef struct _Ext
{
	FILETIME ftAfterTime;
	int      after[3];
	FILETIME ftBeforeTime;
	int      before[3];
	DWORD    dwMoreThanSize;
	DWORD    dwLessThanSize;
	BOOL     fExtEnabled;
}EXTENDED;

//typedef struct tagFileInfo FILEINFO;

typedef struct tagFileInfo
{	DWORD 	 dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD 	 nFileSizeHigh;
	DWORD 	 nFileSizeLow;
	char 	 * pathname;
	char 	 * filename;
	int        numsearch;	// number of found matches in file
}FILEINFO;

#define MAKELONGLONG(a,b) ((long long) (((long) (a)) | ((long long ) ((long) (b))) << 32))

typedef struct tagFindFileOptions
{	BOOL   subdirs;   		// search subdirectories
	char * initialdir;      // initial directory to search
	char * filter;          // filter - files to be included
	char * txtsearch;       // text to be searched for
	int    iFileCounts;		// just filecounts with text search
	int    iCase;			// case sensitive?
	void  (*CallBackProc)(int msg, FILEINFO * name);
	int    count;			// how many file found?
	int    iSubFolder;		// do you want to search subfolders?
	EXTENDED ext;
	FILEINFO ** ppfi;
}FINDFILE_OPTIONS;

#define MAX_NUMBER 1000				// max number of matches allowed

void AbortFindFile(void);
int  FindFile(FINDFILE_OPTIONS * fileopts);
void FreeFindFile(void);
void FindFileStop(FINDFILE_OPTIONS * fileopts);
int  FindFileLastErr(void);
void ViewSearchResults(FINDFILE_OPTIONS * fileopts, HWND hwndHead, int index);

#endif //_FIND_H_
