// Utils.c
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include "findfileres.h"
#include "find.h"
#include "utils.h"
#include "dialogs.h"
#include "findfile.h"

// For Menu, normal and contextmenu
static char * strMenu[] = {"Explore Folder", "View Dependencies", "Open", "Rename", "CopyTo", "MoveTo", "Delete", "Recycle Bin", "Properties", "Exit", "SendTo Menu", "Extended Search", "About FindFile" };
static HBITMAP hMenuBmps[MAX_BITMAPS];
static int menuHeight;

// for sorting -
static int direction[4] = {0,0,0,0};
// if syscolor change need to know which column has a bitmap
static int lastdirection = -1;
static int lastwhich = -1;

#define NUMCOLS 4
static char * g_strCol[NUMCOLS] = {"Name", "Folder", "Size", "Modified"};

BOOL IsModuleFile(char * pathname)
{
	#define NUMEXTENS  7
	char *strExeExt[NUMEXTENS] = {"EXE", "DLL", "SYS", "DRV", "CPL", "SCR", "COM"};
	char ext[90];
	ExtractExt(pathname, ext);
	strupr(ext);
	if(ext[0])
	{
		for(int i = 0; i<NUMEXTENS; i++)
		{
			if(!strcmp(strExeExt[i], ext))
				return TRUE;
		}
	}
	return FALSE;
}

BOOL IsImageFile(char * pathname)
{
	#define NUMPICEXT  7
	char *strPicExt[NUMPICEXT]   = {"bmp", "dib", "gif", "jpg", "ico", "emf", "wmf"};
	char ext[90];
	ExtractExt(pathname, ext);
	if(!ext[0])
		return FALSE;
	strlwr(ext);
	for(int i = 0; i<NUMPICEXT; i++)
	{
		if(!strcmp(ext, strPicExt[i]))
			return TRUE;
	}
	return FALSE;
}

void ResetColumnBitmaps(void)
{
	lastwhich     = -1;
	lastdirection = -1;
}

void SetColumnBitmap(HWND hListview)
{
	// do the listview header images and text
	HWND hHeader = (HWND)SndMsg(hListview, LVM_GETHEADER, 0, 0);
	HDITEM hdi;
	memset(&hdi, 0, sizeof(HDITEM));
	
	// first clear all bitmaps from the header
	hdi.mask 	= HDI_FORMAT | HDI_TEXT;
	for(int i = 0; i<NUMCOLS; i++)
	{
		hdi.fmt  	= HDF_STRING;
		hdi.cchTextMax = strlen(g_strCol[i]);
		hdi.pszText    = g_strCol[i];
		SendMessage(hHeader, HDM_SETITEM, i, (long)&hdi);
	}
	
	if(lastwhich != -1 && lastdirection != -1)
	{
		if(lastdirection == 0)
			hdi.hbm  	= hMenuBmps[ID_DOWN];
		else
			hdi.hbm  	= hMenuBmps[ID_UP];
		
		hdi.mask 	= HDI_FORMAT | HDI_BITMAP | HDI_TEXT;
		hdi.fmt  	= HDF_BITMAP_ON_RIGHT | HDF_BITMAP | HDF_STRING;
		hdi.cchTextMax = strlen(g_strCol[lastwhich]);
		hdi.pszText    = g_strCol[lastwhich];
		SendMessage(hHeader, HDM_SETITEM, lastwhich, (long)&hdi);
	}
}

BOOL CreateSBar(HWND hwndParent, char *initialText, int nrOfParts)
{
	int  ptArray[4];  // Array defining the number of parts/sections
	RECT rc;
	HWND hStatusbar = CreateStatusWindow(WS_CHILD | WS_VISIBLE |
		WS_BORDER|SBARS_SIZEGRIP, initialText, hwndParent, IDC_STATUSBAR);
	if(hStatusbar)
	{
		GetClientRect(hwndParent, &rc);
		ptArray[nrOfParts-1] = rc.right;
		SndMsg(hStatusbar, SB_SETPARTS, nrOfParts, (LPARAM)(LPINT)ptArray);
		return TRUE;
	}
	return FALSE;
}

int __cdecl CompareFolders(const void * param1, const void * param2)
{
	FILEINFO * pfi1 = *(FILEINFO ** )param1;
	FILEINFO * pfi2 = *(FILEINFO ** )param2;
	if(!lastdirection)
		return stricmp(pfi2->pathname, pfi1->pathname);
	else
		return stricmp(pfi1->pathname, pfi2->pathname);
}

