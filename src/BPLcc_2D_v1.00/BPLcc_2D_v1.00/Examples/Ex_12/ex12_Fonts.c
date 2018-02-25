#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;

#define BPLCC_WINDOW_TITLE "Example Loading TGA Fonts" // Change Title.
#define BPLCC_SCREEN_X 320
#define BPLCC_SCREEN_Y 240
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA
#define BPLCC_SCREEN_MAGNIFIER 1
#define BPLCC_SHOWMOUSECURSOR true
#define BPLCC_EXIT_ON_ESCAPE true
#define BPLCC_CLEARCOLOR 0x00000000 // Black Background..
#define BPLCC_RENDERTIME 0

BPLcc_uFont F1, F2, F3; // Declaring 2 Fonts.

void ToDo_Initialize(void) {
	// Creating the Fonts.
	BPLcc_UFNT_Create(&F1);
	BPLcc_UFNT_Create(&F2);
	BPLcc_UFNT_Create(&F3);

	// Loading Font F1 by TGA File.
	// 135 = Characters Defined in the file. 12, 12 is the width and height of any character of this set. And finally the file TGA.
	BPLcc_UFNT_TGALoad(&F1,135,12,12,".\\Data\\Fonts_Arial12GR.tga");
	BPLcc_UFNT_SetColor(&F1,255,0,0); // Setting Red Color.
	BPLcc_UFNT_SetSpacing(&F1,-4); // Setting Spacing to -4.

	// Loading Font F2 by TGA File.
	// 73 = Characters Defined in the file. 10, 10 is the width and height of any character of this set. And finally the file TGA.
	BPLcc_UFNT_TGALoad(&F2,73,10,10,".\\Data\\Fonts_Arial10.tga");
	BPLcc_UFNT_SetColor(&F2,0,255,0); // Setting Green Color.
	//BPLcc_UFNT_SetSpacing(&F2,0); // Setting Spacing to +0 - Default.

	// Loading Font 3 by TGA File. 73 Characters , 5x5 pixels each.
	BPLcc_UFNT_TGALoad(&F3,73,5,5,".\\Data\\Fonts_5x5.tga");
	BPLcc_UFNT_SetColor(&F3,255,255,0); // Setting Yellow Color.

}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
}

void DrawLayers(void) {
	// Typing with F1 at Layer 10.
	BPLcc_UFNT_SetTrans(&F1,true); // Setting F1 Font as Transparent - Doesn't affect Layers (Z).
	BPLcc_UFNT_Text(&F1,10,10,10,"Hello!");
	BPLcc_UFNT_SetTrans(&F1,false); // Setting F1 Font as not Transparent - Affect Layers.
	BPLcc_UFNT_Text(&F1,10,35,10,"BLcc 2D is a free code source");
	BPLcc_UFNT_Text(&F1,10,50,10,"for easy creating 2D applications.");
	BPLcc_UFNT_Text(&F1,10,65,10,"It was specially design for 2D Games.");

	// Typing with F2 at Layer 11.
	// We get mouse X,P pos and the Layer number at this position (Z). Notice the following.
	// If you go mouse over a letter of the phrase : "Hello!" because the F1 there is set to be Transparent, you don't take 10 as Z.
	// If you go mouse over all the rest red letters notice that when you touch a letter, the Z changes to 10 (The Layer we put them).
	// Finally notice that if you go cursor to Green or Yellow letters Z does not change to 11 and 12 accordinarly.
	// That's because what BPLcc_GetLayer(h2D.M.p.x, h2D.M.p.y) is calculated, the Green And Yellow font's haven't been placed yet!
	BPLcc_UFNT_Text(&F2,0,90,11,"MOUSE (X: %i, Y: %i, Z: %i)",h2D.M.p.x, h2D.M.p.y, BPLcc_GetLayer(h2D.M.p.x, h2D.M.p.y));


	// Typing with F3 at Layer 13.
	// Notice that with yellow letters you can see the Layer(Z) of Green letters, and the non Transparent Red's, but you
	// can not see the Layer(Z) of the yellow letters. The answer is the same as above! :)
	BPLcc_UFNT_Text(&F3,5,110,11,"MOUSE (X: %i, Y: %i, Z: %i)",h2D.M.p.x, h2D.M.p.y, BPLcc_GetLayer(h2D.M.p.x, h2D.M.p.y));
}


void ToDo_AtExitProgram(void) {
	// Destroying the Fonts.
	BPLcc_UFNT_Destroy(&F1);
	BPLcc_UFNT_Destroy(&F2);
	BPLcc_UFNT_Destroy(&F3);
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

