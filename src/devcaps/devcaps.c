/* Mike Caetano, prionx@juno.com, 09/09/2001 */
#include <windows.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////

typedef struct tagCapsTable {
	DWORD value;
	TCHAR concise[MAX_PATH];
	VOID (* func)(TCHAR *);
}
CAPSTABLE, *PCAPSTABLE;

// VOID (* func)(TCHAR *);
VOID  ResultResponse(TCHAR *);

BOOL  __stdcall CheckDeviceCaps(HDC, PCAPSTABLE);
BOOL  __stdcall ListCapsTable(PCAPSTABLE);

BOOL  __stdcall ConfirmFileExists(LPCTSTR);
HWND  __stdcall GetConsoleWindow9X(void);

typedef struct tagGDCTable {
	DWORD value;
	TCHAR concise[MAX_PATH];
	TCHAR verbose[MAX_PATH];
	DWORD condition;
	DWORD result;
	VOID (* func)(TCHAR *);
} GDCTABLE, *PGDCTABLE;


////////////////////////////////////////////////////////////////////////
// G E T  C O N S O L E  H A N D L E  9 X ------------------------------

#define CON_GUID TEXT("CON-{CE654C6F-C5A1-429D-8582-AC0007ED7AED}")

HWND  __stdcall GetConsoleWindow9X(void)
{

	HWND hConWnd = NULL;
	WIN32_FIND_DATA fd;
	HANDLE ff;

	static TCHAR szTempTitle[] = CON_GUID;
	static TCHAR szTempString[MAX_PATH];

	if( GetConsoleTitle(szTempString, sizeof(szTempString)/sizeof(TCHAR) ) > 0 )
	{
		SetConsoleTitle(szTempTitle);
		hConWnd = FindWindow(TEXT("tty"), szTempTitle);
		SetConsoleTitle(szTempString);
	}

	if ( !hConWnd )
	{
		ZeroMemory(&szTempString, MAX_PATH);
		if ( GetModuleFileName(NULL, szTempString, MAX_PATH) > 0 )
		{
			ff = FindFirstFile(szTempString, &fd);
			if ( ff != INVALID_HANDLE_VALUE )
			{
				ZeroMemory(&szTempString, MAX_PATH);
				for ( int i = 0; fd.cFileName[i] != '.'; i++ )
				{
					szTempString[i] = fd.cFileName[i];
				}
				hConWnd = FindWindow(TEXT("tty"), szTempString);
			}
			FindClose(ff);
		}
	}

	return( hConWnd );

}


////////////////////////////////////////////////////////////////////////

CAPSTABLE rastercaps[] = {
	  { RASTERCAPS, TEXT("RASTERCAPS"), ResultResponse }
	, { RC_BITBLT, TEXT("RC_BITBLT"), ResultResponse }
	, { RC_BANDING, TEXT("RC_BANDING"), ResultResponse }
	, { RC_SCALING, TEXT("RC_SCALING"), ResultResponse }
	, { RC_BITMAP64, TEXT("RC_BITMAP64"), ResultResponse }
	, { RC_GDI20_OUTPUT, TEXT("RC_GDI20_OUTPUT"), ResultResponse }
	, { RC_GDI20_STATE, TEXT("RC_GDI20_STATE"), ResultResponse }
	, { RC_SAVEBITMAP, TEXT("RC_SAVEBITMAP"), ResultResponse }
	, { RC_DI_BITMAP, TEXT("RC_DI_BITMAP"), ResultResponse }
	, { RC_PALETTE, TEXT("RC_PALETTE"), ResultResponse }
	, { RC_DIBTODEV, TEXT("RC_DIBTODEV"), ResultResponse }
	, { RC_BIGFONT, TEXT("RC_BIGFONT"), ResultResponse }
	, { RC_STRETCHBLT, TEXT("RC_STRETCHBLT"), ResultResponse }
	, { RC_FLOODFILL, TEXT("RC_FLOODFILL"), ResultResponse }
	, { RC_STRETCHDIB, TEXT("RC_STRETCHDIB"), ResultResponse }
	, { RC_OP_DX_OUTPUT, TEXT("RC_OP_DX_OUTPUT"), ResultResponse }
	, { RC_DEVBITS, TEXT("RC_DEVBITS"), ResultResponse }
	, { RC_NONE, TEXT("RC_NONE"), ResultResponse }
};


