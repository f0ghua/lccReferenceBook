//*************************************************************
// /PC:\/Fconfig*/S+/C-/D+/A-/w-
// FindFile.c -> main module
//*************************************************************

#include <windows.h>
#include <stdio.h>
#include "find.h"
#include "findfileres.h"
#include "olepicture.h"
#include "registry.h"
#include "jmem.h"
#include "shell.h"
#include "dialogs.h"
#include "idragdrop.h"
#include "utils.h"
#include "findfile.h"
#include "ButtonEx.h"

#define PREVIEW_MEMSIZE 2000 // size of buffer for displaying text (header text)

CRITICAL_SECTION g_CriticalSection;
static char * g_strDepends = NULL;
static int    g_activateDepends = 0;
static char * g_strMem;				// 2000 byte buffer for displaying text (header text)

LPSTR g_lpCmdLine;					// s.zorn cmdline extension 04.02.06

HBRUSH g_hBrLite;					// light colour as brush for static control bkgnds.

static BOOL g_bEndding = FALSE;		// if the main window is closed do not continue
// with pending SubClassRenameEditBoxProc()

static IPicture * g_pIPicture;
static WNDPROC    pSubClassPictureBoxProc 	 = NULL;	// subclasing
static WNDPROC    pSubClassTextBoxProc    	 = NULL;	// --ditto--
static WNDPROC    pSubClassRenameEditBoxProc = NULL;	// --ditto--
static WNDPROC    pSubClassExtPaneProc       = NULL;	// --ditto--

static HMEM g_hMem;	// memory handler returned from find.c used in SubClassRenameEditBoxProc()

static HFONT g_hFont;								// Courier New for TextBox
static int g_iCase, g_iSubFolder, g_iFileCounts;	// option check box's
int g_iAutoWildcards;

// the names for the header are stored in Utils.c
static int   g_ColWidth[NUMCOLS] = { 48, 96, 32, 40}; // dialogbaseunits for headers
static char  g_mru[6][MAX_PATH + 100];				  // most recently used files

static DWORD g_dwStart = 0;

char g_strSendto[6][MAX_PATH+100];		// extern in dialogs.c (Folders or Executables)
char g_strDisp[6][MAX_DISP];			// extern in dialogs.c (DisplayNames)

RD dlg = { 1, 3, 320, 203, SW_SHOWNORMAL };	// default dlg pos & size
RD bk  = { 3, 4, 80, 204 };					// default back panel pos & size

static int g_dwLastNative;			// index of combobox "BROWSE"
// -----------------------------------------------------------------

static SYSTEMTIME st;				// used to display date/time in DiaogProc
static FILETIME real;				// convert filetime to local time
static FILEINFO * pfi;				// used for the fileopts.fi[]
static FINDFILE_OPTIONS fileopts;	// this must be a global structure
static EXTENDED ext;				// for extended search params

HWND      g_hwndMain;
HINSTANCE g_hInst;

static HCURSOR g_hCurEW, g_hCurArrow;
static HPEN    g_hLightPen, g_hWhitePen, g_hShadowPen;

// used in the file rename function
// for IDM_COPYTO/IDM_MOVETO etc.
char strGen0[MAX_PATH+100];				// general purpose string
char strGen1[MAX_PATH+100];				// general purpose string
char strGen2[MAX_PATH+100];				// general purpose string

char sInitialDir[MAX_PATH+100];
char sFilterTxt[MAX_PATH+100];
char sTxtSearch[MAX_PATH+100];

HRESULT ResolveLnk(HWND hwnd, LPCSTR lpszLinkFile, LPSTR lpszPath);

void CallBackProc(int msg, FILEINFO * name) // 'name' is cast for (char*) and (HMEM)
{

	static LV_ITEM lvI;
	static char str[150];

	if(fileopts.count == 1)
	{
		SetFocus(GetCtrl(g_hwndMain, IDB_CANCEL));
	}

	if(msg == FF_READ_FILE && fileopts.count<1000)
	{
		// insert item for this index, subitem 0
		lvI.iItem    = ListView_GetItemCount(GetCtrl(g_hwndMain, IDC_LISTVIEW));
		lvI.iSubItem = 0;
		lvI.pszText  = NULL;
		lvI.mask = LVIF_TEXT;
		SendDlgItemMessage(g_hwndMain, IDC_LISTVIEW, LVM_INSERTITEM, 0, (LPARAM)&lvI);
	}

	if(msg == FF_READ_DIR)
	{
		strcpy(strGen0, "Searching ");
		strcat(strGen0, (char*)name);
		SendDlgItemMessage(g_hwndMain, IDC_STATUSBAR, SB_SETTEXT, 0, (LPARAM)strGen0);
	}

	if(msg == FF_FINISHED)
	{
		DWORD dwtime = GetTickCount();
		dwtime -= g_dwStart;
		double timesec = ((double)dwtime) / 1000;
		MessageBeep(MB_ICONEXCLAMATION);
		g_hMem = (HMEM)name; // get the memory handle
		Sleep(200);
		sprintf(str, "search done in %1.3f sec ...  %u File(s) found", timesec,fileopts.count);
		ListView_SetItemCount(GetCtrl(g_hwndMain,IDC_LISTVIEW), fileopts.count);
		EnableWindow(GetCtrl(g_hwndMain, IDB_CANCEL), FALSE);
		EnableWindow(GetCtrl(g_hwndMain, IDB_SEARCH), TRUE);
		PostMessage(g_hwndMain, FF_FINISHED, 0, (LPARAM)str);
	}

}

HICON g_hIcon; // icon for the Search button

int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
	if(OleInitialize(NULL) != S_OK)
		return FALSE;

	g_hInst = hinst;

	g_lpCmdLine = lpCmdLine;

	g_hIcon = LoadImage(hinst, MAKEINTRESOURCE(1),IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

	WNDCLASS wc;
	memset(&wc,0,sizeof(WNDCLASS));
	wc.lpfnWndProc 	 = DefDlgProc;
	wc.style		 = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
	wc.cbWndExtra 	 = DLGWINDOWEXTRA;
	wc.hInstance 	 = hinst;
	wc.hCursor 		 = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon 		 = LoadIcon(hinst, MAKEINTRESOURCE(IDI_FINDFILE));
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName  = MAKEINTRESOURCE(IDMENU_MAIN);
	wc.lpszClassName = "findfile";
	if(!RegisterClass(&wc))
		return 0;

	g_hCurEW    = LoadCursor(NULL, IDC_SIZEWE);
	g_hCurArrow = LoadCursor(NULL, IDC_ARROW);
	g_hWhitePen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
	g_hShadowPen= CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	g_hLightPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));

	// Ensure that the common control DLL is loaded.
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_USEREX_CLASSES | ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	InitButtonEx();

	DialogBox(hinst, MAKEINTRESOURCE(IDDIALOG_MAIN), NULL, (DLGPROC) MainWndProc);

	ExitButtonEx();

	DeleteObject(g_hWhitePen);
	DeleteObject(g_hLightPen);
	DeleteObject(g_hShadowPen);
	DestroyIcon(g_hIcon);
	OleUninitialize();
	return 0;
}

BOOL InitComboBox(HWND hwnd)
{
	HWND hwndCombo = GetCtrl(hwnd, IDC_COMBO);

	COMBOBOXEXITEM cbei;
	memset(&cbei, 0, sizeof(COMBOBOXEXITEM));

	char all[100];
	char buff[1024];
	char * p = buff;

	strcpy(all, "Local Drives ");
	char * pAll = all + 13;

	HIMAGELIST hSmallC = ImageList_Create(16, 16, ILC_COLOR8 | TRUE, 7, 0);

	for(int i=10; i<19; i++)
	{
		ImageList_AddIcon(hSmallC, LoadIcon(g_hInst, MAKEINTRESOURCE(i)));
	}
	SendMessage(hwndCombo, CBEM_SETIMAGELIST, 0, (LPARAM)hSmallC);

	int index = 0;
	GetLogicalDriveStrings(1024, buff);
	while (*p != 0)
	{
		switch (GetDriveType(p))
		{
		case DRIVE_UNKNOWN:
			strupr(p);
			sprintf(strGen0, "Unknown Drive Type %s", p);
			cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
			cbei.iItem          = index++;
			cbei.pszText        = strGen0;
			cbei.cchTextMax     = sizeof(strGen0);
			cbei.iImage         = IDI_UND_DRIVE;
			cbei.iSelectedImage = IDI_UND_DRIVE;
			SndMsg(hwndCombo, CBEM_INSERTITEM, 0, (LPARAM)&cbei);
			break;

		case DRIVE_NO_ROOT_DIR:
			strupr(p);
			sprintf(strGen0, "No Mounted Volume %s", p);
			cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
			cbei.iItem          = index++;
			cbei.pszText        = strGen0;
			cbei.cchTextMax     = sizeof(strGen0);
			cbei.iImage         = IDI_NOT_DRIVE;
			cbei.iSelectedImage = IDI_NOT_DRIVE;
			SndMsg(hwndCombo, CBEM_INSERTITEM, 0, (LPARAM)&cbei);
			break;

		case DRIVE_REMOVABLE:
			strupr(p);
			if(strchr(p, 'A') || strchr(p, 'B'))
				sprintf(strGen0, "Floppy Drive %s", p);
			else
				sprintf(strGen0, "Removable Drive %s", p);

			cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
			cbei.iItem          = index++;
			cbei.pszText        = strGen0;
			cbei.cchTextMax     = sizeof(strGen0);
			cbei.iImage         = IDI_REMOVEABLE_DRIVE;
			cbei.iSelectedImage = IDI_REMOVEABLE_DRIVE;
			SndMsg(hwndCombo, CBEM_INSERTITEM, 0, (LPARAM)&cbei);
			break;

		case DRIVE_FIXED:
			strupr(p);
			sprintf(strGen0, "Local Drive %s", p);
			cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
			cbei.iItem          = index++;
			cbei.pszText        = strGen0;
			cbei.cchTextMax     = sizeof(strGen0);
			cbei.iImage         = IDI_FIXED_DRIVE;
			cbei.iSelectedImage = IDI_FIXED_DRIVE;
			SndMsg(hwndCombo, CBEM_INSERTITEM, 0, (LPARAM)&cbei);
			sprintf(pAll, "%s ", p); // separator == space
			pAll += 4;
			break;

		case DRIVE_REMOTE:
			strupr(p);
			cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
			sprintf(strGen0, "Remote Drive %s", p);
			cbei.iItem          = index++;
			cbei.pszText        = strGen0;
			cbei.cchTextMax     = sizeof(strGen0);
			cbei.iImage         = IDI_REMOTE_DRIVE;
			cbei.iSelectedImage = IDI_REMOTE_DRIVE;
			SndMsg(hwndCombo, CBEM_INSERTITEM, 0, (LPARAM)&cbei);
			break;

		case DRIVE_CDROM:
			strupr(p);
			sprintf(strGen0, "CdRom Drive %s", p);
			cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
			cbei.iItem          = index++;
			cbei.pszText        = strGen0;
			cbei.cchTextMax     = sizeof(strGen0);
			cbei.iImage         = IDI_CD_DRIVE;
			cbei.iSelectedImage = IDI_CD_DRIVE;
			SndMsg(hwndCombo, CBEM_INSERTITEM, 0, (LPARAM)&cbei);
			break;

		case DRIVE_RAMDISK:
			strupr(p);
			sprintf(strGen0, "RAM Drive %s", p);
			cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
			cbei.iItem          = index++;
			cbei.pszText        = strGen0;
			cbei.cchTextMax     = sizeof(strGen0);
			cbei.iImage         = IDI_RAM_DRIVE;
			cbei.iSelectedImage = IDI_RAM_DRIVE;
			SndMsg(hwndCombo, CBEM_INSERTITEM, 0, (LPARAM)&cbei);
			break;
		}
		p += strlen(p);
		p++;
	}

	all[strlen(all)-1] = 0;
	cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
	cbei.iItem          = 0;
	cbei.pszText        = all;
	cbei.cchTextMax     = sizeof(all);
	cbei.iImage         = IDI_FIXED_DRIVE;
	cbei.iSelectedImage = IDI_FIXED_DRIVE;
	SndMsg(hwndCombo, CBEM_INSERTITEM, 0, (LPARAM)&cbei);
	index++;

	cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
	cbei.iItem          = index++;
	cbei.pszText        = "BROWSE";
	cbei.cchTextMax     = sizeof(strGen0);
	cbei.iImage         = IDI_FOLDER;
	cbei.iSelectedImage = IDI_FOLDER;
	SndMsg(hwndCombo, CBEM_INSERTITEM, 0, (LPARAM)&cbei);

	cbei.mask = CBEIF_TEXT;
	g_dwLastNative = index-1;

	for(int i = 0; i<6; i++)
	{
		if(g_mru[i][0])
		{
			cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
			cbei.iItem          = index++;
			cbei.pszText        = g_mru[i];
			cbei.cchTextMax     = sizeof(g_mru[i]);
			cbei.iImage         = IDI_OPENFOLDER;
			cbei.iSelectedImage = IDI_OPENFOLDER;
			SndMsg(hwndCombo, CBEM_INSERTITEM, 0, (LPARAM)&cbei);
		}
	}
	SndMsg(hwndCombo, CB_SETCURSEL, 0, 0);
	return TRUE;
}

