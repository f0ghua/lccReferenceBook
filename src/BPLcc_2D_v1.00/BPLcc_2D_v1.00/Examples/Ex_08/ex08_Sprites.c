#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;
/********* This example must be run as Console Application so the user can undestand it ********/
//  The Basic Idea is that Each Sprite is putted to a Layer. With the capability to retrive the
// most front Layer Number of a given position, it's possible to know which Sprite is right under the
// Cursor's mouse.
//
//  Some extra functions that users might find interesting/usefull in more complicated programs, are
// BPLcc_SPR_IPut(...) - Places a Sprite as Invisible, changing only the Layers of it's pixels.
// BPLcc_SPR_PutTrans(...) - Places a Sprite without affecting the Layers.
// BPLcc_SPR_IBoxSet(...) - It creates an Invisible Rectangle, changing only the Layers of the pixel it includes inside.


#define BPLCC_WINDOW_TITLE "Example - Selecting And Moving Sprites with Mouse..."
#define BPLCC_SCREEN_X 640 // 640x480
#define BPLCC_SCREEN_Y 480
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x000000FF // Red Is Set to be Transparent.
#define BPLCC_SCREEN_MAGNIFIER 1
#define BPLCC_SHOWMOUSECURSOR true
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00A0A0A0
#define BPLCC_RENDERTIME 0

BPLcc_Sprite S1, S2, S3, S4; // Declaring 4 Sprites.
int x[4], y[4], z[4]; // 4 Different positions for each sprite.
int xrel, yrel; // Relative positions - When click on a Sprite, they hold the distance between the
								// origin of the Sprite (upper left) and the mouse.
int SelectedSprite=-1; // Holding the selected sprite.. -1 means no sprite selected.

void ToDo_Initialize(void) {
	// Creating The Sprites.
	BPLcc_SPR_Create(&S1); BPLcc_SPR_Create(&S2); BPLcc_SPR_Create(&S3); BPLcc_SPR_Create(&S4);
	// Loading Texture to Sprites.
	BPLcc_SPR_TGALoad(&S1,".\\Data\\Sprite1.tga");
	BPLcc_SPR_TGALoad(&S2,".\\Data\\Sprite3.tga");
	BPLcc_SPR_TGALoad(&S3,".\\Data\\Sprite4.tga");

	// Copy Sprite S3 to S4.
	BPLcc_SPR_Copy(&S4, &S3);
	BPLcc_SPR_Flip(&S4,0); // Flip horizontal S4 Sprite.

	//Setting Default Coordinates - Each Sprite has it's own Layer... 10-13.
	x[0]=20; y[0]=20; z[0]=10;
	x[1]=400, y[1]=100; z[1]=11;
	x[2]=250, y[2]=160; z[2]=12;
	x[3]=150, y[3]=160; z[3]=13;
}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
	if (SelectedSprite >= 0) { // If user has select a sprite, update it's position by mouse - Do the relative correction.
		x[SelectedSprite] = h2D.M.p.x - xrel;
		y[SelectedSprite] = h2D.M.p.y - yrel;
	}

	// If Left mouse button is DOWN.
	if (h2D.M.keys[MS_LB]==true) {
		if (SelectedSprite == -1) { // If there is no sprite selected.
 			// Get the Layer under Mouse pointer, and substract 10 to take the Sprite number. F.e. 10-10 = [0].
			SelectedSprite = BPLcc_GetLayer(h2D.M.p.x, h2D.M.p.y) - 10;
			if (SelectedSprite > 100) { SelectedSprite = -1; } // Do a correction. So Not Select Background!(Its Error).
			else {
				// If a Sprite is selected, then calculate the relative position of mouse pointer to Sprite origin to
				// do the appropriate correction when moving.
				xrel = h2D.M.p.x - x[SelectedSprite]; yrel = h2D.M.p.y - y[SelectedSprite];
			}
		}
	} else { SelectedSprite = -1; } // If Left mouse button is UP, do not select anything.

}

void DrawLayers(void) {
	// Put the 4 Sprites to their positions.
	BPLcc_SPR_Put(&S1,x[0],y[0],z[0]);
	BPLcc_SPR_Put(&S2,x[1],y[1],z[1]);
	BPLcc_SPR_Put(&S3,x[2],y[2],z[2]);
	BPLcc_SPR_Put(&S4,x[3],y[3],z[3]);
}

void ToDo_AtExitProgram(void) {
	// Destroy The Sprites.
	BPLcc_SPR_Destroy(&S1);
	BPLcc_SPR_Destroy(&S2);
	BPLcc_SPR_Destroy(&S3);
	BPLcc_SPR_Destroy(&S4);
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

