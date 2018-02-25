/********************************************************
  About BPLcc_2D Library:
	Author / Creator	: Vasileios Kon. Pothos	|	GR'05
	E-mail						: terablade2001@yahoo.gr
	Starting Date			: 11/24/05
	Version						: 1.00 (If modify change the version)
	Licence						: GPL
*********************************************************/
#ifndef BPLCC_2D_H
#define BPLCC_2D_H
#include "windows.h"
#include "stdio.h"
#include "stdlib.h"
#include "mmsystem.h"
#include "time.h"
#include "math.h"
#pragma lib <winmm.lib>

#define BPLcc_Pi 3.1415926535897932384626433832795
#define true 1
#define false 0
typedef unsigned char bool;
                                                                           /* Color Struct for user */
/*--------------------------------------------------------------------------------------------------*/
/* Color "Class" Decleration */
struct sct_BPLcc_Color {
	unsigned char r, g, b;
	unsigned long Col;
};
typedef struct sct_BPLcc_Color BPLcc_Color;
                                                                                      /* ViewMatrix */
/*--------------------------------------------------------------------------------------------------*/
/* ViewMatrix "Class" Decleration */
struct sct_BPLcc_ViewMatrix {
	unsigned char *rgb, *Crgb;
	int *Lev, *CLev;
	int sx,sy;
	unsigned char ClearR, ClearG, ClearB;
	HBITMAP T;
};
typedef struct sct_BPLcc_ViewMatrix BPLcc_ViewMatrix;
                                                                                        /* KeyBoard */
/*--------------------------------------------------------------------------------------------------*/
/* Keyboard "Class" Decleration */
struct sct_BP_KeyBoard {
	bool keys[256], keysb[256];
	int Last;
	char IString[1024];
	bool StringInput;
	int StrNum, MaxChars;
};
typedef struct sct_BP_KeyBoard BPLcc_Keys;
                                                                                           /* Mouse */
/*--------------------------------------------------------------------------------------------------*/
/* Mouse "Class" Decleration */
struct str_BPLcc_Mouse {
	bool keys[7], keysb[7], bCursorState;
	int Last, CursorState;
	short Wheel;
	POINT pos, p;
};
typedef struct str_BPLcc_Mouse BPLcc_Mouse;
                                                                         /* BPLcc Basic Image Struct*/
/*--------------------------------------------------------------------------------------------------*/
/* Basic Image Structure.. All image formats must end at this structure in order to use textures. */
struct sct_BPLcc_Image{
	unsigned int sizeX, sizeY;
  unsigned char *data;
};
typedef struct sct_BPLcc_Image BPLcc_Image;

                                                                               /* Win Main Structure*/
/*--------------------------------------------------------------------------------------------------*/
/* Win Main Data Decleration... - The Basic Struct.*/
struct sct_BPLcc_WinMain {
	char Name[256], ClassName[256];
	HWND hWnd;
	DWORD dwStyle;
	HDC hDC, MainDC, FontDC, TransDC;
	HINSTANCE hInst, hInstICON, hInstCURSOR;
	LPCSTR DefaultIcon, DefaultCursor;
	RECT hRect;
	BITMAPINFO BitmapInfo;
	LPVOID MainBitmapData;
	bool DC_Draw, RenderByTime;
	unsigned long RenderTime, RenderLastTime,RenderNowTime;
	LPWINDOWPOS lpwp;
	int Style, BackgroundBrush, ExitOnESC, ShowCursor, WinPosX, WinPosY, Auto_Keyboard, Auto_Mouse;
	int winx, winy, ScreenX, ScreenY, Transparent_Color, Clear_Color, Screen_Magnifier, MaxLayer;
	int RelWinx, RelWiny;
	BPLcc_ViewMatrix ViewMatrix;
	BPLcc_Keys K;
	BPLcc_Mouse M;
};

typedef struct sct_BPLcc_WinMain BPLcc_WinMain;
                                                                                 /* Sprite Structure*/