void Rename(HWND hListView, int idx)
{
	POINT pt;
	RECT rc;
	HFONT hF = (HFONT)SndMsg(hListView, WM_GETFONT, 0, 0);
	int cx = ListView_GetColumnWidth(hListView, 0); // width of the first column

	ListView_GetItemPosition(hListView, idx, &pt);

	ListView_GetItemRect(hListView, idx, &rc, LVIR_ICON);

	int h = GetFontHeight(hListView, hF);

	if(h<15){		// when the display is set to small fonts the
		h++;		// editbox height and position needs adjustint
		pt.y += 2;
	}

	int x = rc.left + rc.right;
	HWND hw = CreateWindow("edit", fileopts.ppfi[idx]->filename,
		ES_MULTILINE | WS_CHILD | ES_WANTRETURN, x, pt.y, cx - x, h, hListView,
		(HMENU)IDE_RENAME, g_hInst, NULL);

	SetWindowLong(hw, GWL_USERDATA, idx);		// listview items index
	SndMsg(hw, WM_SETFONT, (WPARAM)hF, 0);
	SndMsg(hw, EM_SETSEL, 0, -1);
	pSubClassRenameEditBoxProc = (WNDPROC)SetWindowLong(hw, GWL_WNDPROC, (DWORD)SubClassRenameEditBoxProc);
	ShowWindow(hw, SW_SHOW);
	SetFocus(hw);
}

void GetUserParams(HWND hwnd)
{
	int idx = SendDlgItemMessage(hwnd, IDC_COMBO, CB_GETCURSEL, 0, 0);
	SendDlgItemMessage(hwnd, IDC_COMBO, CB_GETLBTEXT, idx, (long)sInitialDir);
	int c;
	char * p = sInitialDir;
	c = strlen(sInitialDir);
	// all drives
	if(c==1)
		sInitialDir[c++] = ':';

	if((c==2)||(p[1] == ':' && p[2] == '\\'))
	{
		if(p[c-1] != '\\')
		{
			sInitialDir[c++] = '\\';
		}
		sInitialDir[c++] = ';';		// was 188;
		sInitialDir[c] = 0;
	}
	else if(strstr(sInitialDir, "Local Drives"))
	{
		p = strchr(sInitialDir, ':') -2;
		idx = 0;
		while(*p++)
		{
			if(*p == ' ')		// was 32)
				sInitialDir[idx++] = ';';		// was 188;
			else if(*p != 0)
				sInitialDir[idx++] = *p;
		}
		sInitialDir[idx++] = ';';		// was 188;
		sInitialDir[idx]   = '\0';
	}
	else
	{
		c = strlen(sInitialDir) -3;
		for(idx = c; idx<strlen(sInitialDir); idx++)
		{
			*p++ = sInitialDir[idx];
		}
		*p++ = ';';		// was 188;
		*p   = '\0';
	}
	// filter text
	sFilterTxt[0] = 0;
	GetWindowText(GetCtrl(hwnd, IDE_FILTER), sFilterTxt, MAX_PATH+100);

	// case sensitive? 	file counts only?
	g_iCase       = IsDlgButtonChecked(hwnd, IDCHK_CASE);
	g_iSubFolder  = IsDlgButtonChecked(hwnd, IDCHK_SUBFOLDER);
	g_iFileCounts = IsDlgButtonChecked(hwnd, IDCHK_OCCUR);
	g_iAutoWildcards  = IsDlgButtonChecked(hwnd, IDCHK_AUTOWILDC);

	// text search
	sTxtSearch[0] = 0;
	GetWindowText(GetCtrl(hwnd, IDE_SEARCH), sTxtSearch, MAX_PATH+100);
}

void SizeControls(HWND hwnd, int dx, int dy)
{
	RECT rc, rc1;
	GetClientRect(hwnd, &rc);
	GetClientRect(GetCtrl(hwnd,IDC_STATUSBAR), &rc1);
	// do status bar
	SendDlgItemMessage(hwnd, IDC_STATUSBAR, SB_SETPARTS, 1, (LPARAM)(LPINT)&rc.right);
	SendDlgItemMessage(hwnd, IDC_STATUSBAR, WM_SIZE, LOWORD(rc.right), 0);

	bk.b = ((rc.bottom-rc1.bottom)*8)/dy - 6;

	// all controls are relative to the left back panel
	int left = ((bk.l+3)*dx)/4;
	MoveWindow(GetCtrl(hwnd, IDS_FILE), left, ((bk.t+4)*dy)/8, ((bk.r-6)*dx)/4, (10*dy)/8, TRUE);
	MoveWindow(GetCtrl(hwnd, IDE_FILTER), left, ((bk.t+12)*dy)/8, ((bk.r-6)*dx)/4, (10*dy)/8, TRUE);

	SetWindowPos(GetCtrl(hwnd, IDC_COMBO), HWND_TOP, left, ((bk.t+26)*dy)/8, ((bk.r-6)*dx)/4, (58*dy)/4, SWP_NOACTIVATE);
	MoveWindow(GetCtrl(hwnd, IDCHK_SUBFOLDER), left, ((bk.t+40)*dy)/8, ((bk.r-6)*dx)/4, (10*dy)/8, TRUE);

	// sz 040623
	MoveWindow(GetCtrl(hwnd, IDCHK_AUTOWILDC), left, ((bk.t+50)*dy)/8, ((bk.r-6)*dx)/4, (10*dy)/8, TRUE);
	#define MOVEDOWN 10+8

	MoveWindow(GetCtrl(hwnd, IDS_TSEARCH), left, ((bk.t+54+MOVEDOWN)*dy)/8, ((bk.r-6)*dx)/4, (10*dy)/8, TRUE);
	MoveWindow(GetCtrl(hwnd, IDE_SEARCH), left, ((bk.t+64+MOVEDOWN)*dy)/8, ((bk.r-6)*dx)/4, (10*dy)/8, TRUE);
	MoveWindow(GetCtrl(hwnd, IDCHK_CASE), left, ((bk.t+74+MOVEDOWN)*dy)/8, ((bk.r-6)*dx)/4, (10*dy)/8, TRUE);
	MoveWindow(GetCtrl(hwnd, IDCHK_OCCUR), left, ((bk.t+84+MOVEDOWN)*dy)/8, ((bk.r-6)*dx)/4, (10*dy)/8, TRUE);

	// all Extended controls here bk.b
	MoveWindow(GetCtrl(hwnd, IDCHK_ENABLE_EXT), left, ((bk.t+99+MOVEDOWN)*dy)/8, ((bk.r-6)*dx)/4, (10*dy)/8, TRUE);
	int depth = bk.t + bk.b - 122;
	MoveWindow(GetCtrl(hwnd, IDS_EXTBACK), left, ((bk.t+110+MOVEDOWN)*dy)/8, ((bk.r-6)*dx)/4, (44*dy)/8, TRUE);

	HWND h = GetCtrl(hwnd, IDS_EXTBACK);
	MoveWindow(GetCtrl(h, IDS_DATE_AFTER), (2*dx)/4, ((4)*dy)/8, (32*dx)/4, (8*dy)/8, TRUE);
	MoveWindow(GetCtrl(h, IDS_DATE_AFTER_TXT), (36*dx)/4, ((4)*dy)/8, (32*dx)/4, (8*dy)/8, TRUE);

	MoveWindow(GetCtrl(h, IDS_DATE_BEFORE), (2*dx)/4, ((14)*dy)/8, (32*dx)/4, (8*dy)/8, TRUE);
	MoveWindow(GetCtrl(h, IDS_DATE_BEFORE_TXT), (36*dx)/4, ((14)*dy)/8, (32*dx)/4, (8*dy)/8, TRUE);

	MoveWindow(GetCtrl(h, IDS_SIZE_MORE), (2*dx)/4, ((24)*dy)/8, (32*dx)/4, (8*dy)/8, TRUE);
	MoveWindow(GetCtrl(h, IDS_SIZE_MORE_TXT), (36*dx)/4, ((24)*dy)/8, (32*dx)/4, (8*dy)/8, TRUE);

	MoveWindow(GetCtrl(h, IDS_SIZE_LESS), (2*dx)/4, ((34)*dy)/8, (32*dx)/4, (8*dy)/8, TRUE);
	MoveWindow(GetCtrl(h, IDS_SIZE_LESS_TXT), (36*dx)/4, ((34)*dy)/8, (32*dx)/4, (8*dy)/8, TRUE);

	//=================

	depth = bk.t + bk.b - 12;
	MoveWindow(GetCtrl(hwnd, IDB_CANCEL), ((bk.r+50)*dx)/4, (depth*dy)/8, (40*dx)/4, (12*dy)/8, TRUE);
	MoveWindow(GetCtrl(hwnd, IDB_SEARCH), ((bk.r+6)*dx)/4, (depth*dy)/8, (40*dx)/4, (12*dy)/8, TRUE);

	int width  = (rc.right*4)/dx - (bk.l + bk.r + 4); // width & height in dlg units
	int width1 = (rc.right*4)/dx - (bk.l + bk.r + 72);

	MoveWindow(GetCtrl(hwnd, IDS_PICBOX),((bk.r+6)*dx)/4, ((bk.t)*dy)/8, (width*dx)/4-1, (67*dy)/8, TRUE);
	MoveWindow(GetCtrl(hwnd, IDE_HEADER), ((bk.r+73)*dx)/4, ((bk.t)*dy)/8+2, (width1*dx)/4, (67*dy)/8-4, TRUE);

	MoveWindow(GetCtrl(hwnd, IDC_LISTVIEW), ((bk.r+6)*dx)/4, ((bk.t+70)*dy)/8, (width*dx)/4, ((bk.b-84)*dy)/8, TRUE);
	SetWindowPos(GetCtrl(hwnd, IDS_BACK), 0, (bk.l*dx)/4-1, (bk.t*dy)/8, (bk.r*dx)/4, (bk.b*dy)/8, SWP_NOACTIVATE);

	InvalidateRect(GetCtrl(hwnd, IDS_PICBOX), NULL, TRUE);
	InvalidateRect(GetCtrl(hwnd, IDE_HEADER), NULL, TRUE);
}

