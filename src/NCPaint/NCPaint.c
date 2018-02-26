// NCPaint.c
//
// TODO: Drawing the menu in a custom colour.
//
// John
//

#include <windows.h>

NONCLIENTMETRICS ncm;

#define SM(a) GetSystemMetrics(a)

static LRESULT CALLBACK NewLookProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int InitNewLook(HWND hwnd);

static int InitOnNCPaint(HWND hwnd);
static void OnNCPaint(HWND hwnd);
static void OnNCLButtonUp(HWND hwnd, WPARAM wParam, LPARAM lParam);
static void OnNCLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
static void OnNCActivate(HWND hwnd, BOOL bActive);
static void DestroyNCPaint(HWND hwnd);

typedef struct _MYRGBTRIPLE
{	BYTE r;
	BYTE g;
	BYTE b;
} MYRGBTRIPLE;

static MYRGBTRIPLE ctab[] = {
	{15, 69, 175}, {0, 72, 219}, {6, 96, 245}, {2, 100, 251}, {1, 105, 254},
	{1, 107, 255}, {1, 105, 255}, {1, 105, 253}, {0, 101, 253},	{0, 98, 251},
	{2, 95, 250}, {1, 92, 245}, {0, 88, 242}, {0, 87, 239},	{1, 86, 237},
	{1, 85, 236}, {1, 85, 233}, {1, 85, 233}, {2, 84, 230},	{1, 83, 227},
	{1, 84, 226}, {1, 84, 226},	{2, 83, 227}, {2, 83, 227},	{2, 83, 227},
	{2, 83, 227}};

typedef struct _Win
{	WNDPROC OldWndProc;			// apps window proc
	int 	capButWidth;		// caption buttons
	int 	capButHeight;		//
	int 	captionHeight;		// title bar
	int 	capFontHeight;		//
	int 	frame;				// width of window border
	HFONT   hCapFont;			// title font
	HFONT   hMenuFont;			//
	HBITMAP hBmpActive;			// bright bitmap
	HBITMAP hBmpNonActive;		// dull bitmap
	BOOL    bActive;			// title bar active
}WIN;

static WIN w;					// global struct

// cursors needed when dealing with WM_NCHITTEST
HCURSOR hCurArrow, hCurNS, hCurWE, hCurNESW, hCurNWSE;

typedef unsigned char byte;

// load cursors for the hittest and subclass the window.
int InitNewLook(HWND hwnd)
{
	// cursors required when doing hittest.
	hCurArrow = LoadCursor(NULL, IDC_ARROW);
	hCurNS    = LoadCursor(NULL, IDC_SIZENS);
	hCurWE    = LoadCursor(NULL, IDC_SIZEWE);
	hCurNESW  = LoadCursor(NULL, IDC_SIZENESW);
	hCurNWSE  = LoadCursor(NULL, IDC_SIZENWSE);

	w.OldWndProc = (WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (long)NewLookProc);

	InitOnNCPaint(hwnd);

	return 1;
}

static void DestroyNCPaint(HWND hwnd)
{
	DeleteObject(w.hCapFont);
	DeleteObject(w.hMenuFont);
	DeleteObject(w.hBmpActive);
	DeleteObject(w.hBmpNonActive);
}

// determine the left most position of system buttons.
static int LeftEdgeofButs(HWND hwnd)
{
	DWORD dwStyle   = GetWindowLong(hwnd, GWL_STYLE);
	DWORD dwExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

	int nButSize = 0;
	int nSysButSize;

	#define GAP 2

	if(dwExStyle & WS_EX_TOOLWINDOW){
		nSysButSize = SM(SM_CXSMSIZE) - GAP;

		if(dwStyle & WS_SYSMENU)
			nButSize += nSysButSize + GAP;

		return nButSize + GAP;
	}else{
		nSysButSize = SM(SM_CXSIZE) - GAP;

		// close button has
		if(dwStyle & WS_SYSMENU){
			nButSize += nSysButSize + GAP;
		}

		// if either minimize or maximize - both are shown
		if( dwStyle & WS_MAXIMIZEBOX || dwStyle & WS_MINIMIZEBOX ){
			nButSize += GAP + nSysButSize * 2;
		}

		// question mark button
		else if(dwExStyle & WS_EX_CONTEXTHELP){
			nButSize += GAP + nSysButSize;
		}

		return nButSize + w.frame;
	}
}

