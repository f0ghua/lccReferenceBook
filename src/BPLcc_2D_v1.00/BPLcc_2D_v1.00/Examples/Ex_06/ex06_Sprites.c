#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;

#define BPLCC_WINDOW_TITLE "Example : Load 2 Sprites from TGA files.." // Change Title.
#define BPLCC_SCREEN_X 160 // 160x120 Size.
#define BPLCC_SCREEN_Y 120
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x000000FF // Transparent Color is RED - All Red Pixels of images is Transparent.
#define BPLCC_SCREEN_MAGNIFIER 2 // x2 Magnifier.
#define BPLCC_SHOWMOUSECURSOR false // Don't show cursor.
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00000000 // Black Clear Color.
#define BPLCC_RENDERTIME 0

BPLcc_Sprite SPR1, SPR2; // Defining two Sprite objects - SPR2 will me the background!
int x = 50,y = 50, Layer=10; // Sprite's 1 Coordinates & Layer.

void ToDo_Initialize(void) {
  // Creating the Sprite objects
	BPLcc_SPR_Create(&SPR1);
	BPLcc_SPR_Create(&SPR2);

	BPLcc_SPR_TGALoad(&SPR1,".\\Data\\Sprite1.tga"); // Loading from folder ./Data/ the Sprite1.tga image to SPR1.
	BPLcc_SPR_TGALoad(&SPR2,".\\Data\\Sprite2.tga"); // Loading from folder ./Data/ the Sprite2.tga image to SPR2.

}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
	// Set x,y accordinarly to Mouse Position.
	x = h2D.M.p.x / h2D.Screen_Magnifier;
	y = h2D.M.p.y / h2D.Screen_Magnifier;

	if (h2D.M.keys[MS_LB]==true) { h2D.M.keys[MS_LB]=false; Layer++; } // Left Button - Send SPR1 forward to background.
	// Right Button - Move SPR1 closer to foreground.
	if (h2D.M.keys[MS_RB]==true) { h2D.M.keys[MS_RB]=false; Layer--; if (Layer < 0) {Layer = 0; } }

}

void DrawLayers(void) {
	BPLcc_SPR_Put(&SPR2,0,0,10); // Put The BackGround (SPR2) At  0,0 and Layer 10.
	BPLcc_SPR_Put(&SPR1,x,y,Layer); // Put the SPR1

}


void ToDo_AtExitProgram(void) {
	// Releasing the memory that sprite Object's had allocate.
	BPLcc_SPR_Destroy(&SPR1);
	BPLcc_SPR_Destroy(&SPR2);
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

