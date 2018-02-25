// RunDlg.h
//
// Define various styles for dialog and controls
#define DLGSTYLE DS_MODALFRAME | DS_3DLOOK | DS_SETFONT | WS_POPUP \
   					 | WS_CAPTION | WS_SYSMENU | WS_VISIBLE

#define BUTSTYLE BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE

#define STATSTYLE SS_CENTER | WS_CHILD | WS_VISIBLE

#define STATSTYLE1 SS_GRAYRECT | WS_CHILD | WS_VISIBLE

// The structs used for the dialog
typedef struct {
	DLGITEMTEMPLATE dli;
	WORD ch;
	WORD c;
	WORD t;
	WORD dummy;
	WORD cd;
} DLGITEM;

typedef struct {
	DLGTEMPLATE dlt;
	WORD menu;
	WORD class;
	WCHAR title[8];
	WORD pt;
	WCHAR font[14];
	DLGITEM ia;
	DLGITEM ib;
	DLGITEM ic;
	DLGITEM id;
} DLGDATA;

