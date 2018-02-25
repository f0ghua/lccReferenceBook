/********************************************************
  About BPLcc_2D Library:
	Author / Creator	: Vasileios Kon. Pothos	|	GR'05
	E-mail						: terablade2001@yahoo.gr
	Starting Date			: 11/24/05
	Version						: 1.00 (If modify change the version)
	Licence						: GPL
*********************************************************/
#include "BPLcc_2D.h"



/*************************************************************************************************************
 *************************************************************************************************************
							Windows Initialization Functions							WWWWWWW
 *************************************************************************************************************
 *************************************************************************************************************/
BPLcc_WinMain h2D;
HWND hWnd;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);
LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam);
WPARAM MainLoop(void);

/* WIN MAIN ****************************************************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow) {
	int i,j;
	h2D.hInst=hInstance;
	h2D.Screen_Magnifier=1; h2D.ScreenX=640; h2D.ScreenY=480;
	strcpy(h2D.ClassName,"BPLcc 2D"); strcpy(h2D.Name,"BPLcc 2D Application");
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINPOSX, 23);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_WINPOSY, 23);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_AUTO_KEYBOARD, 1);
	BPLcc_WinData_Set(&h2D, BPLCC_INITWINOBJ_AUTO_MOUSE, 1);
	BPLcc_WinData_SetID(&h2D, BPLCC_INITWINOBJ_ICON, IDI_APPLICATION, true);
	BPLcc_WinData_SetID(&h2D, BPLCC_INITWINOBJ_CURSOR, IDC_ARROW, true);
	BPLcc_Window_Parameters_Set();
	BPLcc_VMAT_Create(&(h2D.ViewMatrix), &h2D);
	BPLcc_Keys_Create(&(h2D.K));
	BPLcc_Mus_Create(&(h2D.M));


	WNDCLASS wndclass;
	memset(&wndclass, 0, sizeof(WNDCLASS));
	wndclass.style = h2D.Style;
 	wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WinProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(h2D.hInstICON, h2D.DefaultIcon);
	wndclass.hCursor = LoadCursor(h2D.hInstCURSOR, h2D.DefaultCursor);
	wndclass.hbrBackground = (HBRUSH) h2D.BackgroundBrush;
	wndclass.lpszClassName = h2D.ClassName;
	RegisterClass(&wndclass);
 	h2D.hRect.left = 0;
  h2D.hRect.top = 0;
  h2D.hRect.right = h2D.ScreenX*h2D.Screen_Magnifier;
  h2D.hRect.bottom = h2D.ScreenY*h2D.Screen_Magnifier;
	AdjustWindowRect( &(h2D.hRect), h2D.dwStyle, false);
	h2D.hRect.bottom -= h2D.hRect.top;  h2D.hRect.right -= h2D.hRect.left;
  hWnd =	CreateWindow(h2D.ClassName,h2D.Name,
					h2D.dwStyle ,h2D.WinPosX, h2D.WinPosY,
					h2D.hRect.right, h2D.hRect.bottom,
					NULL, NULL, h2D.hInst, NULL);
	h2D.hWnd = hWnd;
	if (h2D.hWnd==NULL) { printf("\nhWnd Wasn't created.... Aborting...\n"); return true; }

	h2D.BitmapInfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
  h2D.BitmapInfo.bmiHeader.biPlanes = 1;
  h2D.BitmapInfo.bmiHeader.biBitCount = 24;
  h2D.BitmapInfo.bmiHeader.biCompression = BI_RGB;
  h2D.BitmapInfo.bmiHeader.biSizeImage = 0;
  h2D.BitmapInfo.bmiHeader.biClrUsed = 0;
  h2D.BitmapInfo.bmiHeader.biClrImportant = 0;
  h2D.BitmapInfo.bmiHeader.biWidth = h2D.ScreenX*h2D.Screen_Magnifier;
  h2D.BitmapInfo.bmiHeader.biHeight = h2D.ScreenY*h2D.Screen_Magnifier;
	HBITMAP MainBitmap;
	h2D.MainDC=CreateCompatibleDC(NULL); MainBitmap = CreateDIBSection (h2D.MainDC, &(h2D.BitmapInfo),
					DIB_RGB_COLORS, &(h2D.MainBitmapData), NULL, 0);
	SelectObject(h2D.MainDC, MainBitmap);
	h2D.FontDC=CreateCompatibleDC(NULL); MainBitmap = CreateDIBSection (h2D.FontDC, &(h2D.BitmapInfo),
					DIB_RGB_COLORS, &(h2D.MainBitmapData), NULL, 0);
	SelectObject(h2D.FontDC, MainBitmap);
	ToDo_Initialize();
	h2D.hDC = GetDC(h2D.hWnd);
	ShowWindow (h2D.hWnd, SW_SHOWNORMAL);
	if (h2D.ShowCursor == false) { ShowCursor(false); }
	h2D.DC_Draw=true;
	return MainLoop();
}

/* WIN PROC ************************************************************************************************************************/
LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LONG    lRet = 0;
		switch (uMsg) {
			case WM_CLOSE: {
				DeleteDC(h2D.MainDC);
				PostQuitMessage(0);
			break; }
			case WM_WINDOWPOSCHANGED:
				h2D.lpwp = (LPWINDOWPOS) lParam;
				h2D.winx=(*h2D.lpwp).x;
				h2D.winy=(*h2D.lpwp).y;
			break;
			default:
				lRet = DefWindowProc (hWnd, uMsg, wParam, lParam);
      break;
    }
		if( h2D.Auto_Keyboard==true) { BPLcc_Keys_Update(&(h2D.K), uMsg, wParam); }
		if( h2D.Auto_Mouse==true) { BPLcc_Mus_Update(&(h2D.M), uMsg, wParam); }
		if (h2D.ExitOnESC==1) { if (h2D.K.keys[VK_ESCAPE] == true) { h2D.K.keys[VK_ESCAPE]=false; PostQuitMessage(0); } }
		ToDo_WinProcMessage(uMsg, wParam, lParam);
    return lRet;
}

/* MAIN LOOP *************************************************************************************************************************/
WPARAM MainLoop(void) {
	MSG msg;
	while(1) {
		if (h2D.RenderByTime==true) {	h2D.RenderNowTime=timeGetTime();
			if ((h2D.RenderNowTime-h2D.RenderLastTime) > h2D.RenderTime) {
			h2D.RenderLastTime = h2D.RenderNowTime; h2D.DC_Draw=true;
			}
		}
		ToDo_MainLoop();
		if (h2D.DC_Draw==true) { h2D.DC_Draw=false;
			BPLcc_VMAT_Reset(&(h2D.ViewMatrix));
			DrawLayers();
			BPLcc_VMAT_CopyToDC(&(h2D.ViewMatrix), &(h2D.MainDC));
			if (h2D.Screen_Magnifier==1) {
				BitBlt(h2D.hDC, 0, 0, h2D.ScreenX, h2D.ScreenY, h2D.MainDC, 0, 0, SRCCOPY);
			} else {
				StretchBlt(h2D.hDC,0,0,h2D.ScreenX*h2D.Screen_Magnifier,
									h2D.ScreenY*h2D.Screen_Magnifier,h2D.MainDC,0,0,
									h2D.ScreenX, h2D.ScreenY, SRCCOPY);
			}
		}
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT)	{ break; }
      TranslateMessage(&msg); DispatchMessage(&msg);
    }
	}
	ToDo_AtExitProgram();
	BPLcc_VMAT_Destroy(&(h2D.ViewMatrix));
	return(msg.wParam);
}

/* CopyBitmap ********************************************************************************************************************/
bool CopyBitmap(HDC* OhDC,HBITMAP *im,int x, int y, int xsize, int ysize, DWORD type) {
	HDC tempDC = CreateCompatibleDC(*OhDC);
	if (!SelectObject(tempDC,*im)) { DeleteDC(tempDC); return false; }
	if (!BitBlt(*OhDC,x,y,xsize,ysize,tempDC,0,0,SRCCOPY)) { DeleteDC(tempDC); return false; }
	DeleteDC(tempDC); return true;
}

/* GET LAYER *********************************************************************************************************************/
int BPLcc_GetLayer(int x, int y) {
	if ((x >= 0) && (x < h2D.ScreenX*h2D.Screen_Magnifier) && (y >= 0) && (y < h2D.ScreenY*h2D.Screen_Magnifier)) {
		x=x/h2D.Screen_Magnifier; y=y/h2D.Screen_Magnifier;
		return (int)(h2D.ViewMatrix.Lev[x+y*h2D.ScreenX]);
	} else return -1;
}

/* GET COLOR *********************************************************************************************************************/
BPLcc_Color BPLcc_GetColor(int x, int y) {
	BPLcc_Color Col;
	if ((x >= 0) && (x < h2D.ScreenX*h2D.Screen_Magnifier) && (y >= 0) && (y < h2D.ScreenY*h2D.Screen_Magnifier)) {
		x=x/h2D.Screen_Magnifier; y=y/h2D.Screen_Magnifier;
		int pos3 = (x+y*h2D.ScreenX)*3;
		Col.b = h2D.ViewMatrix.rgb[pos3];
		Col.g = h2D.ViewMatrix.rgb[pos3+1];
		Col.r = h2D.ViewMatrix.rgb[pos3+2];
		Col.Col = ((((unsigned long)(Col.r))<<16) | (((unsigned long)(Col.g))<<8) | ((unsigned long)(Col.b))) & 0x00FFFFFF;
		return Col;
	}
	Col.r=0xFF; Col.g=0xFF; Col.b=0xFF; Col.Col=0xFFFFFFFF;
	return Col;
}
/* Play Sound Resource *****************************************************************************************/
/* Thanks to Josef Planeta and his "digital Dog" ;) */
bool BPLcc_PlaySound (const int Res) {
	HRSRC hr;
	HGLOBAL hg;
	LPVOID lpSndData;
	LPCTSTR Resource = (LPCTSTR) Res;
	hr = FindResource (h2D.hInst, Resource, RT_RCDATA);
	if (hr != NULL) {
		hg = LoadResource (h2D.hInst, hr);
		if (hg != NULL)	{
			lpSndData = LockResource (hg);
			if (lpSndData != NULL)	{
				return PlaySound((LPCTSTR)lpSndData, NULL, SND_MEMORY | SND_ASYNC);
			}
		}
	}
	return false;
}