// crerate two bitmaps, one bright and one dull
static int CreateBitmaps(HWND hwnd)
{
	// create bitmaps
	HBITMAP oldBmp;
	HDC hdcMem, hdc = GetDC(hwnd);
	HPEN hPen, oldPen;

	hdcMem 			= CreateCompatibleDC(hdc);
	w.hBmpActive 	= CreateCompatibleBitmap(hdc, 32, w.captionHeight);
	w.hBmpNonActive = CreateCompatibleBitmap(hdc, 32, w.captionHeight);
	ReleaseDC(hwnd, hdc);

	// first bitmap
	oldBmp = SelectObject(hdcMem, w.hBmpActive);

	for(int i = 0; i<=(w.captionHeight); i++){
		if (i<26){
			hPen = CreatePen(PS_SOLID, 1, RGB(ctab[i].r, ctab[i].g, ctab[i].b));
		}else{
			hPen = CreatePen(PS_SOLID, 1, RGB(2, 83, 227));
		}
		oldPen = SelectObject(hdcMem, hPen);
		MoveToEx(hdcMem, 0, w.captionHeight - i, NULL);
		LineTo(hdcMem, 32, w.captionHeight - i);
		SelectObject(hdcMem, oldPen);
		DeleteObject(hPen);
	}

	// second bitmap
	SelectObject(hdcMem, w.hBmpNonActive);
	for(int i = 0; i<=(w.captionHeight); i++){
		if (i<26){
			hPen = CreatePen(PS_SOLID, 1, RGB(ctab[i].r, ctab[i].g, ctab[i].b-80));
		}else{
			hPen = CreatePen(PS_SOLID, 1, RGB(2, 83, 227-80));
		}
		oldPen = SelectObject(hdcMem, hPen);
		MoveToEx(hdcMem, 0, w.captionHeight - i, NULL);
		LineTo(hdcMem, 32, w.captionHeight - i);
		SelectObject(hdcMem, oldPen);
		DeleteObject(hPen);
	}

	SelectObject(hdcMem, oldBmp);
	DeleteDC(hdcMem);
	return 1;
}

static int InitOnNCPaint(HWND hwnd)
{

	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	DWORD dwExStyle	= GetWindowLong(hwnd, GWL_EXSTYLE);

	// get extra metrics
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);

	// make menu font font
	ncm.lfCaptionFont.lfWeight = FW_BOLD;
	w.hMenuFont = CreateFontIndirect( &ncm.lfMenuFont );

		// get the sizes of the titlebar buttons & title height & title font
	if(dwExStyle & WS_EX_TOOLWINDOW){
		w.capButWidth   = ncm.iSmCaptionWidth - 2;
		w.capButHeight  = ncm.iSmCaptionHeight - 4;
		w.captionHeight = SM(SM_CYSMCAPTION);
		w.hCapFont      = CreateFontIndirect( &ncm.lfSmCaptionFont );
		w.capFontHeight = ncm.lfSmCaptionFont.lfHeight;
	}else{
		w.capButWidth   = ncm.iCaptionWidth - 2;
		w.capButHeight  = ncm.iCaptionHeight - 4;
		w.captionHeight = SM(SM_CYCAPTION);
		w.hCapFont      = CreateFontIndirect( &ncm.lfCaptionFont );
		w.capFontHeight = ncm.lfCaptionFont.lfHeight;
	}

	// thickframe ?
	if(dwStyle & WS_THICKFRAME)
		w.frame = SM(SM_CXSIZEFRAME);
	else
		w.frame = SM(SM_CXFIXEDFRAME);

	return CreateBitmaps(hwnd);
}

// draw the non-client either dull (non-active) or bright (active)
static void OnNCActivate(HWND hwnd, BOOL bActive)
{
	w.bActive = bActive;
	OnNCPaint(hwnd);
}

