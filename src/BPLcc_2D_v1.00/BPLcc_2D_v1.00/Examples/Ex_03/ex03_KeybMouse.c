#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;

/******** NOTE: Changes has done AND to LINE: 82 ***********************/

#define BPLCC_WINDOW_TITLE "Example Keyboard use, with User's Object." // Change Title.
#define BPLCC_SCREEN_X 160 // 160x120 pixels.
#define BPLCC_SCREEN_Y 120
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA
#define BPLCC_SCREEN_MAGNIFIER 4 // Screen Magnifier 4.
#define BPLCC_SHOWMOUSECURSOR true
#define BPLCC_EXIT_ON_ESCAPE false // Do not exit at escape.
#define BPLCC_CLEARCOLOR 0x00A0A0A0
#define BPLCC_RENDERTIME 0

BPLcc_Keys K; // Declaring a Keyboard Object.
BPLcc_StdGraph S; // Declaring a Stadar Graphic.
int x=50, y=10; // Coordinates of pixel.

void ToDo_Initialize(void) {
	BPLcc_Keys_Create(&K); // Creating the Keyboard object.
	BPLcc_SGR_Create(&S); // Creating the Stadar Graphic Object.

}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  // Updating the keyboard object using the uMsg and wParam variables of the system.
	BPLcc_Keys_Update(&K, uMsg, wParam);
}


void ToDo_MainLoop(void) {
	if (K.keys[VK_F4]==true) { PostQuitMessage(0); }
	// Now we use K as we used h2D.K object. IT's exactly the same...
	// That's the reason I include the handling of the Keyboard object to the main code of BPLcc Lib.
	// Just to minimize the code had to be written by the users of my lib.

	if (K.keys[VK_LEFT] == true) { // If Left Arrow pressed, move left (decr. x coord).
		x--; if (x < 0) { x=0; }
	}
	if (K.keys[VK_RIGHT] == true) { // If Right Arrow pressed, move right (inc. x coord).
		x++; if (x >= h2D.ScreenX) { x=h2D.ScreenX-1; } // If x is bigger than screen width, se it to right side.
	}
	if (K.keys[VK_UP] == true) { // If UP Arrow pressed, move up (decrease y coord).
		y--; if (y < 0) { y=0; }
	}
	if (K.keys[VK_DOWN] == true) { // If Down Arrow pressed, move down (inc. y coord).
		y++; if (y >= h2D.ScreenY) { y=h2D.ScreenY-1; } // If y is bigger than screen height, se it to down side.
	}
}

void DrawLayers(void) {
	BPLcc_SGR_SetPixel(&S,x,y,0);
}


void ToDo_AtExitProgram(void) {
	BPLcc_SGR_Destroy(&S); // Destroying the S
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
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_AUTO_KEYBOARD, false); // ******* EXAMPLE HERE. If user want to use his own Keyboard object
	// he can set the above statement from "true" to "false" as it is now. That mean's system won't use internal keyboard object.
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