CAPSTABLE curvecaps[] = {
	  { CURVECAPS, TEXT("CURVECAPS"), ResultResponse }
	, { CC_CIRCLES, TEXT("CC_CIRCLES"), ResultResponse }
	, { CC_PIE, TEXT("CC_PIE"), ResultResponse }
	, { CC_CHORD, TEXT("CC_CHORD"), ResultResponse }
	, { CC_ELLIPSES, TEXT("CC_ELLIPSES"), ResultResponse }
	, { CC_WIDE, TEXT("CC_WIDE"), ResultResponse }
	, { CC_STYLED, TEXT("CC_STYLED"), ResultResponse }
	, { CC_WIDESTYLED, TEXT("CC_WIDESTYLED"), ResultResponse }
	, { CC_INTERIORS, TEXT("CC_INTERIORS"), ResultResponse }
	, { CC_ROUNDRECT, TEXT("CC_ROUNDRECT"), ResultResponse }
	, { CC_NONE, TEXT("CC_NONE"), ResultResponse }
};


CAPSTABLE linecaps[] = {
	  { LINECAPS, TEXT("LINECAPS"), ResultResponse }
	, { LC_POLYLINE, TEXT("LC_POLYLINE"), ResultResponse }
	, { LC_MARKER, TEXT("LC_MARKER"), ResultResponse }
	, { LC_POLYMARKER, TEXT("LC_POLYMARKER"), ResultResponse }
	, { LC_WIDE, TEXT("LC_WIDE"), ResultResponse }
	, { LC_STYLED, TEXT("LC_STYLED"), ResultResponse }
	, { LC_WIDESTYLED, TEXT("LC_WIDESTYLED"), ResultResponse }
	, { LC_INTERIORS, TEXT("LC_INTERIORS"), ResultResponse }
	, { LC_NONE, TEXT("LC_NONE"), ResultResponse }
};


CAPSTABLE polycaps[] = {
	  { POLYGONALCAPS, TEXT("POLYGONALCAPS"), ResultResponse }
	, { PC_POLYGON, TEXT("PC_POLYGON"), ResultResponse }
	, { PC_RECTANGLE, TEXT("PC_RECTANGLE"), ResultResponse }
	, { PC_WINDPOLYGON, TEXT("PC_WINDPOLYGON"), ResultResponse }
	, { PC_TRAPEZOID, TEXT("PC_TRAPEZOID"), ResultResponse }
	, { PC_SCANLINE, TEXT("PC_SCANLINE"), ResultResponse }
	, { PC_WIDE, TEXT("PC_WIDE"), ResultResponse }
	, { PC_STYLED, TEXT("PC_STYLED"), ResultResponse }
	, { PC_WIDESTYLED, TEXT("PC_WIDESTYLED"), ResultResponse }
	, { PC_INTERIORS, TEXT("PC_INTERIORS"), ResultResponse }
	, { PC_POLYPOLYGON, TEXT("PC_POLYPOLYGON"), ResultResponse }
	, { PC_PATHS, TEXT("PC_PATHS"), ResultResponse }
	, { PC_NONE, TEXT("PC_NONE"), ResultResponse }
};


CAPSTABLE textcaps[] = {
	  { TEXTCAPS, TEXT("TEXTCAPS"), ResultResponse }
	, { TC_OP_CHARACTER, TEXT("TC_OP_CHARACTER"), ResultResponse }
	, { TC_OP_STROKE, TEXT("TC_OP_STROKE"), ResultResponse }
	, { TC_CP_STROKE, TEXT("TC_CP_STROKE"), ResultResponse }
	, { TC_CR_90, TEXT("TC_CR_90"), ResultResponse }
	, { TC_CR_ANY, TEXT("TC_CR_ANY"), ResultResponse }
	, { TC_SF_X_YINDEP, TEXT("TC_SF_X_YINDEP"), ResultResponse }
	, { TC_SA_DOUBLE, TEXT("TC_SA_DOUBLE"), ResultResponse }
	, { TC_SA_INTEGER, TEXT("TC_SA_INTEGER"), ResultResponse }
	, { TC_SA_CONTIN, TEXT("TC_SA_CONTIN"), ResultResponse }
	, { TC_EA_DOUBLE, TEXT("TC_EA_DOUBLE"), ResultResponse }
	, { TC_IA_ABLE, TEXT("TC_IA_ABLE"), ResultResponse }
	, { TC_UA_ABLE, TEXT("TC_UA_ABLE"), ResultResponse }
	, { TC_SO_ABLE, TEXT("TC_SO_ABLE"), ResultResponse }
	, { TC_RA_ABLE, TEXT("TC_RA_ABLE"), ResultResponse }
	, { TC_VA_ABLE, TEXT("TC_VA_ABLE"), ResultResponse }
	, { TC_RESERVED, TEXT("TC_RESERVED"), ResultResponse }
	, { TC_SCROLLBLT, TEXT("TC_SCROLLBLT"), ResultResponse }
	, { 0, TEXT("TC_NONE"), ResultResponse }
};