static void	DrawSystemGadets(HWND hwnd, HDC hdc)
{
	RECT min, max, close;	// remember the system gadgets rectangles
	RECT wrc;
	GetWindowRect(hwnd, &wrc);

	int leftedge = wrc.right - wrc.left - LeftEdgeofButs(hwnd);
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	HPEN origPen = SelectObject(hdc, GetStockObject(WHITE_PEN));
	HBRUSH origBrh = SelectObject(hdc, GetStockObject(NULL_BRUSH));

	// make the button rectangles
	max.left   = leftedge + w.capButWidth - 1;
	max.top    = w.frame + 2;
	max.right  = max.left + w.capButWidth;
	max.bottom = w.frame + 2 + w.capButHeight;

	min.left   = leftedge;
	min.top    = w.frame + 2;
	min.right  = leftedge + w.capButWidth;
	min.bottom = w.frame + 2 + w.capButHeight;

	// if MAXIMIZEBOX or MINIMIZEBOX then both and CLOSEBOX will be adjusted
	if( dwStyle & WS_MAXIMIZEBOX || dwStyle & WS_MINIMIZEBOX ){
		close.left   = leftedge + (w.capButWidth * 2) + 2;
		close.top    = w.frame + 2;
		close.right  = leftedge + w.capButWidth;
		close.bottom = w.frame + 2 + w.capButHeight;
	}else{
	// else CLOSEBOX will here
		close.left   = leftedge + 2;
		close.top    = w.frame + 2;
		close.right  = leftedge + w.capButWidth;
		close.bottom = w.frame + 2 + w.capButHeight;
	}

	// if MINIMIZEBOX draw both MINIMIZEBOX & MAXIMIZEBOX
	HPEN btnPen;
	if(dwStyle & WS_MINIMIZEBOX){
		// max gadget
		btnPen = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
		if(!(dwStyle & WS_MAXIMIZEBOX))
			SelectObject(hdc, btnPen);

		RoundRect(hdc, max.left, max.top, max.right, max.top + w.capButHeight, 4, 4);
		Rectangle(hdc, max.left + 4, max.top + 3, max.right - 4, max.top + w.capButHeight - 3);
		MoveToEx(hdc, max.left + 4, max.top + 4, NULL);
		LineTo(hdc, max.left + w.capButWidth - 4, max.top + 4);

		SelectObject(hdc, GetStockObject(WHITE_PEN));
		// min gadget
		RoundRect(hdc, min.left, min.top, min.right, min.bottom, 4, 4);
		Rectangle(hdc, min.left + 4, min.bottom - 6, min.right - 5, min.bottom - 4);
		DeleteObject(btnPen);
	}

	// draclose box
	if(dwStyle & WS_SYSMENU){
		RoundRect(hdc, close.left, close.top, close.left + w.capButWidth, close.top + w.capButHeight, 4, 4);
		// draw the cross
		MoveToEx(hdc, close.left + 5, close.top + 5, NULL);
		LineTo(hdc, close.left + w.capButWidth - 6, close.top + w.capButHeight - 4);
		MoveToEx(hdc, close.left + 6, close.top + 5, NULL);
		LineTo(hdc, close.left + w.capButWidth - 5, close.top + w.capButHeight - 4);

		MoveToEx(hdc, close.left + 5, close.top + w.capButHeight - 5, NULL);
		LineTo(hdc, close.left + w.capButWidth - 6, close.top + 4);
		MoveToEx(hdc, close.left + 6, close.top + w.capButHeight - 5, NULL);
		LineTo(hdc, close.left + w.capButWidth - 5, close.top + 4);
	}
}

static void OnNCPaint(HWND hwnd)
{
	int i ;
	char sCaption[200];

	HDC hdc;
	RECT rc, wrc;
	HFONT origFont;
	HPEN hPen, origPen;

	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	DWORD dwExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

	hdc = GetWindowDC(hwnd);
	GetWindowRect(hwnd, &wrc);
	HDC memDc = CreateCompatibleDC(hdc);

	// draw the active or non-active bitmap across the title bar
	if(w.bActive)
		SelectObject(memDc, w.hBmpActive);
	else
		SelectObject(memDc, w.hBmpNonActive);
	for (int i = 0; i<wrc.right ; i += 32){
		BitBlt(hdc, i, w.frame, 32, w.captionHeight, memDc, 0, 0, SRCCOPY);
	}

	DeleteDC(memDc);

	// DrawFrame
	HBRUSH blueBrh;
	if(w.bActive){
		blueBrh = CreateSolidBrush(RGB(10, 90, 240));
	}else{
		blueBrh = CreateSolidBrush(RGB(10, 90, 160));
	}

	if(dwStyle & WS_BORDER){
		// draw borders
		SetRect(&rc, 0, 0, wrc.right - wrc.left, w.frame );
		FillRect(hdc, &rc, blueBrh);
		SetRect(&rc, 0, 0, w.frame, wrc.bottom - wrc.top);
		FillRect(hdc, &rc, blueBrh);
		SetRect(&rc, wrc.right - wrc.left - w.frame, 0, wrc.right - wrc.left, wrc.bottom - wrc.top);
		FillRect(hdc, &rc, blueBrh);
		SetRect(&rc, w.frame, wrc.bottom - wrc.top - w.frame, wrc.right - wrc.left, wrc.bottom - wrc.top );
		FillRect(hdc, &rc, blueBrh);
	}
	DeleteObject(blueBrh);

	if(dwStyle & WS_CAPTION){
		// draw title text
		GetWindowText(hwnd, sCaption, 100);
		SetTextColor(hdc, RGB(255,255,255));
		SetBkMode(hdc, TRANSPARENT);
		origFont = SelectObject(hdc, w.hCapFont);

		if(dwStyle & WS_SYSMENU){
			TextOut(hdc, SM(SM_CXSMICON) + w.frame + 1, (w.captionHeight + 3 + w.capFontHeight)/2 , sCaption, strlen(sCaption));
		}else{
			TextOut(hdc, w.frame + 1, (w.captionHeight + 3 + w.capFontHeight)/2 , sCaption, strlen(sCaption));
		}
		SelectObject(hdc, origFont);
	}

	// Draw the menu
//	if(GetMenu(hwnd)){
//		DrawMenuBar(...)
//	}

	// Draw the icon
	if(dwStyle & WS_SYSMENU && !(dwExStyle & WS_EX_TOOLWINDOW)){
		HICON hIcon = (HICON)GetClassLong(hwnd, GCL_HICON);
		DrawIconEx(hdc, w.frame, w.frame, hIcon, SM(SM_CXSMICON), SM(SM_CYSMICON), 0, 0, DI_NORMAL);
	}

	// Draw buttons
	DrawSystemGadets(hwnd, hdc);

	// cleanup
	ReleaseDC(hwnd, hdc);
}

