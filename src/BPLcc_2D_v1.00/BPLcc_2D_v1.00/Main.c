/* Basic structure of C code, in order to utilize BPLcc_2D Library.... (NOTE: I use TAB = 2 Spaces) */
#include "BPLcc_2D.h"																	/* Include Lib's definitions...											*/

extern BPLcc_WinMain h2D;															/* Declaring External WinMain Structure.						*/

#define BPLCC_WINDOW_TITLE "BPLcc 2D Application" 		/* Window Title																			*/
#define BPLCC_SCREEN_X 320														/* Window Width																			*/
#define BPLCC_SCREEN_Y 240														/* Widnow Height																		*/
#define BPLCC_LAYERS 0xFFFF 													/* 65536 Layers - Depth															*/
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA							/* Transparent Color. (RGB: 250,250,250)						*/
#define BPLCC_SCREEN_MAGNIFIER 1											/* Screen Magnifier.																*/
#define BPLCC_SHOWMOUSECURSOR true										/* Mouse Cursor Visible.														*/
#define BPLCC_EXIT_ON_ESCAPE true											/* If ESC Pressed the Application Ends.							*/
#define BPLCC_CLEARCOLOR 0x00A0A0A0										/* BackGround Color set to RGB: (160,160,160)				*/
#define BPLCC_RENDERTIME 0														/* Rendering Time Difference - 0 means Continusly.	*/

/* Initialize here all your data ... */
void ToDo_Initialize(void) {
}

/* Use here any code, that might use the uMsg, wParam and lParam variables of the system */
void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}

/* Use here any code that will have to running repeatedly - F.e. Cheking if a global variable has change repeatedly*/
void ToDo_MainLoop(void) {
}

/* Draw your 2D Scene Here. This function is Called when ( h2D.DC_Draw == true ) */
void DrawLayers(void) {
}

/* Use here any code that must be called at the closing of you applications - F.e. to free memory from your data */
void ToDo_AtExitProgram(void) {
}


/* ***************************************************************************************** */
/* Parameters For Inialization of the 2D Window... - Modify Only Experts. */
/* ***************************************************************************************** */
void BPLcc_Window_Parameters_Set(void) {
BPLcc_WinData_SetPc(&h2D, BPLCC_INITWINOBJ_NAME, BPLCC_WINDOW_TITLE);
	BPLcc_WinData_SetPc(&h2D, BPLCC_INITWINOBJ_CLASSNAME, "BPLcc 2D");
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_STYLE, 0);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_BACKBRUSH, (COLOR_WINDOW-1));
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_DWSTYLE, (WS_CAPTION | WS_SYSMENU));
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_SCR_WIDTH, BPLCC_SCREEN_X);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_SCR_HEIGHT, BPLCC_SCREEN_Y);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_SCR_LAYERS, BPLCC_LAYERS);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_SCR_TRANSCOLOR, BPLCC_TRANSPARENTCOLOR);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_SCR_MAGNIFIER, BPLCC_SCREEN_MAGNIFIER);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_SCR_EXITONESCAPE, BPLCC_EXIT_ON_ESCAPE);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_SCR_SHOWCURSOR, BPLCC_SHOWMOUSECURSOR);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_SCR_CLEARCOLOR, BPLCC_CLEARCOLOR);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_RENDERTIME, BPLCC_RENDERTIME);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_AUTO_KEYBOARD, true); // Enable Internal Keyboard Handler Object for updating.
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_AUTO_MOUSE, true); // Enable Internal Mouse Handler Object for updating.


/* The Below Statements Initilalize Window with the Resource ICON/CURSOR. If they are not used(Like now) the Library
	 uses the Default ICON (IDI_APPLICATION) and the Default Cursor (IDC_ARROW) defined in Win32... To Start with your
	 ICON / CURSOR change IDIICON100 / IDCURSOR200 names to your corresponding resources at the below statements.
	BPLcc_WinData_SetID(&h2D, BPLCC_INITWINOBJ_ICON, (LPCSTR) IDIICON100, false);
	BPLcc_WinData_SetID(&h2D, BPLCC_INITWINOBJ_CURSOR, (LPCSTR) IDCURSOR200, false);
*/
	/* Use this Statement's to Set Your Window OpenGL Application Position to Screen. CW_USEDEFAULT uses windows default(moving)*/
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINPOSX, 23); /* Upper Left corner Of Window Start At Position (23,23) */
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINPOSY, 23);

	/* The relative coordinates in pixels, where the game's screen start's (without the window's graphics...)
		 F.e. here means the Drawing Area is 3 pixels left to the position of the system Window of our application,
		 and 22 means the Drawing Area is 22 pixel of the upper side of the system Window (-The 22 pixels are for the
		 Title Bar, in my computer.) */
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINRELPOSX, 3);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINRELPOSY, 22);
}

/********************************************************
  About BPLcc_2D Library:
	Author / Creator	: Vasileios Kon. Pothos	|	GR'05
	E-mail						: terablade2001@yahoo.gr
	Starting Date			: 11/24/05
	Version						: 1.00
	Licence						: GPL
*********************************************************/