/*--------------------------------------------------------------------------------------------------*/
/* This Stucture is the basic unit of BPLcc_2D lib. Defines the SPRITES. */
struct sct_BPLcc_Sprite {
	int i,j,k,sx,sy;
	unsigned char *mat;
	unsigned long *mat32;
	unsigned long TransColor;
	unsigned char TransR, TransG, TransB;
	bool Vis;
	int* touched;
	int tfr,tbck;
};
typedef struct sct_BPLcc_Sprite BPLcc_Sprite;
                                                                     /* Sprite's Animation Structure*/
/*--------------------------------------------------------------------------------------------------*/
/* A structure to control grouped SPRITES, in order to do animation Sprites.*/
struct sct_BPLcc_Anim {
	int xpos, ypos, dpos;
	bool Vis;
	BPLcc_Sprite *AnSpr;
	int CurrSpr, NSprites, sx, sy;
	int *Msx, *Msy, *touched;
	bool ContChange, ChangeByTime;
	unsigned long ChangeTime, ChangeLastTime,ChangeNowTime;
	unsigned long TransColor;
	int tfr,tbck;
	unsigned char TransR, TransG, TransB;
	int CyclicUpdate;
	bool CyclicAnim;
};
typedef struct sct_BPLcc_Anim BPLcc_Anim;
                                                                    /* User Defined Fonts Structure */
/*--------------------------------------------------------------------------------------------------*/
/* Creating FONT Images to SPRITES and controling them */
struct sct_BPLcc_uFont {
	int FX, FY, xpos, ypos, dpos, MaxFonts, Space;
	unsigned char CR, CG, CB;
	BPLcc_Sprite *Let;
	unsigned char chars[256];
	bool TextTrans;
};
typedef struct sct_BPLcc_uFont BPLcc_uFont;

                                                                        /* Timing Control Structure */
/*--------------------------------------------------------------------------------------------------*/
/* Creating &Handling Timers for User Use. */
struct sct_BPLcc_Timer {
	bool *t;
	unsigned long *LTime;
	unsigned long *TThres;
	int MaxNum;
};
typedef struct sct_BPLcc_Timer BPLcc_Timer;

                                                                    /* Std. Simple Graphics Structure */
/*----------------------------------------------------------------------------------------------------*/
/* Data for simple objects like, lines, rectangles, circles e.t.c. - Doesn't have to be all, Sprites! */
struct sct_BPLcc_StdGraph {
	unsigned long Col;
	double RDev;
};
typedef struct sct_BPLcc_StdGraph BPLcc_StdGraph;


/******************************************************************************************************
 ******************************************************************************************************
                            User's BASIC Functions
 ******************************************************************************************************
 ******************************************************************************************************/
void ToDo_Initialize(void);
void ToDo_WinProcMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
void ToDo_MainLoop(void);
void ToDo_AtExitProgram(void);
void DrawLayers(void);

void BPLcc_Window_Parameters_Set(void);
void BPLcc_WinData_Set(BPLcc_WinMain *hObj, int SelectValue, int SetValue);
void BPLcc_WinData_SetPc(BPLcc_WinMain *hObj, int SelectValue, char* SetValue);
void BPLcc_WinData_SetID(BPLcc_WinMain *hObj, int SelectValue, LPCSTR SetValue, bool Default);

/* Extra functions! */
int BPLcc_GetLayer(int x, int y);
BPLcc_Color BPLcc_GetColor(int x, int y);
bool BPLcc_PlaySound (const int Res);

/******************************************************************************************************
 ******************************************************************************************************
                            Library's Supporting Functions
 ******************************************************************************************************
 ******************************************************************************************************/
bool CopyBitmap(HDC* OhDC,HBITMAP *im, int x, int y, int xsize, int ysize, DWORD type);