#define GETCURSORPOS(h, x) GetCursorPos(&x); ScreenToClient(h, &x)

int MoveBar(HWND hwnd, int pos, int dx, int dy)
{
	int left, top, right, bot, curpos, limit_right;
	RECT   rc;
	POINT  pt;
	HPEN   oldP;
	HBRUSH oldB;
	HDC    hdc;

	// get the width of the window
	GetClientRect(hwnd, &rc);
	limit_right = rc.right - 100;

	// The bitmap bits
	short Bits[8] = { 0x0055, 0x00AA, 0x0055, 0x00AA,
		0x0055, 0x00AA, 0x0055, 0x00AA };

	HBITMAP hBitmap = CreateBitmap( 8, 8, 1, 1, Bits );
	HBRUSH hBrush = CreatePatternBrush( hBitmap );

	// select objects
	hdc = GetDC(hwnd);
	oldP = SelectObject(hdc, GetStockObject(NULL_PEN)); //hPen
	oldB = SelectObject(hdc, hBrush);

	int barwidth = (2*dx)/4 + 2;

	// set up bar rectangle
	left = ((bk.r + 2 + 1)*dx)/4;
	right = left + barwidth;
	top = ((bk.t+1)*dy)/8-1;
	bot = ((bk.b+3)*dy)/8+3;

	// get first mouse position
	GETCURSORPOS(hwnd, pt);
	curpos = pt.x;

	// XOR
	SetROP2(hdc, R2_XORPEN);

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(hEvent);

	// draw the first Rectangle
	Rectangle(hdc, left, top, right, bot);
	while(GetAsyncKeyState(VK_LBUTTON) && 0x8000)
	{
		MsgWaitForMultipleObjects(1, &hEvent, TRUE, 10, QS_MOUSEMOVE);
		GETCURSORPOS(hwnd, pt);
		if(curpos != pt.x)
		{

			if (pt.x < curpos)
			{		// moving left
				if(left + (pt.x - curpos) > 100)
				{
					Rectangle(hdc, left, top, right, bot);	// erase
					left -= (curpos - pt.x);
					right = left + barwidth;
					curpos = pt.x;
					Rectangle(hdc, left, top, right, bot);	// draw new position
				}
			}
			else if (pt.x > curpos)
			{ // moving right
				if(left + (pt.x - curpos) < limit_right)
				{
					Rectangle(hdc, left, top, right, bot); // erase
					left -= (curpos - pt.x);
					right = left + barwidth;
					curpos = pt.x;
					Rectangle(hdc, left, top, right, bot);	// draw new position
				}
			}
		}
	}
	// erase last rectangle
	Rectangle(hdc, left, top, right, bot);
	SelectObject(hdc, oldP);
	SelectObject(hdc, oldB);
	ReleaseDC(hwnd, hdc);

	DeleteObject(hBrush);
	DeleteObject(hBitmap);

	if(((bk.r+2)*dx)/4 != left)
	{
		bk.r = ((left)*4)/dx - 2; 	// remake bk.r
		return 1;
	}
	else
		return 0;
}

void ClearPreview(HWND hwnd)
{
	SndMsg(GetCtrl(hwnd, IDE_HEADER), WM_SETTEXT, 0, 0);
	SetWndTxt(hwnd, "FindFile");
	if(g_pIPicture)
	{
		OleReleasePic(g_pIPicture);
		g_pIPicture = NULL;
		InvalidateRect(GetCtrl(hwnd, IDS_PICBOX), NULL, TRUE);		// Paint it!
	}
}

void DisplayFileHead(HWND hwnd, char * pathname, long long filesize)
{
	FILE *f;

	if(!g_strMem)
		return;

	ClearPreview(hwnd);
	// clear previous text
	memset(g_strMem, 0, PREVIEW_MEMSIZE);
	SendDlgItemMessage(hwnd, IDE_HEADER, WM_SETTEXT, 0, (LPARAM)g_strMem);

	if(!filesize)
		return;

	f = fopen(pathname, "rb");

	if(!f)
		return;

	fread(g_strMem, PREVIEW_MEMSIZE, 1, f);
	int size;

	if (filesize<PREVIEW_MEMSIZE)
		size = filesize;
	else
		size = PREVIEW_MEMSIZE;

	for(int i = 0; i<size; i++)
	{
		if(g_strMem[i] == 0)
			g_strMem[i] = 27; // replace zero with '' (only 'Arial || Courier New')
	}
	g_strMem[size-1] = 0;

	SendDlgItemMessage(hwnd, IDE_HEADER, WM_SETTEXT, 0, (LPARAM)g_strMem);
	fclose(f);
	if(IsImageFile(pathname))
	{
		if(g_pIPicture)
			g_pIPicture->lpVtbl->Release(g_pIPicture);

		if (S_OK == OleLoadPicFile(pathname, &g_pIPicture))
		{
			// Paint it! The IPicture * g_pIPicture is global
			InvalidateRect(GetCtrl(hwnd, IDS_PICBOX), NULL, TRUE);
		}
	}
}