// NOTE: DT_PLOTTER == 0
CAPSTABLE techtype[] = {
	  { TECHNOLOGY, TEXT("TECHNOLOGY"), ResultResponse }
	, { DT_RASDISPLAY, TEXT("DT_RASDISPLAY"), ResultResponse }
	, { DT_RASPRINTER, TEXT("DT_RASPRINTER"), ResultResponse }
	, { DT_RASCAMERA, TEXT("DT_RASCAMERA"), ResultResponse }
	, { DT_CHARSTREAM, TEXT("DT_CHARSTREAM"), ResultResponse }
	, { DT_METAFILE, TEXT("DT_METAFILE"), ResultResponse }
	, { DT_DISPFILE, TEXT("DT_DISPFILE"), ResultResponse }
	, { DT_PLOTTER, TEXT("DT_PLOTTER"), ResultResponse }
	, { 0,0,0 }
};


// GENERAL DEVICE CAPS

GDCTABLE gdctable[] = {
	  { HORZSIZE, TEXT("HORZSIZE"), TEXT("Width, in millimeters, of the physical screen."), 0, 0, ResultResponse }
	, { VERTSIZE, TEXT("VERTSIZE"), TEXT("Height, in millimeters, of the physical screen."), 0, 0, ResultResponse }
	, { HORZRES, TEXT("HORZRES"), TEXT("Width, in pixels, of the screen."), 0, 0, ResultResponse }
	, { VERTRES, TEXT("VERTRES"), TEXT("Height, in raster lines, of the screen."), 0, 0, ResultResponse }
	, { LOGPIXELSX, TEXT("LOGPIXELSX"), TEXT("Number of pixels per logical inch along the screen width."), 0, 0, ResultResponse }
	, { LOGPIXELSY, TEXT("LOGPIXELSY"), TEXT("Number of pixels per logical inch along the screen height."), 0, 0, ResultResponse }
	, { BITSPIXEL, TEXT("BITSPIXEL"), TEXT("Number of adjacent color bits for each pixel."), 0, 0, ResultResponse }
	, { PLANES, TEXT("PLANES"), TEXT("Number of color planes."), 0, 0, ResultResponse }
	, { NUMBRUSHES, TEXT("NUMBRUSHES"), TEXT("Number of device-specific brushes."), 0, 0,ResultResponse }
	, { NUMPENS,  TEXT("NUMPENS"),  TEXT("Number of device-specific pens."), 0, 0, ResultResponse }
	, { NUMFONTS, TEXT("NUMFONTS"),  TEXT("Number of device-specific fonts."), 0, 0, ResultResponse }
	, { NUMCOLORS, TEXT("NUMCOLORS"),  TEXT("Number of entries in the device's color table."), 0, 0, ResultResponse }
	, { ASPECTX, TEXT("ASPECTX"), TEXT("Relative width of a device pixel used for line drawing."), 0, 0,ResultResponse }
	, { ASPECTY, TEXT("ASPECTY"),  TEXT("Relative height of a device pixel used for line drawing."), 0, 0,ResultResponse }
	, { ASPECTXY, TEXT("ASPECTXY"),  TEXT("Diagonal width of the device pixel used for line drawing."), 0, 0,ResultResponse }
	, { PDEVICESIZE, TEXT("PDEVICESIZE"),  TEXT("Reserved."), 0, 0,ResultResponse }
	, { CLIPCAPS, TEXT(" CLIPCAPS"), TEXT("Flag that indicates the clipping capabilities of the device."), 0, 0, ResultResponse }
	, { SIZEPALETTE, TEXT("SIZEPALETTE"), TEXT("Number of entries in the system palette."), 0, 0, ResultResponse }
	, { NUMRESERVED, TEXT("NUMRESERVED"), TEXT("Number of reserved entries in the system palette."), 0, 0, ResultResponse }
	, { COLORRES, TEXT("COLORRES"),  TEXT("Actual color resolution of the device, in bits per pixel."), 0, 0, ResultResponse }
	, { VREFRESH, TEXT("VREFRESH"), TEXT("Windows NT only: For display devices: the current vertical refresh rate of the device, in cycles per second (Hz)."), 0, 0, ResultResponse }
	, { DESKTOPHORZRES, TEXT("DESKTOPHORZRES"), TEXT("Windows NT only: Width, in pixels, of the virtual desktop."), 0, 0, ResultResponse }
	, { DESKTOPVERTRES, TEXT("DESKTOPVERTRES"), TEXT("Windows NT only: Height, in pixels, of the virtual desktop."), 0, 0, ResultResponse }
	, { BLTALIGNMENT, TEXT("BLTALIGNMENT"), TEXT("Windows NT only: Preferred horizontal drawing alignment, expressed as a multiple of pixels."), 0, 0, ResultResponse }
	, { PHYSICALWIDTH, TEXT("PHYSICALWIDTH"), TEXT("For printing devices: the width of the physical page, in device units."), 0, 0, ResultResponse }
	, { PHYSICALHEIGHT, TEXT("PHYSICALHEIGHT"), TEXT("For printing devices: the height of the physical page, in device units."), 0, 0, ResultResponse }
	, { PHYSICALOFFSETX, TEXT("PHYSICALOFFSETX"), TEXT("For printing devices: the distance  from the left edge of the physical page to the left edge of the printable area, in device units."), 0, 0, ResultResponse }
	, { PHYSICALOFFSETY, TEXT("PHYSICALOFFSETY"), TEXT("For printing devices: the distance  from the top edge of the physical page to the top edge of the printable area, in device units."), 0, 0, ResultResponse }
	, { 0, 0, 0, 0, 0, 0 }
};


