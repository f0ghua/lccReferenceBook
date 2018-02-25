/* Basic structure of C code, in order to utilize BPLcc_2D Library.... (NOTE: I use TAB = 2 Spaces) */
#include "BPLcc_2D.h"																	/* Include Lib's definitions...											*/
#include "resources.h"

extern BPLcc_WinMain h2D;															/* Declaring External WinMain Structure.						*/

#define BPLCC_WINDOW_TITLE "BPLcc 2D : Girls Game 2D"	/* Window Title																			*/
#define BPLCC_SCREEN_X 500														/* Window Width																			*/
#define BPLCC_SCREEN_Y 550														/* Widnow Height																		*/
#define BPLCC_LAYERS 0xFFFF 													/* 65536 Layers - Depth															*/
#define BPLCC_TRANSPARENTCOLOR 0x00FAFAFA							/* Transparent Color. (RGB: 250,250,250)						*/
#define BPLCC_SCREEN_MAGNIFIER 1											/* Screen Magnifier.																*/
#define BPLCC_SHOWMOUSECURSOR true										/* Mouse Cursor Visible.														*/
#define BPLCC_EXIT_ON_ESCAPE true											/* If ESC Pressed the Application Ends.							*/
#define BPLCC_CLEARCOLOR 0x00000000										/* BackGround Color set to RGB: (160,160,160)				*/
#define BPLCC_RENDERTIME 0														/* Rendering Time Difference - 0 means Continusly.	*/

void LoadImages(void);

BPLcc_Sprite BoxImages[25], Back, Winner, Looser;
int mat[10][10] = {0};
POINT Pos;
bool Select=false;
int Selected=50;
int Found=0;
int WrongChoices=2;
BPLcc_Timer Tim;

/* Initialize here all your data ... */
void ToDo_Initialize(void) {
	BPLcc_PlaySound(SND01);
	int x=0,y=0,pos,i;
	LoadImages();
	srand((unsigned int)time(NULL));
	int bins[25];
	for (i=0; i < 25; i++) { bins[i]=4; }
	for (y=0; y < 500; y+=50) {
		for (x=0; x < 500; x+= 50) {
			pos = (int)((((float)rand())/0x8000)*25);
			while (1) {
				if (bins[pos] > 0) { bins[pos]--; mat[y/50][x/50]=pos; break;}
				pos++; if (pos >=25) { pos = 0; }
			}
		}
	}
	unsigned long timer[1]; timer[0] = 100;
	BPLcc_TIME_Create(&Tim);
	BPLcc_TIME_Init(&Tim,1,timer);

}

/* Use here any code, that might use the uMsg, wParam and lParam variables of the system */
void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
}

/* Use here any code that will have to running repeatedly - F.e. Cheking if a global variable has change repeatedly*/
void ToDo_MainLoop(void) {
	BPLcc_TIME_Update(&Tim);
	if (Tim.t[0]==true) { BPLcc_TIME_Clear(&Tim,0);
		int pos = (int)((((float)rand())/0x8000)*25);
		int Rot = ((int)((((float)rand())/0x8000)*4)+1)*90;
		BPLcc_SPR_Rotate(&(BoxImages[pos]),Rot);
	}
	if (h2D.M.keys[MS_LB] == true) { h2D.M.keys[MS_LB]=false;
		if (Select == false) { Select=true; Selected=BPLcc_GetLayer(h2D.M.p.x, h2D.M.p.y); Pos.x = h2D.M.p.x; Pos.y=h2D.M.p.y; }
		else { Select=false;
			if ((Selected==BPLcc_GetLayer(h2D.M.p.x, h2D.M.p.y)) && ((Pos.x/50 != h2D.M.p.x/50) || (Pos.y/50 != h2D.M.p.y/50))) {
				mat[Pos.y/50-1][Pos.x/50]=100;
				mat[h2D.M.p.y/50-1][h2D.M.p.x/50]=100;
				Selected = 50; Found ++;
				BPLcc_PlaySound(ID_SLRP);
			} else {
				WrongChoices--; if (WrongChoices <= 0) { Found = -1; }
				BPLcc_PlaySound(ID_PR);
			}
		}
	}
}

/* Draw your 2D Scene Here. This function is Called when ( h2D.DC_Draw == true ) */
void DrawLayers(void) {
	int x, y, type;

	if (Found >= 50) {BPLcc_SPR_Put(&Winner,190,5,200); }
	else if (Found < 0) { BPLcc_SPR_Put(&Looser,190,225,200); return; }

	BPLcc_SPR_Put(&Back,0,50,200);
	for (y=0; y < 500; y+=50) {
		for (x=0; x < 500; x+= 50) {
			type = mat[y/50][x/50];
			if (type < 50) {
				BPLcc_SPR_Put(&BoxImages[type],x,y+50,type);
			}
		}
	}
}

