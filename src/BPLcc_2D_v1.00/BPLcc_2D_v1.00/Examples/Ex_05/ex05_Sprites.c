#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;

#define BPLCC_WINDOW_TITLE "Example Sprites - Make A Sprite By a Matrix" // Change The Title.
#define BPLCC_SCREEN_X 60 // 60x40 ViewMatrix
#define BPLCC_SCREEN_Y 40 //
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA
#define BPLCC_SCREEN_MAGNIFIER 8 // 8x Magnifier.
#define BPLCC_SHOWMOUSECURSOR false // Don't show cursor.
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00000000 // Black Clear Color
#define BPLCC_RENDERTIME 0

BPLcc_Sprite SPR; // Declaring a Sprite Object.
int x=100,y=80; // Setting Sprite Coordinates.

unsigned long DataMatrix[12] = { // Creating a Simple Matrix containing the Texture of Our Sprite.
	0x000000FF, // Red ...
	0x000000FF,
	0x000000FF,
	0x000000FF,
	0x0000FF00, // Green
	0x00FAFAFA, // TRANSPARENT COLOR
	0x00FAFAFA, // TRANSPARENT COLOR
	0x0000FF00, // Green
	0x00FF0000, // Blue ...
	0x00FF0000,
	0x00FF0000,
	0x00FF0000
};

// NOTE: The Transparent Color can be used to make sprites that are not rectangular!
// Change the Clear Color to see that the 2 medium pixels are really transparent!

void ToDo_Initialize(void) {
	BPLcc_SPR_Create(&SPR); // Creting the Sprite Object.

	BPLcc_SPR_InitByMatrix(&SPR, 4, 3, DataMatrix); // Loading DataMatrix to the Sprite.
	// The Sprite has 4 columns, and 3 rows! :)
}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
	x = h2D.M.p.x / h2D.Screen_Magnifier; // Updating X,Y positions to be where mouse is pointing...
	y = h2D.M.p.y / h2D.Screen_Magnifier; // Divide with the Magnifier Value to more properly results.
}

void DrawLayers(void) {
	BPLcc_SPR_Put(&SPR,x,y,0); // Putting the Sprite at X,Y position and 0 Layer.
}


void ToDo_AtExitProgram(void) {
	BPLcc_SPR_Destroy(&SPR); // Destroying our Sprite, releasing the memory.
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