/* SETTING WINDOW DATA ************************************************************************************************************/
void BPLcc_WinData_Set(BPLcc_WinMain *hObj, int SelectValue, int SetValue) {
	if (SelectValue == BPLCC_INITWINOBJ_STYLE) { h2D.Style = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_BACKBRUSH) { h2D.BackgroundBrush = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_DWSTYLE) { h2D.dwStyle = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_SCR_WIDTH) { h2D.ScreenX = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_SCR_HEIGHT) { h2D.ScreenY = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_SCR_LAYERS) { h2D.MaxLayer = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_SCR_TRANSCOLOR) { h2D.Transparent_Color = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_SCR_EXITONESCAPE) { h2D.ExitOnESC = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_SCR_SHOWCURSOR) { h2D.ShowCursor = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_WINPOSX) { h2D.WinPosX = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_WINRELPOSX) { h2D.RelWinx = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_WINPOSY) { h2D.WinPosY = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_WINRELPOSY) { h2D.RelWiny = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_SCR_MAGNIFIER) { h2D.Screen_Magnifier = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_SCR_CLEARCOLOR) { h2D.Clear_Color = SetValue; }
	else if (SelectValue == BPLCC_INITWINOBJ_RENDERTIME) {
		if (SetValue >= 0) { h2D.RenderByTime=true; h2D.RenderTime=SetValue;  h2D.RenderLastTime=timeGetTime(); } else { h2D.RenderByTime=false; }
	} else if (SelectValue == BPLCC_INITWINOBJ_AUTO_KEYBOARD) {
		if (SetValue > 0) { h2D.Auto_Keyboard = true; } else { h2D.Auto_Keyboard = false; }
	} else if (SelectValue == BPLCC_INITWINOBJ_AUTO_MOUSE) {
		if (SetValue > 0) { h2D.Auto_Mouse = true; } else { h2D.Auto_Mouse = false; }
	}

}

void BPLcc_WinData_SetID(BPLcc_WinMain *hObj, int SelectValue, LPCSTR SetValue, bool Default) {
	if (SelectValue == BPLCC_INITWINOBJ_ICON) {
		h2D.DefaultIcon = SetValue;
		if (Default == false) { h2D.hInstICON = h2D.hInst;
		} else { h2D.hInstICON = NULL; }
	}
	else if (SelectValue == BPLCC_INITWINOBJ_CURSOR) {
		h2D.DefaultCursor = SetValue;
		if (Default == false) {	h2D.hInstCURSOR = h2D.hInst;
		} else { h2D.hInstCURSOR = NULL; }
	}
}
void BPLcc_WinData_SetPc(BPLcc_WinMain *hObj, int SelectValue, char* SetValue) {
	int i=0;
	if (SelectValue == BPLCC_INITWINOBJ_NAME) {
		while(SetValue[i]!=0) { h2D.Name[i]=SetValue[i]; i++;
			if (i >= 255) { h2D.Name[0]='E'; h2D.Name[1]='R'; h2D.Name[2]='R';
			h2D.Name[3]='O'; h2D.Name[4]='R'; h2D.Name[5]=0; }
		}
		if (i < 255) { h2D.Name[i] = 0; }
	}
	else if (SelectValue == BPLCC_INITWINOBJ_CLASSNAME) {
		while(SetValue[i]!=0) { h2D.ClassName[i]=SetValue[i]; i++;
			if (i >= 255) { h2D.ClassName[0]='E'; h2D.ClassName[1]='R'; h2D.ClassName[2]='R';
			h2D.ClassName[3]='O'; h2D.ClassName[4]='R'; h2D.ClassName[5]=0; }
		}
		if (i < 255) { h2D.ClassName[i] = 0; }
	}
}


/*************************************************************************************************************
 *************************************************************************************************************
							View Matrix Functions							VMFVMFVMFVMFVMFVMFMVF
 *************************************************************************************************************
 *************************************************************************************************************/

void BPLcc_VMAT_Create(BPLcc_ViewMatrix* hObj, BPLcc_WinMain* WinMain) {
	int i;
	hObj->sx = WinMain->ScreenX;
	hObj->sy = WinMain->ScreenY;
	hObj->rgb = malloc(sizeof(unsigned char)*hObj->sx*hObj->sy*3);
	hObj->Crgb = malloc(sizeof(unsigned char)*hObj->sx*hObj->sy*3);
	hObj->Lev = malloc(sizeof(int)*hObj->sx*hObj->sy);
	hObj->CLev = malloc(sizeof(int)*hObj->sx*hObj->sy);
	hObj->ClearR = (unsigned char) (WinMain->Clear_Color);
	hObj->ClearG = (unsigned char) (WinMain->Clear_Color >> 8);
	hObj->ClearB = (unsigned char) (WinMain->Clear_Color >> 16);
	for (	i=0; i < hObj->sx*hObj->sy*3; i+=3) {
		hObj->Crgb[i]=hObj->ClearR; hObj->Crgb[i+1]=hObj->ClearG; hObj->Crgb[i+2]=hObj->ClearB;
		hObj->Lev[i/3]=WinMain->MaxLayer-1; hObj->CLev[i/3]=WinMain->MaxLayer-1;
 }
}

void BPLcc_VMAT_Destroy(BPLcc_ViewMatrix* hObj) {
	if (hObj->rgb!=NULL) { free(hObj->rgb); hObj->rgb=NULL; }
	if (hObj->Lev!=NULL) { free(hObj->Lev); hObj->Lev=NULL; }
	if (hObj->Crgb!=NULL) { free(hObj->Crgb); hObj->Crgb=NULL; }
	if (hObj->CLev!=NULL) { free(hObj->CLev); hObj->CLev=NULL; }
}

void BPLcc_VMAT_CopyToDC(BPLcc_ViewMatrix* hObj, HDC* targetDC ) {
	//if (hObj->rgb==NULL) { return; } else if (hObj->Lev==NULL) { return; }
	hObj->T=CreateCompatibleBitmap(*targetDC,hObj->sx,hObj->sy);
	SetBitmapBits(hObj->T,hObj->sx*hObj->sy*3,hObj->rgb);
	SelectObject(*targetDC, hObj->T );
	DeleteObject(hObj->T);
}

void BPLcc_VMAT_Reset(BPLcc_ViewMatrix* hObj) {
	int i;
	//if (hObj->rgb==NULL) { return; } else if ( hObj->Lev==NULL ) { return; }
	memcpy(hObj->rgb, hObj->Crgb, hObj->sx*hObj->sy*3*sizeof(unsigned char));
	//for (i=0; i<hObj->sx*hObj->sy*3; i++) { hObj->rgb[i]=hObj->Crgb[i]; }
	//for (i=0; i<hObj->sx*hObj->sy; i++) { hObj->Lev[i]=h2D.MaxLayer-1; }
	memcpy(hObj->Lev, hObj->CLev,	hObj->sx*hObj->sy*sizeof(int));
}

/*************************************************************************************************************
 *************************************************************************************************************
							Keyboard And Mouse Functions							KBKBKBKBKBKBKB
 *************************************************************************************************************
 *************************************************************************************************************/
 int BPLcc_Gi;
bool BPLcc_Gb;

void BPLcc_Keys_Create(BPLcc_Keys* hObj) {
	int i=0;
	for (int i=0; i < 256; i++) { hObj->keys[i] = false; hObj->keysb[i]=false; }
	hObj->Last=-1;
	for (i=0; i < 1024; i++) { hObj->IString[i] = 0; }
	hObj->StrNum=0; hObj->MaxChars=1;
}

void BPLcc_Keys_Update(BPLcc_Keys* hObj, UINT uMsg, WPARAM wParam) {
	hObj->Last=-1;
	switch (uMsg) {
		case WM_KEYDOWN: {  // Is a Key Down?
			hObj->keysb[wParam]=hObj->keys[wParam]; hObj->Last=wParam;
			hObj->keys[wParam]=true;
			// If wanna string input, and letter or number has been pressed.
			if ((hObj->StringInput == true)&&(((wParam >= 0x41)&&(wParam <=0x5A))||((wParam >= 0x30)&&(wParam <=0x39))
					||(wParam==VK_SPACE)||(wParam==0x08))) {
				if (wParam==0x08) { //BackSpace
					if (hObj->StrNum >= 1) { hObj->StrNum--; hObj->IString[hObj->StrNum]=0;}
				} else if (wParam==VK_ESCAPE) {
					for (int i =0; i < 1024; i++) { hObj->IString[i] = 0; }
					hObj->StrNum=0; hObj->StringInput=false;
				} else {
					if (hObj->keys[16]==false) { if ((wParam >= 0x41)&&(wParam <= 0x5A)) { wParam+=0x20; } }
					if ((wParam == 0x0D)||(hObj->StrNum >= hObj->MaxChars)) {
						hObj->StringInput=false;
						hObj->IString[hObj->StrNum]=0;
					}	else {
						hObj->IString[hObj->StrNum]=wParam;
					}
					hObj->StrNum++;
					if ((hObj->StrNum >= hObj->MaxChars)||(hObj->StrNum >= 1023)) { hObj->IString[hObj->StrNum]=0; hObj->StringInput=false; }
				}
			}
		break; }
		case WM_KEYUP:  // Is a Key Up
			hObj->keysb[wParam]=hObj->keys[wParam]; hObj->Last=wParam;
			hObj->keys[wParam]=false;
		break;
		default: break;
	}
}

bool BPLcc_Keys_Key(BPLcc_Keys* hObj, int key) {
	return hObj->keys[key];
}

void BPLcc_Keys_SetALL(BPLcc_Keys* hObj, bool val) {
	int i;
	for (i=0; i<255; i++) { hObj->keys[i]=val; hObj->keysb[i]=val; }
}

bool BPLcc_Keys_Shift(BPLcc_Keys* hObj) {
	return hObj->keys[16];
}

bool BPLcc_Keys_Ctrl(BPLcc_Keys* hObj) {
	return hObj->keys[17];
}

void BPLcc_Keys_GetString(BPLcc_Keys* hObj, int maxchars) {
	int i;
	hObj->MaxChars=maxchars;
	for (i = 0; i < 1024; i++) { hObj->IString[i] = 0; }
	hObj->StrNum=0; hObj->StringInput=true;
}

/* MOUSE */
/* ******************************************************************************* */
void BPLcc_Mus_Create(BPLcc_Mouse* hObj) {
	int i;
	for (i=0; i<7; i++) { hObj->keys[i]=false; hObj->keysb[i]=false; }
	hObj->Last=-1; hObj->Wheel=0; hObj->CursorState=-1; hObj->bCursorState=false;
}