/************************************* ViewMatrix Functions *****************************************************/
void BPLcc_VMAT_Create(BPLcc_ViewMatrix* hObj, BPLcc_WinMain* WinMain);
void BPLcc_VMAT_Destroy(BPLcc_ViewMatrix* hObj);
void BPLcc_VMAT_CopyToDC(BPLcc_ViewMatrix* hObj, HDC* targetDC );
void BPLcc_VMAT_Reset(BPLcc_ViewMatrix* hObj);

/************************************* Keyboard Functions *****************************************************/
void BPLcc_Keys_Create(BPLcc_Keys* hObj);
void BPLcc_Keys_Update(BPLcc_Keys* hObj, UINT uMsg, WPARAM wParam);
bool BPLcc_Keys_Key(BPLcc_Keys* hObj, int key);
void BPLcc_Keys_SetALL(BPLcc_Keys* hObj, bool val);
bool BPLcc_Keys_Shift(BPLcc_Keys* hObj);
bool BPLcc_Keys_Ctrl(BPLcc_Keys* hObj);
void BPLcc_Keys_GetString(BPLcc_Keys* hObj, int maxchars);

/************************************* Mouse Functions *****************************************************/
void BPLcc_Mus_Create(BPLcc_Mouse* hObj);
void BPLcc_Mus_Update(BPLcc_Mouse* hObj, UINT uMsg, WPARAM wParam);
void BPLcc_Mus_ClWheel(BPLcc_Mouse* hObj);
void BPLcc_Mus_ClDBLClk(BPLcc_Mouse* hObj);
bool BPLcc_Mus_Key(BPLcc_Mouse* hObj, int key);
void BPLcc_Mus_SetPos(BPLcc_Mouse* hObj, int x, int y);
void BPLcc_Mus_ShowC(BPLcc_Mouse* hObj, bool state);

/************************************* TGA Files Loading Functions *********************************************/
void BPLcc_TGA_Create(BPLcc_Image* hObj, char *Filename);
void BPLcc_TGA_Reload(BPLcc_Image* hObj, char *Filename);
void BPLcc_TGA_Destroy(BPLcc_Image* hObj);
void BPLcc_TGA_Load(BPLcc_Image* hObj, char *Filename);

/************************************* BPLcc_Sprite Functions **************************************************/
void BPLcc_SPR_Create(BPLcc_Sprite* hObj);
void BPLcc_SPR_Destroy(BPLcc_Sprite* hObj);
void BPLcc_SPR_Put(BPLcc_Sprite* hObj, int x, int y, int c);
void BPLcc_SPR_Put2(BPLcc_Sprite* hObj, int x, int y);
void BPLcc_SPR_PutCopy(BPLcc_Sprite* hObj, BPLcc_Sprite* hObj2, int x, int y, int x1, int y1, int width, int height);
void BPLcc_SPR_IPut(BPLcc_Sprite* hObj, int x, int y, int c);
void BPLcc_SPR_PutTrans(BPLcc_Sprite* hObj, int x, int y, int c);
void BPLcc_SPR_IBoxSet(BPLcc_Sprite* hObj, int x, int y, int c, int sizex, int sizey);
void BPLcc_SPR_SetVis(BPLcc_Sprite* hObj, bool Val);
bool BPLcc_SPR_Init(BPLcc_Sprite* hObj, int Resourc, int sxx, int syy);
bool BPLcc_SPR_InitByMatrix(BPLcc_Sprite* hObj, int sxx, int syy, unsigned long* matrix);
void BPLcc_SPR_Touch(BPLcc_Sprite* hObj, int x, int y, int c);
void BPLcc_SPR_Flip(BPLcc_Sprite* hObj, int horvert);
void BPLcc_SPR_Rotate(BPLcc_Sprite* hObj, int Degrees);
void BPLcc_SPR_SetTransRGBs(BPLcc_Sprite* hObj, int r1, int r2, int g1, int g2, int b1, int b2);
bool BPLcc_SPR_TGALoad(BPLcc_Sprite* hObj, char* filename);
void BPLcc_SPR_RandomColor(BPLcc_Sprite* hObj, int mix, int x, int y, int sizex, int sizey);
void BPLcc_SPR_Copy(BPLcc_Sprite* hObj, BPLcc_Sprite* hObj2);

