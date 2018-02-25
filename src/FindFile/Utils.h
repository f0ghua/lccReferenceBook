// Utils.h

#ifndef _UTILS_H_
#define _UTILS_H_

#define SndMsg SendMessage
#define SetWndTxt SetWindowText
#define GetCtrl GetDlgItem

int  DrawBitMap(HBITMAP hBmp, HDC hDc, RECT * rc);
int  MenuMeasureItem(HWND hwnd, WPARAM wParam, LPARAM lParam);
int  MenuItemDraw(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int  InitMenu(HWND hwnd, HINSTANCE hInst, char * szDepends);
void RemoveFileCountBraces(char * s);
int  GetFontHeight(HWND hWnd, HFONT hFont);
void ListView_SetColHeadTxtAll( HWND hwndLbx, int nCols);
void ListView_SetColumnWidths(HWND hwndLbx, int scale, int numcols, int * width);
void ExtractExt(char * path, char * ext);
int  CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
BOOL CreateSBar(HWND hwndParent, char *initialText, int nrOfParts);
void DestroyBitmaps(void);
void LoadBitmaps(HINSTANCE hInst);
void SortItems(HWND hListview, FINDFILE_OPTIONS * fileopts, int type);
void SetColumnBitmap(HWND hListview);
void ResetColumnBitmaps(void);
int  GetMenuItemState(HMENU hmenu, int id);
BOOL IsImageFile(char * pathname);
BOOL IsModuleFile(char * pathname);

#endif //_UTILS_H_