void BPLcc_Mus_Update(BPLcc_Mouse* hObj, UINT uMsg, WPARAM wParam) {
	hObj->Last=-1;
	GetCursorPos(&(h2D.M.pos));
	h2D.M.p.x=h2D.M.pos.x-(h2D.winx+h2D.RelWinx);
	h2D.M.p.y=h2D.M.pos.y-(h2D.winy+h2D.RelWiny);

	BPLcc_Gi=-1; BPLcc_Gb=false;
			 if (uMsg == WM_LBUTTONDBLCLK) 	{ BPLcc_Gi = 0; BPLcc_Gb = true; }
	else if (uMsg == WM_LBUTTONDOWN) 		{ BPLcc_Gi = 1; BPLcc_Gb = true; }
	else if (uMsg == WM_LBUTTONUP)			{ BPLcc_Gi = 1; BPLcc_Gb = false; }
	else if (uMsg == WM_RBUTTONDBLCLK)	{ BPLcc_Gi = 2; BPLcc_Gb = true; }
	else if (uMsg == WM_RBUTTONDOWN)		{ BPLcc_Gi = 3; BPLcc_Gb = true; }
	else if (uMsg == WM_RBUTTONUP)			{ BPLcc_Gi = 3; BPLcc_Gb = false; }
	else if (uMsg == WM_MBUTTONDBLCLK)	{ BPLcc_Gi = 4; BPLcc_Gb = true; }
	else if (uMsg == WM_MBUTTONDOWN)		{ BPLcc_Gi = 5; BPLcc_Gb = true; }
	else if (uMsg == WM_MBUTTONUP)			{ BPLcc_Gi = 5; BPLcc_Gb = false; }
	else if (uMsg == WM_MOUSEWHEEL)			{ BPLcc_Gi = 6; BPLcc_Gb = true; 	hObj->Wheel = (short)HIWORD(wParam); }
	hObj->keysb[BPLcc_Gi]=hObj->keys[BPLcc_Gi]; hObj->Last=BPLcc_Gi;
	hObj->keys[BPLcc_Gi]=BPLcc_Gb;
}

void BPLcc_Mus_ClWheel(BPLcc_Mouse* hObj) {
	hObj->keys[6]=false; hObj->Last=-1; hObj->Wheel=0;
}

void BPLcc_Mus_ClDBLClk(BPLcc_Mouse* hObj) {
	hObj->keys[0]=false; hObj->keys[2]=false; hObj->Last=-1;
}

bool BPLcc_Mus_Key(BPLcc_Mouse* hObj, int key) {
	return hObj->keys[key];
}

void BPLcc_Mus_SetPos(BPLcc_Mouse* hObj, int x, int y) {
	SetCursorPos(x, y); hObj->pos.x=x; hObj->pos.y=y;
}

void BPLcc_Mus_ShowC(BPLcc_Mouse* hObj, bool state) {
	if (state == true) { hObj->CursorState++;  ShowCursor(true); }
	else { hObj->CursorState--;  ShowCursor(false); }
	if (hObj->CursorState >= 0) { hObj->bCursorState=true; } else { hObj->bCursorState=false; }
}

/*************************************************************************************************************
 *************************************************************************************************************
							TGA Images Support							TGATGATGATGATGATGATGA
 *************************************************************************************************************
 *************************************************************************************************************/
typedef struct sct_BPLcc_TGAImageHeader{
	unsigned char	id;	unsigned char colormap; unsigned char imageType; unsigned char colormapSpec[5];
	unsigned char	xOrigin[2]; unsigned char	yOrigin[2]; unsigned char	width[2]; unsigned char	height[2];
	unsigned char	bitDepth; unsigned char	imageDescription;
} BPLcc_TGAImageHeader;

void BPLcc_TGA_Create(BPLcc_Image* hObj, char *Filename) {
	hObj->data = NULL; BPLcc_TGA_Load(hObj, Filename);
}
void BPLcc_TGA_Reload(BPLcc_Image* hObj, char *Filename) {
	if (hObj->data!=NULL) { free(hObj->data); } BPLcc_TGA_Load(hObj, Filename);
}

void BPLcc_TGA_Destroy(BPLcc_Image* hObj) {
	if (hObj->data != NULL) { free(hObj->data); }
}

void BPLcc_TGA_Load(BPLcc_Image* hObj, char *Filename) {
	FILE *fp; fp=fopen(Filename, "rb");
	if (fp == NULL){ MessageBox(h2D.hWnd, "TGA File Open Error", "TGA Loading Error.", MB_OK); return; }
	BPLcc_TGAImageHeader header;
	fread(&header, 1, sizeof(BPLcc_TGAImageHeader), fp);
	unsigned int width		= header.width[0]		+	header.width[1]*256;
	unsigned int height	= header.height[0]	+	header.height[1]*256;
	hObj->sizeX = width;
	hObj->sizeY = height;
	hObj->data = malloc(width*height*3);
	if (!(hObj->data)){
		fclose(fp); MessageBox(h2D.hWnd, "TGA Memory Allocation Error.", "TGA Loading Error", MB_OK); return;
	}
	fread(hObj->data, 1, width*height*3, fp);
	int c; unsigned char temp;
	for (c=0; c < width*height*3; c+=3 ) { temp = hObj->data[c]; hObj->data[c]=hObj->data[c+2]; hObj->data[c+2]=temp; }
	fclose(fp);
}



/*************************************************************************************************************
 *************************************************************************************************************
							SPRITES Support							SPRSPRSPRSPRSPRSPRSPR
 *************************************************************************************************************
 *************************************************************************************************************/
void BPLcc_SPR_Create(BPLcc_Sprite* hObj) {
	int cc;
	hObj->i=0; hObj->j=0; hObj->k=0; hObj->sx=0; hObj->sy=0; hObj->mat=NULL; hObj->touched = NULL;
	hObj->TransColor=h2D.Transparent_Color; hObj->Vis=true;	hObj->mat32=NULL;
	hObj->touched = malloc(sizeof(int)*h2D.MaxLayer);
	for (cc=0; cc < h2D.MaxLayer; cc++) { hObj->touched[cc]=-1; } hObj->tfr=0; hObj->tbck=0;
	hObj->TransR = (unsigned char)(h2D.Transparent_Color >> 16);
	hObj->TransG = (unsigned char)(h2D.Transparent_Color >> 8);
	hObj->TransB = (unsigned char)(h2D.Transparent_Color);


}

void BPLcc_SPR_Destroy(BPLcc_Sprite* hObj) {
	if (hObj->mat!=NULL) { free(hObj->mat); hObj->mat=NULL; }
	if (hObj->mat32!=NULL) { free(hObj->mat32); hObj->mat32=NULL; }
	if (hObj->mat32!=NULL) { free(hObj->mat32); hObj->mat32=NULL; }
	if (hObj->touched!=NULL) { free(hObj->touched); hObj->touched=NULL; }

}

void BPLcc_SPR_Put(BPLcc_Sprite* hObj, int x, int y, int c) {
	if ((hObj->mat==NULL)||(hObj->mat32==NULL)) { return; }
	if (c >= h2D.MaxLayer) { return; }
	if (hObj->Vis == false) { return; }
	int temp, temp3, pos, pos3, ci, cj;
	hObj->j=x; hObj->i=y; hObj->k=c;

	for (ci=0; ci < hObj->sy; ci++) {
		if ((hObj->i+ci) < 0) { continue; }	if ((hObj->i+ci) >= h2D.ScreenY) { break; }
		for (cj=0; cj < hObj->sx; cj++) {
			if ((hObj->j+cj) < 0) { continue; }	if ((hObj->j+cj) >= h2D.ScreenX) { break; }
			temp=cj+ci*(hObj->sx);
			if (hObj->mat32[temp] != hObj->TransColor) {
				if (c <= (h2D.ViewMatrix.Lev[(cj+hObj->j)+(ci+hObj->i)*h2D.ScreenX])) {
					pos = (cj+hObj->j)+(ci+hObj->i)*h2D.ScreenX; pos3=pos*3;  temp3=temp*3;
					h2D.ViewMatrix.Lev[pos] = c;
					h2D.ViewMatrix.rgb[pos3] = hObj->mat[temp3];
					h2D.ViewMatrix.rgb[pos3+1] = hObj->mat[temp3+1];
					h2D.ViewMatrix.rgb[pos3+2] = hObj->mat[temp3+2];
				}
			}
		}
	}
}

void BPLcc_SPR_Put2(BPLcc_Sprite* hObj, int x, int y) {
	BPLcc_SPR_Put(hObj,x,y,0);
}

void BPLcc_SPR_PutCopy(BPLcc_Sprite* hObj, BPLcc_Sprite* hObj2, int x, int y, int x1, int y1, int width, int height) {
	if (hObj->Vis == false) { return; }
	if ((hObj2->mat == NULL)||(hObj2->mat32 == NULL)) { return; }
	int temp; int mpos, mpos3, temp3, ci, cj;
	int tsx, tsy;
	if (width==-1) { tsx=hObj->sx; x1=0;} else { tsx=width; }
	if (height==-1) { tsy=hObj->sy; y1=0; } else { tsy=height; }
	for (ci=0; ci < tsy; ci++) {
		if ((y+ci) < 0) { continue; }	if ((y+ci) >= (hObj2->sy)) { break; }
		if ((y1+ci) < 0) { continue; }	if ((y1+ci) >= hObj->sy) { break; }
		for (cj=0; cj < tsx; cj++) {
			if ((x+cj) < 0) { continue; }	if ((x+cj) >= hObj2->sx) { break; }
			if ((x1+cj) < 0) { continue; }	if ((x1+cj) >= hObj->sx) { break; }
			temp=cj+x1+(ci+y1)*(hObj->sx);
			if (hObj->mat32[temp] != h2D.Transparent_Color) {
				mpos = (cj+x)+((ci+y)*(hObj2->sx)); mpos3=mpos*3; temp3=temp*3;
				hObj2->mat32[mpos]  = hObj->mat32[temp];
				hObj2->mat[mpos3]   = hObj->mat[temp3];
				hObj2->mat[mpos3+1] = hObj->mat[temp3+1];
				hObj2->mat[mpos3+2] = hObj->mat[temp3+2];
			}
		}
	}
}

void BPLcc_SPR_IPut(BPLcc_Sprite* hObj, int x, int y, int c) {
	if ((hObj->mat==NULL)||(hObj->mat32==NULL)) { return; }
	if (c >= h2D.MaxLayer) { return; }
	if (hObj->Vis == false) { return; }
	int temp, pos, ci, cj;	hObj->j=x; hObj->i=y; hObj->k=c;
	for ( ci=0; ci < hObj->sy; ci++) {
		if ((hObj->i+ci) < 0) { continue; }	if ((hObj->i+ci) >= h2D.ScreenY) { break; }
		for ( cj=0; cj < hObj->sx; cj++) {
			if ((hObj->j+cj) < 0) { continue; }	if ((hObj->j+cj) >= h2D.ScreenX) { break; }
			temp=cj+ci*(hObj->sx);
			if (hObj->mat32[temp] != h2D.Transparent_Color) {
				if (c <= (h2D.ViewMatrix.Lev[(cj+hObj->j)+(ci+hObj->i)*h2D.ScreenX])) {
					pos = (cj+hObj->j)+(ci+hObj->i)*h2D.ScreenX;	h2D.ViewMatrix.Lev[pos] = c; //Change only the Level and not the colors.
				}
			}
		}
	}
}

