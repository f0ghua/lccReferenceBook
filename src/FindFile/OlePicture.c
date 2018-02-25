// OlePicture.c
//
// John Findlay
//
// You may want to add error checking if using this stuff in an application,
// especially in OleLoadPicFile() and OleLoadPicRes()
//
// Link with: ole32.lib olepro32.lib uuid.lib
//
#define  COBJMACROS

#include <windows.h>
#include <stdio.h>
#include <olectl.h>
//#include <ocidl.h>

extern const IID IID_IPicture;
#define HIMETRIC_PER_INCH   2540      // number HIMETRIC units per inch

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : ShrinkBitmap(hwnd, hbm)                                    *
 *                                                                          *
 *  PURPOSE    : This function shrinks a 64x64 bitmap into a bitmap useable *
 *               for the user-defined checkmark for menu items. This can be *
 *               easily generalized to shrink bitmaps of any size.          *
 *                                                                          *
 *  RETURNS    : HBITMAP - A handle to the new bitmap.                      *
 *                                                                          *
 ****************************************************************************/
HBITMAP APIENTRY ShrinkBitmap(HWND hwnd, int cx, int cy, HBITMAP hbm)
{
    HDC     hdc;
    HDC     hmemorydcNew;
    HDC     hmemorydcOld;
    LONG    checkMarkSize;
    HBITMAP hCheckBitmap;
    HBITMAP hOldBitmapSave;
    HBITMAP hNewBitmapSave;

    hdc = GetDC (hwnd);

    /* Create DCs for the source (old) and target (new) bitmaps */
    hmemorydcNew = CreateCompatibleDC (hdc);
    hmemorydcOld = CreateCompatibleDC (hdc);

    /* Determine the dimensions of the default menu checkmark and
     * create a target bitmap of the same dimensions
     */
    checkMarkSize = GetMenuCheckMarkDimensions ();
    hCheckBitmap  = CreateCompatibleBitmap (hdc, LOWORD(checkMarkSize), HIWORD(checkMarkSize));

    /* Select the source bitmap and the target bitmap into their
     * respective DCs.
     */
    hOldBitmapSave = SelectObject (hmemorydcNew, hCheckBitmap);
    hNewBitmapSave = SelectObject (hmemorydcOld, hbm);

    /* Shrink the source bitmap into the target DC */
    StretchBlt (hmemorydcNew,
                0,
                0,
                LOWORD(checkMarkSize),
                HIWORD(checkMarkSize),
                hmemorydcOld,
                0,
                0,
                cx,
                cy,
                SRCCOPY);

    /* De-select the bitmaps and clean up .. */
    SelectObject (hmemorydcNew, hOldBitmapSave);
    SelectObject (hmemorydcOld, hNewBitmapSave);
    DeleteDC (hmemorydcNew);
    DeleteDC (hmemorydcOld);
    ReleaseDC (hwnd, hdc);

    /* .. and return a handle to the target bitmap */
    return hCheckBitmap;
}

void OleReleasePic(IPicture * pIPicture)
{
	if (pIPicture)
		pIPicture->lpVtbl->Release(pIPicture);
}

long OleGetPicWidth(IPicture * pIPicture)
{
	long Width;
	pIPicture->lpVtbl->get_Width(pIPicture, &Width);
	return Width;
}

long OleGetPicHeight(IPicture * pIPicture)
{
	long Height;
	pIPicture->lpVtbl->get_Height(pIPicture, &Height);
	return Height;
}

void OlePicRender(IPicture * pIPicture, HDC hdc, RECT *rc, BOOL fit)
{
	long hmWidth  = OleGetPicWidth(pIPicture);
	long hmHeight = OleGetPicHeight(pIPicture);

	// convert himetric to pixels
	float nWidth	= MulDiv(hmWidth, GetDeviceCaps(hdc, LOGPIXELSX), HIMETRIC_PER_INCH);
	float nHeight	= MulDiv(hmHeight, GetDeviceCaps(hdc, LOGPIXELSY), HIMETRIC_PER_INCH);
	float ratio;
	if(fit)
	{
		if(nWidth > nHeight && nWidth > rc->right)
		{
			ratio = nWidth/(float)rc->right;
			nWidth = rc->right;
			nHeight /= ratio;
		}
		else if(nHeight > nWidth && nHeight > rc->bottom)
		{
			ratio = nHeight/(float)rc->bottom;
			nHeight = rc->bottom;
			nWidth /= ratio;
		}
		else if(nHeight == nWidth && nWidth > rc->right)
		{
			ratio = nWidth/(float)rc->right;
			nWidth = rc->right;
			nHeight /= ratio;
		}
	}
	pIPicture->lpVtbl->Render(pIPicture, hdc, rc->left, rc->top, 
			nWidth, nHeight, 0, hmHeight, hmWidth, -hmHeight, rc);
}

DWORD OleLoadPicFile(char *File, IPicture ** pIPicture)
{
	// open file
	HANDLE hFile = CreateFile(File, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return 1;

	// get file size
	DWORD dwFileSize = GetFileSize(hFile, NULL);

	LPVOID pvData = NULL;
	// alloc memory based on file size
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);

	pvData = GlobalLock(hGlobal);

	DWORD dwBytesRead = 0;
	// read file and store in global memory
	ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
	GlobalUnlock(hGlobal);
	CloseHandle(hFile);

	LPSTREAM pstm = NULL;
	// create IStream* from global memory
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);

	hr = OleLoadPicture(pstm, dwFileSize, FALSE, (REFIID)&IID_IPicture, (LPVOID)pIPicture);
	pstm->lpVtbl->Release(pstm);

	return hr;
}

