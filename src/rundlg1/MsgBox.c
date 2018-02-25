//
// MsgBox.c
//
// John Findlay
//
#include <windows.h>
#include "msgbox.h"

// To align a pointer on dword boundary
#define PDWORD_ALIGN(p) (LPWORD)((((DWORD)(p)+3) >> 2) << 2)

BOOL CALLBACK OwnerDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg){
	case WM_INITDIALOG:
		return 1;

    case WM_COMMAND:{
		switch (LOWORD(wParam))
		case IDOK:
		case IDCANCEL:
			return EndDialog(hwnd, LOWORD(wParam));
		}
	}
	return FALSE;
}

LPWORD MakeDlg(LPDLGTEMPLATE pdlgt, DWORD style, DWORD exstyle, int num, short x, short y,
		short cx, short cy, short menu, short class, LPSTR szTitle, short pt, LPSTR facename)
{
	LPWORD lpw;
	LPWSTR lpwsz;

	//==============================================================
	// First, the pointer of allocated memory is passed as pdlgt,
	// the struct is filled in with appropriate values.
	//==============================================================
	// Do dialog
	pdlgt->style = style;
	pdlgt->dwExtendedStyle = exstyle;// extended style
	pdlgt->cdit = num; 				 // number of controls
	pdlgt->x    = x;				 // left
	pdlgt->y    = y;				 // top
	pdlgt->cx   = cx;				 // width
	pdlgt->cy   = cy;				 // height

	//==============================================================
	// Second, the pointer is taken past DLGTEMPLATE struct, and
	// the three variable-length arrays that specify the menu,
	// class, and title for the dialog box are filled in.
	//==============================================================

	// Increment the DWORD pointer past the DLGTEMPLATE struct
	lpw = (LPWORD) (pdlgt+1);

	*lpw++ = menu;   // menu
	*lpw++ = class;  // class

	// Set the title (could use MultiByteToWideChar() here)
	for (lpwsz = (LPWSTR)lpw;
        *lpwsz++ = (WCHAR)*szTitle++;
    );

    lpw = (LPWORD)lpwsz;

	// Set font point size
	*lpw++ = pt;

	// Set the face name (could use MultiByteToWideChar() here)
	for (lpwsz = (LPWSTR)lpw;
        *lpwsz++ = (WCHAR)*facename++;
    );

	// Return lastest addressed
    lpw = (LPWORD)lpwsz;

	return lpw;

}

LPWORD MakeDlgItem(LPWORD lpw, DWORD style, DWORD exstyle, short x, short y, short cx,
		short cy, WORD id, WORD arclass, WORD wclass, LPSTR szTitle, int ord, WORD cb)
{
	LPDLGITEMTEMPLATE pdlgIt;
	LPWSTR lpwsz;

	//=====================================================
	//
	// The pointer is passed after creating the DLG itself.
	//
	// The pointer is then aligned on a DWORD boundary, the
	// DLGITEMTEMPLATE struct is filled in and then the class
	// and title arrays are filled in.
	//=====================================================

	lpw = PDWORD_ALIGN(lpw); // align DLGITEMTEMPLATE on DWORD boundary
	pdlgIt = (LPDLGITEMTEMPLATE) lpw;
	pdlgIt->style = style;
	pdlgIt->dwExtendedStyle = exstyle;	// extended style
	pdlgIt->x  = x;
	pdlgIt->y  = y;
	pdlgIt->cx = cx;
	pdlgIt->cy = cy;
	pdlgIt->id = id;  					// identifier

	// Increment the pointer past the DLGITEMTEMPLATE struct
	lpw = (LPWORD) (pdlgIt + 1);

	*lpw++ = arclass;	// arr
	*lpw++ = wclass;	// class

	if (ord){
		*lpw++ = 0xFFFF;	// title
		*lpw++ = ord;		// ordinal value of icon
	}else{

		if (szTitle != NULL){
			// Set button text (could use MultiByteToWideChar() here)
			for (lpwsz = (LPWSTR)lpw;
		        *lpwsz++ = (WCHAR)*szTitle++;
		    );
		    lpw = (LPWORD)lpwsz;
		}
	}

	*lpw++ = 0x0000;       // no creation data

	return lpw;

}