void BPLcc_SPR_PutTrans(BPLcc_Sprite* hObj, int x, int y, int c) {
	if ((hObj->mat==NULL)||(hObj->mat32==NULL)) { return; }
	if (c >= h2D.MaxLayer) { return; }
	if (hObj->Vis == false) { return; }
	int temp, temp3, pos, pos3, ci, cj; hObj->j=x; hObj->i=y; hObj->k=c;

	for (int ci=0; ci < hObj->sy; ci++) {
		if ((hObj->i+ci) < 0) { continue; }	if ((hObj->i+ci) >= h2D.ScreenY) { break; }
		for (int cj=0; cj < hObj->sx; cj++) {
			if ((hObj->j+cj) < 0) { continue; }	if ((hObj->j+cj) >= h2D.ScreenX) { break; }
			temp=cj+ci*(hObj->sx);
			if (hObj->mat32[temp] != h2D.Transparent_Color) {
				if (c <= (h2D.ViewMatrix.Lev[(cj+hObj->j)+(ci+hObj->i)*h2D.ScreenX])) {
					pos = (cj+hObj->j)+(ci+hObj->i)*h2D.ScreenX; pos3=pos*3;  temp3=temp*3;
					h2D.ViewMatrix.rgb[pos3] = hObj->mat[temp3];
					h2D.ViewMatrix.rgb[pos3+1] = hObj->mat[temp3+1];
					h2D.ViewMatrix.rgb[pos3+2] = hObj->mat[temp3+2];
				}
			}
		}
	}
}

void BPLcc_SPR_IBoxSet(BPLcc_Sprite* hObj, int x, int y, int c, int sizex, int sizey) {
	if (c >= h2D.MaxLayer) { return; }
	int pos, ci, cj;
	hObj->j=x; hObj->i=y; hObj->k=c;
	for ( ci=0; ci < sizey; ci++) {
		if ((hObj->i+ci) < 0) { continue; }	if ((hObj->i+ci) >= h2D.ScreenY) { break; }
		for ( cj=0; cj < sizex; cj++) {
			if ((hObj->j+cj) < 0) { continue; }	if ((hObj->j+cj) >= h2D.ScreenX) { break; }
			if (c <= (h2D.ViewMatrix.Lev[(cj+hObj->j)+(ci+hObj->i)*h2D.ScreenX])) {
				pos = (cj+hObj->j)+(ci+hObj->i)*h2D.ScreenX;	h2D.ViewMatrix.Lev[pos] = c; //Change only the Level and not the colors.
			}
		}
	}
}

void BPLcc_SPR_SetVis(BPLcc_Sprite* hObj, bool Val) {
	hObj->Vis = Val;
}

bool BPLcc_SPR_Init(BPLcc_Sprite* hObj, int Resourc, int sxx, int syy) {
	HBITMAP bmSpr; int c=0,ci,cj; unsigned long tempc;
	if (!(bmSpr = LoadBitmap(h2D.hInst,MAKEINTRESOURCE(Resourc)))) { return false; }

	if (!SelectObject(h2D.FontDC,bmSpr)) { return false; }
	hObj->i=0; hObj->j=0; hObj->k=0; hObj->sx=sxx; hObj->sy=syy; hObj->Vis=true;
	if (hObj->mat!=NULL) { free(hObj->mat); }
	if (hObj->mat32!=NULL) { free(hObj->mat32); }
	hObj->mat=malloc(sizeof(unsigned char)*hObj->sx*hObj->sy*3);
	hObj->mat32=malloc(sizeof(unsigned long)*hObj->sx*hObj->sy);
	for ( ci=0; ci < hObj->sy; ci++) {	for (cj=0; cj < hObj->sx; cj++) {
		tempc = GetPixel(h2D.FontDC,cj,ci);
		hObj->mat32[c]=tempc;
		hObj->mat[c*3]= (unsigned char) (tempc >> 16);
		hObj->mat[c*3+1]= (unsigned char) (tempc >> 8);
		hObj->mat[c*3+2]= (unsigned char) (tempc); c++;
	}}
	return true;
}

bool BPLcc_SPR_InitByMatrix(BPLcc_Sprite* hObj, int sxx, int syy, unsigned long* matrix) {
	int c;
	if (matrix == NULL) { return false; }
	if (hObj->mat!=NULL) { free(hObj->mat); }
	if (hObj->mat32!=NULL) { free(hObj->mat32); }
	hObj->sx=sxx; hObj->sy=syy; hObj->Vis=true;
	hObj->mat   = malloc(sizeof(unsigned char)*hObj->sx*hObj->sy*3);
	hObj->mat32 = malloc(sizeof(unsigned long)*hObj->sx*hObj->sy);
	for (c=0; c < hObj->sx*hObj->sy; c++) {
		hObj->mat[c*3]=(unsigned char)(matrix[c]>>16);
		hObj->mat[c*3+1]=(unsigned char)(matrix[c]>>8);
		hObj->mat[c*3+2]=(unsigned char)(matrix[c]);
		hObj->mat32[c]=matrix[c] & 0x00FFFFFF;
	}
	return true;
}

void BPLcc_SPR_Touch(BPLcc_Sprite* hObj, int x, int y, int c) {
	if (c >= h2D.MaxLayer) { return; }
	if (hObj->Vis == false) { return; }
	int temp;
	int *t = malloc(sizeof(int)*h2D.MaxLayer);
	int ci,cj;
	for ( ci=0; ci < hObj->sy; ci++) {
		if ((y+ci) < 0) { continue; }	if ((y+ci) >= h2D.ScreenY) { break; }
		for ( cj=0; cj < hObj->sx; cj++) {
			if ((x+cj) < 0) { continue; }	if ((x+cj) >= h2D.ScreenX) { break; }
			temp=cj+ci*hObj->sx;
			if (hObj->mat32[temp] != h2D.Transparent_Color) {	t[h2D.ViewMatrix.Lev[(cj+x)+(ci+y)*h2D.ScreenX]] = 1;	}
		}
	}
	hObj->tfr=0; hObj->tbck=0;
	for (ci=0; ci < c; ci++) { if (t[ci]==1) { hObj->touched[hObj->tfr++]=ci; }	}
	for (ci=c+1; ci < h2D.MaxLayer; ci++) { if (t[ci]==1) { hObj->touched[hObj->tfr+hObj->tbck]=ci; hObj->tbck++; }	}
}

void BPLcc_SPR_Flip(BPLcc_Sprite* hObj, int horvert) {
	if (hObj->mat32== NULL) { return; } else if (hObj->mat==NULL) { return; } else if ((horvert<0)||(horvert > 1)) { return; }
	int ii,jj,sx,sy; sx=hObj->sx; sy=hObj->sy;
	if (horvert==0) { //Horizontal Flip
		unsigned long *temp = malloc(sizeof(unsigned long)*sx*sy);
		for (ii=0; ii<sx*sy; ii++) {	temp[ii] = hObj->mat32[ii]; }
		for (jj=0; jj < sx; jj++) {
			for (ii=0; ii < sy; ii++) {
				hObj->mat32[jj+ii*sx]=temp[(sx-jj-1)+ii*sx];
			}
		}
		free(temp);
	} else if (horvert==1) {
		unsigned long *temp = malloc(sizeof(unsigned long)*sx*sy);
		for (ii=0; ii<sx*sy; ii++) {	temp[ii] = hObj->mat32[ii]; }
		for (ii=0; ii < sy; ii++) {
			for (jj=0; jj < sx; jj++) {
				hObj->mat32[jj+ii*sx]=temp[jj+(sy-ii-1)*sx];
			}
		}
		free(temp);
	}
	for (ii=0; ii < sx*sy; ii++) {
		hObj->mat[ii*3] = (unsigned char) (hObj->mat32[ii]>>16);
		hObj->mat[ii*3+1] = (unsigned char) (hObj->mat32[ii]>>8);
		hObj->mat[ii*3+2] = (unsigned char) (hObj->mat32[ii]);
	}
}

void BPLcc_SPR_Rotate(BPLcc_Sprite* hObj, int Degrees) {
	if (hObj->mat32== NULL) { return; } else if (hObj->mat==NULL) { return; }

	int ii,jj,t,sx,sy; sx=hObj->sx; sy=hObj->sy;
	if (Degrees==90) {
		unsigned long *temp = malloc(sizeof(unsigned long)*sx*sy);
		for (ii=0; ii<sx*sy; ii++) {	temp[ii] = hObj->mat32[ii]; }
		for (jj=0; jj < sx; jj++) {
			for (ii=0; ii < sy; ii++) {
				hObj->mat32[(sy-ii-1)+(sy*jj)]=temp[jj+ii*sx];
			}
		}
		free(temp); t=hObj->sx; hObj->sx=hObj->sy; hObj->sy=t;
	} else	if ((Degrees==180)||(Degrees==-180)) {
		unsigned long *temp = malloc(sizeof(unsigned long)*sx*sy);
		for (ii=0; ii<sx*sy; ii++) {	temp[ii] = hObj->mat32[ii]; }
		for (jj=0; jj < sx; jj++) {
			for (ii=0; ii < sy; ii++) {
				hObj->mat32[(sx-jj-1)+(sy-ii-1)*sx]=temp[jj+ii*sx];
			}
		}
		free(temp);
	} else	if ((Degrees==-90)||(Degrees==270)) {
		unsigned long *temp = malloc(sizeof(unsigned long)*sx*sy);
		for (ii=0; ii<sx*sy; ii++) {	temp[ii] = hObj->mat32[ii]; }
		for (jj=0; jj < sx; jj++) {
			for (ii=0; ii < sy; ii++) {
				hObj->mat32[ii+(sx-jj-1)*sy]=temp[jj+ii*sx];
			}
		}
		free(temp);  t=hObj->sx; hObj->sx=hObj->sy; hObj->sy=t;
	}
	if ((Degrees == 90)||(Degrees == -90)||(Degrees == 180)||(Degrees == -180)||(Degrees == 270)) {
		for (ii=0; ii < hObj->sx*hObj->sy; ii++) {
			hObj->mat[ii*3] = (unsigned char) (hObj->mat32[ii]>>16);
			hObj->mat[ii*3+1] = (unsigned char) (hObj->mat32[ii]>>8);
			hObj->mat[ii*3+2] = (unsigned char) (hObj->mat32[ii]);
		}
	}
}