////////////////////////////////////////////////////////////////////////
//  ---------------------------------
BOOL  __stdcall GetDeviceCapNumbers(HDC hdc, PGDCTABLE pTable)
{

	BOOL ok = FALSE;
	TCHAR szBuffer[512];

	if ( hdc != NULL )
	{

		// pTable++;

		while ( pTable->value )
		{

			pTable->result = GetDeviceCaps(hdc, pTable->value);

			if ( pTable->result )
			{

				ZeroMemory(szBuffer,512);
				wsprintf(szBuffer,"%13s %8d", pTable->concise, pTable->result);
				pTable->func(szBuffer);

			}

			pTable++;

		}

		ok = TRUE;

	}

	return(ok);

}


////////////////////////////////////////////////////////////////////////
// function pointed to by xtable function pointer

VOID ResultResponse(TCHAR *str)
{
	// its not about whether the os is unicode
    // but if the string is unicode
	if ( IsTextUnicode(str, lstrlen(str), NULL) ) {
		// use with Unicode
		fprintf(stdout, "\n%S", str);
	}
    else {
		// use with Ascii
		fprintf(stdout, "\n%30s", str);
	}

}


////////////////////////////////////////////////////////////////////////
// list table contents

BOOL  __stdcall ListCapsTable(PCAPSTABLE pCapsTable)
{

	BOOL ok = TRUE;

	TCHAR szBuffer[512];

	while ( pCapsTable->value )
	{

		wsprintf(szBuffer
				, "%08X %08X %s"
				, &*pCapsTable
				, pCapsTable->value
				, pCapsTable->concise);

		pCapsTable->func(szBuffer);

		pCapsTable++;

	}

	return(ok);

}


////////////////////////////////////////////////////////////////////////
// perform dev caps check and output results

BOOL  __stdcall CheckDeviceCaps(HDC hdc, PCAPSTABLE pCapsTable)
{

	BOOL ok = FALSE;

	if ( hdc != NULL )
	{

		int caps = GetDeviceCaps(hdc, pCapsTable->value);

		pCapsTable++;

		while ( pCapsTable->value )
		{

			if ( caps & pCapsTable->value )
			{

				pCapsTable->func(pCapsTable->concise);

			}

			pCapsTable++;

		}

		ok = TRUE;

	}

	return(ok);

}


int main(int argc, char *argv[])
{

	HWND hConsole = GetConsoleWindow9X();

// Confirm Tables

//	ListCapsTable(techtype);
//	ListCapsTable(rastercaps);
//	ListCapsTable(curvecaps);
//	ListCapsTable(linecaps);
//	ListCapsTable(polycaps);
//	ListCapsTable(textcaps);

// Perform Dev Cap Checks

	HDC hdc = GetDC(hConsole);

	CheckDeviceCaps(hdc, techtype);
	CheckDeviceCaps(hdc, rastercaps);
	CheckDeviceCaps(hdc, curvecaps);
	CheckDeviceCaps(hdc, linecaps);
	CheckDeviceCaps(hdc, polycaps);
	CheckDeviceCaps(hdc, textcaps);

	GetDeviceCapNumbers(hdc, gdctable);

	ReleaseDC(hConsole,hdc);

	return(0);

}