int InitDialog(HWND hwnd, int dx, int dy)
{
	g_strMem  = malloc(PREVIEW_MEMSIZE);			// 2000 byte buffer for displaying text (header text)

	ReadRegistry();

	// position main window
	MoveWindow(hwnd, (dlg.l*dx)/4, (dlg.t*dy)/8, (dlg.r*dx)/4, (dlg.b*dy)/8, FALSE);

	CreateSBar(hwnd, "Ready", 1);

	// check, 'one click activate' or 'two click activate' and set listview accordingly
	SHELLFLAGSTATE sfs;
	memset(&sfs, 0, sizeof(SHELLFLAGSTATE));
	SHGetSettings(&sfs, SSF_DOUBLECLICKINWEBVIEW);
	DWORD dwExStyle;
	if(sfs.fDoubleClickInWebView)
		dwExStyle = LVS_EX_UNDERLINEHOT | LVS_EX_GRIDLINES | LVS_EX_TRACKSELECT;
	else
		dwExStyle = LVS_EX_UNDERLINEHOT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE | LVS_EX_TRACKSELECT;

	SendDlgItemMessage(hwnd, IDC_LISTVIEW, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwExStyle );

	EnableWindow(GetCtrl(hwnd, IDB_CANCEL), FALSE);
	ListView_SetColumnWidths(GetCtrl(hwnd,IDC_LISTVIEW), dx, NUMCOLS, g_ColWidth);
	ListView_SetColHeadTxtAll(GetCtrl(hwnd,IDC_LISTVIEW), NUMCOLS);		// set all header texts

	InitComboBox(hwnd);

	HideCaret(GetCtrl(hwnd, IDE_SEARCH));
	SetFocus(GetCtrl(hwnd, IDE_FILTER));

	// subclass static box so that we can draw in it's WM_PAINT.
	pSubClassPictureBoxProc = (WNDPROC)SetWindowLong(GetCtrl(hwnd, IDS_PICBOX),GWL_WNDPROC, (DWORD)SubClassPictureBoxProc) ;
	pSubClassTextBoxProc = (WNDPROC)SetWindowLong(GetCtrl(hwnd, IDE_HEADER),GWL_WNDPROC, (DWORD)SubClassTextBoxProc) ;

	// font is for the main head text box
	//g_hFont = CreateFont(-16, 0, 0, 0, 400, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, FIXED_PITCH, FF_MODERN, "Courier New");
	g_hFont = CreateFont(-14, 0, 0, 0, 400, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, FIXED_PITCH, FF_MODERN, "Arial");
	SendDlgItemMessage(hwnd, IDE_HEADER, WM_SETFONT, (WPARAM)g_hFont, 0);
	SendDlgItemMessage(hwnd, IDE_HEADER, EM_SETLIMITTEXT, (WPARAM)65535, 0);
	if(g_iCase == 1)
		CheckDlgButton(hwnd, IDCHK_CASE, TRUE);
	if(g_iSubFolder == 1)
		CheckDlgButton(hwnd, IDCHK_SUBFOLDER, TRUE);
	if(g_iFileCounts == 1)
		CheckDlgButton(hwnd, IDCHK_OCCUR, TRUE);
	if(g_iAutoWildcards == 1)
		CheckDlgButton(hwnd, IDCHK_AUTOWILDC, TRUE);

	// retrieve and associate the system icon list with the listview control
	SHFILEINFO shfi;
	memset(&shfi, 0, sizeof(SHFILEINFO));
	HIMAGELIST himl = (HIMAGELIST)SHGetFileInfo ("CabView.exe", FILE_ATTRIBUTE_NORMAL,
		&shfi, sizeof (SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	if (himl)
		ListView_SetImageList(GetCtrl(hwnd, IDC_LISTVIEW), himl, LVSIL_SMALL);

	InitMenu(hwnd, g_hInst, g_strDepends);
	Butx_Style(GetCtrl(hwnd,IDB_SEARCH), BXS_XP | BXS_ICON);
	SendMessage(GetCtrl(hwnd,IDB_SEARCH), BXM_SETIMAGE, IMAGE_ICON, (long)g_hIcon);


	// parse commandline for options
	{
		int  n=strlen(g_lpCmdLine);

		if ( n>0 )
		{

			char* cpact;
			char* ftext = calloc(MAX_PATH,sizeof(char));
			char* spath = calloc(MAX_PATH,sizeof(char));
			char* texts = calloc(MAX_PATH,sizeof(char));

			//			for ( int i=0; i<MAX_PATH; i++) { ftext[i] = spath[i] = texts[i] = 0; }

			int* ipact;
			int  icsens=0;		// case sensitive search
			int  ioccur=0;		// file counts only
			int  isubfd=1;		// search subfolders
			int  ifiltf=0;		// filter edit control focus
			int  idoit=0;		// execute search with cmdline parameters
			int  iautowc=g_iAutoWildcards;		// automatic wildcard expansion

			enum { eparse, eptext, epint };
			int istate=eparse;

			char c;
			char* cp = strcpy(g_strMem, g_lpCmdLine);

			while(c=*cp)
			{
				switch(istate)
				{
				case eparse:
					if(c=='/') c=*(++cp);
					switch (c)
					{
					case 'f':
					case 'F':
						cpact  =  ftext;
						istate = eptext;
						break;
					case 'p':
					case 'P':
						cpact  =  spath;
						istate = eptext;
						break;
					case 't':
					case 'T':
						cpact  =  texts;
						istate = eptext;
						break;
					case 'c':
					case 'C':
						ipact  = &icsens;
						istate = epint;
						break;
					case 'o':
					case 'O':
						ipact  = &ioccur;
						istate = epint;
						break;
					case 's':
					case 'S':
						ipact  = &isubfd;
						istate = epint;
						break;
					case 'd':
					case 'D':
						ipact  = &idoit;
						istate = epint;
						break;
					case 'e':
					case 'E':
						ipact  = &ifiltf;
						istate = epint;
						break;
					case 'w':
					case 'W':
						ipact  = &iautowc;
						istate = epint;
						break;
					}
					cp++;
					n=0;
					break;

				case eptext:
					if ( c=='/')
					{
						istate = eparse;
					}
					else
					{
						cpact[n++]=c;
						cpact[n]=0;
						cp++;
					}
					break;

				case epint:
					switch(c)
					{
					case '/':
						istate = eparse;
						cp--;
						break;
					case '+':
						*ipact = 1;
						break;
					case '-':
						*ipact = 0;
					}
					cp++;
				} //switch (istate)
			} //while


			//feed dialogs with cmdline values

			//search path
			if (0<strlen(spath))
			{
				HWND hwndCombo = GetCtrl(hwnd, IDC_COMBO);
				DWORD dwIndex = 0;

				COMBOBOXEXITEM cbei;
				memset(&cbei, 0, sizeof(COMBOBOXEXITEM));
				dwIndex = SndMsg(hwndCombo, CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
				cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
				cbei.iItem          = dwIndex;
				cbei.pszText        = spath;
				cbei.cchTextMax     = sizeof(spath);
				cbei.iImage         = IDI_OPENFOLDER;
				cbei.iSelectedImage = IDI_OPENFOLDER;
				dwIndex = SndMsg(hwndCombo, CBEM_INSERTITEM, (WPARAM)dwIndex, (LPARAM)&cbei);
				SndMsg(hwndCombo, CB_SETCURSEL, (WPARAM)dwIndex, (LPARAM)0);
			}

			// filter text
			if (0<strlen(ftext))
				SetWindowText(GetCtrl(hwnd, IDE_FILTER), ftext);

			// text search
			if (0<strlen(texts))
				SetWindowText(GetCtrl(hwnd, IDE_SEARCH), texts);

			//case sensitive, search subfolders, check occurrences
			CheckDlgButton(hwnd, IDCHK_SUBFOLDER, 	(isubfd==1) ? TRUE  : FALSE );
			CheckDlgButton(hwnd, IDCHK_AUTOWILDC, 	(iautowc==1) ? TRUE  : FALSE );
			CheckDlgButton(hwnd, IDCHK_CASE, 		(icsens==1) ? TRUE  : FALSE );
			CheckDlgButton(hwnd, IDCHK_OCCUR, 		(ioccur==1) ? TRUE  : FALSE );

			//which edit control receives the focus ?
			if (ifiltf==1)
			{
				HideCaret(GetCtrl(hwnd, IDE_SEARCH));
				SetFocus(GetCtrl(hwnd, IDE_FILTER));
			}

			if (ifiltf==0)
			{
				HideCaret(GetCtrl(hwnd, IDE_FILTER));
				SetFocus(GetCtrl(hwnd, IDE_SEARCH));
			}

			free(spath);
			free(ftext);
			free(texts);

			//start the search
			if ( idoit == 1 )
				SndMsg(hwnd, WM_COMMAND, (WPARAM)IDB_SEARCH, (LPARAM)0 );
		}//if CmdLine exists
	}// parse commandline for options

	ShowWindow(hwnd, dlg.f);
	return FALSE;
}

void SortMru(char * stradd)
{
	int i = 0;
	char s[MAX_PATH+100];
	HWND hwndCombo = GetCtrl(g_hwndMain, IDC_COMBO);
	DWORD dwIndex = g_dwLastNative + 1;

	// first delete same string from combobox if exists
	while( i<5 && CB_ERR != SndMsg(hwndCombo, CB_GETLBTEXT, dwIndex, (LPARAM)s))
	{
		if(!strcmp(stradd, s))
			SndMsg(hwndCombo, CBEM_DELETEITEM, dwIndex, 0);

		dwIndex++; i++;
	}

	// if more than 6 entries delete the last one
	if(i == 6)
	{
		SndMsg(hwndCombo, CBEM_DELETEITEM, dwIndex, 0);
	}

	// insert the string just after "BROWSE"
	COMBOBOXEXITEM cbei;
	memset(&cbei, 0, sizeof(COMBOBOXEXITEM));

	cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
	cbei.iItem          = g_dwLastNative + 1;
	cbei.pszText        = stradd;
	cbei.cchTextMax     = sizeof(stradd);
	cbei.iImage         = 7;
	cbei.iSelectedImage = 7;
	dwIndex = SndMsg(hwndCombo, CBEM_INSERTITEM, 0, (LPARAM)&cbei);

	SndMsg(hwndCombo, CB_SETCURSEL, dwIndex, 0);

	// Fill g_mru strings with current combo strings after "BROWSE"
	dwIndex = g_dwLastNative + 1; i = 0;
	while( CB_ERR != SndMsg(hwndCombo, CB_GETLBTEXT, dwIndex++, (LPARAM)g_mru[i]))
	{
		i++;
		if(i>5)
			break;
	}

}

void DisplayContextMenu(HWND hwnd, POINT * pt, int idx)
{
	MENUITEMINFO mii;
	int i;
	HMENU hPopup	 = CreatePopupMenu();
	HMENU hSubPopup  = CreatePopupMenu();
	AppendMenu(hPopup, MF_STRING, IDM_FOLDER, "Explore Folder");

	char pathname[MAX_PATH+100];

	LVITEM lvi;
	lvi.iSubItem = 0;
	lvi.pszText    = pathname;
	lvi.cchTextMax = MAX_PATH+100;

	SndMsg(GetCtrl(hwnd, IDC_LISTVIEW), LVM_GETITEMTEXT, idx, (long)&lvi);

	g_activateDepends = 0;
	if(IsModuleFile(pathname))
	{
		g_activateDepends = 1;
	}

	AppendMenu(hPopup, MF_STRING, IDM_DEPENDS, "View Dependencies");
	AppendMenu(hPopup, MF_STRING, IDM_OPEN, "Open");
	AppendMenu(hPopup, MF_SEPARATOR, 0, NULL);

	// IDM_DEPENDS & IDM_VSEARCH are drawn by the system
	if(fileopts.txtsearch[0] && fileopts.count)
		AppendMenu(hPopup, MF_STRING, IDM_VSEARCH, "View Search Results");
	else
		AppendMenu(hPopup, MF_STRING | MF_GRAYED, IDM_VSEARCH, "Text Search Results");

	AppendMenu(hPopup, MF_SEPARATOR, 0, NULL);
	AppendMenu(hPopup, MF_STRING, IDM_RENAME, "Rename");
	AppendMenu(hPopup, MF_STRING, IDM_COPYTO, "CopyTo");
	AppendMenu(hPopup, MF_STRING, IDM_MOVETO, "MoveTo");
	AppendMenu(hPopup, MF_SEPARATOR, 0, NULL);
	AppendMenu(hPopup, MF_STRING | MF_POPUP, (UINT)hSubPopup, "Send To");

	if(g_strDisp[0][0] == 0)
		AppendMenu(hSubPopup, MF_STRING | MF_GRAYED, IDM_SENDTOFIRST, "(none)");
	else
	{
		for(i = 0; i<6; i++)
		{
			if(g_strDisp[i][0])
				AppendMenu(hSubPopup, MF_STRING, IDM_SENDTOFIRST + i, g_strDisp[i]);
		}
	}
	AppendMenu(hPopup, MF_SEPARATOR, 0, NULL);
	AppendMenu(hPopup, MF_STRING, IDM_DELETE, "Delete");
	AppendMenu(hPopup, MF_STRING, IDM_RECYCLE, "Recycle Bin");
	AppendMenu(hPopup, MF_SEPARATOR, 0, NULL);
	AppendMenu(hPopup, MF_STRING, IDM_PROPERTIES, "Properties");

	mii.cbSize   = sizeof(MENUITEMINFO);
	mii.fMask    = MIIM_FTYPE | MIIM_DATA;
	mii.fType    = MFT_OWNERDRAW;
	mii.dwItemData = ENABLED;

	SetMenuItemInfo(hPopup, IDM_FOLDER, FALSE, &mii);

	if(g_strDepends != NULL && !g_activateDepends)
	{
		mii.dwItemData = DISABLED;
	}
	SetMenuItemInfo(hPopup, IDM_DEPENDS, FALSE, &mii);

	mii.dwItemData = ENABLED;
	SetMenuItemInfo(hPopup, IDM_OPEN, FALSE, &mii);
	SetMenuItemInfo(hPopup, IDM_RENAME, FALSE, &mii);
	SetMenuItemInfo(hPopup, IDM_COPYTO, FALSE, &mii);
	SetMenuItemInfo(hPopup, IDM_MOVETO, FALSE, &mii);
	SetMenuItemInfo(hPopup, IDM_DELETE, FALSE, &mii);
	SetMenuItemInfo(hPopup, IDM_RECYCLE, FALSE, &mii);
	SetMenuItemInfo(hPopup, IDM_PROPERTIES, FALSE, &mii);

	SetForegroundWindow(hwnd);
	TrackPopupMenu(hPopup, TPM_RIGHTBUTTON, pt->x, pt->y, 0, hwnd, NULL);
	PostMessage(hwnd, WM_NULL, 0, 0);
	DestroyMenu(hPopup);
}

//=================================================================
//
// GetListViewItems extracts the selected paths and names from the
// listview, allocates memory and returns two buffers if 'action' is
// TWO_BUFFER. 'ToDir' is only used for TWO_BUFFER.
//
//=================================================================
char ** GetListViewItems(HWND hListView, char * ToDir, BOOL action)
{
	char tmp[MAX_PATH+100];		// temp buffer
	LVITEM lvi;
	static char * buf[2];
	buf[0] = NULL;
	buf[1] = NULL;
	int lenFrom, index;

	lenFrom = 0, index = -1;

	lvi.pszText    = tmp;
	lvi.cchTextMax = MAX_PATH+100;

	// get total lenFrom & lenTO of buffers required
	while ((-1) != (index = ListView_GetNextItem(hListView, index, LVNI_SELECTED)))
	{
		// pathname
		lvi.iSubItem = 1;
		lenFrom += SndMsg(hListView, LVM_GETITEMTEXT, index, (long)&lvi);
		// filename
		lvi.iSubItem = 0;
		lenFrom += SndMsg(hListView, LVM_GETITEMTEXT, index, (long)&lvi);
		lenFrom++; 			// inc for null termination of each string
	}
	buf[0] = calloc(1, lenFrom+2); // double null termination at end of buffer
	if(!buf[0]) return buf;

	index = -1;

	char * p0 = buf[0];

	// get all strings into bufffer
	while ((-1) != (index = ListView_GetNextItem(hListView, index, LVNI_SELECTED)))
	{

		// pathname
		lvi.iSubItem = 1;
		SndMsg(hListView, LVM_GETITEMTEXT, index, (long)&lvi);
		if(fileopts.txtsearch[0])   // text search was done
			RemoveFileCountBraces(tmp);
		strcat(p0, tmp);			// dir into FROM

		// filename
		lvi.iSubItem = 0;
		SndMsg(hListView, LVM_GETITEMTEXT, index, (long)&lvi);

		// file into FROM (TO does not need the filename)
		strcat(p0, tmp);

		// move pointer on to next string
		p0 += strlen(p0) + 1;
	}

	// for the shell copy/move only one destination is required
	if(action == TWO_BUFFER)
	{
		buf[1] = calloc(1, strlen(ToDir)+2);   // double null termination at end of buffer
		if(!buf[1])
			return buf;
		strcpy(buf[1], ToDir);			// dir into TO
	}
	return buf;
}

void DeleteListViewEntres(HWND hListView)
{
	char s[MAX_PATH+100];

	int index = -1;
	ClearPreview(GetParent(hListView));
	while ((-1) != (index = ListView_GetNextItem(hListView, index, LVNI_SELECTED)))
	{

		strcpy(s, fileopts.ppfi[index]->pathname);
		strcat(s, fileopts.ppfi[index]->filename);
		if(!fexists(s))
		{
			// shuffle all items (FINDFILE_OPTIONS struct) up one place
			for(int i = index; i<fileopts.count; i++)
			{
				fileopts.ppfi[i] = fileopts.ppfi[i+1];
			}
			fileopts.count--;
			ListView_DeleteItem(hListView, index);
			index = -1;
		}
	}
	sprintf(s, "%u File(s)", fileopts.count);
	SendDlgItemMessage(GetParent(hListView), IDC_STATUSBAR, SB_SETTEXT, 0, (LPARAM)s);
}

int OnNotify(HWND hwnd, LPARAM lParam)
{
	LPNM_LISTVIEW lpnmlv;
	lpnmlv = (LPNM_LISTVIEW) lParam;
	int idx;

	switch(lpnmlv->hdr.code)
	{
	case LVN_KEYDOWN:
		{
			NMLVKEYDOWN * lvkd = (NMLVKEYDOWN *)lParam;
			// delete plus shift keys (DELETE)
			if(lvkd->wVKey == VK_DELETE && (GetAsyncKeyState(VK_SHIFT) & 0x8000))
				PostMessage(hwnd, WM_COMMAND, MAKELONG(IDM_DELETE, 0), 0);
			// delete key only (RECYCLE)
			else if(lvkd->wVKey == VK_DELETE)
				PostMessage(hwnd, WM_COMMAND, MAKELONG(IDM_RECYCLE, 0), 0);
			return 0;
		}

	case LVN_COLUMNCLICK:
		if(fileopts.count < 2 || lpnmlv->iItem != -1 ) // || lpnmlv->iSubItem == 1)
			return 0;
		SortItems(GetCtrl(hwnd, IDC_LISTVIEW), &fileopts, lpnmlv->iSubItem);
		ListView_RedrawItems(GetCtrl(hwnd, IDC_LISTVIEW), 0, fileopts.count);
		return 0;

	case LVN_BEGINDRAG:
	case LVN_BEGINRDRAG:
		//		DragMoveImage(hwnd, GetCtrl(hwnd, IDC_LISTVIEW));
		if(DROPEFFECT_MOVE == HandleDragDrop(hwnd, GetCtrl(hwnd, IDC_LISTVIEW)))
			DeleteListViewEntres(GetCtrl(hwnd, IDC_LISTVIEW));
		return 0;

	case NM_RCLICK:
		{
			POINT point;
			GetCursorPos(&point);
			idx = ListView_GetSelectionMark(GetCtrl(hwnd, IDC_LISTVIEW));
			if(-1 != idx)
				DisplayContextMenu(hwnd, &point, idx);
		}
		return 0;

	case LVN_ITEMCHANGED:
		if(lpnmlv->uChanged == LVIF_STATE && lpnmlv->uNewState == 3)
		{
			pfi = fileopts.ppfi[lpnmlv->iItem];
			sprintf(strGen0, "%d", lpnmlv->iItem);
			strcpy(strGen0, pfi->pathname);
			strcat(strGen0, pfi->filename);
			DisplayFileHead(hwnd, strGen0, (long long)MAKELONGLONG(pfi->nFileSizeLow, pfi->nFileSizeHigh));
			strcpy(strGen0, "FindFile -> ");
			strcat(strGen0, pfi->pathname);
			strcat(strGen0, pfi->filename);
			SetWndTxt(hwnd, strGen0);
		}
		return 0;

	case LVN_ITEMACTIVATE:
		{
			int idx = ListView_GetSelectionMark(GetCtrl(hwnd,IDC_LISTVIEW));
			if(idx > -1 && idx < fileopts.count)
			{
				pfi = fileopts.ppfi[idx];
				ListView_GetItemText(GetCtrl(hwnd,IDC_LISTVIEW), idx, 0, strGen0, MAX_PATH+100);
				ShellExecute(hwnd, NULL, strGen0, NULL, pfi->pathname, SW_SHOW);
			}
			return 0;
		}

	case LVN_GETDISPINFO:
		{
			long long llsize;
			LV_DISPINFO *lpdi = (LV_DISPINFO*)lParam;

			EnterCriticalSection(&g_CriticalSection);

			pfi = fileopts.ppfi[lpdi->item.iItem];

			if(lpdi->item.iSubItem)
			{
				if(lpdi->item.mask & LVIF_TEXT)
				{
					switch(lpdi->item.iSubItem)
					{
					case 1: // column 1 (Folders)
						if(pfi->numsearch && fileopts.iFileCounts != 1)
						{
							sprintf(strGen0, "[%d] %s", pfi->numsearch, pfi->pathname);
							strcpy(lpdi->item.pszText, strGen0);
						}
						else
						{
							strcpy(lpdi->item.pszText, pfi->pathname);
						}
						break;

					case 2: // column 2 (Sizes)
						llsize = MAKELONGLONG(pfi->nFileSizeLow, pfi->nFileSizeHigh);
						//_ultoa(llsize, strGen0, 10);
						sprintf(strGen0, "%lld", llsize);
						strcpy(lpdi->item.pszText, strGen0);
						break;

					case 3: // column 3 (Dates)
						FileTimeToLocalFileTime(&pfi->ftLastWriteTime, &real);
						FileTimeToSystemTime(&real, &st);
						GetDateFormat(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &st,	NULL, strGen0, MAX_PATH +100);
						sprintf(strGen0 + strlen(strGen0), " %.2d:%.2d", st.wHour, st.wMinute);
						//sprintf(strGen0, "%.2d\\%.2d\\%d", st.wDay, st.wMonth, st.wYear);
						strcpy(lpdi->item.pszText, strGen0);
						break;
					}
				}
			}
			else
			{ // column 0
				if(lpdi->item.mask & LVIF_TEXT)
				{
					strcpy(lpdi->item.pszText, pfi->filename);
				}
				if(lpdi->item.mask & LVIF_IMAGE)
				{
					// stop the system "the disk is write protected" for floppy's
					DWORD tmp = SetErrorMode(SEM_FAILCRITICALERRORS);
					SHFILEINFO     sfi;
					strcpy(strGen0, pfi->pathname);
					strcat(strGen0, pfi->filename);
					if(SHGetFileInfo(strGen0, 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX|SHGFI_ICON))
					{
						lpdi->item.iImage = sfi.iIcon;
					}
					else
					{
						lpdi->item.iImage = 4; // give default icon if not given
					}
					SetErrorMode(tmp);
				}
			}
			LeaveCriticalSection(&g_CriticalSection);
		}

		SetWindowLong(hwnd, DWL_MSGRESULT, TRUE);
		return 0;
	} // end switch
	return 0;
}

int OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	int idx;
	char ** pp;

	// The SENDTO menu is dealt with here, not in the switch/case block
	if(LOWORD(wParam) >= IDM_SENDTOFIRST  && LOWORD(wParam) < IDM_SENDTO)
	{
		// find the last character of the g_strSendto string
		idx = strlen(g_strSendto[LOWORD(wParam)-IDM_SENDTOFIRST])-1;
		char c = g_strSendto[LOWORD(wParam)-IDM_SENDTOFIRST][idx];
		if(c == '\\')
		{
			// must be a folder
			strcpy(strGen2, g_strSendto[LOWORD(wParam)-IDM_SENDTOFIRST]);
			pp = GetListViewItems(GetCtrl(hwnd, IDC_LISTVIEW), strGen2, TWO_BUFFER);
			if(pp[0] && pp[1])
			{
				SHCopyTo(pp);
				free(pp[0]);
				free(pp[1]);
			}
			//			DoCopyFiles(hwnd, pp[0], g_strSendto[LOWORD(wParam)-IDM_SENDTOFIRST]);
		}
		else
		{
			// must be a file(s)
			pp = GetListViewItems(GetCtrl(hwnd, IDC_LISTVIEW), NULL, ONE_BUFFER);
			if(pp[0])
			{
				DoShellExecuteEx(hwnd, pp[0], g_strSendto[LOWORD(wParam)-IDM_SENDTOFIRST]);
				free(pp[0]);
			}
		}

		return 0;
	}

	switch (LOWORD(wParam))
	{
	case IDCHK_ENABLE_EXT:
		// messages sent by this app
		if(lParam == 1)
		{
			ext.fExtEnabled = TRUE;
			AnimateWindow(GetCtrl(hwnd, IDS_EXTBACK), 400, AW_VER_POSITIVE | AW_SLIDE);
			CheckDlgButton(hwnd, IDCHK_ENABLE_EXT, BST_CHECKED);
			return 0;
		}
		else if(lParam == 0)
		{
			ext.fExtEnabled = FALSE;
			AnimateWindow(GetCtrl(hwnd, IDS_EXTBACK), 400,  AW_HIDE | AW_VER_NEGATIVE | AW_SLIDE);
			CheckDlgButton(hwnd, IDCHK_ENABLE_EXT, BST_UNCHECKED);

			// messages sent by the system (user clicked on check button)
		}
		else if((HWND)lParam == GetCtrl(hwnd, IDCHK_ENABLE_EXT))
		{
			if(IsWindowVisible(GetCtrl(hwnd, IDS_EXTBACK)))
			{
				// hide it, disabled
				ext.fExtEnabled = FALSE;
				AnimateWindow(GetCtrl(hwnd, IDS_EXTBACK), 400, AW_HIDE | AW_VER_NEGATIVE | AW_SLIDE);
				CheckDlgButton(hwnd, IDCHK_ENABLE_EXT, BST_UNCHECKED);
			}
			else
			{
				// show it, enabled
				ext.fExtEnabled = TRUE;
				AnimateWindow(GetCtrl(hwnd, IDS_EXTBACK), 400, AW_VER_POSITIVE | AW_SLIDE);
				CheckDlgButton(hwnd, IDCHK_ENABLE_EXT, BST_CHECKED);
			}
		}
		return 0;

	case IDM_VSEARCH:
		idx = ListView_GetSelectionMark(GetCtrl(hwnd, IDC_LISTVIEW));
		ViewSearchResults(&fileopts, GetCtrl(hwnd, IDE_HEADER), idx);
		return 0;

	case IDM_ABOUT:
		//ListView_Scroll(GetCtrl(hwnd, IDC_LISTVIEW),0,100);
		EnableWindow(hwnd, FALSE);
		DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDDIALOG_ABOUT), hwnd,
			(DLGPROC)AboutDialogProc, (LPARAM)0);
		EnableWindow(hwnd, TRUE);
		break;

	case IDM_EXTENDEDSEARCH:
		DoExtendedParams(hwnd, &ext);
		return 0;

	case IDM_EDITSENDTO:
		DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDDIALOG_SENDTO), hwnd,
			(DLGPROC)DisplaySendToDialogProc, (LPARAM)g_strSendto);
		break;

	case IDM_COPYTO:
		if(SHBrowseDir("Choose a Folder to Copy To", strGen2))
		{
			pp = GetListViewItems(GetCtrl(hwnd, IDC_LISTVIEW), strGen2, TWO_BUFFER);
			if(pp[0] && pp[1])
			{
				SHCopyTo(pp);
				free(pp[0]);
				free(pp[1]);
			}
		}
		return 0;

	case IDM_MOVETO:
		if(SHBrowseDir("Choose a Folder to Move To", strGen2))
		{
			strcpy(strGen1, "Are you sure you want to Move these item(s)?");
			if(IDB_CONFIRMYES == MsgBoxYesNo(hwnd, strGen1))
			{
				pp = GetListViewItems(GetCtrl(hwnd, IDC_LISTVIEW), strGen2, TWO_BUFFER);
				if(pp[0] && pp[1])
				{
					SHMoveTo(pp);
					//MsgBox(hwnd, "Not all files could be moved!");
					DeleteListViewEntres(GetCtrl(hwnd, IDC_LISTVIEW));
					free(pp[0]);
					free(pp[1]);
				}
			}
		}
		return 0;

	case IDM_RENAME:
		idx = ListView_GetSelectionMark(GetCtrl(hwnd, IDC_LISTVIEW));
		if(idx > -1 && idx < fileopts.count)
		{
			Rename(GetCtrl(hwnd, IDC_LISTVIEW), idx);
		}
		return 0;

	case IDM_PROPERTIES:
		idx = ListView_GetSelectionMark(GetCtrl(hwnd,IDC_LISTVIEW));
		if(idx > -1 && idx < fileopts.count)
		{
			pfi = fileopts.ppfi[idx];
			ListView_GetItemText(GetCtrl(hwnd,IDC_LISTVIEW), idx, 0, strGen1, MAX_PATH +100);
			sprintf(strGen0, "%s%s", pfi->pathname, strGen1);
			SHProperties(strGen0);
		}
		return 0;

	case IDM_DELETE:
		idx = ListView_GetSelectionMark(GetCtrl(hwnd,IDC_LISTVIEW));
		if(idx > -1 && idx < fileopts.count)
		{
			strcpy(strGen0, "Are you sure you want to Delete these item(s)?");
			if(IDB_CONFIRMYES == DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDDIALOG_CONFIRM),
					hwnd, (DLGPROC) ConfirmDialogProc, (LPARAM)strGen0))
			{
				pp = GetListViewItems(GetCtrl(hwnd, IDC_LISTVIEW), strGen2, ONE_BUFFER);
				if(pp[0])
				{
					if(SHDelete(GetCtrl(hwnd, IDC_LISTVIEW), &fileopts) == 0)
						MsgBox(hwnd, "Not all of the files could be deleted!");
					free(pp[0]);
					ClearPreview(hwnd);
				}
			}
		}
		return 0;

	case IDM_RECYCLE:
		idx = ListView_GetSelectionMark(GetCtrl(hwnd,IDC_LISTVIEW));
		if(idx > -1 && idx < fileopts.count)
		{
			pp = GetListViewItems(GetCtrl(hwnd, IDC_LISTVIEW), NULL, ONE_BUFFER);
			if(pp[0])
			{
				strcpy(strGen0, "Are you sure you want to move these item(s) to the RecycleBin?");
				if(IDB_CONFIRMYES == DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDDIALOG_CONFIRM),
						hwnd, (DLGPROC) ConfirmDialogProc, (LPARAM)strGen0))
				{

					if(SHRecycleBin(pp))
					{
						MsgBox(hwnd, "Not all files could be sent to the ReCycleBin!");
					}
					DeleteListViewEntres(GetCtrl(hwnd, IDC_LISTVIEW));
				}
				free(pp[0]);
			}
		}
		return 0;

	case IDE_HEADER:							// hide this edit's caret
		if (HIWORD(wParam) == EN_SETFOCUS)
		{
			HideCaret((HWND)lParam);
		}
		return 0;

	case IDC_COMBO:
		{
			static int iLastCombo = 0;
			// if "BROWSE" is selected it becomes the entry, if cancel is clicked
			// replace "BROWSE" with last entry (sLastCombo), else if ok is clicked
			// put new string into selection (str) and copy to (sLastCombo).
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				idx = SendDlgItemMessage(hwnd, IDC_COMBO, CB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hwnd, IDC_COMBO, CB_GETLBTEXT, idx, (long)sInitialDir);
				if(!strcmp(sInitialDir, "BROWSE"))
				{
					if(SHBrowseDir("Choose a Folder to Search", strGen0))
					{
						SortMru(strGen0);
					}
					else
					{
						SndMsg(GetCtrl(hwnd, IDC_COMBO), CB_SETCURSEL, iLastCombo, 0);
					}
				}
				else
				{
					iLastCombo = idx;
				}
			}
			return 0;
		}

	case IDM_FOLDER:
		{
			int idx = ListView_GetSelectionMark(GetCtrl(hwnd,IDC_LISTVIEW));
			if(idx > -1 && idx < fileopts.count)
			{
				pfi = fileopts.ppfi[idx];
				ShellExecute(hwnd, "explore", pfi->pathname, NULL, NULL, SW_SHOWNORMAL);
			}
			return 0;
		}

	case IDM_DEPENDS:
		{
			if(g_strDepends == NULL || !g_activateDepends)
				return 0;

			idx = ListView_GetSelectionMark(GetCtrl(hwnd,IDC_LISTVIEW));
			if(idx > -1 && idx < fileopts.count)
			{
				memset(strGen2, 0, MAX_PATH+100);
				ListView_GetItemText(GetCtrl(hwnd,IDC_LISTVIEW), idx, 0, strGen0, MAX_PATH +100);
				ListView_GetItemText(GetCtrl(hwnd,IDC_LISTVIEW), idx, 1, strGen1, MAX_PATH +100);
				sprintf(strGen2, "%s%s", strGen1, strGen0);
				DoShellExecuteEx(hwnd, strGen2, g_strDepends);
			}
			return 0;
		}

	case IDM_OPEN:
		{
			int idx = ListView_GetSelectionMark(GetCtrl(hwnd,IDC_LISTVIEW));
			if(idx > -1 && idx < fileopts.count)
			{
				pfi = fileopts.ppfi[idx];
				ListView_GetItemText(GetCtrl(hwnd,IDC_LISTVIEW), idx, 0, strGen0, MAX_PATH +100);
				ShellExecute(hwnd, NULL, strGen0, NULL, pfi->pathname, SW_SHOW);
			}
			return 0;
		}

	case IDB_SEARCH:
		GetUserParams(hwnd);
		FreeFindFile();							// free previous run
		ClearPreview(hwnd);
		ListView_DeleteAllItems(GetCtrl(hwnd,IDC_LISTVIEW));
		memset(&fileopts, 0, sizeof(FINDFILE_OPTIONS));	// clear the struct (important)
		fileopts.subdirs    = TRUE;   					// search subdirectories
		fileopts.initialdir = sInitialDir;   			// initial directory to search
		fileopts.filter     = sFilterTxt;
		fileopts.txtsearch  = sTxtSearch;
		fileopts.iCase      = g_iCase;
		fileopts.iSubFolder = g_iSubFolder;
		fileopts.iFileCounts = g_iFileCounts;
		fileopts.CallBackProc = CallBackProc;				// callback proc
		memset(&fileopts.ext, 0, sizeof(EXTENDED));
		memcpy(&fileopts.ext, &ext, sizeof(EXTENDED));
		EnableWindow(GetCtrl(hwnd, IDB_CANCEL), TRUE);
		EnableWindow(GetCtrl(hwnd, IDB_SEARCH), FALSE);
		ResetColumnBitmaps();
		SetColumnBitmap(GetCtrl(hwnd, IDC_LISTVIEW));
		g_dwStart = GetTickCount();
		FindFile(&fileopts);
		return 0;

	case IDB_CANCEL:
		FindFileStop(&fileopts);
		return 0;

	case IDM_EXIT:
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		return 0;
	}
	return 0;
}