void BPLcc_SPR_SetTransRGBs(BPLcc_Sprite* hObj, int r1, int r2, int g1, int g2, int b1, int b2) {
	unsigned char ch[6];
	ch[0]=(unsigned char)r1;	ch[1]=(unsigned char)g1;	ch[2]=(unsigned char)b1;
	ch[3]=(unsigned char)r2;	ch[4]=(unsigned char)g2;	ch[5]=(unsigned char)b2;
	if ((hObj->mat==NULL)||(hObj->mat32==NULL)) { return; }
	int i,j,pos;
	for (i=0; i < hObj->sy; i++) {
		for (j=0; j < hObj->sx; j++) {
			pos=(j+i*hObj->sx)*3;
			if ((hObj->mat[pos] >= ch[2])&&(hObj->mat[pos] <= ch[5])) {
				if ((hObj->mat[pos+1] >= ch[1])&&(hObj->mat[pos+1] <= ch[4])) {
					if ((hObj->mat[pos+2] >= ch[0])&&(hObj->mat[pos+2] <= ch[3])) {
						hObj->mat[pos]=hObj->TransB;
						hObj->mat[pos+1]=hObj->TransG;
						hObj->mat[pos+2]=hObj->TransB;
						hObj->mat32[pos/3]=h2D.Transparent_Color & 0x00FFFFFF;
					}
				}
			}
		}
	}
}

bool BPLcc_SPR_TGALoad(BPLcc_Sprite* hObj, char* filename) {
	int c;
	BPLcc_Image TempImage;
	BPLcc_TGA_Create(&TempImage, filename);
	if (TempImage.data == NULL) { return false; }
	hObj->i=0; hObj->j=0; hObj->k=0;
	hObj->sx = TempImage.sizeX;
	hObj->sy = TempImage.sizeY;
	if (hObj->mat != NULL) { free(hObj->mat); hObj->mat=NULL; }
	if (hObj->mat32 != NULL) { free(hObj->mat32); hObj->mat32=NULL; }
	hObj->mat = malloc(sizeof(unsigned char)*hObj->sx*hObj->sy*3);
	hObj->mat32 = malloc(sizeof(unsigned long)*hObj->sx*hObj->sy);

	for (c=0; c < hObj->sx*hObj->sy*3; c+=3) {
		hObj->mat[c]   = (unsigned char) TempImage.data[c+2];
		hObj->mat[c+1] = (unsigned char) TempImage.data[c+1];
		hObj->mat[c+2] = (unsigned char) TempImage.data[c];
		hObj->mat32[c/3]= ((((unsigned long)(TempImage.data[c+2]))<<16) | (((unsigned long)(TempImage.data[c+1]))<<8) | ((unsigned long)(TempImage.data[c])));
		hObj->mat32[c/3] &= 0x00FFFFFF;
	}
	BPLcc_SPR_Flip(hObj,1);
	BPLcc_TGA_Destroy(&TempImage);
	return true;
}

void BPLcc_SPR_RandomColor(BPLcc_Sprite* hObj, int mix, int x, int y, int sizex, int sizey) {
	if ((hObj->mat==NULL)||(hObj->mat32==NULL)) { return; }
	if (x==-1) { x = 0; } if (y==-1) { y=0; } if (sizex==-1) { sizex = hObj->sx; } if (sizey==-1) { sizey=hObj->sy; }
	int tr,tb,tg,i,j,c=0; float tf;
	int r = (int)((((float)rand())/0x8000)*256); tr=r;
	int g = (int)((((float)rand())/0x8000)*256); tg=g;
	int b = (int)((((float)rand())/0x8000)*256); tb=b;
	unsigned long tempc = ((0x00000000 | (((unsigned char)tb) << 16) | (((unsigned char)tg)<<8) | ((unsigned char)tr)) & (0x00FFFFFF));
	for (j=y; j < y+sizey; j++) {
		if (j < 0 ) { continue; } else if (j>=hObj->sy) { break; }
		for (i=x; i < x+sizex; i++) {
			if (i < 0 ) { continue; } else if (i>=hObj->sx) { break; }
			if (mix == 1) {
				tb=hObj->mat[c*3]; tg=hObj->mat[c*3+1];	tr=hObj->mat[c*3+2];
				tf=sqrt(pow(tr,2)+pow(tg,2)+pow(tb,2))/450;
				tr=(int)r*tf;	tg=(int)g*tf;	tb=(int)b*tf;
				tempc = ((0x00000000 | (((unsigned char)tb) << 16) | (((unsigned char)tg)<<8) | ((unsigned char)tr)) & (0x00FFFFFF));
			}
			hObj->mat32[c]=tempc;
			hObj->mat[c*3]= (unsigned char) tb;
			hObj->mat[c*3+1]= (unsigned char) tg;
			hObj->mat[c*3+2]= (unsigned char) tr;
			c++;
		}
	}
}

void BPLcc_SPR_Copy(BPLcc_Sprite* hObj, BPLcc_Sprite* hObj2) {
	if (hObj2->mat==NULL) { return; }
	if (hObj->mat!=NULL) { BPLcc_SPR_Destroy(hObj); }
	BPLcc_SPR_InitByMatrix(hObj,hObj2->sx, hObj2->sy, hObj2->mat32);
	hObj->i = hObj2->i; hObj->j = hObj2->j; hObj->k = hObj2->k;
	hObj->TransColor = hObj2->TransColor;
	hObj->TransR = hObj2->TransR; hObj->TransG = hObj2->TransG; hObj->TransB = hObj2->TransB;
	hObj->Vis = hObj2->Vis;
}

/*************************************************************************************************************
 *************************************************************************************************************
							ANIMATION Support							ANMANMANMANMANMANMANM
 *************************************************************************************************************
 *************************************************************************************************************/
void BPLcc_ANM_Create(BPLcc_Anim* hObj) {
	hObj->xpos=0; hObj->ypos=0; hObj->dpos=0; hObj->Vis=true; hObj->AnSpr=NULL; hObj->CurrSpr=-1; hObj->NSprites=0;
	hObj->sx=0; hObj->sy=0; hObj->Msx=NULL; hObj->Msy=NULL; hObj->ContChange=false; hObj->touched=NULL;
	hObj->ChangeByTime=false; hObj->ChangeTime=500; hObj->ChangeNowTime=timeGetTime(); hObj->ChangeLastTime=hObj->ChangeNowTime;
	hObj->TransColor = h2D.Transparent_Color; int cc;
	hObj->TransR = (unsigned char)(hObj->TransColor >> 16);
	hObj->TransG = (unsigned char)(hObj->TransColor >> 8);
	hObj->TransB = (unsigned char)(hObj->TransColor);
	hObj->touched = malloc(sizeof(int)*h2D.MaxLayer);
	for (cc=0; cc < h2D.MaxLayer; cc++) { hObj->touched[cc]=-1; } hObj->tfr=0; hObj->tbck=0;
	hObj->CyclicAnim=true; hObj->CyclicUpdate=1;
}

void BPLcc_ANM_Destroy(BPLcc_Anim* hObj) {
	if (hObj->AnSpr!=NULL) { int c;	for (c=0; c < hObj->NSprites; c++) { BPLcc_SPR_Destroy(&(hObj->AnSpr[c])); }	free(hObj->AnSpr); }
	if (hObj->Msx!=NULL) { free(hObj->Msx); }
	if (hObj->Msy!=NULL) { free(hObj->Msy); }
	if (hObj->touched!=NULL) { free(hObj->touched); hObj->touched=NULL; }
}

bool BPLcc_ANM_LoadSprites(BPLcc_Anim* hObj, int* ResourcMat, int NumSpr, int sxx, int syy, int* Msxx, int* Msyy) {
	if (NumSpr <= 0) {return false; }
	int i,c;
	hObj->NSprites=NumSpr;
	if (Msxx!=NULL) {
		hObj->sx=-1; hObj->sy=-1;
		hObj->Msx= malloc(sizeof(int)*NumSpr); hObj->Msy=malloc(sizeof(int)*NumSpr);
		for (i=0; i< NumSpr; i++) { hObj->Msx[i]=Msxx[i];}
		if (Msyy!=NULL) {	for (i=0; i< NumSpr; i++) { hObj->Msy[i]=Msyy[i];} }
		else {	for (i=0; i< NumSpr; i++) { hObj->Msy[i]=hObj->Msx[i];} }
		hObj->AnSpr = malloc(sizeof(BPLcc_Sprite)*NumSpr);
		for (c=0; c < NumSpr; c++) { BPLcc_SPR_Create(&(hObj->AnSpr[c])); }
		for (i=0; i< NumSpr; i++) { 	BPLcc_SPR_Init(&(hObj->AnSpr[i]),ResourcMat[i],hObj->Msx[i],hObj->Msy[i]); }
	} else {
		hObj->sx=sxx; hObj->sy=syy;
		hObj->AnSpr = malloc(sizeof(BPLcc_Sprite)*NumSpr);
		for (c=0; c < NumSpr; c++) { BPLcc_SPR_Create(&(hObj->AnSpr[c])); }
		for (i=0; i< NumSpr; i++) { 	BPLcc_SPR_Init(&(hObj->AnSpr[i]),ResourcMat[i],hObj->sx,hObj->sy); }
	} hObj->CurrSpr=0;
	return true;
}

void BPLcc_ANM_SetSprite(BPLcc_Anim* hObj, int c) {
	if ((c >= 0)&&(c < hObj->NSprites)) { hObj->CurrSpr=c; }
}

void BPLcc_ANM_Put(BPLcc_Anim* hObj, int x, int y, int d) {
	if (hObj->AnSpr==NULL) { return; }
	if (hObj->Vis==false) { return; }
	BPLcc_SPR_Put(&(hObj->AnSpr[hObj->CurrSpr]),x,y,d);
	if (hObj->ContChange==true) { hObj->CurrSpr++; if (hObj->CurrSpr >= hObj->NSprites) { hObj->CurrSpr=0; } }
}

void BPLcc_ANM_IPut(BPLcc_Anim* hObj, int x, int y, int c, int status) {
	if (hObj->AnSpr==NULL) { return; }
	if (hObj->Vis==false) { return; }
	if (status > 0)  {x=hObj->xpos; y=hObj->ypos;	if (status == 2) { c=hObj->dpos; }}
	BPLcc_SPR_IPut(&(hObj->AnSpr[hObj->CurrSpr]),x, y, c);
	if (hObj->ContChange==true) { hObj->CurrSpr++; if (hObj->CurrSpr >= hObj->NSprites) { hObj->CurrSpr=0; } }
}