int __cdecl CompareNames(const void * param1, const void * param2)
{
	FILEINFO * pfi1 = *(FILEINFO ** )param1;
	FILEINFO * pfi2 = *(FILEINFO ** )param2;
	if(!lastdirection)
		return stricmp(pfi2->filename, pfi1->filename);
	else
		return stricmp(pfi1->filename, pfi2->filename);
}

int __cdecl CompareSizes(const void * param1, const void * param2)
{
	FILEINFO * pfi1 = *(FILEINFO ** )param1;
	FILEINFO * pfi2 = *(FILEINFO ** )param2;
	long long llsize1;
	long long llsize2;
	if(!lastdirection)
	{
		llsize1 = MAKELONGLONG(pfi1->nFileSizeLow, pfi1->nFileSizeHigh);
		llsize2 = MAKELONGLONG(pfi2->nFileSizeLow, pfi2->nFileSizeHigh);
	}
	else
	{
		llsize2 = MAKELONGLONG(pfi1->nFileSizeLow, pfi1->nFileSizeHigh);
		llsize1 = MAKELONGLONG(pfi2->nFileSizeLow, pfi2->nFileSizeHigh);
	}
	
	if(llsize1 < llsize2)
		return 1;
	else if(llsize1 > llsize2)
		return -1;
	
	return 0;
}

int __cdecl CompareDates(const void * param1, const void * param2)
{
	FILEINFO * pfi1 = *(FILEINFO ** )param1;
	FILEINFO * pfi2 = *(FILEINFO ** )param2;
	FILETIME * ft1;
	FILETIME * ft2;
	if(!lastdirection)
	{
		ft1 = &pfi1->ftLastWriteTime;
		ft2 = &pfi2->ftLastWriteTime;
	}
	else
	{
		ft2 = &pfi1->ftLastWriteTime;
		ft1 = &pfi2->ftLastWriteTime;
	}
	if(ft1->dwHighDateTime < ft2->dwHighDateTime)
		return 1;
	else if(ft1->dwHighDateTime > ft2->dwHighDateTime)
		return -1;
	
	return 0;
}

void SortItems(HWND hListview, FINDFILE_OPTIONS * pfileopts, int type)
{
	//	sort folders also 04 12 25 zo
	if (lastdirection == -1 ) lastdirection = 1;
	if (type == lastwhich ) 
		direction[type] = lastdirection ? 0 : 1;
	else 
		direction[type] = lastdirection ;

	FILEINFO ** ppfi = pfileopts->ppfi;
	
	HCURSOR hCur = SetCursor(LoadCursor(NULL, IDC_WAIT));
	SetCursor(hCur);
	
	lastdirection = direction[type];
	lastwhich     = type;
	
	switch(type) {
	case 0: qsort(ppfi, pfileopts->count, sizeof(FILEINFO*), CompareNames); break;
	case 1: qsort(ppfi, pfileopts->count, sizeof(FILEINFO*), CompareFolders); break;
	case 2: qsort(ppfi, pfileopts->count, sizeof(FILEINFO*), CompareSizes); break;
	case 3: qsort(ppfi, pfileopts->count, sizeof(FILEINFO*), CompareDates);
	}
	SetColumnBitmap(hListview);
	return;
}

void ExtractExt(char * path, char * ext)
{
	char * p;
	if((p = strrchr(path, '.')) != NULL)
		strcpy(ext, p+1);
	else
		*ext = 0;
}

void ListView_SetColHeadTxtAll( HWND hwndLbx, int nCols)
{
	LV_COLUMN lvc;
	memset( &lvc, 0, sizeof(LV_COLUMN) );
	for (int i = 0; i<nCols; i++)
	{
		lvc.iSubItem   = i;
		lvc.mask    = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
		lvc.pszText = g_strCol[i];
		lvc.fmt     = LVCFMT_LEFT;
		ListView_SetColumn(	hwndLbx, i, &lvc );
	}
}

void ListView_SetColumnWidths(HWND hwndLbx, int scale, int numcols, int * width)
{
	LV_COLUMN lvc;
	memset( &lvc, 0, sizeof(LV_COLUMN) );
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	for(int i = 0; i<numcols; i++)
	{
		lvc.iSubItem   = i;
		lvc.cx         = (*width++ * scale)/4;
		ListView_InsertColumn( hwndLbx, i, &lvc );
	}
}