LRESULT CALLBACK SubClassExtPaneProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, RGB(234, 232, 227));
		return (BOOL)g_hBrLite;

	default:
		return CallWindowProc(pSubClassExtPaneProc, hwnd, message, wParam, lParam) ;
	}
}

static BOOL CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static PAINTSTRUCT ps;
	static RECT rc;
	static HPEN hPn;
	static BOOL bDoCapture  = FALSE;	// used when drag-ing the separator bar
	static BOOL bInPosition = FALSE;	// ------ditto------
	static int dx, dy;		// dialog base units

	switch (msg)
	{
	case WM_INITMENU:
		return 1;

	case WM_MENUSELECT:
		return 1;

	case WM_MEASUREITEM:
		return MenuMeasureItem(hwnd, wParam ,lParam);

	case WM_DRAWITEM:
		return MenuItemDraw(hwnd, msg, wParam, lParam);

	case WM_INITDIALOG:
		// init this struct for first time
		memset(&ext, 0, sizeof(EXTENDED));
		ext.after[0] = -1; 	ext.after[1] = -1;	ext.after[2] = -1;
		ext.before[0] = -1; ext.before[1] = -1;	ext.before[2] = -1;

		CreateWindow("ComboBoxEx32", NULL, WS_BORDER | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
			10,	10,	155, 540, hwnd,	(HMENU)IDC_COMBO, g_hInst,	NULL);

		g_hBrLite  = CreateSolidBrush(RGB(234, 232, 227));

		HFONT hF = (HFONT)SndMsg(GetCtrl(hwnd, IDS_EXTBACK), WM_GETFONT, 0, 0);

		// create the extended static controls
		HWND h = CreateWindow("static", "Date After", WS_VISIBLE | WS_CHILD, 10, 10, 80, 20, GetCtrl(hwnd, IDS_EXTBACK),(HMENU)IDS_DATE_AFTER, g_hInst, NULL);
		SndMsg(h, WM_SETFONT, (LPARAM)hF, 0);
		h = CreateWindow("static", "", WS_VISIBLE | WS_CHILD, 90, 10, 80, 20, GetCtrl(hwnd, IDS_EXTBACK),(HMENU)IDS_DATE_AFTER_TXT, g_hInst, NULL);
		SndMsg(h, WM_SETFONT, (LPARAM)hF, 0);

		h = CreateWindow("static", "Date Before", WS_VISIBLE | WS_CHILD, 10, 40, 80, 20, GetCtrl(hwnd, IDS_EXTBACK),(HMENU)IDS_DATE_BEFORE, g_hInst,NULL);
		SndMsg(h, WM_SETFONT, (LPARAM)hF, 0);
		h = CreateWindow("static", "", WS_VISIBLE | WS_CHILD, 90, 40, 80, 20, GetCtrl(hwnd, IDS_EXTBACK),(HMENU)IDS_DATE_BEFORE_TXT, g_hInst,NULL);
		SndMsg(h, WM_SETFONT, (LPARAM)hF, 0);

		h = CreateWindow("static", "Size Greater", WS_VISIBLE | WS_CHILD, 10, 70, 80, 20, GetCtrl(hwnd, IDS_EXTBACK), (HMENU)IDS_SIZE_MORE, g_hInst, NULL);
		SndMsg(h, WM_SETFONT, (LPARAM)hF, 0);
		h = CreateWindow("static", "", WS_VISIBLE | WS_CHILD, 90, 70, 80, 20, GetCtrl(hwnd, IDS_EXTBACK), (HMENU)IDS_SIZE_MORE_TXT, g_hInst, NULL);
		SndMsg(h, WM_SETFONT, (LPARAM)hF, 0);

		h = CreateWindow("static", "Size Less", WS_VISIBLE | WS_CHILD, 10, 100, 80, 20, GetCtrl(hwnd, IDS_EXTBACK),(HMENU)IDS_SIZE_LESS, g_hInst, NULL);
		SndMsg(h, WM_SETFONT, (LPARAM)hF, 0);
		h = CreateWindow("static", "", WS_VISIBLE | WS_CHILD, 90, 100, 80, 20, GetCtrl(hwnd, IDS_EXTBACK),(HMENU)IDS_SIZE_LESS_TXT, g_hInst, NULL);
		SndMsg(h, WM_SETFONT, (LPARAM)hF, 0);
		CheckDlgButton(hwnd, IDCHK_ENABLE_EXT, BST_CHECKED);
		ShowWindow(GetCtrl(hwnd, IDS_EXTBACK), SW_HIDE);
		ShowWindow(GetCtrl(hwnd, IDCHK_ENABLE_EXT), SW_HIDE);
		// get dialog units for x & y
		dx = LOWORD(GetDialogBaseUnits());
		dy = HIWORD(GetDialogBaseUnits());

		g_hwndMain = hwnd;
		LoadBitmaps(g_hInst);
		InitializeCriticalSection(&g_CriticalSection);
		pSubClassExtPaneProc = (WNDPROC)SetWindowLong(GetCtrl(hwnd, IDS_EXTBACK), GWL_WNDPROC, (DWORD)SubClassExtPaneProc) ;
		return InitDialog(hwnd, dx, dy);

	case WM_SYSCOLORCHANGE:
		DestroyBitmaps();
		LoadBitmaps(g_hInst);
		SetColumnBitmap(GetCtrl(hwnd, IDC_LISTVIEW));
		return 0;

	case FF_FINISHED:
		SendDlgItemMessage(hwnd, IDC_STATUSBAR, SB_SETTEXT, 0, lParam);
		SetFocus(GetCtrl(hwnd, IDC_LISTVIEW));
		return 0;

	case WM_SIZE:
		SizeControls(hwnd, dx, dy);
		return 0;

	case WM_PAINT:
		BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);
		hPn = SelectObject(ps.hdc, g_hShadowPen);
		MoveToEx(ps.hdc, rc.left, 0, NULL);
		LineTo(ps.hdc, rc.right, 0);
		SelectObject(ps.hdc, g_hLightPen);
		MoveToEx(ps.hdc, rc.left, 1, NULL);
		LineTo(ps.hdc, rc.right+1, 1);
		SelectObject(ps.hdc, hPn);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_GETMINMAXINFO:
		{
			MINMAXINFO * r = (MINMAXINFO*)lParam;
			r->ptMinTrackSize.x = (198*dx)/4;
			r->ptMinTrackSize.y = (194*dy)/8;
			return 0;
		}

	case WM_LBUTTONDOWN:
		if(bInPosition)
		{ 		// bInPosition is when the mouse is over the separator bar
			SetCapture(hwnd);	// determined in WM_MOUSEMOVE
			bDoCapture = TRUE;
			if(MoveBar(hwnd, LOWORD(lParam), dx, dy))
			{
				SizeControls(hwnd, dx, dy);
				InvalidateRect(hwnd, NULL, TRUE);
			}
		}
		return 0;

	case WM_LBUTTONUP:
		if(bDoCapture)
		{
			ReleaseCapture();
			bDoCapture = FALSE;
		}
		return 0;

	case WM_MOUSEMOVE:
		{
			int x = ((bk.r + 2)*dx)/4;
			int y = (2*dy)/8;
			int cy = ((bk.b-11)*dy)/8;
			int cx = ((bk.r + 6)*dx)/4;
			if( LOWORD(lParam) > x && LOWORD(lParam) < cx && HIWORD(lParam) > y && HIWORD(lParam) < cy)
			{
				SetClassLong(hwnd, GCL_HCURSOR, (long)g_hCurEW);
				bInPosition = TRUE;
			}
			else
			{
				bInPosition = FALSE;
				SetClassLong(hwnd, GCL_HCURSOR, (long)g_hCurArrow);
			}

			break;
		}

	case WM_NOTIFY:
		return OnNotify(hwnd, lParam);

	case WM_COMMAND:
		return OnCommand(hwnd, wParam, lParam);

	case WM_CTLCOLORSTATIC:
		{
			int id = GetDlgCtrlID((HWND)lParam);
			if(id >= IDS_BACK && id<= IDCHK_OCCUR)
			{
				SetBkColor((HDC)wParam, RGB(234, 232, 227));
				return (BOOL)g_hBrLite;
			}
			break;
		}

	case WM_CLOSE:
		{
			WINDOWPLACEMENT g_wp;
			g_bEndding = TRUE;
			g_wp.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(hwnd, &g_wp);

			if(g_wp.showCmd == SW_SHOWMAXIMIZED)
				dlg.f = SW_SHOWMAXIMIZED;
			else if(g_wp.showCmd == SW_SHOWMINIMIZED)
				dlg.f = SW_SHOWMINIMIZED;
			else
				dlg.f = SW_SHOWNORMAL;

			GetWindowRect(hwnd, (RECT*)&dlg);
			WriteRegistry(dx, dy);
			if(g_strMem)
				free(g_strMem);

			return EndDialog(hwnd,0);
			break;
		}

	case WM_DESTROY:
		DeleteCriticalSection(&g_CriticalSection);
		FreeGlobalMem();
		if(g_pIPicture)
		{
			OleReleasePic(g_pIPicture);
		}
		AbortFindFile();
		DestroyBitmaps();
		DeleteObject(g_hFont);
		SetWindowLong(GetCtrl(hwnd, IDS_EXTBACK), GWL_WNDPROC, (DWORD)pSubClassExtPaneProc);
		SetWindowLong(GetCtrl(hwnd, IDS_PICBOX), GWL_WNDPROC, (DWORD)pSubClassPictureBoxProc);
		SetWindowLong(GetCtrl(hwnd, IDE_HEADER), GWL_WNDPROC, (DWORD)pSubClassTextBoxProc);
		ListView_DeleteAllItems(GetCtrl(hwnd,IDC_LISTVIEW));
		DeleteObject(g_hBrLite);
		FreeFindFile();
		PostQuitMessage(0);
		return FALSE;

	default:
		return FALSE;
	}
	return FALSE;

}