/************************************* BPLcc_Anim Functions **************************************************/
void BPLcc_ANM_Create(BPLcc_Anim* hObj);
void BPLcc_ANM_Destroy(BPLcc_Anim* hObj);
bool BPLcc_ANM_LoadSprites(BPLcc_Anim* hObj, int* ResourcMat, int NumSpr, int sxx, int syy, int* Msxx, int* Msyy);
void BPLcc_ANM_SetSprite(BPLcc_Anim* hObj, int c);
void BPLcc_ANM_Put(BPLcc_Anim* hObj, int x, int y, int d);
void BPLcc_ANM_IPut(BPLcc_Anim* hObj, int x, int y, int c, int status);
void BPLcc_ANM_PutTrans(BPLcc_Anim* hObj, int x, int y, int c, int status);
void BPLcc_ANM_PutCopy(BPLcc_Anim* hObj, BPLcc_Sprite* hObj2, int x, int y, int numAn);
void BPLcc_ANM_PutXY(BPLcc_Anim* hObj, int d);
void BPLcc_ANM_PutXYZ(BPLcc_Anim* hObj);
void BPLcc_ANM_LoadByOne(BPLcc_Anim* hObj, int Resourc, int NumSpr, int fx, int fy, int* PosData);
bool BPLcc_ANM_Update(BPLcc_Anim* hObj, int dc);
void BPLcc_ANM_Touch(BPLcc_Anim* hObj, int x, int y, int c);
void BPLcc_ANM_Flip(BPLcc_Anim* hObj, int horvert, bool flipall);
void BPLcc_ANM_Rotate(BPLcc_Anim* hObj, int Degrees, bool rotall);
void BPLcc_ANM_SetTrans(BPLcc_Anim* hObj, int r1,int r2, int g1, int g2, int b1, int b2, int NumSpr);
bool BPLcc_ANM_TGALoad(BPLcc_Anim* hObj, int NumSpr,char *filename,int* PosData);
void BPLcc_ANM_Copy(BPLcc_Anim* hObj, BPLcc_Anim* hObj2);

/************************************* BPLcc_uFont Functions **************************************************/
void BPLcc_UFNT_Create(BPLcc_uFont* hObj);
void BPLcc_UFNT_Destroy(BPLcc_uFont* hObj);
bool BPLcc_UFNT_TGALoad(BPLcc_uFont* hObj, int NFonts, int fontx, int fonty, char* fname);
bool BPLcc_UFNT_CreateFont(BPLcc_uFont* hObj, int NFonts, int fontx, int fonty, int fboxx, int fboxy, int Resourc);
void BPLcc_UFNT_SetColor(BPLcc_uFont* hObj, int r, int g ,int b);
void BPLcc_UFNT_SetSpacing(BPLcc_uFont* hObj, int spacing);
void BPLcc_UFNT_SetTrans(BPLcc_uFont* hObj, bool onoff);
void BPLcc_UFNT_Text(BPLcc_uFont* hObj, int x, int y, int d, LPCTSTR str, ...);

/************************************* BPLcc_Time Functions **************************************************/
void BPLcc_TIME_Create(BPLcc_Timer* hObj);
void BPLcc_TIME_Destroy(BPLcc_Timer* hObj);
void BPLcc_TIME_Init(BPLcc_Timer* hObj, int Ntim, unsigned long* Thres);
void BPLcc_TIME_Update(BPLcc_Timer* hObj);
void BPLcc_TIME_Clear(BPLcc_Timer* hObj, int Timer);
void BPLcc_TIME_ClearAll(BPLcc_Timer* hObj);

