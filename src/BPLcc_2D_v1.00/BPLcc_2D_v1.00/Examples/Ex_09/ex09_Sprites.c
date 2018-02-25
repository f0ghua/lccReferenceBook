#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;

#define BPLCC_WINDOW_TITLE "Ex.Collision Checking" // Change Title.
#define BPLCC_SCREEN_X 160
#define BPLCC_SCREEN_Y 120
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA
#define BPLCC_SCREEN_MAGNIFIER 1
#define BPLCC_SHOWMOUSECURSOR true
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00000080 // Blue Dark Clear Color.
#define BPLCC_RENDERTIME 40 // Do Render Every 0.04 sec.

BPLcc_Sprite S1, S2; // Declaring 2 sprites.
float x[2],y[2]; // Positions Of Sprites.
float dx[2], dy[2]; // Speeds Of Sprites.


void ToDo_Initialize(void) {
	// Creating the Sprites Objects.
	BPLcc_SPR_Create(&S1);
	BPLcc_SPR_Create(&S2);

	// Loading Sprites.
	BPLcc_SPR_TGALoad(&S1,".\\Data\\Sprite5.tga");
	BPLcc_SPR_TGALoad(&S2,".\\Data\\DiskBall.tga");

	// Setting Starting Position And Speeds.
	x[0]=0; y[0]=0; dx[0] = 6; dy[0]= 3;
	x[1]=120; y[1]=80; dx[1] = -2; dy[1]=-3;
}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
}

void DrawLayers(void) {
	int i;
	// Draw the Disks
	//BPLcc_SPR_Put(&S1,x[0],y[0],10); // We Don't Draw the S1 Sprite. First We Draw Any other sprite we want to check if S1 collides.
	BPLcc_SPR_Put(&S2,x[1],y[1],20);

	// Then We "Put" S1 Sprite As "Touch". The internal code will find which Layers this pixel touches.
	// And will give the to the S1.touch matrix as: touch[0 ... S1.tfr (S1.tfr+1) ... S1.tbck ... h2D.MaxLayer]
	// Meaning: From S1.touch[0] to  S1.touch[S1.tfr] we get the "foreground than the Sprite" layers touched and
	// from S1.touch[S1.tfr+1] to S1.touch[S1.tbck] we get the "background than the Sprite" layers touched.
	// Here We have one spite that we check if collide, so if it really collides then S1.touch[0] must have the
	// Value of the collision Sprite (S2), meaning S1.touch[0] = 20.
	BPLcc_SPR_Touch(&S1,x[0],y[0],10);

	// If S1 touched Layer 20, in which is S2 Sprite then...
	if (S1.touched[0]==20) {
		float xx, yy, R;
		//Recalculate the Speeds of both Sprites.....
		xx = (x[0]-x[1]); yy=(y[0]-y[1]); R=sqrt(xx*xx+yy*yy)/5;
		dx[0]=xx/R; dy[0]=yy/R;
		dx[1]=-xx/R; dy[1]=-yy/R;
	}

	// Finally after checking S1 for collision with other sprites, we put it so the user can see it.
	BPLcc_SPR_Put(&S1,x[0],y[0],10);


	// Updating position via speed here. We could do that and at the MainLoop.
	// But here the speed is updated every time we render, and not continiusly.
	for (i=0; i < 2; i++) {
		x[i]+=dx[i]; y[i]+=dy[i];
		if ((x[i] <= 0)||(x[i] > h2D.ScreenX-40)) { dx[i]=-dx[i]; }
		if ((y[i] <= 0)||(y[i] > h2D.ScreenY-40)) { dy[i]=-dy[i]; }
	}
}


void ToDo_AtExitProgram(void) {
	// Destroying The Sprites Objects.
	BPLcc_SPR_Destroy(&S1);
	BPLcc_SPR_Destroy(&S2);
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

