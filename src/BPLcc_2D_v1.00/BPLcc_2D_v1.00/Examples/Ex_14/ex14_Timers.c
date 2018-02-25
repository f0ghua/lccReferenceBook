#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;

#define BPLCC_WINDOW_TITLE "Example - Timers..." // Change window title.
#define BPLCC_SCREEN_X 320 // Screen 640x480
#define BPLCC_SCREEN_Y 240
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA
#define BPLCC_SCREEN_MAGNIFIER 1
#define BPLCC_SHOWMOUSECURSOR true
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00000000 // Clear Color BLACK.
#define BPLCC_RENDERTIME 0

BPLcc_Timer T; // Declare a Timer object.
BPLcc_uFont F10; // Declare a Font object.
int scores[5]={0}; // Make a score board.

void ToDo_Initialize(void) {
	// Create the Fonts.
	BPLcc_UFNT_Create(&F10);
	BPLcc_UFNT_TGALoad(&F10,135,12,12,".\\Data\\Fonts_Arial12GR.tga");
	BPLcc_UFNT_SetColor(&F10,255,255,0);

	// Create the Timer Object.
	BPLcc_TIME_Create(&T);

	// Set 5 Thresholds in msec, for the 5 timers we are going to use.
	int Timers_Thresholds[5] = { 125, 250, 500, 1000, 5000 };
	// Create the 5 timers on T Object timer.
	BPLcc_TIME_Init(&T, 5, Timers_Thresholds);

}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
	// Update The Timers
	BPLcc_TIME_Update(&T);
	// Check each of 5 timers if has reached it's threshold in time.
	int i;
	for (i=0; i < 5; i++) {
		// and if the timer (i) have reach it, then increase the score accordinarly, Reseting the counter.
		if ( T.t[i] == true ) { scores[i]++; BPLcc_TIME_Clear(&T,i); }
	}
}

void DrawLayers(void) {
	// type some words...
	BPLcc_UFNT_Text(&F10,10,70,0,"Scores(Timers):");
	BPLcc_UFNT_Text(&F10,10,98,0,"0.125 0.25 0.5 1   5 (Sec)");
	// type the scores!
	// Each score increasing at 0.1, 0.25, 0.5, 1 and 5 seconds....
	BPLcc_UFNT_Text(&F10,20,118,0,"%3i %3i %3i %3i %3i", scores[0], scores[1], scores[2], scores[3], scores[4]);

	// In my PC there is a little dis-sychronization if the updates are too fast (for the case of 100 msec).
	// For 125 msec works right!
}


void ToDo_AtExitProgram(void) {
	// Destroy Our Objects.
	BPLcc_TIME_Destroy(&T);
	BPLcc_UFNT_Destroy(&F10);
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