void BPLcc_ANM_PutTrans(BPLcc_Anim* hObj, int x, int y, int c, int status) {
	if (hObj->AnSpr==NULL) { return; }
	if (hObj->Vis==false) { return; }
	if (status > 0)  {x=hObj->xpos; y=hObj->ypos;	if (status == 2) { c=hObj->dpos; }}
	BPLcc_SPR_PutTrans(&(hObj->AnSpr[hObj->CurrSpr]),x,y,c);
	if (hObj->ContChange==true) { hObj->CurrSpr++; if (hObj->CurrSpr >= hObj->NSprites) { hObj->CurrSpr=0; } }
}

void BPLcc_ANM_PutCopy(BPLcc_Anim* hObj, BPLcc_Sprite* hObj2, int x, int y, int numAn) {
	if (hObj->Vis == false) { return; }
	if (hObj2->mat == NULL) { return; }
	if ((numAn < 0) || (numAn >= hObj->NSprites)) { return; }
	if ((hObj->Msx!=NULL) || (hObj->Msy!=NULL)) { hObj->sx=hObj->Msx[numAn];hObj->sy=hObj->Msy[numAn]; }
	int ci,cj;
	int temp, temp3, pos, pos3;
	for ( ci=0; ci < hObj->sy; ci++) {
		if ((y+ci) < 0) { continue; }	if ((y+ci) >= hObj2->sy) { break; }
		for ( cj=0; cj < hObj->sx; cj++) {
			if ((x+cj) < 0) { continue; }	if ((x+cj) >= hObj2->sx) { break; }
			temp=cj+ci*hObj->sx;
			if (hObj->AnSpr[numAn].mat[temp] != h2D.Transparent_Color) {
				pos = (cj+x)+((ci+y)*hObj2->sx); pos3=pos*3; temp3=temp*3;
				hObj2->mat32[pos] = hObj->AnSpr[numAn].mat32[temp];
				hObj2->mat[pos3] = hObj->AnSpr[numAn].mat[temp3];
				hObj2->mat[pos3+1] = hObj->AnSpr[numAn].mat[temp3+1];
				hObj2->mat[pos3+2] = hObj->AnSpr[numAn].mat[temp3+2];
			}
		}
	}
}

void BPLcc_ANM_PutXY(BPLcc_Anim* hObj, int d) {
	if (hObj->Vis==false) { return; }
	BPLcc_SPR_Put(&(hObj->AnSpr[hObj->CurrSpr]),hObj->xpos,hObj->ypos,d);
	if (hObj->ContChange==true) { hObj->CurrSpr++; if (hObj->CurrSpr >= hObj->NSprites) { hObj->CurrSpr=0; } }
}

void BPLcc_ANM_PutXYZ(BPLcc_Anim* hObj) {
	if (hObj->Vis==false) { return; }
	BPLcc_SPR_Put(&(hObj->AnSpr[hObj->CurrSpr]),hObj->xpos,hObj->ypos,hObj->dpos);
	if (hObj->ContChange==true) { hObj->CurrSpr++; if (hObj->CurrSpr >= hObj->NSprites) { hObj->CurrSpr=0; } }
}

void BPLcc_ANM_LoadByOne(BPLcc_Anim* hObj, int Resourc, int NumSpr, int fx, int fy, int* PosData) {
	if (NumSpr <= 0) {return; }
	int i,j,k,c,count=0; HBITMAP bmSpr; unsigned long* t2mat;
	// Loading Resource to tempDC.
	if (!(bmSpr = LoadBitmap(h2D.hInst,MAKEINTRESOURCE(Resourc)))) { return; }
	if (!SelectObject(h2D.FontDC,bmSpr)) { return; }
	// Loading Bitmat to temporary matrix tmat from TempDC.
	unsigned long* tmat = malloc(sizeof(unsigned long)*fx*fy);
	for (j=0; j < fy; j++) {	for (i=0; i < fx; i++) {	tmat[count++]=GetPixel(h2D.FontDC,i,j);		}	}
	// Creating Variables.
	hObj->NSprites=NumSpr;
	hObj->AnSpr = malloc(sizeof(BPLcc_Sprite)*NumSpr);
	for (c=0; c < NumSpr; c++) { BPLcc_SPR_Create(&(hObj->AnSpr[c])); }
	hObj->Msx=malloc(sizeof(int)*NumSpr); hObj->Msy=malloc(sizeof(int)*NumSpr);
	for (k=0; k < hObj->NSprites; k++) {
		hObj->Msx[k]=PosData[k*4+2]; hObj->Msy[k]=PosData[k*4+3];
		t2mat=malloc(sizeof(unsigned long)*hObj->Msx[k]*hObj->Msy[k]); int c2=0;
		for (i=PosData[k*4+1]; i < PosData[k*4+1]+hObj->Msy[k]; i++) {	for (j=PosData[k*4]; j < PosData[k*4]+hObj->Msx[k]; j++) {
			t2mat[c2++] = tmat[j+i*fx];
		}		}
		BPLcc_SPR_InitByMatrix(&(hObj->AnSpr[k]),hObj->Msx[k],hObj->Msy[k],t2mat);
		free(t2mat);
	}
	hObj->CurrSpr=0;
}

bool BPLcc_ANM_Update(BPLcc_Anim* hObj, int dc) {
	if (hObj->ChangeByTime==true) {
		hObj->ChangeNowTime=timeGetTime();
		if ((hObj->ChangeNowTime-hObj->ChangeLastTime) > hObj->ChangeTime) {
			hObj->ChangeLastTime=hObj->ChangeNowTime; h2D.DC_Draw=true;
			if (hObj->CyclicAnim==false) {
				hObj->CurrSpr+=dc;
				if (hObj->CurrSpr >= hObj->NSprites) { hObj->CurrSpr=0; return true;}
				else if (hObj->CurrSpr < 0) { hObj->CurrSpr=hObj->NSprites-1; return true;}
			} else {
				hObj->CurrSpr+=hObj->CyclicUpdate;
				if (hObj->CurrSpr >= hObj->NSprites) { hObj->CurrSpr=hObj->NSprites-1; hObj->CyclicUpdate=-hObj->CyclicUpdate; return true;}
				else if (hObj->CurrSpr < 0) { hObj->CurrSpr=0; hObj->CyclicUpdate=-hObj->CyclicUpdate; return true;}
			}
		}
	}
	return false;
}

void BPLcc_ANM_Touch(BPLcc_Anim* hObj, int x, int y, int c) {
	if (c >= h2D.MaxLayer) { return; }
	if (hObj->Vis == false) { return; }
	if (hObj->AnSpr==NULL) { return; }
	int i;
	BPLcc_SPR_Touch(&(hObj->AnSpr[hObj->CurrSpr]),x,y,c);
	hObj->tfr=hObj->AnSpr[hObj->CurrSpr].tfr;
	hObj->tbck=hObj->AnSpr[hObj->CurrSpr].tbck;
	for ( i=0; i< (hObj->tfr+hObj->tbck); i++) {
		hObj->touched[i]=hObj->AnSpr[hObj->CurrSpr].touched[i];
	}
}

void BPLcc_ANM_Flip(BPLcc_Anim* hObj, int horvert, bool flipall) {
	int i,c1,c2;
	if ((hObj->AnSpr==NULL)||(hObj->Msx==NULL)||(hObj->Msy==NULL)) { return; }
	if (flipall==true) { c1=0; c2=hObj->NSprites; } else { c1=hObj->CurrSpr; c2=c1+1; }
	for (i=c1; i<c2; i++) {
		BPLcc_SPR_Flip(&(hObj->AnSpr[i]),horvert);
	}
}

void BPLcc_ANM_Rotate(BPLcc_Anim* hObj, int Degrees, bool rotall) {
	int i,c1,c2,t;
	if ((hObj->AnSpr==NULL)||(hObj->Msx==NULL)||(hObj->Msy==NULL)) { return; }
	if (rotall==true) { c1=0; c2=hObj->NSprites; } else { c1=hObj->CurrSpr; c2=c1+1; }
	for (i=c1; i<c2; i++) {
		BPLcc_SPR_Rotate(&(hObj->AnSpr[i]),Degrees);
		if ((Degrees==90)||(Degrees==-90)||(Degrees==270)) { t=hObj->Msx[i]; hObj->Msx[i]=hObj->Msy[i]; hObj->Msy[i]=t; }
	}
}

void BPLcc_ANM_SetTrans(BPLcc_Anim* hObj, int r1,int r2, int g1, int g2, int b1, int b2, int NumSpr) {
	if ((hObj->AnSpr==NULL)||(hObj->Msx==NULL)||(hObj->Msy==NULL)) { return; }
	if (NumSpr == -1) {
		for (int i=0; i < hObj->NSprites; i++) {
			BPLcc_SPR_SetTransRGBs(&(hObj->AnSpr[i]),r1,r2,g1,g2,b1,b2);
		}
	} else {
		if ((NumSpr >= 0)&&(NumSpr < hObj->NSprites)) {
			BPLcc_SPR_SetTransRGBs(&(hObj->AnSpr[NumSpr]),r1,r2,g1,g2,b1,b2);
		}
	}
}

bool BPLcc_ANM_TGALoad(BPLcc_Anim* hObj, int NumSpr,char *filename,int* PosData) {
	if (NumSpr <= 0) {return false; }
	BPLcc_Image t;
	BPLcc_TGA_Create(&t, filename);
	if (t.data == NULL) { return false; }
	int fx,fy,k,i,j,c; unsigned long* t2mat;
	fx=t.sizeX; fy= t.sizeY;
	hObj->NSprites=NumSpr;
	hObj->AnSpr = malloc(sizeof(BPLcc_Sprite)*NumSpr);
	for (c=0; c < NumSpr; c++) { BPLcc_SPR_Create(&(hObj->AnSpr[c])); }
	hObj->Msx=malloc(sizeof(int)*NumSpr); hObj->Msy=malloc(sizeof(int)*NumSpr);
	unsigned long* tmat = malloc(sizeof(unsigned long)*fx*fy);
	for (c=0; c < fx*fy*3; c+=3) {
		tmat[c/3]=( (((unsigned long)(t.data[c+2]))<<16) | (((unsigned long)(t.data[c+1]))<<8) | ((unsigned long)(t.data[c])));
	}
	for (k=0; k < hObj->NSprites; k++) {
		hObj->Msx[k]=PosData[k*4+2]; hObj->Msy[k]=PosData[k*4+3];
		t2mat=malloc(sizeof(unsigned long)*hObj->Msx[k]*hObj->Msy[k]); int c2=0;
		for (i=PosData[k*4+1]; i < PosData[k*4+1]+hObj->Msy[k]; i++) {	for (j=PosData[k*4]; j < PosData[k*4]+hObj->Msx[k]; j++) {
		t2mat[c2++] = tmat[j+i*fx];		}		}
		BPLcc_SPR_InitByMatrix(&(hObj->AnSpr[k]),hObj->Msx[k],hObj->Msy[k],t2mat);
		free(t2mat);
	}
	hObj->CurrSpr=0;
	free(tmat); BPLcc_TGA_Destroy(&t);
	BPLcc_ANM_Flip(hObj,1,true);
  return true;
}