int GetFontHeight(HWND hwnd, HFONT hFont)
{
	HANDLE hObj;
	TEXTMETRIC tm;
	HDC hdc = GetDC(hwnd);
	hObj = SelectObject(hdc, hFont);
	GetTextMetrics(hdc,	&tm); 	// address of text metrics structure
	SelectObject(hdc, hObj);
	ReleaseDC(hwnd, hdc);
	return tm.tmHeight;
}

void RemoveFileCountBraces(char * s)
{
	char * p = strchr(s, ']');
	if(p)
		p += 2;
	else
		return;
	int len  = strlen(s);
	memmove(s, s + (p-s), len - (p-s)+1);
}

void DestroyBitmaps(void)
{
	for(int i = 0 ; i<MAX_BITMAPS; i++)
	{
		if(hMenuBmps[i]) 
			DeleteObject(hMenuBmps[i]);
	}
}

HBITMAP LoadBitmapMappedRes(HINSTANCE hinst, int ord)
{
	HBITMAP hBitmap;
	BITMAP bm;
	
	hBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(ord));
	
	// Create memDC and Select the bitmap
	HDC 	hdcMem    = CreateCompatibleDC(GetDC(NULL));
	HGDIOBJ hOldObj   = SelectObject(hdcMem, hBitmap);
	int x, y;
	
	COLORREF ColFrom = RGB(192, 192, 192);
	COLORREF ColTo   = GetSysColor(COLOR_MENU);
	
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	
	// Change the background colour to ColTo
	for(x = 0; x<bm.bmWidth; x++)
	{
		for(y = 0; y<bm.bmHeight; y++)
		{
			if(GetPixel(hdcMem, x, y) == ColFrom)
				SetPixelV(hdcMem, x, y, ColTo);
		}
	}
	
	// Select old object back and delete memDC.
	SelectObject(hdcMem, hOldObj);
	DeleteDC(hdcMem);
	
	return hBitmap;
}

void LoadBitmaps(HINSTANCE hInst)
{
	for(int i = ID_FOLDER; i<MAX_BITMAPS; i++)
		hMenuBmps[ID_FOLDER + i] = LoadBitmapMappedRes(hInst, i+200);
}

int GetMenuItemState(HMENU hMenu, int id)
{
	MENUITEMINFO mii;
	GetMenuItemInfo(hMenu, id, FALSE, &mii);
	return mii.dwItemData;
}

int InitMenu(HWND hwnd, HINSTANCE hInst, char * szDepends)
{
	MENUITEMINFO mii;
	mii.cbSize   = sizeof(MENUITEMINFO);
	mii.fMask    = MIIM_FTYPE | MIIM_DATA;
	mii.fType    = MFT_OWNERDRAW;
	mii.dwItemData = ENABLED;
	
	HMENU hMenu = GetMenu(hwnd);
	SetMenuItemInfo(hMenu, IDM_FOLDER, FALSE, &mii);
	SetMenuItemInfo(hMenu, IDM_OPEN, FALSE, &mii);
	SetMenuItemInfo(hMenu, IDM_RENAME, FALSE, &mii);
	SetMenuItemInfo(hMenu, IDM_COPYTO, FALSE, &mii);
	SetMenuItemInfo(hMenu, IDM_MOVETO, FALSE, &mii);
	SetMenuItemInfo(hMenu, IDM_DELETE, FALSE, &mii);
	SetMenuItemInfo(hMenu, IDM_RECYCLE, FALSE, &mii);
	SetMenuItemInfo(hMenu, IDM_PROPERTIES, FALSE, &mii);
	SetMenuItemInfo(hMenu, IDM_EXIT, FALSE, &mii);
	SetMenuItemInfo(hMenu, IDM_EDITSENDTO, FALSE, &mii);
	SetMenuItemInfo(hMenu, IDM_EXTENDEDSEARCH, FALSE, &mii);
	SetMenuItemInfo(hMenu, IDM_ABOUT, FALSE, &mii);
	
	return 1;
}

