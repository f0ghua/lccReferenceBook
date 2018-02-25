#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;

#define BPLCC_WINDOW_TITLE "Stadar Graphics Example 01" // Change Title
#define BPLCC_SCREEN_X 60 // 60x40 pixels Drawing Area = 2400 pixels.
#define BPLCC_SCREEN_Y 40 // <--
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA
#define BPLCC_SCREEN_MAGNIFIER 8 // (60x8)*(40x8) = Window size (480x320) pixels.
#define BPLCC_SHOWMOUSECURSOR true
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00000000 // Change background color to black.
#define BPLCC_RENDERTIME 5000 // Render scene every 5 second - Try to overlap the application's window with other window for 1 sec.

BPLcc_StdGraph S1, S2, S3; //Creating 3 Stadar Graphics - For using 3 dif. colors.
BPLcc_StdGraph SS; // This will be a Stader Graphic for Dotted lines, e.t.c.

void ToDo_Initialize(void) {
	BPLcc_SGR_Create(&S1); // Initializing these objects. S1
	BPLcc_SGR_Create(&S2); // S2
	BPLcc_SGR_Create(&S3); // S3
	BPLcc_SGR_Create(&SS); // Creating the Dotted Stadar Graphics.

	BPLcc_SGR_Color(&S1,255,0,0); // S1 = RED
	BPLcc_SGR_Color(&S2,0,255,0); // S2 = GREEN
	BPLcc_SGR_Color(&S3,0,0,255); // S3 = BLUE

	BPLcc_SGR_Step(&SS,2); // Settign Stepping to 2. This value is nothing more that the step
	// the algorithm of line wiil have to create the line, point per point. Default = 1.
	// See That I don't give color to the SS object, so it will be white by default.


}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
}

void DrawLayers(void) {
	// --------- Layer Level 10 Drawing Pixels & Lines -----------------------
	BPLcc_SGR_SetPixel(&S1,0,0,10); // Setting as S1's color (RED) the pixel (0x0) - upper-left.
	BPLcc_SGR_SetPixel(&S2,59,39,10); // Setting as S2's color (GREEN) the pixel (59,39) - down-right.
	// Don't forget we have a magnifier of 8, meaning each pixel is a rectangle of 8x8 pixels.
	BPLcc_SGR_Line(&S3,1,0,58,37,10); // Creating Blue line.|||
	BPLcc_SGR_Line(&S3,1,1,58,38,10); // Creating Blue line.|-- These Lines are parallel to make a bold line
	BPLcc_SGR_Line(&S3,1,2,58,39,10); // Creating Blue line.|||
	BPLcc_SGR_Line(&SS,58,1,1,38,10); // Creating White(default) dotted line!

	// --------- Layer Level 11 Drawing Circles ---------------------
	BPLcc_SGR_Circle(&S1,29,19,11,7);  // Radius 7  |||
	BPLcc_SGR_Circle(&S1,29,19,11,10); // Radius 10 |-- X:29, Y:29, Layer:11
	//Note the above circles are overwritten by the lines - Because they have bigger Layer.

	// --------- Layer Level 12 Drawing Rectangle -------------------
	BPLcc_SGR_Rect(&S2,13,13,45,25,12); // Creating a Green Rectangle at Layer 12.
	// Note the Rectangle is overwritten by anything else, beacause it has the bigger Layer.

	// --------- Layer Level 5 Drawing Ellipses ---------------------
	BPLcc_SGR_Ellipse(&SS,29,19,5,8,15); // Creating an Ellipse with default Color White.
	// Not that the ellipse is not dotted. To make it dotted, just change the Step of the SS from 2 to 200 f.e.
	// but the line will change too - and might not be visible.
	// With value 200, means a pixel every: 200/max(8,15) = 200/15 ~= 13 degrees.

}


void ToDo_AtExitProgram(void) {
	BPLcc_SGR_Destroy(&S1); // Destroying our Stadar Graphics objects...
	BPLcc_SGR_Destroy(&S2); // at the end of our application to free memory...
	BPLcc_SGR_Destroy(&S3); // (StdGraphs at v1.00 doesn't "allocate" memory)
	BPLcc_SGR_Destroy(&SS); // Destroy and the SS object.
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