/* Use here any code that must be called at the closing of you applications - F.e. to free memory from your data */
void ToDo_AtExitProgram(void) {
	int i;
	for (i=0;i<25;i++) { BPLcc_SPR_Destroy(&(BoxImages[i])); }
	BPLcc_SPR_Destroy(&Back);
	BPLcc_SPR_Destroy(&Winner);
	BPLcc_SPR_Destroy(&Looser);
	BPLcc_TIME_Destroy(&Tim);
}


void LoadImages(void) {
	int i;
	for (i=0;i<25;i++) { BPLcc_SPR_Create(&(BoxImages[i])); }
	BPLcc_SPR_Create(&Back);
	BPLcc_SPR_Create(&Winner);
	BPLcc_SPR_Create(&Looser);
	BPLcc_SPR_Init(&(BoxImages[0]),ID01,50,50);
	BPLcc_SPR_Init(&(BoxImages[1]),ID02,50,50);
	BPLcc_SPR_Init(&(BoxImages[2]),ID03,50,50);
	BPLcc_SPR_Init(&(BoxImages[3]),ID04,50,50);
	BPLcc_SPR_Init(&(BoxImages[4]),ID05,50,50);
	BPLcc_SPR_Init(&(BoxImages[5]),ID06,50,50);
	BPLcc_SPR_Init(&(BoxImages[6]),ID07,50,50);
	BPLcc_SPR_Init(&(BoxImages[7]),ID08,50,50);
	BPLcc_SPR_Init(&(BoxImages[8]),ID09,50,50);
	BPLcc_SPR_Init(&(BoxImages[9]),ID10,50,50);
	BPLcc_SPR_Init(&(BoxImages[10]),ID11,50,50);
	BPLcc_SPR_Init(&(BoxImages[11]),ID12,50,50);
	BPLcc_SPR_Init(&(BoxImages[12]),ID13,50,50);
	BPLcc_SPR_Init(&(BoxImages[13]),ID14,50,50);
	BPLcc_SPR_Init(&(BoxImages[14]),ID15,50,50);
	BPLcc_SPR_Init(&(BoxImages[15]),ID16,50,50);
	BPLcc_SPR_Init(&(BoxImages[16]),ID17,50,50);
	BPLcc_SPR_Init(&(BoxImages[17]),ID18,50,50);
	BPLcc_SPR_Init(&(BoxImages[18]),ID19,50,50);
	BPLcc_SPR_Init(&(BoxImages[19]),ID20,50,50);
	BPLcc_SPR_Init(&(BoxImages[20]),ID21,50,50);
	BPLcc_SPR_Init(&(BoxImages[21]),ID22,50,50);
	BPLcc_SPR_Init(&(BoxImages[22]),ID23,50,50);
	BPLcc_SPR_Init(&(BoxImages[23]),ID24,50,50);
	BPLcc_SPR_Init(&(BoxImages[24]),ID25,50,50);
	BPLcc_SPR_Init(&Back,IDBACK,500,500);
	BPLcc_SPR_Init(&Winner,IDWINNER,120,40);
	BPLcc_SPR_Init(&Looser,IDLOOSER,120,40);
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
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_AUTO_KEYBOARD, true); // Enable Internal Keyboard Handler Object for updating.
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_AUTO_MOUSE, true); // Enable Internal Mouse Handler Object for updating.


/* The Below Statements Initilalize Window with the Resource ICON/CURSOR. If they are not used(Like now) the Library
	 uses the Default ICON (IDI_APPLICATION) and the Default Cursor (IDC_ARROW) defined in Win32... To Start with your
	 ICON / CURSOR change IDIICON100 / IDCURSOR200 names to your corresponding resources at the below statements.
	BPLcc_WinData_SetID(&h2D, BPLCC_INITWINOBJ_ICON, (LPCSTR) IDIICON100, false);
	BPLcc_WinData_SetID(&h2D, BPLCC_INITWINOBJ_CURSOR, (LPCSTR) IDCURSOR200, false);
*/
	BPLcc_WinData_SetID(&h2D, BPLCC_INITWINOBJ_ICON, (LPCSTR) API_ICON, false);
BPLcc_WinData_SetID(&h2D, BPLCC_INITWINOBJ_CURSOR, (LPCSTR) API_CUR, false);
	/* Use this Statement's to Set Your Window OpenGL Application Position to Screen. CW_USEDEFAULT uses windows default(moving)*/
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINPOSX, 23); /* Upper Left corner Of Window Start At Position (23,23) */
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINPOSY, 23);

	/* The relative coordinates in pixels, where the game's screen start's (without the window's graphics...)
		 F.e. here means the Drawing Area is 3 pixels left to the position of the system Window of our application,
		 and 22 means the Drawing Area is 22 pixel of the upper side of the system Window (-The 22 pixels are for the
		 Title Bar, in my computer.) */
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINRELPOSX, 3);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINRELPOSY, 22);
}

/*****************************************************
  About BPLcc_2D Library:
	Author / Creator	: Vasileios Kon. Pothos	|	GR'05
	E-mail						: terablade2001@yahoo.gr
	Starting Date			: 11/24/05
	Licence						: GPL
******************************************************/