void ReadRegistry(void)
{
	// read various values from registry
	dlg.f = ReadRegistryInt("WindowPos", "full", dlg.f);
	dlg.l = ReadRegistryInt("WindowPos", "left", dlg.l);
	dlg.t = ReadRegistryInt("WindowPos", "top", dlg.t);
	dlg.r = ReadRegistryInt("WindowPos", "width", dlg.r);
	dlg.b = ReadRegistryInt("WindowPos", "height", dlg.b);

	bk.r = ReadRegistryInt("BarPos", "rpos", bk.r);
	bk.b = ReadRegistryInt("BarPos", "bpos", bk.b);

	g_ColWidth[0] =	ReadRegistryInt("HeaderCol", "col0", g_ColWidth[0]);
	g_ColWidth[1] =	ReadRegistryInt("HeaderCol", "col1", g_ColWidth[1]);
	g_ColWidth[2] =	ReadRegistryInt("HeaderCol", "col2", g_ColWidth[2]);
	g_ColWidth[3] =	ReadRegistryInt("HeaderCol", "col3", g_ColWidth[3]);
	g_iCase       = ReadRegistryInt("Opts", "case", 1);
	g_iSubFolder  = ReadRegistryInt("Opts", "sub", 1);
	g_iFileCounts = ReadRegistryInt("Opts", "counts", 1);
	g_iAutoWildcards = ReadRegistryInt("Opts", "wild", 1);

	ReadRegistryString("MRU", "1st", "", g_mru[0]);
	ReadRegistryString("MRU", "2nd", "", g_mru[1]);
	ReadRegistryString("MRU", "3rd", "", g_mru[2]);
	ReadRegistryString("MRU", "4th", "", g_mru[3]);
	ReadRegistryString("MRU", "5th", "", g_mru[4]);
	ReadRegistryString("MRU", "6th", "", g_mru[5]);

	ReadRegistryString("SendTo", "1st", "", g_strSendto[0]);
	ReadRegistryString("SendTo", "2nd", "", g_strSendto[1]);
	ReadRegistryString("SendTo", "3rd", "", g_strSendto[2]);
	ReadRegistryString("SendTo", "4th", "", g_strSendto[3]);
	ReadRegistryString("SendTo", "5th", "", g_strSendto[4]);
	ReadRegistryString("SendTo", "6th", "", g_strSendto[5]);

	ReadRegistryString("Disp", "1st", "", g_strDisp[0]);
	ReadRegistryString("Disp", "2nd", "", g_strDisp[1]);
	ReadRegistryString("Disp", "3rd", "", g_strDisp[2]);
	ReadRegistryString("Disp", "4th", "", g_strDisp[3]);
	ReadRegistryString("Disp", "5th", "", g_strDisp[4]);
	ReadRegistryString("Disp", "6th", "", g_strDisp[5]);

	g_strDepends = ReadRegistryDepends();

}

