// buttonex.h
//
// John Findlay
//

#ifndef _BUTX_H_
#define _BUTX_H_

#include <windows.h>

// the button class name
#define BUTTONEX "ButtonExClass"

// messages
#define BXM_SETIMAGE   0x00F7
#define BXM_SETBKCOLOR 0x8200
#define BXM_GETBKCOLOR 0x8201
#define BXM_GETFONT    0x31
#define BXM_SETFONT    0x30
#define BXM_SETTEXT    0xC
#define BXM_GETTEXT    0xD

// styles
#define BXS_LEFTTEXT   0x1
#define BXS_RIGHTTEXT  0x2
#define BXS_TOPTEXT    0x4
#define BXS_BOTTOMTEXT 0x8
#define BXS_FOCUSRECT  0x20
#define BXS_ICON       0x40
#define BXS_BITMAP     0x80
#define BXS_XP         0x100		// BXS_FLAT & BXS_FLATROUND have been abandoned
#define BXS_LITE       0x800
#define BXS_BMPONLY    0x1400		// explicitly when only bmp is to be displayed
#define BXS_TOOLGROUP  0x2000
#define BXS_TOOL       0x4000
#define BXS_TABSTOP    0x10000
#define BXS_BORDER     0x800000L
#define BXS_NORMAL     0x20000L

// means, do not alter a bitmap when WM_SYSCOLORCHANGE message arrives.
#define BXS_NOSYSCOLORCHANGE  0x10

// main creation API
HWND CreateButtonEx(char * text, DWORD dwStyle, int x, int y, int cx, int cy, HWND hWndParent, DWORD ctrlID);

// Functions
HFONT	 Butx_GetFont(HWND hBut);
void 	 Butx_SetFont(HWND hBut, HFONT hFont);

void 	 Butx_GetText(HWND hBut, char * buf, int len);
int 	 Butx_SetText(HWND hBut, char * str);

void 	 Butx_SetBkColor(HWND hBut, COLORREF colref);
COLORREF Butx_GetBkColor(HWND hBut);

void 	 Butx_SetTextColor(HWND hBut, COLORREF colref);
COLORREF Butx_GetTextColor(HWND hBut);

void 	 Butx_ShowWindow(HWND hBut, DWORD show);

void     Butx_libvers(void);

void     Butx_SysColorChange(HWND hwnd);

int      Butx_AddToolTipText(HWND hBut, char * str);
void 	 Butx_SetCursor(HWND hBut, HCURSOR hCur);

void     Butx_Style(HWND but, DWORD style);

void     Butx_SetToolGroup(HWND hBut);

BOOL     InitButtonEx(void);
void     ExitButtonEx(void);

HBITMAP  LoadConvertBitmapCols(int res, COLORREF colfrom, COLORREF colto);

#endif