/************************************* BPLcc_StdGraph Functions **************************************************/
void BPLcc_SGR_Create(BPLcc_StdGraph* hObj);
void BPLcc_SGR_Destroy(BPLcc_StdGraph* hObj);
void BPLcc_SGR_Color(BPLcc_StdGraph* hObj, int r, int g, int b);
void BPLcc_SGR_Step(BPLcc_StdGraph* hObj, double RD);
void BPLcc_SGR_Line(BPLcc_StdGraph* hObj, int x1, int y1, int x2, int y2, int Layer);
void BPLcc_SGR_Rect(BPLcc_StdGraph* hObj, int x1, int y1, int x2, int y2, int Layer);
void BPLcc_SGR_SetPixel(BPLcc_StdGraph* hObj, int x, int y, int Layer);
void BPLcc_SGR_Ellipse(BPLcc_StdGraph* hObj, int x1, int y1, int Layer, float Rx, float Ry);
void BPLcc_SGR_Circle(BPLcc_StdGraph* hObj, int x1, int y1, int Layer, float R);

/*****************************************************************************************************************/
/************************************* Definitions For User Use **************************************************/
/*****************************************************************************************************************/
/* Arguments for changing the variables of the parameters of
	h2D, for the Window Processes of the lib... */
#define BPLCC_INITWINOBJ_NAME 1
#define BPLCC_INITWINOBJ_CLASSNAME 2
#define BPLCC_INITWINOBJ_STYLE 3
#define BPLCC_INITWINOBJ_DWSTYLE 4
#define BPLCC_INITWINOBJ_BACKBRUSH 5
#define BPLCC_INITWINOBJ_FULLSCR_APPSTYLE 6
#define BPLCC_INITWINOBJ_WINDOWSCR_APPSTYLE 7
#define BPLCC_INITWINOBJ_SCR_WIDTH 8
#define BPLCC_INITWINOBJ_SCR_HEIGHT 9
#define BPLCC_INITWINOBJ_SCR_LAYERS 10
#define BPLCC_INITWINOBJ_SCR_TRANSCOLOR 24
#define BPLCC_INITWINOBJ_SCR_EXITONESCAPE 11
#define BPLCC_INITWINOBJ_SCR_SHOWCURSOR 12
#define BPLCC_INITWINOBJ_ICON 13
#define BPLCC_INITWINOBJ_CURSOR 14
#define BPLCC_INITWINOBJ_WINPOSX 15
#define BPLCC_INITWINOBJ_WINPOSY 16
#define BPLCC_INITWINOBJ_WINRELPOSX 17
#define BPLCC_INITWINOBJ_WINRELPOSY 18
#define BPLCC_INITWINOBJ_SCR_MAGNIFIER 19
#define BPLCC_INITWINOBJ_SCR_CLEARCOLOR 20
#define BPLCC_INITWINOBJ_RENDERTIME 21
#define BPLCC_INITWINOBJ_AUTO_KEYBOARD 22
#define BPLCC_INITWINOBJ_AUTO_MOUSE 23


/* MOUSE STATES */
#define MS_DLB 0x00
#define MS_LB 0x01
#define MS_DRB 0x02
#define MS_RB 0x03
#define MS_DMB 0x04
#define MS_MB 0x05
#define MS_WHEEL 0x06
#define MS_WHEEL_UP 120
#define MS_WHEEL_DOWN -120

/* KEYCODES */
#define VK_A 0x41	/* Letters A-Z */
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_I 0x49
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define VK_M 0x4D
#define VK_N 0x4E
#define VK_O 0x4F
#define VK_P 0x50
#define VK_Q 0x51
#define VK_R 0x52
#define VK_S 0x53
#define VK_T 0x54
#define VK_U 0x55
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
#define VK_Y 0x59
#define VK_Z 0x5A

#define VK_0 0x30	/* Numbers 0-9 By Basic Keyboard */
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39

#define VK_N0 0x60 /* Numbers 0-9 By Num-Pad Keys */
#define VK_N1 0x61
#define VK_N2 0x62
#define VK_N3 0x63
#define VK_N4 0x64
#define VK_N5 0x65
#define VK_N6 0x66
#define VK_N7 0x67
#define VK_N8 0x68
#define VK_N9 0x69


#endif
