// OlePicture.h
//
// John Findlay

#ifndef _OLEPICTURE_H_
#define _OLEPICTURE_H_

#include <windows.h>
#include <olectl.h>
#define HIMETRIC_PER_INCH   2540      // number HIMETRIC units per inch
HBITMAP APIENTRY ShrinkBitmap(HWND hwnd, int cx, int cy, HBITMAP hbm);
void OleReleasePic(LPPICTURE pIPicture);
DWORD OleLoadPicFile(char *szFile, LPPICTURE *pIPicture);
DWORD OleLoadPicRes(HINSTANCE hInst, int resID, LPPICTURE *pIPicture);
HRESULT OleSavePicture(IPicture * pIPicture, char * file, long filesize);
HANDLE OleGetPicHandle(LPPICTURE pIPicture);
int OleGetPicWidth(LPPICTURE pIPicture);
int OleGetPicHeight(LPPICTURE pIPicture);
void OlePicRender(IPicture * pIPicture, HDC hdc, RECT *rc, BOOL fit);
short OleGetPicType(LPPICTURE pIPicture);

#endif // _OLEPICTURE_H_