static void OnNCLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);

	if(dwStyle & WS_CAPTION && wParam == HTCAPTION){
		SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, lParam);
	}
	if( GetMenu(hwnd) && wParam ){
		if(wParam == HTSYSMENU){
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MOUSEMENU + HTSYSMENU, lParam);
		}else if(wParam == HTMENU){
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MOUSEMENU, lParam);
		}
	}

	if(dwStyle & WS_THICKFRAME){
		switch(wParam){
			case HTRIGHT:
			case HTLEFT:
			case HTTOP:
			case HTTOPLEFT:
			case HTTOPRIGHT:
			case HTBOTTOM:
			case HTBOTTOMLEFT:
			case HTBOTTOMRIGHT:
			// I've no idea why -9 works here ??
			SendMessage(hwnd, WM_SYSCOMMAND, SC_SIZE + wParam - 9, lParam);
		}
	}
}

static void OnNCLButtonUp(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);

	if(HTREDUCE == wParam && (dwStyle & WS_MINIMIZEBOX)){
		SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	if(HTCLOSE == wParam && (dwStyle & WS_SYSMENU)){
		SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
	}
	if(HTZOOM == wParam && (dwStyle & WS_MAXIMIZEBOX)){
		if(IsZoomed(hwnd))
			SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		else
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
}

/*

 This is not used but you might need it at some time.

static int OnHCHitTest(HWND hwnd, LPARAM lParam)
{
	POINT pt;
	RECT rc;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	DWORD dwExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	GetWindowRect(hwnd, &rc );

	if (!PtInRect( &rc, pt ))
		return HTNOWHERE;

	// Check borders
	if (dwStyle & WS_THICKFRAME ){
		InflateRect( &rc, -SM(SM_CXFRAME), -SM(SM_CYFRAME) );

		if (dwStyle & WS_BORDER)
			InflateRect(&rc, -SM(SM_CXBORDER), -SM(SM_CYBORDER));

		if (!PtInRect( &rc, pt )){
			// Check top sizing border
			if (pt.y < rc.top){

				if (pt.x < rc.left + SM(SM_CXSIZE))
					return HTTOPLEFT;

				if (pt.x >= rc.right - SM(SM_CXSIZE))
					return HTTOPRIGHT;

				return HTTOP;
			}
			// Check bottom sizing border
			if (pt.y >= rc.bottom){

				if (pt.x < rc.left + SM(SM_CXSIZE))
					return HTBOTTOMLEFT;

				if (pt.x >= rc.right - SM(SM_CXSIZE))
					return HTBOTTOMRIGHT;

				return HTBOTTOM;
			}
			// Check left sizing border
			if (pt.x < rc.left){

				if (pt.y < rc.top + SM(SM_CYSIZE))
					return HTTOPLEFT;

				if (pt.y >= rc.bottom - SM(SM_CYSIZE))
					return HTBOTTOMLEFT;

				return HTLEFT;
			}
			// Check right sizing border
			if (pt.x >= rc.right){

				if (pt.y < rc.top + SM(SM_CYSIZE))
					return HTTOPRIGHT;

				if (pt.y >= rc.bottom - SM(SM_CYSIZE))
					return HTBOTTOMRIGHT;

				return HTRIGHT;
			}
		}
	}else{  // No thick frame
		if (dwStyle & WS_DLGFRAME || dwExStyle & WS_EX_DLGMODALFRAME)
			InflateRect(&rc, -SM(SM_CXDLGFRAME), -SM(SM_CYDLGFRAME));

		else if (dwStyle & WS_BORDER)
			InflateRect(&rc, -SM(SM_CXBORDER), -SM(SM_CYBORDER));

		if (!PtInRect( &rc, pt ))
			return HTBORDER;
	}

	// Check caption
	if ((dwStyle & WS_CAPTION) == WS_CAPTION){
		rc.top += SM(SM_CYCAPTION) - 1;
		if (!PtInRect( &rc, pt )){

			// Check system menu
			if (dwStyle & WS_SYSMENU)
				rc.left += SM(SM_CXSIZE);
			if (pt.x <= rc.left)
				return HTSYSMENU;

			// Check maximize box
			if (dwStyle & WS_MAXIMIZEBOX)
				rc.right -= SM(SM_CXSIZE) + 1;
			if (pt.x >= rc.right)
				return HTMAXBUTTON;

			// Check minimize box
			if (dwStyle & WS_MINIMIZEBOX)
				rc.right -= SM(SM_CXSIZE) + 1;
			if (pt.x >= rc.right)
				return HTMINBUTTON;

			return HTCAPTION;
		}
	}

	// Check client area
	ScreenToClient( hwnd, &pt );
	GetClientRect( hwnd, &rc );
	if (PtInRect( &rc, pt ))
		return HTCLIENT;

	// Check vertical scroll bar
	if (dwStyle & WS_VSCROLL){
		rc.right += SM(SM_CXVSCROLL);
		if (PtInRect( &rc, pt ))
			return HTVSCROLL;
	}

	// Check horizontal scroll bar
	if (dwStyle & WS_HSCROLL){
		rc.bottom += SM(SM_CYHSCROLL);
		if (PtInRect( &rc, pt )){
			// Check size box
			if ((dwStyle & WS_VSCROLL) && (pt.x >= rc.right - SM(SM_CXVSCROLL)))
				return HTSIZE;

			return HTHSCROLL;
		}
	}

	// Check menu bar

	if (GetMenu(hwnd)){
		if ((pt.y < 0) && (pt.x >= 0) && (pt.x < rc.right))
			return HTMENU;
	}

	// Should never get here
	return HTERROR;
}
*/

// Proc that deals with various messages from the apps window. (subclassed)
static LRESULT CALLBACK NewLookProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg){
	// Let the system do the hittest then set the cursor appropriately.
	// Reason: On WinME and maybe other OS's, on WM_SETCURSOR the window
	// gadgets/buttons (close/min/max) are redrawn. Dealing with
	// WM_SETCURSOR stops this. Win2K does not need this action and
	// propably neither does XP.
	case WM_NCHITTEST:{
		DWORD hittest = DefWindowProc(hwnd, msg, wParam, lParam);
		SendMessage(hwnd, WM_SETCURSOR, 0, MAKELONG(hittest, 0));
		return hittest;
	}

	case WM_SETCURSOR:
		switch (LOWORD(lParam)){
		case HTTOP:
		case HTBOTTOM:
			SetCursor(hCurNS);
			break;
		case HTRIGHT:
		case HTLEFT:
			SetCursor(hCurWE);
			break;
		case HTTOPLEFT:
		case HTBOTTOMRIGHT:
			SetCursor(hCurNWSE);
			break;
		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
			SetCursor(hCurNESW);
			break;
		default:
			SetCursor(hCurArrow);
		}
		return TRUE; 					// stop further processing

	case WM_SYSCOLORCHANGE:				// if user changes title bar height etc.
		DestroyNCPaint(hwnd);
		InitOnNCPaint(hwnd);
		break;

	case WM_NCACTIVATE:
		OnNCActivate(hwnd, wParam);		// keep track of the active state
		return 0;

	case WM_NCLBUTTONUP:
		OnNCLButtonUp(hwnd, wParam, lParam);
		return 0;

	case WM_NCLBUTTONDOWN:
		OnNCLButtonDown(hwnd, wParam, lParam);
		return 0;

	case WM_NCPAINT:
		// do the menu
		DefWindowProc(hwnd, msg, wParam, lParam);
		// paint the non-client area of the window
		OnNCPaint(hwnd);
		return 0;

	case WM_DESTROY:
		DestroyNCPaint(hwnd);
		// Pass WM_DESTROY to apps window.
		return CallWindowProc(w.OldWndProc, hwnd, msg, wParam, lParam);

	default:
		return CallWindowProc(w.OldWndProc, hwnd, msg, wParam, lParam);
	}

	return 0;
}


