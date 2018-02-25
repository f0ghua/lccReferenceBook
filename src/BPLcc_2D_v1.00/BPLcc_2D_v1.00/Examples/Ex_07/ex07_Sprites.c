#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;

#define BPLCC_WINDOW_TITLE "Example Sprites - Extra Support Capabilities Part A" // Change the Title.
#define BPLCC_SCREEN_X 320
#define BPLCC_SCREEN_Y 240
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA
#define BPLCC_SCREEN_MAGNIFIER 2 // x2 Magnifier.
#define BPLCC_SHOWMOUSECURSOR true
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00000000 // Black Clear Color.
#define BPLCC_RENDERTIME 0

BPLcc_Sprite Back, S1, S2, S3, S4; // Declare 5 Sprites.
int x=50, y=50; // Setting the position of the S1 Sprite (FUN STAMP).


void ToDo_Initialize(void) {
	// Create the Sprites Objects.
	BPLcc_SPR_Create(&Back); BPLcc_SPR_Create(&S1);  BPLcc_SPR_Create(&S2);  BPLcc_SPR_Create(&S3); BPLcc_SPR_Create(&S4);
	// Loading Sprites from TGA files.
	BPLcc_SPR_TGALoad(&Back,".\\Data\\GrayBackground.tga");
	BPLcc_SPR_TGALoad(&S1,".\\Data\\FunStamp.tga");
	BPLcc_SPR_TGALoad(&S2,".\\Data\\RightBlock.tga");
	BPLcc_SPR_TGALoad(&S3,".\\Data\\FunStamp.tga");

	BPLcc_SPR_SetVis(&S2,false); // Set Sprite's S2 visibility = false. (Not Visible).

	srand((unsigned int)time(NULL)); // Take a random SEED for random colors.
 	// Each time you run the program the S3 Sprite will have different colors. Variable 1 defines the
	// way the random colors will be produced. Version 1.00 of BPLcc_2D lib support only one method.
	BPLcc_SPR_RandomColor(&S3,1,0,0,S3.sx,S3.sy);


}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
	// Recalculate x,y from mouse position - Minus 20, to put the Sprite's center at the mouse pointer.
	x = (h2D.M.p.x) / h2D.Screen_Magnifier - 20;
	y = (h2D.M.p.y) / h2D.Screen_Magnifier - 20;

	// If mouse Left button pressed - put a copy of the S1 Sprite (STAMP) to Sprite Back (BACKGROUND) at the
	// x,y position.
	if (h2D.M.keys[MS_LB]==true) { h2D.M.keys[MS_LB] = false;
		// Copy from Sprite S1 (from position 0,0),  to Sprite Back (to pos x,y), All the width & height of the sprite S1.
		BPLcc_SPR_PutCopy(&S1, &Back, x, y, 0, 0, -1, -1);
		//BPLcc_SPR_PutCopy(&S1, &Back, x, y, 10, 10, 20, 20); // <- Try this do understand better.
	}

	// If right mouse button is pressed, then change the visibility status of S2 Sprite.
	if (h2D.M.keys[MS_RB]==true) { h2D.M.keys[MS_RB] = false;
		BPLcc_SPR_SetVis(&S2,!S2.Vis);
	}

	// If Wheel is rolled...
	if (h2D.M.keys[MS_WHEEL] == true) { h2D.M.keys[MS_WHEEL] = false;
		if (h2D.M.Wheel == MS_WHEEL_UP)	{ // if Rolled up
			BPLcc_SPR_Rotate(&S1,-90);     // Rotate -90 degrees.
		} else if (h2D.M.Wheel == MS_WHEEL_DOWN)	{ // If Rolled down
			BPLcc_SPR_Rotate(&S1,90);      // Rotate 90 degrees.
		}
	}

	// If Middle Mouse button is pressed ...
	if (h2D.M.keys[MS_MB] == true) { h2D.M.keys[MS_MB]= false;
		BPLcc_SPR_Flip(&S3,1);  // Flip Vertical (1) the Sprite S3. ((0) for horizontal flip).
	}

}

void DrawLayers(void) {
	BPLcc_SPR_Put(&Back,0,0,1000); // Put Background At Layer 1000.
	BPLcc_SPR_Put(&S1,x,y,50); // Put S1 (FUN STAMP) at Layer 50.
	BPLcc_SPR_Put(&S2,195,0,40); // Put S2 (BLOCK) at layer 50.
	BPLcc_SPR_Put(&S3,10,10,999); // Put the S3 Sprite.
}


void ToDo_AtExitProgram(void) {
	// Destroy The Sprites.
	BPLcc_SPR_Destroy(&Back); BPLcc_SPR_Destroy(&S1); BPLcc_SPR_Destroy(&S2); BPLcc_SPR_Destroy(&S3); BPLcc_SPR_Destroy(&S4);
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

