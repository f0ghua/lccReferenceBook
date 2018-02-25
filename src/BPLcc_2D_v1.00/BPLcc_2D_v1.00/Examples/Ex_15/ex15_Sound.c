#include "BPLcc_2D.h"
#include "ex15_Resource.h" // Resources of the sound.

extern BPLcc_WinMain h2D;

#define BPLCC_WINDOW_TITLE "Example Frogy's Sound!" // Change Title.
#define BPLCC_SCREEN_X 320
#define BPLCC_SCREEN_Y 240
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA
#define BPLCC_SCREEN_MAGNIFIER 1
#define BPLCC_SHOWMOUSECURSOR false // No cursor!
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00A0A0A0
#define BPLCC_RENDERTIME 0


BPLcc_Anim Frogy; // Frogy animation
int x,y; // Frogy position.

const int N = 2000; // Number of other frogies!
int n = 100; // Number of the other Visible Froggies!
int xs[N], ys[N]; // Other Frogies Positions! :P

void ToDo_Initialize(void) {
	BPLcc_ANM_Create(&Frogy); // Create Frogy animation
	// Define the Sprites coordinates into the tga file.
	int Frogy_PosData[5*4] = {0,0,25,25, 25,0,25,25, 50,0,25,25, 75,0,25,25, 100,0,25,25 };
	// Load the animation.
	BPLcc_ANM_TGALoad(&Frogy, 5, ".\\Data\\Frogy.tga",Frogy_PosData);
  // Set 0.05 sec Sprite update for smoothing animation.
	Frogy.ChangeTime=50;

	srand((unsigned int)time(NULL)); // Set A Random Seed For Random numbers.
	for(int i=0; i < N; i++) {
		xs[i] = (int)((((float)rand())/0x8000)*290);
		ys[i] = (int)((((float)rand())/0x8000)*210);
	}
}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
	// Update the animation. (Default is Cyclic). If animation reached it's End's (0 or N-1), Update() returns true.
	if ((BPLcc_ANM_Update(&Frogy,1) == true) && ( Frogy.CurrSpr==0)) { // If the frog sit down
		Frogy.ChangeByTime=false; // Stop the animation
		BPLcc_ANM_Flip(&Frogy,0,true); // Flip Horizontal Frogy! (0:Horiz, 1:Vert).
	}

	x=h2D.M.p.x-12; // Update Frogy's Position.
	y=h2D.M.p.y-12-Frogy.CurrSpr*2; // Substract Some relative to Frogy's animation CurrentSprite for jumping.

	// If LeftMouse Button hit and Frogy is Down (Sprite 0)
	if ((Frogy.CurrSpr==0) && (h2D.M.keys[MS_LB]==true)) {
		h2D.M.keys[MS_LB]=false; // Set to unhit the mouse button.
		Frogy.ChangeByTime=true; // Set Frogy to do the animation.

		BPLcc_PlaySound(ID_SLRP); // Play the sound of Slourppp! :P
	}

	if (h2D.M.keys[MS_WHEEL] == true) {
		h2D.M.keys[MS_WHEEL] = false;
		if (h2D.M.Wheel == MS_WHEEL_UP) { n += 5; if (n > N) { n = N; } }
		else if (h2D.M.Wheel == MS_WHEEL_DOWN) { n -= 5; if (n < 0) { n = 0; } }
	}




}

void DrawLayers(void) {
	int i;
	// Put Somewhere Froggie.
	BPLcc_ANM_Put(&Frogy,x,y,0);

	// Put The other Froggies!
	for(i=0; i < n; i++) {
		BPLcc_ANM_Put(&Frogy,xs[i],ys[i],10);
	}

}


void ToDo_AtExitProgram(void) {
	BPLcc_ANM_Destroy(&Frogy);
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