void BPLcc_ANM_Copy(BPLcc_Anim* hObj, BPLcc_Anim* hObj2) {
	int cc;
	if (hObj2->AnSpr == NULL) { return; }
	if (hObj->AnSpr != NULL) { BPLcc_ANM_Destroy(hObj); }
	hObj->xpos = hObj2->xpos; hObj->ypos = hObj2->ypos; hObj->dpos = hObj2->dpos;
	hObj->Vis = hObj2->Vis;
	hObj->CurrSpr = hObj2->CurrSpr;
	hObj->NSprites = 	hObj2->NSprites;
	hObj->sx = 	hObj2->sx;
	hObj->sy = 	hObj2->sy;
	hObj->ContChange = hObj2->ContChange;
	hObj->ChangeByTime = hObj2->ChangeByTime;
	hObj->ChangeTime = hObj2->ChangeTime;
	hObj->ChangeLastTime = hObj2->ChangeLastTime;
	hObj->ChangeNowTime =	hObj2->ChangeNowTime;
	hObj->TransColor =	hObj2->TransColor;
	hObj->TransR =	hObj2->TransR;
	hObj->TransG =	hObj2->TransG;
	hObj->TransB =	hObj2->TransB;
	hObj->CyclicUpdate = hObj2->CyclicUpdate;
	hObj->CyclicAnim = hObj2->CyclicAnim;
	if (hObj2->Msx != NULL) {
		hObj->Msx = malloc(sizeof(int)*hObj->NSprites);
	} else { hObj->Msx = NULL; }
	if (hObj2->Msy != NULL) {
		hObj->Msy = malloc(sizeof(int)*hObj->NSprites);
	} else { hObj->Msy = NULL; }
 	hObj->AnSpr = malloc(sizeof(BPLcc_Sprite)*hObj->NSprites);
	hObj->touched = malloc(sizeof(int)*h2D.MaxLayer);
	for (cc=0; cc < h2D.MaxLayer; cc++) { hObj->touched[cc]=-1; } hObj->tfr=0; hObj->tbck=0;
	for (cc=0; cc < hObj->NSprites; cc++) {
		BPLcc_SPR_Copy(&(hObj->AnSpr[cc]),&(hObj2->AnSpr[cc]));
	}

}

/*************************************************************************************************************
 *************************************************************************************************************
							User Defined Fonts Support							UFNTUFNTUFNTUFNTUFNTUFNTUFNT
 *************************************************************************************************************
 *************************************************************************************************************/
void BPLcc_UFNT_Create(BPLcc_uFont* hObj) {
	int i;
	hObj->FX=0; hObj->FY=0; hObj->xpos=0; hObj->ypos=0; hObj->dpos=0; hObj->MaxFonts=0; hObj->Let=NULL;
	hObj->CR=0; hObj->CG=0; hObj->CB=0; hObj->TextTrans=false;
	for ( i=0; i < 256; i++) { hObj->chars[i]=63; }
	hObj->Space=0;
	hObj->chars[65]=0;hObj->chars[66]=1;hObj->chars[67]=2;hObj->chars[68]=3;hObj->chars[69]=4;hObj->chars[70]=5;hObj->chars[71]=6;hObj->chars[72]=7;hObj->chars[73]=8;hObj->chars[74]=9;
	hObj->chars[75]=10;hObj->chars[76]=11;hObj->chars[77]=12;hObj->chars[78]=13;hObj->chars[79]=14;hObj->chars[80]=15;hObj->chars[81]=16;hObj->chars[82]=17;hObj->chars[83]=18;hObj->chars[84]=19;hObj->chars[85]=20;
	hObj->chars[86]=21;hObj->chars[87]=22;hObj->chars[88]=23;hObj->chars[89]=24;hObj->chars[90]=25;
	hObj->chars[193]=0;hObj->chars[194]=1;hObj->chars[195]=62;hObj->chars[196]=63;hObj->chars[197]=4;hObj->chars[198]=25;hObj->chars[199]=7;hObj->chars[200]=64;hObj->chars[201]=8;hObj->chars[202]=10;hObj->chars[203]=65;
	hObj->chars[204]=12;hObj->chars[205]=13;hObj->chars[206]=66;hObj->chars[207]=14;hObj->chars[208]=67;hObj->chars[209]=15;hObj->chars[211]=68;hObj->chars[212]=19;hObj->chars[213]=24;hObj->chars[214]=69;hObj->chars[215]=23;
	hObj->chars[216]=70;hObj->chars[217]=71;
	hObj->chars[48]=35;hObj->chars[49]=26;hObj->chars[50]=27;hObj->chars[51]=28;hObj->chars[52]=29;hObj->chars[53]=30;hObj->chars[54]=31;hObj->chars[55]=32;hObj->chars[56]=33;hObj->chars[57]=34;
	hObj->chars[126]=36;hObj->chars[33]=37;hObj->chars[64]=38;hObj->chars[35]=39;hObj->chars[36]=40;hObj->chars[37]=41;hObj->chars[94]=42;hObj->chars[38]=43;hObj->chars[42]=44;hObj->chars[40]=45;hObj->chars[41]=46;hObj->chars[95]=47;hObj->chars[43]=48;hObj->chars[45]=49;hObj->chars[45]=49;
	hObj->chars[124]=50;hObj->chars[61]=51;hObj->chars[58]=52;hObj->chars[59]=53;hObj->chars[34]=54;hObj->chars[63]=55;hObj->chars[47]=56;hObj->chars[92]=57;hObj->chars[60]=58;hObj->chars[62]=59;hObj->chars[46]=60;hObj->chars[44]=61;hObj->chars[32]=72;
	hObj->chars[97]=73;hObj->chars[98]=74;hObj->chars[99]=75;hObj->chars[100]=76;hObj->chars[101]=77;hObj->chars[102]=78;hObj->chars[103]=79;hObj->chars[104]=80;hObj->chars[105]=81;hObj->chars[106]=82;hObj->chars[107]=83;hObj->chars[108]=84;hObj->chars[109]=85;hObj->chars[110]=86;
	hObj->chars[111]=87;hObj->chars[112]=88;hObj->chars[113]=89;hObj->chars[114]=90;hObj->chars[115]=91;hObj->chars[116]=92;hObj->chars[117]=93;hObj->chars[118]=94;hObj->chars[119]=95;hObj->chars[120]=96;hObj->chars[121]=97;hObj->chars[122]=98;
	hObj->chars[225]=99;hObj->chars[226]=100;hObj->chars[227]=101;hObj->chars[228]=102;hObj->chars[229]=103;hObj->chars[230]=104;hObj->chars[231]=105;hObj->chars[232]=106;hObj->chars[233]=107;hObj->chars[234]=108;hObj->chars[235]=109;hObj->chars[236]=110;hObj->chars[237]=111;hObj->chars[238]=112;
	hObj->chars[239]=113;hObj->chars[240]=114;hObj->chars[241]=115;hObj->chars[243]=116;hObj->chars[244]=117;hObj->chars[245]=118;hObj->chars[246]=119;hObj->chars[247]=120;hObj->chars[248]=121;hObj->chars[249]=122;//hObj->chars[250]=123;
	hObj->chars[220]=123;hObj->chars[221]=124;hObj->chars[222]=125;hObj->chars[223]=126;hObj->chars[252]=127;hObj->chars[253]=128;hObj->chars[254]=129;hObj->chars[250]=130;hObj->chars[251]=131;hObj->chars[192]=132;hObj->chars[224]=133;	hObj->chars[242]=134;
};

void BPLcc_UFNT_Destroy(BPLcc_uFont* hObj) {
	int i;
	if (hObj->Let!=NULL) {
		for (i=0; i< hObj->MaxFonts; i++) {
			BPLcc_SPR_Destroy(&(hObj->Let[i]));
		}
		free(hObj->Let); hObj->Let=NULL;
	}
}

bool BPLcc_UFNT_TGALoad(BPLcc_uFont* hObj, int NFonts, int fontx, int fonty, char* fname) {
	BPLcc_Sprite im;
	int i,j,k,hi,hj,count,fcount, fboxx;
	BPLcc_SPR_Create(&im); BPLcc_SPR_TGALoad(&im,fname); if (im.mat32 == NULL) { BPLcc_SPR_Destroy(&im); return false; }
	BPLcc_UFNT_Destroy(hObj);
	fboxx = im.sx;
	hObj->FX=fontx; hObj->FY=fonty; hObj->MaxFonts=NFonts;
	hObj->Let=malloc(sizeof(BPLcc_Sprite)*NFonts);
	unsigned long* tmat=malloc(sizeof(unsigned long)*fontx*fonty);
	hi=0; hj=0; fcount=-1;
	for (k=0; k < NFonts; k++) { count=0; fcount++;
		for (j=0; j < fonty; j++) {
			for (i=0; i < fontx; i++) {
				tmat[count++]=im.mat32[i+hi+(j+hj)*im.sx];
			}
		}
		hi+=fontx; if (hi >= fboxx) { hi =0; hj+=fonty; }
		BPLcc_SPR_Create(&(hObj->Let[fcount]));
		BPLcc_SPR_InitByMatrix(&(hObj->Let[fcount]),fontx, fonty, tmat);
	}
	free(tmat);
	BPLcc_SPR_Destroy(&im);
	return true;
}


bool BPLcc_UFNT_CreateFont(BPLcc_uFont* hObj, int NFonts, int fontx, int fonty, int fboxx, int fboxy, int Resourc) {
	HBITMAP bmSpr; int i,j,k,hi,hj,count,fcount; long Ttrans = h2D.Transparent_Color >> 24;
	if (!(bmSpr = LoadBitmap(h2D.hInst,MAKEINTRESOURCE(Resourc)))) { return false; }
	if (!SelectObject(h2D.FontDC,bmSpr)) { return false; }
	hObj->FX=fontx; hObj->FY=fonty; hObj->MaxFonts=NFonts;
	hObj->Let=malloc(sizeof(BPLcc_Sprite)*NFonts);
	unsigned long* tmat=malloc(sizeof(unsigned long)*fontx*fonty);
	hi=0; hj=0; fcount=-1;
	for (k=0; k < NFonts; k++) { count=0; fcount++;
		for (j=0; j < fonty; j++) {
			for (i=0; i < fontx; i++) {
				tmat[count++]=GetPixel(h2D.FontDC,i+hi,j+hj) | (Ttrans << 24);
			}
		}
		hi+=fontx; if (hi >= fboxx) { hi =0; hj+=fonty; }
		BPLcc_SPR_Create(&(hObj->Let[fcount]));
		BPLcc_SPR_InitByMatrix(&(hObj->Let[fcount]),fontx, fonty, tmat);
	}
	free(tmat);
	return true;
}