LRESULT MsgBox(HINSTANCE hInst, HWND hwndOwner, char *szTitle, char *szMess)
{
	LPDLGTEMPLATE pdlgt;
	LPWORD lpw;
	LRESULT ret;

	//					font name      but     but
	char *pszStr[] = {"MS Sans Serif", "OK"};

	// Allocate enought memory for DLGTEMPLATE + 4 * DLGITEMTEMPLATE + Unicode strings etc
	pdlgt = (LPDLGTEMPLATE)malloc(1024);

	if (!pdlgt)
		return -1;

	// Define the Dialogbox.
	DWORD style = DS_MODALFRAME | DS_SETFONT | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
	lpw = MakeDlg(pdlgt, style, 0, 4, 0, 0, 178, 96, 0, 0, szTitle, 8, pszStr[0]);

	// Define an OK button.
	style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON;
	lpw = MakeDlgItem(lpw, style, 0, 64, 75, 50, 14, IDOK, 0xFFFF, BUTTON, pszStr[1], 0, 0);

	// Define a static with icon.
	style = WS_CHILD | WS_VISIBLE | SS_ICON;
	lpw = MakeDlgItem(lpw, style, 0, 4, 4, 50, 14, -1, 0xFFFF, STATIC, NULL, 1, 0);

	// Define a static text control.
	style = WS_CHILD | WS_VISIBLE | SS_LEFT;
	lpw = MakeDlgItem(lpw, style, 0, 27, 7, 144, 60, ID_TEXT, 0xFFFF, STATIC, szMess, 0, 0);

	// Define a static control.
	style = WS_CHILD | WS_VISIBLE | SS_GRAYRECT;
	lpw = MakeDlgItem(lpw, style, 0, 7, 62, 164, 1, -1, 0xFFFF, STATIC, NULL, 0, 0);

	ret = DialogBoxIndirect(hInst, (LPDLGTEMPLATE)pdlgt, hwndOwner,	(DLGPROC)OwnerDialogProc);

	free(pdlgt);

	return ret;
}


/*		Relevant info for creating dialogs for memory templates

	  	In a standard template for a dialog box, the DLGTEMPLATE structure
	    is always immediately followed by three variable-length arrays
	    that specify the menu, class, and title for the dialog box. When
	 	the DS_SETFONT style is specified, these arrays are also followed
	 	by a 16-bit value specifying point size and another variable-length
	 	array specifying a typeface name. Each array consists of one or
	 	more 16-bit elements. The menu, class, title, and font arrays must
	 	be aligned on WORD boundaries.

	 	Immediately following the DLGTEMPLATE structure is a menu array that
	 	identifies a menu resource for the dialog box. If the first element
	 	of this array is 0x0000, the dialog box has no menu and the array
	 	has no other elements. If the first element is 0xFFFF, the array has
	 	one additional element that specifies the ordinal value of a menu
	 	resource in an executable file. If the first element has any other
	 	value, the system treats the array as a null-terminated Unicode
	 	string that specifies the name of a menu resource in an executable file.

	 	Following the menu array is a class array that identifies the window
	 	class of the control. If the first element of the array is 0x0000, the
	 	system uses the predefined dialog box class for the dialog box and the
	 	array has no other elements. If the first element is 0xFFFF, the array
	 	has one additional element that specifies the ordinal value of a
	 	predefined system window class. If the first element has any other
	 	value, the system treats the array as a null-terminated Unicode string
	 	that specifies the name of a registered window class.

	 	Following the class array is a title array that specifies a
	 	null-terminated Unicode string that contains the title of the dialog
	 	box. If the first element of this array is 0x0000, the dialog box has
	 	no title and the array has no other elements.

	 	The 16-bit point size value and the typeface array follow the title
	 	array, but only if the style member specifies the DS_SETFONT style. The
	 	point size valuespecifies the point size of the font to use for the
	 	text in the dialog box and its controls. The typeface array is a
	 	null-terminated Unicode string specifying the name of the typeface
	 	for the font. When these values are specified, the system creates a
	 	font having the specified size and typeface (if possible) and sends
	 	a WM_SETFONT message to the dialog box procedure and the control window
	 	procedures as it creates the dialog box and controls.

	 	Following the DLGTEMPLATE header in a standard dialog box template are
	 	one or more DLGITEMTEMPLATE structures that define the dimensions and
	 	style of the controls in the dialog box. The cdit member specifies
	 	the number of DLGITEMTEMPLATE structures in the template. These
	 	DLGITEMTEMPLATE structures must be aligned on DWORD boundaries.
*/

/*		Immediately following each DLGITEMTEMPLATE structure is a class array
		that Specifies the window class of the control. If the first element
		of this array is any value other than 0xFFFF, the system treats the
		array as a null-terminated Unicode string that specifies the name of
		a registered window class. If the first element is 0xFFFF, the array
		has one additional element that specifies the ordinal value of a
		predefined system class. The ordinal can be one of the following atom values.
*/

/*		Following the class array is a title array that contains the initial
		text or resource identifier of the control. If the first element of
		this array is 0xFFFF, the array has one additional element that
		specifies an ordinal value of a resource, such as an icon, in an
		executable file. You can use a resource identifier for controls, such
		as static icon controls, that load and display an icon or other
		resource rather than text. If the first element is any value other
		than 0xFFFF, the system treats the array as a null-terminated Unicode
		string that specifies the initial text.
*/


