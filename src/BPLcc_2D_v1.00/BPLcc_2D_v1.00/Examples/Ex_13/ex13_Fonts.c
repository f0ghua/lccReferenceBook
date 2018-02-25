#include "BPLcc_2D.h"
#include "ex13_Resources.h" // Include Resources...

extern BPLcc_WinMain h2D;

#define BPLCC_WINDOW_TITLE "Example Resources Loading & Using Fonts" // Change Title.
#define BPLCC_SCREEN_X 320
#define BPLCC_SCREEN_Y 240
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x00FF0000 // Blue
#define BPLCC_SCREEN_MAGNIFIER 1
#define BPLCC_SHOWMOUSECURSOR true
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00A0A0A0
#define BPLCC_RENDERTIME 0

BPLcc_uFont F; // Declare a Font Object.
int x, y;

void ToDo_Initialize(void) {
	BPLcc_UFNT_Create(&F); // Create the Font Object.
	BPLcc_UFNT_CreateFont(&F,6,32,29,192,29,IDFONTS); // Create the Font from IDFONTS resource (Fonts_color.bmp)
}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
	x=h2D.M.p.x - 15; // Set X,Y relative to Mouse pos.
	y=h2D.M.p.y - 15;
}

void DrawLayers(void) {
	// Typing Different Words with the A-F letters the Resource Font has.
	BPLcc_UFNT_Text(&F,10,10,10,"ABCDEF");
	BPLcc_UFNT_Text(&F,10,60,10,"FADE");
	BPLcc_UFNT_Text(&F,40,100,10,"BAD");
	// Playing with the Layers! :P
	BPLcc_UFNT_Text(&F,75,145,11,"DECADE");
	BPLcc_UFNT_Text(&F,70,140,10,"DECADE");
	BPLcc_UFNT_Text(&F,80,150,12,"DECADE");
	// Upper Layer overwrites the all! :)
	BPLcc_UFNT_Text(&F,x,y,0,"BABE");
}


void ToDo_AtExitProgram(void) {
	// Destroy the Font Object to free memory.! :)
	BPLcc_UFNT_Destroy(&F);
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
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_AUTO_KEYBOARD, true);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_AUTO_MOUSE, true);

	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINPOSX, 23);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINPOSY, 23);

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