void BPLcc_UFNT_SetColor(BPLcc_uFont* hObj, int r, int g ,int b) {
	if (hObj->Let == NULL) { return; }
	hObj->CR=r; hObj->CG=g; hObj->CB=b;
	unsigned long Color=(((BYTE) (r) | ((WORD) (g) << 8)) | (((DWORD) (BYTE) (b)) << 16));
	for (int i=0; i < hObj->MaxFonts; i++) {
		for (int j=0; j < hObj->Let[i].sx*hObj->Let[i].sy; j++) {
			if ((hObj->Let[i].mat32[j]!=Color)&&(hObj->Let[i].mat32[j]!=h2D.Transparent_Color)) {
				hObj->Let[i].mat32[j]=Color;
				hObj->Let[i].mat[j*3]=hObj->CB;
				hObj->Let[i].mat[j*3+1]=hObj->CG;
				hObj->Let[i].mat[j*3+2]=hObj->CR;
			}
		}
	}
}


void BPLcc_UFNT_SetSpacing(BPLcc_uFont* hObj, int spacing) {
	hObj->Space = spacing;
}

void BPLcc_UFNT_SetTrans(BPLcc_uFont* hObj, bool onoff) {
	hObj->TextTrans=onoff;
}


void BPLcc_UFNT_Text(BPLcc_uFont* hObj, int x, int y, int d, LPCTSTR str, ...) {
	if (d > h2D.MaxLayer) { return; }
	if (hObj->Let == NULL) { return; }
	int i,j; hObj->xpos=x; hObj->ypos=y; hObj->dpos=d;
	char	strText[256];
	va_list	argumentPtr; va_start(argumentPtr, str); vsprintf(strText, str, argumentPtr);	va_end(argumentPtr);
	//	for (i=0; i < (int)strlen(strText); i++) { strText[i]=toupper(strText[i]); }

	for (i=0; i < (int)strlen(strText); i++) {
		j=hObj->chars[(unsigned char)strText[i]];
		if (hObj->TextTrans==false) {	BPLcc_SPR_Put(&(hObj->Let[j]),x,y,d); }
		else { BPLcc_SPR_PutTrans(&(hObj->Let[j]),x,y,d); }
		x+=hObj->FX+hObj->Space;
	}
}

/*************************************************************************************************************
 *************************************************************************************************************
							Timing Functions Support							TIMETIMETIMETIMETIMETIMETIME
 *************************************************************************************************************
 *************************************************************************************************************/
void BPLcc_TIME_Create(BPLcc_Timer* hObj) {
	hObj->t=NULL; hObj->LTime=NULL; hObj->MaxNum=0; hObj->TThres=NULL;
}

void BPLcc_TIME_Destroy(BPLcc_Timer* hObj) {
	if (hObj->t != NULL) { free(hObj->t); hObj->t=NULL; }
	if (hObj->LTime != NULL) { free(hObj->LTime); hObj->LTime=NULL; }
	if (hObj->TThres != NULL) { free(hObj->TThres); hObj->TThres=NULL; }
	hObj->MaxNum=0;
}

void BPLcc_TIME_Init(BPLcc_Timer* hObj, int Ntim, unsigned long* Thres) {
	int i; unsigned long CreatingTime;
	if (hObj->t != NULL) { BPLcc_TIME_Destroy(hObj); }
	CreatingTime = timeGetTime();
	hObj->MaxNum=Ntim;
	hObj->t = malloc(sizeof(bool)*Ntim);
	hObj->LTime =malloc(sizeof(unsigned long)*Ntim);
	hObj->TThres =malloc(sizeof(unsigned long)*Ntim);
	for (i=0; i < Ntim; i++) {
		hObj->t[i]=false; hObj->LTime[i]=CreatingTime; hObj->TThres[i] = Thres[i];
	}
}

void BPLcc_TIME_Update(BPLcc_Timer* hObj) {
	int i; unsigned long lt = timeGetTime();
	for ( i=0; i < hObj->MaxNum; i++) {
		if ((lt-hObj->LTime[i]) >= hObj->TThres[i]) { hObj->t[i] = true; hObj->LTime[i]=lt; }
	}
}

void BPLcc_TIME_Clear(BPLcc_Timer* hObj, int Timer) {
	if ((Timer < 0) || (Timer >= hObj->MaxNum) || (hObj->t == NULL)) { return; }
	hObj->t[Timer]=false; hObj->LTime[Timer]=timeGetTime();
}

void BPLcc_TIME_ClearAll(BPLcc_Timer* hObj) {
	int i; unsigned long lt = timeGetTime();
	for ( i=0; i < hObj->MaxNum; i++) {	hObj->t[i]=false; 	hObj->LTime[i]=lt;}
}

/*************************************************************************************************************
 *************************************************************************************************************
							Simple Stadar Graphics Functions Support							STDGRSTDGRSTDGRSTDGRSTDGRSTDGRSTDGR
 *************************************************************************************************************
 *************************************************************************************************************/
void BPLcc_SGR_Create(BPLcc_StdGraph* hObj) {
	hObj->Col=0x00FFFFFF; hObj->RDev=1;
}

void BPLcc_SGR_Destroy(BPLcc_StdGraph* hObj) { /* Might be used for future use... */ }

void BPLcc_SGR_Color(BPLcc_StdGraph* hObj, int r, int g, int b) {
	hObj->Col= ((r<<16) | (g<<8)) | b;
}

void BPLcc_SGR_Step(BPLcc_StdGraph* hObj, double RD) {
	if (RD <=0 ) { hObj->RDev=1; } else { hObj->RDev=RD; }
}

void BPLcc_SGR_Line(BPLcc_StdGraph* hObj, int x1, int y1, int x2, int y2, int Layer) {
	double Sx, Sy, Ex, Ey, Stepx, Stepy, R, x, y, dfx, dfy;
	int pos, pos3;
	if (Layer >= h2D.MaxLayer) { return; }
	if (x1 < 0) { x1 = 0; } else if (x1 > h2D.ScreenX-1) { x1 = h2D.ScreenX-1; }
	if (x2 < 0) { x2 = 0; } else if (x2 > h2D.ScreenX-1) { x2 = h2D.ScreenX-1; }
	if (y1 < 0) { y1 = 0; } else if (y1 > h2D.ScreenY-1) { y1 = h2D.ScreenY-1; }
	if (y2 < 0) { y2 = 0; } else if (y2 > h2D.ScreenY-1) { y2 = h2D.ScreenY-1; }
	Sx=x1; Ex=x2; Sy=y1; Ey=y2;
	R=sqrt(pow(x2-x1,2)+pow(y2-y1,2))/hObj->RDev;
	Stepx = (Ex-Sx)/R;
	Stepy = (Ey-Sy)/R;
	x=Sx; y=Sy;
	dfx = x-Ex; if (dfx < 0) {dfx=-dfx;	}
	dfy = y-Ey; if (dfy < 0) { dfy=-dfy; }
	while ((dfx>=1)||(dfy>=1)) {
		x+=Stepx; y+=Stepy;
		if ((x < 0) || (x>h2D.ScreenX-1) || (y < 0) || ( y > h2D.ScreenY-1)) { break; }
			if (Layer <= (h2D.ViewMatrix.Lev[(int)x+((int)y)*h2D.ScreenX])) {
				pos=(int)x+((int)y)*h2D.ScreenX; pos3=pos*3;
				h2D.ViewMatrix.Lev[pos]=Layer;
				h2D.ViewMatrix.rgb[pos3]=(char)(hObj->Col);
				h2D.ViewMatrix.rgb[pos3+1]=(char)(hObj->Col >> 8);
				h2D.ViewMatrix.rgb[pos3+2]=(char)(hObj->Col >> 16);
			}
		dfx = x-Ex; if (dfx < 0) {dfx=-dfx;	}
		dfy = y-Ey; if (dfy < 0) { dfy=-dfy; }
	}
	if (hObj->RDev <= 1.5) {
		BPLcc_SGR_SetPixel(hObj,x2,y2,Layer);
		BPLcc_SGR_SetPixel(hObj,x1,y1,Layer);
	}
}

void BPLcc_SGR_Rect(BPLcc_StdGraph* hObj, int x1, int y1, int x2, int y2, int Layer) {
	BPLcc_SGR_Line(hObj,x1,y1,x2,y1,Layer);
	BPLcc_SGR_Line(hObj,x2,y1,x2,y2,Layer);
	BPLcc_SGR_Line(hObj,x2,y2,x1,y2,Layer);
	BPLcc_SGR_Line(hObj,x1,y2,x1,y1,Layer);
}

void BPLcc_SGR_SetPixel(BPLcc_StdGraph* hObj, int x, int y, int Layer) {
	int pos, pos3;
	if ((x < 0) || ( x >= h2D.ScreenX) || (y < 0) || ( y >= h2D.ScreenY)) { return; }
	pos = x + y * h2D.ScreenX; pos3=pos*3;
	if (Layer <= h2D.ViewMatrix.Lev[pos]) {
		h2D.ViewMatrix.Lev[pos]=Layer;
		h2D.ViewMatrix.rgb[pos3]=(char)(hObj->Col);
		h2D.ViewMatrix.rgb[pos3+1]=(char)(hObj->Col >> 8);
		h2D.ViewMatrix.rgb[pos3+2]=(char)(hObj->Col >> 16);
	}
}

void BPLcc_SGR_Ellipse(BPLcc_StdGraph* hObj, int x1, int y1, int Layer, float Rx, float Ry) {
	float dTh=0.0; int x, y; float MaxR;
	if ((Rx <= 0 )||(Ry <= 0)) { return; }
	if (Rx > Ry) { MaxR=Rx; } else { MaxR=Ry; }
	while (dTh <= 2*BPLcc_Pi) {
		x = x1 + (int)(Rx*cos(dTh));
		y = y1 + (int)(Ry*sin(dTh));
		BPLcc_SGR_SetPixel(hObj,x,y,Layer);
		dTh += (hObj->RDev/MaxR)*2*BPLcc_Pi/360;
	}
}

void BPLcc_SGR_Circle(BPLcc_StdGraph* hObj, int x1, int y1, int Layer, float R) {
	BPLcc_SGR_Ellipse(hObj, x1, y1, Layer, R, R);
}