int DrawBitMap(HBITMAP hBmp, HDC hDc, RECT * rc)
{
	HANDLE hdcMem, hOldObject;
	BITMAP bmp;
	
	//================================================================
	// Create DC and select bmp into it.
	//================================================================
	hdcMem     = CreateCompatibleDC(hDc);
	hOldObject = SelectObject(hdcMem, hBmp);
	GetObject(hBmp, sizeof(BITMAP), &bmp);
	//================================================================
	// Blit it starting from nOffx/nOffy.
	//================================================================
	if(menuHeight < 24)
		BitBlt(hDc, rc->left+2, rc->top+3, rc->right, rc->bottom, hdcMem, 0, 0, SRCCOPY);
	else
		BitBlt(hDc, rc->left+4, rc->top+5, rc->right, rc->bottom, hdcMem, 0, 0, SRCCOPY);
	
	//================================================================
	// Select old object back and delete memDC.
	//================================================================
	SelectObject(hdcMem, hOldObject);
	DeleteDC(hdcMem);
	return TRUE;
}

int MenuMeasureItem(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if(!wParam)
	{
		MEASUREITEMSTRUCT * pmis = (MEASUREITEMSTRUCT *)lParam;
		SIZE size;
		HDC hdc = GetDC(hwnd);
		GetTextExtentPoint32(hdc, "Qwerty", 7, &size);
		menuHeight = size.cy + 6;
		pmis->itemHeight = menuHeight;
		if(pmis->itemID < IDM_EDITSENDTO)
			pmis->itemWidth  = 140;
		else
			pmis->itemWidth  = 160;
		ReleaseDC(hwnd, hdc);
		return TRUE;
	}
	else
		return FALSE;
}

