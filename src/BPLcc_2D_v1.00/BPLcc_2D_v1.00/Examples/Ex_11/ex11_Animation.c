#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;
// Animation Has many common Functions Like Sprite.
// So only an example will be for animation - this one...
// Most functions are alike Sprite's functions...
#define BPLCC_WINDOW_TITLE "Example- Handling Animations with Animation Objects" // Change Title.
#define BPLCC_SCREEN_X 320
#define BPLCC_SCREEN_Y 240
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x000000FF // Red transparent Color
#define BPLCC_SCREEN_MAGNIFIER 2 // x2 Magnifier.
#define BPLCC_SHOWMOUSECURSOR false // No cursor.
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00000000 // Black Clear Color.
#define BPLCC_RENDERTIME 0

BPLcc_Anim A1, A2, A3; // Declaring 3 Animation Objects.
int x, y;

void ToDo_Initialize(void) {
	// Creating the Animation Objects.
	BPLcc_ANM_Create(&A1); BPLcc_ANM_Create(&A2); BPLcc_ANM_Create(&A3);

	// Defining a Matrix that contains the X,Y,Widht,Height of each Sprite of the animation.
	// Because the testing animation has 4 sprites, we need 4*4 = 16 data on this matrix.
	// Notice that all Sprites of the animation has the same size (39x39).
	int A1_PosData[4*4] = { 0,0,39,39, 39,0,39,39, 78,0,39,39, 117,0,39,39};
	// Creating the animation from the Anim01.tga file.
	BPLcc_ANM_TGALoad(&A1, 4, ".\\Data\\Anim01.tga",A1_PosData);

	A1.CyclicAnim=false; // Setting that the Animation won't be "cyclic-playing" but "linear-playing"
	A1.ChangeByTime=true; // Animation will change by the time! To check the time an Update must be done at ToDo_MainLoop().
	A1.ChangeTime=500; // Every 0.5 second Animation must update.

	// Creating a Copy of Animation A1, to Animation A2.
	BPLcc_ANM_Copy(&A2,&A1);
	A2.CyclicAnim=true; // A2 will Change circular.
	A2.ChangeTime=100; // And will change every 0.1 sec.

	// Creating a Copy of A1 to A3.
	BPLcc_ANM_Copy(&A3, &A1);
	A3.ChangeByTime=false; // Animation Won't Change By Time!
	BPLcc_ANM_SetSprite(&A3,3); // Set as selected sprite of the Animation the 4rth Sprite. (0,1,2,[3])

}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
	BPLcc_ANM_Update(&A1,1); // Update Animation status - If redefining time has pass, Anim Will change.
	BPLcc_ANM_Update(&A2,1); // Update Animation status - If redefining time has pass, Anim Will change.
	BPLcc_ANM_Update(&A3,1); // A3 is not change by time,
	// Setting A1's positions...
	x = h2D.M.p.x / h2D.Screen_Magnifier - 39/2;
	y = h2D.M.p.y / h2D.Screen_Magnifier - 39/2;

	// If Left mouse button is pressed - increase Sprite Status for the animation.
	if (h2D.M.keys[MS_LB]==true) {h2D.M.keys[MS_LB]=false;
		A3.CurrSpr++; A3.CurrSpr%=4;
	}
  // If Right mouse button is pressed - decrease Sprite Status for the animation.
	if (h2D.M.keys[MS_RB]==true) {h2D.M.keys[MS_RB]=false;
		A3.CurrSpr--; if (A3.CurrSpr < 0) { A3.CurrSpr = 3; }
	}
}

void DrawLayers(void) {
	// Draw the three animations
	BPLcc_ANM_Put(&A1,x,y,0);
	BPLcc_ANM_Put(&A2,10,90,0);
	BPLcc_ANM_Put(&A3,90,10,0);
}


void ToDo_AtExitProgram(void) {
	// Destroying the Animation Objects.
	BPLcc_ANM_Destroy(&A1);
	BPLcc_ANM_Destroy(&A2);
	BPLcc_ANM_Destroy(&A3);
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

