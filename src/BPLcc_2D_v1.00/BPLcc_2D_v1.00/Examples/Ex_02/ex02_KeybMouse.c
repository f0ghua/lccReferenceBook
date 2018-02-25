#include "BPLcc_2D.h"

extern BPLcc_WinMain h2D;

#define BPLCC_WINDOW_TITLE "Example Using Keyboard with Internal Keyb. Object - F4 Exit" // Change Title
#define BPLCC_SCREEN_X 320
#define BPLCC_SCREEN_Y 240
#define BPLCC_LAYERS 0xFFFF
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA
#define BPLCC_SCREEN_MAGNIFIER 2 // Set to 2.
#define BPLCC_SHOWMOUSECURSOR false // No Mouse cursor over the application.
#define BPLCC_EXIT_ON_ESCAPE false // Do not exit on ESC key. (But on F4 as we will see)
#define BPLCC_CLEARCOLOR 0x00000000 // Clear color Black.
#define BPLCC_RENDERTIME 0

BPLcc_StdGraph S; // A Stadar graphic to test the Keyboard.

int x=160,y=120; // Make two variables that will hold the position of a moving pixel.
int R=255, G=255, B=0; // Some variables to hold the color of our moving pixel.

void ToDo_Initialize(void) {
	BPLcc_SGR_Create(&S); // Create the stadar graphic s.
	BPLcc_SGR_Color(&S,R,G,B); // Give the S color yellow.
}

void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}


void ToDo_MainLoop(void) {
	if (h2D.K.keys[VK_F4] == true) { PostQuitMessage(0); } // If Key "F4" Pressed, Terminate the application.
	if (h2D.K.keys[VK_LEFT] == true) { // If Left Arrow pressed, move left (decr. x coord).
		x--; if (x < 0) { x=0; }
	}
	if (h2D.K.keys[VK_RIGHT] == true) { // If Right Arrow pressed, move right (inc. x coord).
		x++; if (x >= h2D.ScreenX) { x=h2D.ScreenX-1; } // If x is bigger than screen width, se it to right side.
	}
	if (h2D.K.keys[VK_UP] == true) { // If UP Arrow pressed, move up (decrease y coord).
		y--; if (y < 0) { y=0; }
	}
	if (h2D.K.keys[VK_DOWN] == true) { // If Down Arrow pressed, move down (inc. y coord).
		y++; if (y >= h2D.ScreenY) { y=h2D.ScreenY-1; } // If y is bigger than screen height, se it to down side.
	}
	// NOTICE that if you hold down the keys, the pixel consecutively moves...

	if (h2D.K.keys[VK_X] == true ) { // If Key "X" is pressed...
		R--; if (R < 0) { R=0; }    //Change accordinarly the R,G,B values ......
		G--; if (G < 0) { G=0; }
		B++; if (B > 255) { B=255;}
		BPLcc_SGR_Color(&S,R,G,B);  // And set the S object, that color.
	}
	if (h2D.K.keys[VK_Z] == true ) { // If Key "Z" is pressed...
		R++; if (R > 255) { R=255; }    //Change accordinarly the R,G,B values ......
		G++; if (G > 255) { G=255; }
		B--; if (B < 0) { B=0;}
		BPLcc_SGR_Color(&S,R,G,B);  // And set the S object, that color.
	}
	// NOTICE that if you hold down the keys, the pixel consecutively change color...

	if (h2D.K.keys[VK_SPACE] == true ) {
		//h2D.K.keys[VK_SPACE]=false; // <--- Uncomment this comment and see the difference. The speed it's like typing.
		x+=5; if (x >= h2D.ScreenX) { x=0; }
		y+=5; if (y >= h2D.ScreenY) { y=0; }
	}

	// Let's Check Typing - Do see results on this must built the project as Console Application.
	if (h2D.K.keys[VK_RETURN] == true) { // If Enter Is Pressed.
		h2D.K.keys[VK_RETURN] = false; // Make it released so not to interact multiple times.
		// If application doesn't wait for string input from user, then enable the string input by keyboard and return...
		if (h2D.K.StringInput == false) { BPLcc_Keys_GetString(&(h2D.K),512); return; }
		// Else if Application has setted to string input, and ENTER pressed to declare the end of string...
		// type the string:
		printf("\n\n\nCommand inserted: %s",h2D.K.IString);
		h2D.K.StringInput=false;

		// If the user gave as string the word "Exit".. Exit the application.
		if (strcmp(h2D.K.IString,"Exit") == 0) { PostQuitMessage(0); }
	}


}

void DrawLayers(void) {
	BPLcc_SGR_SetPixel(&S,x,y,0); // Rendering a Pixel at position X,Y and at Layer 0 with color of S object.

	if (h2D.K.StringInput==true) { // If application waits for string input, write to stadar output the string.
		printf("\n%s",h2D.K.IString); // Printing the string to stadar output - Project must be build as Console Application.
	}
}


void ToDo_AtExitProgram(void) {
	BPLcc_SGR_Destroy(&S);
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