void WriteRegistry(int dx, int dy)
{
	int i0, i1, i2, i3;

	g_iCase       = IsDlgButtonChecked(g_hwndMain, IDCHK_CASE);
	g_iSubFolder  = IsDlgButtonChecked(g_hwndMain, IDCHK_SUBFOLDER);
	g_iFileCounts = IsDlgButtonChecked(g_hwndMain, IDCHK_OCCUR);
	g_iAutoWildcards = IsDlgButtonChecked(g_hwndMain, IDCHK_AUTOWILDC);

	HWND hListView = GetCtrl(g_hwndMain,IDC_LISTVIEW);
	// pixels to dlg units
	i0 = (ListView_GetColumnWidth(hListView, 0)*4)/dx;
	i1 = (ListView_GetColumnWidth(hListView, 1)*4)/dx;
	i2 = (ListView_GetColumnWidth(hListView, 2)*4)/dx;
	i3 = (ListView_GetColumnWidth(hListView, 3)*4)/dx;

	if (dlg.f != SW_SHOWMINIMIZED)
	{ // do not write if MINIMIZED, will open MINIMIZED
		WriteRegistryInt("WindowPos", "full", dlg.f);
	}

	if(dlg.f == SW_SHOWNORMAL)
	{
		WriteRegistryInt("WindowPos", "left", (dlg.l*4)/dx);
		WriteRegistryInt("WindowPos", "top",  (dlg.t*8)/dy);
		WriteRegistryInt("WindowPos", "width", ((dlg.r - dlg.l)*4)/dx);
		WriteRegistryInt("WindowPos", "height", ((dlg.b - dlg.t)*8)/dy);
	}

	WriteRegistryInt("BarPos",    "rpos", bk.r);
	WriteRegistryInt("BarPos",    "bpos", bk.b);
	WriteRegistryInt("HeaderCol", "col0", i0);
	WriteRegistryInt("HeaderCol", "col1", i1);
	WriteRegistryInt("HeaderCol", "col2", i2);
	WriteRegistryInt("HeaderCol", "col3", i3);
	WriteRegistryInt("Opts", "case", g_iCase);
	WriteRegistryInt("Opts", "sub", g_iSubFolder);
	WriteRegistryInt("Opts", "counts", g_iFileCounts);
	WriteRegistryInt("Opts", "wild", g_iAutoWildcards);

	WriteRegistryString("MRU", "1st", "", g_mru[0]);
	WriteRegistryString("MRU", "2nd", "", g_mru[1]);
	WriteRegistryString("MRU", "3rd", "", g_mru[2]);
	WriteRegistryString("MRU", "4th", "", g_mru[3]);
	WriteRegistryString("MRU", "5th", "", g_mru[4]);
	WriteRegistryString("MRU", "6th", "", g_mru[5]);

	WriteRegistryString("SendTo", "1st", "", g_strSendto[0]);
	WriteRegistryString("SendTo", "2nd", "", g_strSendto[1]);
	WriteRegistryString("SendTo", "3rd", "", g_strSendto[2]);
	WriteRegistryString("SendTo", "4th", "", g_strSendto[3]);
	WriteRegistryString("SendTo", "5th", "", g_strSendto[4]);
	WriteRegistryString("SendTo", "6th", "", g_strSendto[5]);

	WriteRegistryString("Disp", "1st", "", g_strDisp[0]);
	WriteRegistryString("Disp", "2nd", "", g_strDisp[1]);
	WriteRegistryString("Disp", "3rd", "", g_strDisp[2]);
	WriteRegistryString("Disp", "4th", "", g_strDisp[3]);
	WriteRegistryString("Disp", "5th", "", g_strDisp[4]);
	WriteRegistryString("Disp", "6th", "", g_strDisp[5]);

	char file[MAX_PATH+100];

	GetModuleFileName(g_hInst, file, MAX_PATH+99);
	WriteRegistryString("Path", "name", "", file);

}