int MenuItemDraw(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(wParam != 0)
		return FALSE;
	MENUITEMINFO mii;
	
	DRAWITEMSTRUCT * pdis = (DRAWITEMSTRUCT *)lParam;
	HANDLE hOldObj;
	HBRUSH hBrhHigh = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
	HBRUSH hBrhLow  = CreateSolidBrush(GetSysColor(COLOR_MENU));
	HPEN hPenHigh   = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
	HPEN hPenLow    = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	HPEN hPenMid    = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_MENU));
	
	// Get the menu item info first
	GetMenuItemInfo(GetMenu(hwnd), pdis->itemID, FALSE, &mii);
	
	int top    = pdis->rcItem.top + 3;
	int left   = pdis->rcItem.left + menuHeight;
	int right  = pdis->rcItem.right;
	int bottom = pdis->rcItem.bottom;
	
	if( pdis->itemAction == ODA_DRAWENTIRE)
	{
		if(pdis->itemID == IDM_DEPENDS && pdis->itemData == DISABLED)
			DrawBitMap(hMenuBmps[ID_NOTDEPENDS], pdis->hDC, &pdis->rcItem);
		else
			DrawBitMap(hMenuBmps[pdis->itemID - IDM_FIRST], pdis->hDC, &pdis->rcItem);
		
		if( pdis->itemData == DISABLED)
		{
			// If disabled draw text twice with transparency
			SetBkMode(pdis->hDC, TRANSPARENT);
			SetTextColor(pdis->hDC, GetSysColor(COLOR_BTNHIGHLIGHT));
			TextOut(pdis->hDC, left+1, top, strMenu[pdis->itemID - IDM_FIRST], strlen(strMenu[pdis->itemID - IDM_FIRST]));
			SetTextColor(pdis->hDC, GetSysColor(COLOR_GRAYTEXT));
			TextOut(pdis->hDC, left, top - 1, strMenu[pdis->itemID - IDM_FIRST], strlen(strMenu[pdis->itemID - IDM_FIRST]));
			
		}
		else
		{
			// Else draw text normal
			// can be selected first time when user APT+key
			if (pdis->itemState & ODS_SELECTED)
			{
				SetBkColor(pdis->hDC, GetSysColor(COLOR_HIGHLIGHT));
				SetTextColor(pdis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
				TextOut(pdis->hDC, left, top - 1, strMenu[pdis->itemID - IDM_FIRST], strlen(strMenu[pdis->itemID - IDM_FIRST]));
			}
			else
			{
				SetTextColor(pdis->hDC, GetSysColor(COLOR_MENUTEXT));
				TextOut(pdis->hDC, left, top - 1, strMenu[pdis->itemID - IDM_FIRST], strlen(strMenu[pdis->itemID - IDM_FIRST]));
			}
		}
		goto ende;
	}
	
	if( pdis->itemAction == ODA_SELECT )
	{
		if(pdis->itemID == IDM_DEPENDS && pdis->itemData == DISABLED)
			DrawBitMap(hMenuBmps[ID_NOTDEPENDS], pdis->hDC, &pdis->rcItem);
		else
			DrawBitMap(hMenuBmps[pdis->itemID - IDM_FIRST], pdis->hDC, &pdis->rcItem);
		
		if (pdis->itemState & ODS_SELECTED)
		{
			if( pdis->itemData == ENABLED)
			{
				// Draw rectangle for highlight effect
				hOldObj = SelectObject(pdis->hDC, hBrhHigh);
				hOldObj = SelectObject(pdis->hDC, GetStockObject(NULL_PEN));
				Rectangle(pdis->hDC, left, top - 2, right, bottom + 1);
				SelectObject(pdis->hDC, hOldObj);
				
				// Draw highlight lines
				hOldObj = SelectObject(pdis->hDC, hPenHigh);
				MoveToEx(pdis->hDC, 0, bottom - 2, NULL);
				LineTo(pdis->hDC, 0, top - 1);
				LineTo(pdis->hDC, menuHeight - 2, top - 1);
				SelectObject(pdis->hDC, hPenLow);
				LineTo(pdis->hDC, menuHeight - 2, bottom - 2);
				LineTo(pdis->hDC, 0, bottom - 2);
				
				// Draw text
				SetBkColor(pdis->hDC, GetSysColor(COLOR_HIGHLIGHT));
				SetTextColor(pdis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
				TextOut(pdis->hDC, left, top - 1, strMenu[pdis->itemID - IDM_FIRST], strlen(strMenu[pdis->itemID - IDM_FIRST]));
				
			}
			else if (pdis->itemData == DISABLED)
			{
				SetBkMode(pdis->hDC, TRANSPARENT);
				SetTextColor(pdis->hDC, GetSysColor(COLOR_BTNHIGHLIGHT));
				TextOut(pdis->hDC, left+1, top, strMenu[pdis->itemID - IDM_FIRST], strlen(strMenu[pdis->itemID - IDM_FIRST]));
				SetTextColor(pdis->hDC, GetSysColor(COLOR_GRAYTEXT));
				TextOut(pdis->hDC, left, top - 1, strMenu[pdis->itemID - IDM_FIRST], strlen(strMenu[pdis->itemID - IDM_FIRST]));
			}
			
		}
		else
		{ // Not Selected
			hOldObj = SelectObject(pdis->hDC, hPenMid);
			MoveToEx(pdis->hDC, 0, bottom - 2, NULL);
			LineTo(pdis->hDC, 0, top - 1);
			LineTo(pdis->hDC, menuHeight - 2, top - 1);
			LineTo(pdis->hDC, menuHeight - 2, bottom - 2);
			LineTo(pdis->hDC, 0, bottom - 2);
			
			hOldObj = SelectObject(pdis->hDC, hBrhLow);
			hOldObj = SelectObject(pdis->hDC, GetStockObject(NULL_PEN));
			Rectangle(pdis->hDC, left, top - 2, right, bottom + 1);
			SelectObject(pdis->hDC, hOldObj);
			
			if( pdis->itemData == DISABLED)
			{
				// If disabled draw text twice with transparency
				SetBkMode(pdis->hDC, TRANSPARENT);
				SetTextColor(pdis->hDC, GetSysColor(COLOR_BTNHIGHLIGHT));
				TextOut(pdis->hDC, left + 1, top, strMenu[pdis->itemID - IDM_FIRST], strlen(strMenu[pdis->itemID - IDM_FIRST]));
				SetTextColor(pdis->hDC, GetSysColor(COLOR_GRAYTEXT));
				TextOut(pdis->hDC, left, top - 1, strMenu[pdis->itemID - IDM_FIRST], strlen(strMenu[pdis->itemID - IDM_FIRST]));
			}
			else
			{
				// Else draw text normal
				SetBkColor(pdis->hDC, GetSysColor(COLOR_MENU));
				SetTextColor(pdis->hDC, GetSysColor(COLOR_MENUTEXT));
				TextOut(pdis->hDC, left, top - 1, strMenu[pdis->itemID - IDM_FIRST], strlen(strMenu[pdis->itemID - IDM_FIRST]));
			}
		}
	}
	ende:
	DeleteObject(hBrhHigh);
	DeleteObject(hBrhLow);
	DeleteObject(hPenHigh);
	DeleteObject(hPenLow);
	DeleteObject(hPenMid);
	
	return TRUE;
}

