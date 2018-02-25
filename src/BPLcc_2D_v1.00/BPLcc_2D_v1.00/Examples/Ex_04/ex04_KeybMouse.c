#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;

#define BPLCC_WINDOW_TITLE "Example using Internal Mouse Object." // Change title.
#define BPLCC_SCREEN_X 320
#define BPLCC_SCREEN_Y 240
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA
#define BPLCC_SCREEN_MAGNIFIER 1
#define BPLCC_SHOWMOUSECURSOR false // Don't show cursor over our application.
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00A0A0A0
#define BPLCC_RENDERTIME 0

BPLcc_StdGraph S; // Declaring a Stadar Graphic.
int x = 160, y = 120; // Setting the center of our ellipse.
int Rx = 10, Ry=10; // Declaring the Radius of our ellipse.

void ToDo_Initialize(void) {
	BPLcc_SGR_Create(&S); // Creting a Stadar Graphic Object.
	BPLcc_SGR_Color(&S,0,0,190); // Give a color almost blue.
}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
	x = h2D.M.p.x; // Setting the Ellipse's Center to the Mouse position. - X coordinate.
	y = h2D.M.p.y; // Setting the Ellipse's Center to the Mouse position. - Y coordinate.

	// Mouse Wheel checking!
	if (h2D.M.keys[MS_WHEEL] == true) {
		h2D.M.keys[MS_WHEEL] = false;
		if (h2D.M.Wheel == MS_WHEEL_UP) { Rx+=5; Ry+=5; } // If wheel is one time UP.
		if (h2D.M.Wheel == MS_WHEEL_DOWN) { // If Wheel is one time Down.
			Rx-=5; if (Rx < 5) { Rx = 5; }
			Ry-=5; if (Ry < 5) { Ry = 5; }
		}
	}

	if (h2D.M.keys[MS_LB] == true ) { // If Mouse LB is pressed.
		h2D.M.keys[MS_LB]=false;
		Rx+=5; Ry-=5; if (Ry < 5) { Ry = 5; }
	}

	if (h2D.M.keys[MS_RB] == true ) { // If mouse RB is pressed.
		h2D.M.keys[MS_RB]=false;
		Ry+=5; Rx-=5; if (Rx < 5) { Rx = 5; }
	}

}

void DrawLayers(void) {
	// Draw a Rectangle ar relative positions depending on Rx and Ry;
	BPLcc_SGR_Rect(&S, 80+Rx/5,100+Ry/5,220-Rx/5,100-Ry/5,1);
	// Draw the Ellipde! :)
	BPLcc_SGR_Ellipse(&S,x,y,0,Rx, Ry);
}


void ToDo_AtExitProgram(void) {
	BPLcc_SGR_Destroy(&S); // Destroy our Stadar Graphic Object.
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