LRESULT CALLBACK SubClassRenameEditBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:		// trap the RETURN
		if(wParam == VK_RETURN)
		{
			DestroyWindow(hwnd);
			return 0;
		}
		else
		{
			return CallWindowProc(pSubClassTextBoxProc, hwnd, message, wParam, lParam);
		}

	case WM_KILLFOCUS:
		DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		{
			int len = GetWindowText(hwnd, strGen0, MAX_PATH+100);		// strGen0 holds the original
			int id = GetWindowLong(hwnd, GWL_USERDATA);
			FILEINFO * fi = fileopts.ppfi[id];

			sprintf(strGen1, "%s%s", fi->pathname, fi->filename);   // original
			sprintf(strGen2, "%s%s", fi->pathname, strGen0);		// intended

			if(strcmp(strGen1, strGen0) && !g_bEndding)
			{
				if(!SHRenameFile(strGen1, strGen2))
				{
					if(strlen(fi->filename) < (len+1))
					{
						fi->filename = jmalloc(g_hMem, len+1);
						strcpy(fi->filename, strGen0);
					}
					else
					{
						strcpy(fi->filename, strGen0);
					}
					ListView_RedrawItems(GetCtrl(g_hwndMain, IDC_LISTVIEW), id, id);
				}
				else
				{
					MessageBeep(MB_ICONEXCLAMATION);
					MsgBox(g_hwndMain, "Could not rename file! Try a different name.");
				}
			}
			SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)pSubClassRenameEditBoxProc);
			SetFocus(GetCtrl(g_hwndMain, IDC_LISTVIEW));
			return 0;
		}

	default:
		return CallWindowProc(pSubClassTextBoxProc, hwnd, message, wParam, lParam) ;
	}
}

LRESULT CALLBACK SubClassPictureBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rc;
	int dx, dy;

	switch (message)
	{
	case WM_ERASEBKGND:
		GetClientRect(GetCtrl(g_hwndMain, IDS_PICBOX), &rc);
		rc.right -= GetSystemMetrics(SM_CXVSCROLL);
		FillRect((HDC)wParam, &rc, GetStockObject(WHITE_BRUSH));
		return 1;

	case WM_PAINT:
		// get dialog units for x & y
		dx = LOWORD(GetDialogBaseUnits());
		dy = HIWORD(GetDialogBaseUnits());

		BeginPaint(hwnd, &ps);
		if(g_pIPicture)
		{
			SetRect(&rc, (3*dx)/4, (3*dy)/8, (60*dx)/4, (60*dy)/8);
			OlePicRender(g_pIPicture, ps.hdc, &rc, TRUE);
		}
		EndPaint(hwnd, &ps);
		return 0;

	default:
		return CallWindowProc(pSubClassPictureBoxProc, hwnd, message, wParam, lParam) ;
	}
}

LRESULT CALLBACK SubClassTextBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PASTE:		// stop paste in the text box
		return 0;

	case WM_CHAR:		// stop editing in the text box
		return 0;

	default:
		return CallWindowProc(pSubClassTextBoxProc, hwnd, message, wParam, lParam) ;
	}
}


