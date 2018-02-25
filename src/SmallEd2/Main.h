/*********************************************************************
			Main.h - Main.c Header File for Small Edit

	This file contains the prototypes that are static and used
	only in Main.c.
*********************************************************************/
int static RegisterWndClass(void);
int static CreateMainWnd(int);
void static CreateChildWnd(HWND);
void static CreateToolBar(HWND);
void static CreateMenuSystem(HWND);
void static ResizeWnd(HWND);
void static UnRegisterWnd(void);
void static Update(HWND);
void static CloseWnd(HWND);
void static PopUpMenu(HWND,int,int);
