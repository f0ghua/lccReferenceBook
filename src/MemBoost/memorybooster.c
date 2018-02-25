//	http://www.wintools.net
//	Author: Ruslan Smekalov
//	doglike76@wintools.net
//	Copyright (c)2003 All right reserved.
//	10.08.2003

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#include <shellapi.h> // for access to system tray
#include <process.h>
#include "common.h"
#include "memoryboosterres.h"

int ReachMem=0;
BOOL bStart=1;
BOOL bOptimize=0;
//BOOL bDialog=0;
char szTitle[200];
char szINIpath[MAX_PATH];
char szLNGpath[MAX_PATH];
char szProgrampath[MAX_PATH];	//for autostart
char szWinToolspath[MAX_PATH];
char szHelppath[MAX_PATH];
void SetWinTexts(void);

int gPercent=0;

MEMORYSTATUS mem;

long lMemoryForFree=0;

#define WM_SYSTEMTRAY WM_USER
#define ONEMEG 1048576
#define MAX_TASKS   256
DWORD   numTasks;
TASK_LIST tlist[MAX_TASKS];

//For sort LV
HWND hwndLVGlob;
int iGlobLVSort=0;
int CALLBACK CompareFunc(int iItemFirst, int iItemSecond, LPARAM lParamSort);

HWND hDLGMAIN;
HINSTANCE hInst;	// Instance handle
HWND hwndMain;		//Main window handle

NOTIFYICONDATA nid;

LRESULT CALLBACK DLGMAINProc(HWND hwndDLGMAIN,UINT uMsg,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

LV_ITEM lvI;
int OptimizeThread(void);
void MainDlg_EntriesTab(int i);
void FillOptions(void);
void SwitchTab(void);
void CheckAutostart(void);
void AlwaysOnTop(void);
void OptimizeMenu(void);
void InitializeTab(HWND hwndDLGMAIN);
void EditSaveIni(HWND hwndDLGMAIN);
int GetRamStatus(void);
int GetProcesses(HWND hDlg);
int KillProc( HWND hDlg );
void FillListView(HWND hWndList, int numItems );


VOID Error(int i){
	char ch[10];
	itoa(i,ch,10);
	MessageBox(NULL,"Error during running program. Report to support@wintools.net about number of error.","ch",MB_OK);
	ExitProcess(0);
}

char* GetLangName(char *iName){
	static char chGlobName[1024]="unnamed";
	GetPrivateProfileString("MEMORYBOOSTER",iName,NULL,chGlobName,1024,szLNGpath);
	if(strchr(chGlobName,'~')){
		for(int i=0; i<strlen(chGlobName); i++){
			if(chGlobName[i]=='~')chGlobName[i]='\n';
		}
	}
	return chGlobName;
}

//////////////////////////////////////////////////////
// Converting of file size							//
//////////////////////////////////////////////////////
void FormatBytes(TCHAR *szMsg, int bytes){
	if (bytes < 1000)
		wsprintf(szMsg, "%d bytes", bytes);
	else if (bytes < 1000000)
		sprintf(szMsg, "%.4g KB", (float)bytes / 1024.0 );
	else if (bytes < 1000000000)
		wsprintf(szMsg, "%d,%03d KB", bytes / 1024000, (bytes / 1024) % 1000);
	else
	    sprintf(szMsg, "%.4g GB", (float)bytes / (1024.0*1024.0*1024.0) );
}

static BOOL InitApplication(void){
	WNDCLASS wc;
	memset(&wc,0,sizeof(WNDCLASS));
	wc.style = CS_HREDRAW|CS_VREDRAW |CS_DBLCLKS ;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.hInstance = hInst;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "memoryboosterWndClass";
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);

	if (!RegisterClass(&wc))return 0;
	if (FindWindowEx(0,0,"memoryboosterWndClass",NULL))return 0; //Проверка на несколько экземпл	ров
	    return 1;
}

HWND CreatememoryboosterWndClassWnd(void){
	return CreateWindow("memoryboosterWndClass","Memory Booster",
		WS_MINIMIZEBOX|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_MAXIMIZEBOX|WS_CAPTION|WS_BORDER|WS_SYSMENU|WS_THICKFRAME,
		CW_USEDEFAULT,0,CW_USEDEFAULT,0,
		NULL,
		NULL,
		hInst,
		NULL);
}

void MainWndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){
	switch(id) {

	case MEN_HELP:
		{
			//WinHelp( hwndMain, szHelppath, HELP_FINDER, 0 );
			WinHelp( hwndMain, szHelppath, HELP_CONTEXT, 21 );
		}
		break;
	case MEN_WT:
		{
			char chTempLang1[MAX_PATH];
			strcpy(chTempLang1,GetLangName("10"));
			char chTempLang2[MAX_PATH];
			strcpy(chTempLang2,GetLangName("12"));
			if(WinExec(szWinToolspath,SW_SHOW)==ERROR_FILE_NOT_FOUND)MessageBox(NULL,chTempLang1,chTempLang2,MB_OK);
		}
		break;
	case MEN_ALL:
		{
			lMemoryForFree=mem.dwTotalPhys;
			OptimizeMenu();
		}
		break;
	case MEN_UP:
		{
			lMemoryForFree=GetPrivateProfileInt("Options","Free",0,szINIpath)*ONEMEG;
			OptimizeMenu();
		}
		break;
	case MEN_10:
		{
			lMemoryForFree=mem.dwTotalPhys/100*10;
			OptimizeMenu();
		}
		break;
	case MEN_20:
		{
			lMemoryForFree=mem.dwTotalPhys/100*20;
			OptimizeMenu();
		}
		break;
	case MEN_30:
		{
			lMemoryForFree=mem.dwTotalPhys/100*30;
			OptimizeMenu();
		}
		break;
	case MEN_40:
		{
			lMemoryForFree=mem.dwTotalPhys/100*40;
			OptimizeMenu();
		}
		break;
	case MEN_50:
		{
			lMemoryForFree=mem.dwTotalPhys/100*50;
			OptimizeMenu();
		}
		break;
	case MEN_60:
		{
			lMemoryForFree=mem.dwTotalPhys/100*60;
			OptimizeMenu();
		}
		break;
	case MEN_70:
		{
			lMemoryForFree=mem.dwTotalPhys/100*70;
			OptimizeMenu();
		}
		break;
	case MEN_80:
		{
			lMemoryForFree=mem.dwTotalPhys/100*80;
			OptimizeMenu();
		}
		break;
	case MEN_90:
		{
			lMemoryForFree=mem.dwTotalPhys/100*90;
			OptimizeMenu();
		}
		break;
	case MEN_100:
		{
			lMemoryForFree=mem.dwTotalPhys;
			OptimizeMenu();
		}
		break;

	case ID_DLG:
		bStart=0;
		if(FindWindow(0,szTitle))break; // get out before creating new dlg
		gPercent=0;
		DialogBox(hInst,"DLGMAIN",hwndMain,(DLGPROC)DLGMAINProc);
		break;

	case IDM_EXIT:
		PostMessage(hwnd,WM_CLOSE,0,0);
		break;
	}
}

HMENU BuildMenu(VOID){

	char szMsg[256];

	HMENU menu = CreatePopupMenu();
	HMENU hSubMenu = CreatePopupMenu();

	AppendMenu(menu, MF_STRING, MEN_ALL, GetLangName("14"));

	wsprintf(szMsg,GetLangName("15"),GetPrivateProfileInt("Options","Free",0,szINIpath));
	AppendMenu(menu, MF_STRING, MEN_UP, szMsg);
	AppendMenu(menu, MF_SEPARATOR, 0, "");
	AppendMenu(menu, MF_STRING | MF_POPUP, (UINT)hSubMenu, GetLangName("16"));

	int i=mem.dwTotalPhys/ONEMEG;
	wsprintf(szMsg,"10%s of %d MB (%d MB)","%",i,i/100*10);
	AppendMenu(hSubMenu, MF_STRING, MEN_10, szMsg);
	wsprintf(szMsg,"20%s of %d MB (%d MB)","%",i,i/100*20);
	AppendMenu(hSubMenu, MF_STRING, MEN_20, szMsg);
	wsprintf(szMsg,"30%s of %d MB (%d MB)","%",i,i/100*30);
	AppendMenu(hSubMenu, MF_STRING, MEN_30, szMsg);
	wsprintf(szMsg,"40%s of %d MB (%d MB)","%",i,i/100*40);
	AppendMenu(hSubMenu, MF_STRING, MEN_40, szMsg);
	wsprintf(szMsg,"50%s of %d MB (%d MB)","%",i,i/100*50);
	AppendMenu(hSubMenu, MF_STRING, MEN_50, szMsg);
	wsprintf(szMsg,"60%s of %d MB (%d MB)","%",i,i/100*60);
	AppendMenu(hSubMenu, MF_STRING, MEN_60, szMsg);
	wsprintf(szMsg,"70%s of %d MB (%d MB)","%",i,i/100*70);
	AppendMenu(hSubMenu, MF_STRING, MEN_70, szMsg);
	wsprintf(szMsg,"80%s of %d MB (%d MB)","%",i,i/100*80);
	AppendMenu(hSubMenu, MF_STRING, MEN_80, szMsg);
	wsprintf(szMsg,"90%s of %d MB (%d MB)","%",i,i/100*90);
	AppendMenu(hSubMenu, MF_STRING, MEN_90, szMsg);
	wsprintf(szMsg,"100%s of %d MB (%d MB)","%",i,i);
	AppendMenu(hSubMenu, MF_STRING, MEN_100, szMsg);

	AppendMenu(menu, MF_SEPARATOR, 0, "");
	AppendMenu(menu, MF_STRING, ID_DLG, GetLangName("17"));
	AppendMenu(menu, MF_STRING, MEN_WT, GetLangName("18"));
	AppendMenu(menu, MF_STRING, MEN_HELP, GetLangName("19"));
	AppendMenu(menu, MF_SEPARATOR, 0, "");
	AppendMenu(menu, MF_STRING, IDM_EXIT, GetLangName("20"));


	return menu;
}

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	switch (msg) {

		HMENU menu;
		POINT pt;
		BOOL  bbool;

	case WM_TIMER:
		switch (wParam){
		case 1:
			GetRamStatus();
			return 0;

		case 2:
			lMemoryForFree=GetPrivateProfileInt("Options","Free",0,szINIpath)*ONEMEG;
			OptimizeMenu();
			return 0;
		}

	case WM_SYSTEMTRAY:
		switch (lParam) {
		case WM_RBUTTONUP:
			bStart=0;
			bbool = GetCursorPos(&pt);
			if(bbool == FALSE) Error(1);
			bbool = SetForegroundWindow(hwnd);
			if(bbool == FALSE) Error(2);
			menu = BuildMenu();
			if(menu == NULL) Error(3);
			TrackPopupMenu(menu,TPM_CENTERALIGN, pt.x, pt.y,0, hwnd, NULL);
			if(menu == FALSE) Error(4);
			bbool = PostMessage(hwnd, 0, 0, 0);
			if(bbool == FALSE) Error(5);
			bbool = DestroyMenu(menu);
			if(bbool == FALSE) Error(6);
			break;

		case WM_LBUTTONDBLCLK:
			bStart=0;
			if(FindWindow(0,szTitle)){
				SetForegroundWindow(hDLGMAIN);
				break; // get out before creating new dlg
			}
			gPercent=0;
			DialogBox(hInst,"DLGMAIN",hwndMain,(DLGPROC)DLGMAINProc);
			break;
		}
		return TRUE;

	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd,wParam,lParam,MainWndProc_OnCommand);
		break;
	case WM_DESTROY:
		// Windows don't remove icon from system tray if application
		// quit, it only check if app exist when mouse is over icon.
		ZeroMemory(&nid,sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		nid.uID = 1;
		Shell_NotifyIcon(NIM_DELETE, &nid);
		KillTimer(hwndMain, 1);
		KillTimer(hwndMain, 2);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow){


	MSG msg;
	HANDLE hAccelTable;

	//Necessary for fill menu mem sizes
	mem.dwLength = sizeof( MEMORYSTATUS );
	GlobalMemoryStatus( &mem );

	GetModuleFileName(NULL,szINIpath,MAX_PATH);
	szINIpath[(strrchr(szINIpath,'\\')-szINIpath)]='\0';

	strcpy(szProgrampath,szINIpath);
	strcpy(szWinToolspath,szINIpath);
	strcpy(szHelppath,szINIpath);		//get Help path
	strcpy(szLNGpath,szINIpath);
	strcat(szLNGpath,"\\Lang\\default.lng");
	strcat(szINIpath,"\\memorybooster.ini");//get INI path
	strcat(szProgrampath,"\\memorybooster.exe");//get Program path
	strcat(szWinToolspath,"\\wintools.exe");//WinTools


	strcat(szHelppath,"\\help\\");		//Получаем help путь
	char chTemp[MAX_PATH];
	GetPrivateProfileString("LANGUAGE","4",NULL,chTemp,MAX_PATH,szLNGpath);
	strcat(szHelppath,chTemp);			//Получаем help путь



	//if selected memory for optimize == 0
	if(!GetPrivateProfileInt("Options","Free",0,szINIpath)){
		char szMsg[10];
		wsprintf(szMsg, "%d",(mem.dwTotalPhys-mem.dwTotalPhys/3)/ONEMEG);
		WritePrivateProfileString("Options","Free",szMsg,szINIpath);
	}

	//Init CommonControl dlls
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_USEREX_CLASSES;
	InitCommonControlsEx(&icex);


	hInst = hInstance;
	if (!InitApplication())
		return 0;
	hAccelTable = LoadAccelerators(hInst,MAKEINTRESOURCE(IDACCEL));
	if ((hwndMain = CreatememoryboosterWndClassWnd()) == (HWND)0)
		return 0;

	// Fill out NOTIFYICONDATA structure for system tray icon
	ZeroMemory(&nid,sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwndMain;
	nid.uID = 1;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = WM_SYSTEMTRAY; // message sent to nid.hWnd
	nid.hIcon = LoadImage(hInstance, MAKEINTRESOURCE(ICO_RAM0), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	lstrcpy(nid.szTip, "Memory Booster");
	if(!Shell_NotifyIcon(NIM_ADD, &nid))return FALSE;

	//CheckAutostart
	CheckAutostart();

    GetRamStatus();

	//SetTimer for refresh
	SetTimer(hwndMain, 1, GetPrivateProfileInt("Options","Refresh",1,szINIpath)*1000, NULL);

	//SetTimer for automatic optimizing
	if(GetPrivateProfileInt("Options","Auto",0,szINIpath)){
		SetTimer(hwndMain, 2, GetPrivateProfileInt("Options","Interval",1,szINIpath)*60000, NULL);
	}

	//For automatic optimizing
	ReachMem=GetPrivateProfileInt("Options","Reach",20,szINIpath);

	//	ShowWindow(hwndMain,SW_SHOW);
	while (GetMessage(&msg,NULL,0,0)) {
		if (!TranslateAccelerator(msg.hwnd,hAccelTable,&msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK DLGMAINProc(HWND hwndDLGMAIN,UINT uMsg,WPARAM wParam,LPARAM lParam){
	switch(uMsg){
	case WM_INITDIALOG :
		{

			hDLGMAIN=hwndDLGMAIN;

			SetWinTexts();

			//Always on top
			AlwaysOnTop();

			//Initialization TabControl
			InitializeTab(hwndDLGMAIN);

			//Show icon
			SendMessage(hwndDLGMAIN, WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst, MAKEINTRESOURCE(ICO_RAM0)));

			//Fill RAM status
			GetRamStatus();

			//Show Tab elements
			SwitchTab();

			//Initialise Processes
			GetProcesses(hDLGMAIN);

			//Initialise Options
			FillOptions();


			if(bStart)beginthread(OptimizeThread,0,NULL);

			bStart=1;

		}
		break;

	case WM_HSCROLL:
		{
			switch (LOWORD(wParam)){
			case TB_ENDTRACK:
			case TB_THUMBTRACK:
				{
					char szMsg[256];
					int iTic=SendMessage(GetDlgItem(hwndDLGMAIN,ID_DMTB),TBM_GETPOS, 0,0);
					wsprintf(szMsg, GetLangName("21"),iTic);
					SetWindowText(GetDlgItem(hwndDLGMAIN,ID_DMT1),szMsg);
					wsprintf(szMsg, "%d",iTic);
					WritePrivateProfileString("Options","Free",szMsg,szINIpath);
				}
				break;
			}
		}
		break;

	case WM_NOTIFY:
		{
			    NM_LISTVIEW * lpnmch = (NM_LISTVIEW*)lParam;		//Нужно дл			CheckBoxes
			    LPNMHDR pnmh=(LPNMHDR)lParam;						//Нужно дл			Notify
			    switch (pnmh->code){

			case LVN_COLUMNCLICK:
				{
					static int iParamSort=1;
					hwndLVGlob=pnmh->hwndFrom;
					iGlobLVSort=lpnmch->iSubItem;
					ListView_SortItemsEx(hwndLVGlob, CompareFunc, iParamSort);
					iParamSort=iParamSort?0:1;
				}
				break;

			case TCN_SELCHANGE:
				{							//Получаем переключение TabControl

					if((HWND)pnmh->hwndFrom==GetDlgItem(hwndDLGMAIN,ID_DMTAB)){
						SwitchTab();
					}
				}
				break;

			}
			break;
		}

	case WM_COMMAND :
		switch(LOWORD(wParam)){

		case ID_CH1:
			{
				char szTemp[5];
				itoa(SendMessage(GetDlgItem(hDLGMAIN,ID_CH1),BM_GETCHECK,0,0),szTemp,10);
				WritePrivateProfileString("Options","Autostart",szTemp,szINIpath);
				CheckAutostart();
			}
			break;

		case ID_CH2:
			{
				char szTemp[5];
				itoa(SendMessage(GetDlgItem(hDLGMAIN,ID_CH2),BM_GETCHECK,0,0),szTemp,10);
				WritePrivateProfileString("Options","Show",szTemp,szINIpath);
			}
			break;

		case ID_CH3:
			{
				char szTemp[5];
				itoa(SendMessage(GetDlgItem(hDLGMAIN,ID_CH3),BM_GETCHECK,0,0),szTemp,10);
				WritePrivateProfileString("Options","Clipboard",szTemp,szINIpath);
			}
			break;

		case ID_CH4:
			{
				char szTemp[5];
				itoa(SendMessage(GetDlgItem(hDLGMAIN,ID_CH4),BM_GETCHECK,0,0),szTemp,10);
				WritePrivateProfileString("Options","Ontop",szTemp,szINIpath);

				//Always on top
				AlwaysOnTop();
			}
			break;

		case ID_CH5:
			{
				char szTemp[5];
				itoa(SendMessage(GetDlgItem(hDLGMAIN,ID_CH5),BM_GETCHECK,0,0),szTemp,10);
				WritePrivateProfileString("Options","Auto",szTemp,szINIpath);

				//Set new Interval automatic optimizing
				if(GetPrivateProfileInt("Options","Auto",0,szINIpath)){
					SetTimer(hwndMain, 2, GetPrivateProfileInt("Options","Interval",1,szINIpath)*60000, NULL);
				}
				else{
					KillTimer(hwndMain, 2);
				}

				FillOptions();
			}
			break;

		case ID_CH6:
			{
				MessageBox(hDLGMAIN,
					"You can get a FREE and FULL version of MemoryBooster from the official site: http://www.wintools.net/\nFull version of MemoryBooster is part of the WinTools.net installation.\nYou NOT NEED REGISTER OR PAY ANYTHING for using of FULL version of MemoryBooster from WinTools.net pack.\n\nIn the FULL version of MemoryBooster:\nIntellectual optimization\nMultilanguage support\nBug fixes\nHelp","This option is available only in the FULL version of MemoryBooster!",
					MB_OK|MB_ICONINFORMATION);
			}
			break;

		case ID_OPTIMIZE:
			{
				//bStart=1;
				lMemoryForFree=GetPrivateProfileInt("Options","Free",0,szINIpath)*ONEMEG;
				beginthread(OptimizeThread,0,NULL);
			}
			break;

		case ID_KILL:
			KillProc(hDLGMAIN);
			break;

		case ID_REFR:
			GetProcesses(hDLGMAIN);
			break;

		case IDOK:
			{
				//Save changes
				EditSaveIni(hwndDLGMAIN);

				//Set new Reach automatic optimizing
				ReachMem=GetPrivateProfileInt("Options","Reach",20,szINIpath);

				//Set interval for refresh
				SetTimer(hwndMain, 1, GetPrivateProfileInt("Options","Refresh",1,szINIpath)*1000, NULL);

				//Set new Interval automatic optimizing
				if(GetPrivateProfileInt("Options","Auto",0,szINIpath)){
					SetTimer(hwndMain, 2, GetPrivateProfileInt("Options","Interval",1,szINIpath)*60000, NULL);
				}
				else{
					KillTimer(hwndMain, 2);
				}
				EndDialog(hwndDLGMAIN,IDOK);
			}
			break;

		case IDCANCEL:
			//					EditSaveIni(hwndDLGMAIN);
			EndDialog(hwndDLGMAIN,IDCANCEL);
			break;

		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////
// Start Optimizing									//
//////////////////////////////////////////////////////
int OptimizeThread(void){
	bOptimize=1;

	EnableWindow(GetDlgItem(hDLGMAIN,ID_OPTIMIZE),FALSE);

	//small addon
	lMemoryForFree+=lMemoryForFree/10;

	//Show "Optimization" in process
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMT1),"Optimization");
	//SetWindowText();

	//Clear Clipboard if selected
	if(GetPrivateProfileInt("Options","Clipboard",0,szINIpath)){
		OpenClipboard(hDLGMAIN);
		EmptyClipboard();
	}

	SendMessage(hDLGMAIN, WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst, MAKEINTRESOURCE(ICO_RAM)));
	nid.hIcon = LoadImage(hInst, MAKEINTRESOURCE(ICO_RAM), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	Shell_NotifyIcon(NIM_MODIFY, &nid);


	//Hide Ticker, and text
	ShowWindow(GetDlgItem(hDLGMAIN,ID_DMTB),SW_HIDE);

	ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR3),SW_SHOW);
	SendMessage(GetDlgItem(hDLGMAIN,ID_DMPROGR3), PBM_SETPOS, 0, 0);

	SendMessage(GetDlgItem(hDLGMAIN,ID_DMPROGR3), PBM_SETRANGE, 0, MAKELPARAM(0, 100/4-1));

	int step=0;

	long * pMem = malloc( lMemoryForFree + 4);

	long percent=lMemoryForFree/100;
	long chlen=percent;

	if(!pMem){
		ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR3),SW_HIDE);
		EnableWindow(GetDlgItem(hDLGMAIN,ID_OPTIMIZE),TRUE);
		return 0;
	}

	long * p = pMem;
	long * d = pMem + lMemoryForFree/sizeof(long);
	while(pMem < d){
		*pMem++ = 0x524D524D;

		if((pMem-p)>chlen){

			SendMessage(GetDlgItem(hDLGMAIN,ID_DMPROGR3), PBM_SETPOS, step++, 0);
			chlen+=percent;

		}
	}


	free( p );
	GetRamStatus();

	ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR3),SW_HIDE);
	ShowWindow(GetDlgItem(hDLGMAIN,ID_DMTB),SW_SHOW);

	bOptimize=0;
	gPercent=0;
	EnableWindow(GetDlgItem(hDLGMAIN,ID_OPTIMIZE),TRUE);
	endthread;
	return 1;
}

//////////////////////////////////////////////////////
// Fill RAM status									//
//////////////////////////////////////////////////////
int GetRamStatus(void){

	//Get memory
	char szMsg[256];
	mem.dwLength = sizeof( MEMORYSTATUS );
	GlobalMemoryStatus( &mem );

	int iPercent=(mem.dwAvailPhys/ONEMEG)*100/(mem.dwTotalPhys/ONEMEG);

	//Run only if dialog opened
	if(FindWindow(0,szTitle)||!bStart){
		TCHAR szMsg2[16], szMsg3[16];
		// Show Physical memory
		FormatBytes(szMsg2, mem.dwAvailPhys);
		FormatBytes(szMsg3, mem.dwTotalPhys - mem.dwAvailPhys);
		wsprintf(szMsg, GetLangName("22"), mem.dwTotalPhys/ONEMEG, szMsg2, szMsg3);
		SetWindowText(GetDlgItem(hDLGMAIN,ID_DMT2),szMsg);
		SendMessage(GetDlgItem(hDLGMAIN,ID_DMPROGR1), PBM_SETPOS, 0, 0);						//Обнул		ем
		    SendMessage(GetDlgItem(hDLGMAIN,ID_DMPROGR1), PBM_SETRANGE, 0, MAKELPARAM(0, mem.dwTotalPhys/ONEMEG));	//Устанавливаем размер
		SendMessage(GetDlgItem(hDLGMAIN,ID_DMPROGR1), PBM_SETPOS, mem.dwAvailPhys/ONEMEG, 0);
		// Show Page File
		FormatBytes(szMsg2, mem.dwTotalPageFile - mem.dwAvailPageFile);
		FormatBytes(szMsg3, mem.dwAvailPageFile);
		wsprintf(szMsg, GetLangName("23"), mem.dwTotalPageFile/ONEMEG, szMsg3, szMsg2);
		SetWindowText(GetDlgItem(hDLGMAIN,ID_DMT3),szMsg);
		SendMessage(GetDlgItem(hDLGMAIN,ID_DMPROGR2), PBM_SETPOS, 0, 0);						//Обнул		ем
		    SendMessage(GetDlgItem(hDLGMAIN,ID_DMPROGR2), PBM_SETRANGE, 0, MAKELPARAM(0, mem.dwTotalPageFile/ONEMEG));	//Устанавливаем размер
		SendMessage(GetDlgItem(hDLGMAIN,ID_DMPROGR2), PBM_SETPOS, mem.dwAvailPageFile/ONEMEG, 0);

		wsprintf(szMsg,"%d",mem.dwTotalPhys/ONEMEG);
		SetWindowText(GetDlgItem(hDLGMAIN,ID_DMT5),szMsg);

		//fill trackbar
		SendMessage(GetDlgItem(hDLGMAIN,ID_DMTB), TBM_SETPOS, (WPARAM) 0, (LPARAM)0);
		SendMessage(GetDlgItem(hDLGMAIN,ID_DMTB), TBM_SETRANGE,(WPARAM) 0,(LPARAM) MAKELONG(0, mem.dwTotalPhys/ONEMEG));
		SendMessage(GetDlgItem(hDLGMAIN,ID_DMTB), TBM_SETPAGESIZE, 0, (LPARAM) 10);
		//		SendMessage(GetDlgItem(hDLGMAIN,ID_DMTB), TBM_SETPOS, (WPARAM) 1, (LPARAM) (mem.dwTotalPhys-mem.dwTotalPhys/3)/ONEMEG);
		SendMessage(GetDlgItem(hDLGMAIN,ID_DMTB), TBM_SETPOS, (WPARAM) 1, (LPARAM)GetPrivateProfileInt("Options","Free",0,szINIpath));
	}

	//Check for autooptimize
	if(GetPrivateProfileInt("Options","Auto",0,szINIpath)){
		if(ReachMem>((mem.dwAvailPhys/ONEMEG))){
			//if < then optimize
			lMemoryForFree=GetPrivateProfileInt("Options","Free",0,szINIpath)*ONEMEG;
			OptimizeMenu();
		}
	}


	if(bOptimize)return 3;


	//Only if was changes
	if(iPercent!=gPercent){

		//Fill Program Name
		wsprintf(szTitle, GetLangName("24"),mem.dwAvailPhys/ONEMEG,iPercent,"%");
		SetWindowText(hDLGMAIN,szTitle);
		lstrcpy(nid.szTip, szTitle);
		Shell_NotifyIcon(NIM_MODIFY, &nid);


		int iTic=SendMessage(GetDlgItem(hDLGMAIN,ID_DMTB),TBM_GETPOS, 0,0);
		wsprintf(szMsg, GetLangName("21"),iTic);
		SetWindowText(GetDlgItem(hDLGMAIN,ID_DMT1),szMsg);


		if(iPercent<20){
			SendMessage(hDLGMAIN, WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst, MAKEINTRESOURCE(ICO_RAM0)));
			nid.hIcon = LoadImage(hInst, MAKEINTRESOURCE(ICO_RAM0), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
			Shell_NotifyIcon(NIM_MODIFY, &nid);
			gPercent=iPercent;
			return 20;
		}

		if(iPercent<40){
			SendMessage(hDLGMAIN, WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst, MAKEINTRESOURCE(ICO_RAM1)));
			nid.hIcon = LoadImage(hInst, MAKEINTRESOURCE(ICO_RAM1), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
			Shell_NotifyIcon(NIM_MODIFY, &nid);
			gPercent=iPercent;
			return 40;
		}

		if(iPercent<60){
			SendMessage(hDLGMAIN, WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst, MAKEINTRESOURCE(ICO_RAM2)));
			nid.hIcon = LoadImage(hInst, MAKEINTRESOURCE(ICO_RAM2), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
			Shell_NotifyIcon(NIM_MODIFY, &nid);
			gPercent=iPercent;
			return 60;
		}

		if(iPercent<80){
			SendMessage(hDLGMAIN, WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst, MAKEINTRESOURCE(ICO_RAM3)));
			nid.hIcon = LoadImage(hInst, MAKEINTRESOURCE(ICO_RAM3), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
			Shell_NotifyIcon(NIM_MODIFY, &nid);
			gPercent=iPercent;
			return 80;
		}


		SendMessage(hDLGMAIN, WM_SETICON, ICON_BIG,(LPARAM) LoadIcon(hInst, MAKEINTRESOURCE(ICO_RAM4)));
		nid.hIcon = LoadImage(hInst, MAKEINTRESOURCE(ICO_RAM4), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
		Shell_NotifyIcon(NIM_MODIFY, &nid);
		gPercent=iPercent;

	}
	return 2;
}


//////////////////////////////////////////////////////
// Tab switch										//
//////////////////////////////////////////////////////
void SwitchTab(void){
	switch(TabCtrl_GetCurSel(GetDlgItem(hDLGMAIN,ID_DMTAB))){
	case 0:
		{
			ShowWindow(GetDlgItem(hDLGMAIN,ID_LV),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_KILL),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_REFR),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_STATIC1),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP1),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP2),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_OPTIMIZE),SW_SHOW);
			//			ShowWindow(GetDlgItem(hDLGMAIN,IDCANCEL),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMTB),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR1),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR2),SW_SHOW);
			//ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR3),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT1),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT2),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT3),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT4),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT5),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH4),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH6),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP4),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH5),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT6),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT7),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT8),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_UD1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_UD2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT9),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT10),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP5),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP6),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ICO_BRAM),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ICO_WT),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT11),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT12),SW_HIDE);
		}
		break;
	case 1:
		{
			ShowWindow(GetDlgItem(hDLGMAIN,ID_KILL),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_REFR),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_LV),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_OPTIMIZE),SW_HIDE);
			//			ShowWindow(GetDlgItem(hDLGMAIN,IDCANCEL),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMTB),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT4),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT5),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_STATIC1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH4),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP4),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH5),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH6),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT6),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT7),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT8),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_UD1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_UD2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT9),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT10),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP5),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP6),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ICO_BRAM),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ICO_WT),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT11),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT12),SW_HIDE);
		}
		break;
	case 2:
		{
			ShowWindow(GetDlgItem(hDLGMAIN,ID_KILL),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_REFR),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_LV),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_OPTIMIZE),SW_HIDE);
			//			ShowWindow(GetDlgItem(hDLGMAIN,IDCANCEL),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMTB),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT4),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT5),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_STATIC1),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH1),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH2),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH3),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH4),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH6),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP3),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP4),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH5),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT6),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT7),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT8),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_UD1),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_UD2),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED1),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED2),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED3),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT9),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT10),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP5),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP6),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ICO_BRAM),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ICO_WT),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT11),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT12),SW_HIDE);
		}
		break;
	case 3:
		{
			ShowWindow(GetDlgItem(hDLGMAIN,ID_KILL),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_REFR),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_LV),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_OPTIMIZE),SW_HIDE);
			//			ShowWindow(GetDlgItem(hDLGMAIN,IDCANCEL),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMTB),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMPROGR3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT4),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT5),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT6),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_STATIC1),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH4),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP4),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH5),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_CH6),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT7),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT8),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_UD1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_UD2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED1),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED2),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_ED3),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT9),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT10),SW_HIDE);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP5),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMGP6),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ICO_BRAM),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ICO_WT),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT11),SW_SHOW);
			ShowWindow(GetDlgItem(hDLGMAIN,ID_DMT12),SW_SHOW);
		}
		break;
	};
}

int GetProcesses(HWND hDlg)
{
	HANDLE hList;
	DWORD rnd;

	DWORD             i;
	TASK_LIST_ENUM    te;
	BOOL              fTree;
	OSVERSIONINFO     verInfo = {
		0	};
	LPGetTaskList     GetTaskList;
	LPEnableDebugPriv EnableDebugPriv;

	// Determine what system we're on and do the right thing
	verInfo.dwOSVersionInfoSize = sizeof (verInfo);
	GetVersionEx(&verInfo);

	switch (verInfo.dwPlatformId){

	case VER_PLATFORM_WIN32_NT:
		GetTaskList     = GetTaskListNT;
		EnableDebugPriv = EnableDebugPrivNT;
		break;

	case VER_PLATFORM_WIN32_WINDOWS:
		GetTaskList = GetTaskList95;
		EnableDebugPriv = EnableDebugPriv95;
		break;

	default:
		// O dear!
		return 0;
	}

	fTree = FALSE;

	// Obtain the ability to manipulate other processes
	EnableDebugPriv();

	// get the task list for the system
	numTasks = GetTaskList( tlist, MAX_TASKS );

	// enumerate all windows and try to get the window
	// titles for each task
	te.tlist = tlist;
	te.numtasks = numTasks;
	GetWindowTitles( &te );

	FillListView(GetDlgItem(hDlg, ID_LV), numTasks);

	return 1;
}

void FillListView(HWND hWndList, int numItems )
{
	//	LV_ITEM	lvIt;
	lvI.mask = LVIF_TEXT;
	char txtbuf[1024];

	ListView_DeleteAllItems(hWndList);

	for (int i = 0; i<numItems; i++ ) {

		lvI.iItem = i;
		lvI.iSubItem = 0;

		sprintf( txtbuf, "%u", tlist[i].dwProcessId );
		lvI.pszText = txtbuf;
		ListView_InsertItem( hWndList, &lvI );

		sprintf( txtbuf, "%s", tlist[i].ProcessName );
		lvI.pszText = txtbuf;
		ListView_SetItemText( hWndList, i, 1, txtbuf );

		if (tlist[i].WindowTitle[0] != '0'){
			sprintf( txtbuf, "%s", tlist[i].WindowTitle );
			lvI.pszText = txtbuf;
			ListView_SetItemText( hWndList, i, 2, txtbuf );
		}
	}

	//Сортируем список
	iGlobLVSort=2;
	hwndLVGlob=hWndList;
	ListView_SortItemsEx(hwndLVGlob, CompareFunc, 0);
}

int KillProc( HWND hDlg )
{
	HWND hListview = GetDlgItem( hDlg, ID_LV );
	int i;
	int cnt = ListView_GetItemCount(hListview);

	// find which item has been selected
	for (i = 0; i<cnt; i++){
		if ( LVIS_SELECTED == ListView_GetItemState(hListview, i, LVIS_SELECTED ) )
			break;
	}

	// nothing selected
	if (i>=cnt){
		char chTempLang1[MAX_PATH];
		strcpy(chTempLang1,GetLangName("26"));
		char chTempLang2[MAX_PATH];
		strcpy(chTempLang2,GetLangName("27"));
		MessageBox(hDlg, chTempLang1, chTempLang2, MB_OK);
		return 0;
	}

	// items in a list box are '0' based, the same as our array tlist[]
	// so just pass tlist[i]
	if (!KillProcess( &tlist[i] )){
		MessageBox(hDlg, GetLangName("28"), "Memory Booster", MB_OK);
	}
	// wait for 1 second before refreshing the list box
	sleep(1000);
	GetProcesses( hDlg );
	return 1;
}

////////////////////////////////////////////////////////////////////////////
//
//		Сортировка записей любого ListView
//
////////////////////////////////////////////////////////////////////////////
int CALLBACK CompareFunc(int iItemFirst, int iItemSecond, LPARAM lParamSort){
	char cmpstr1[MAX_PATH];
	char cmpstr2[MAX_PATH];
	ListView_GetItemText(hwndLVGlob,iItemFirst,iGlobLVSort,cmpstr1,200);
	ListView_GetItemText(hwndLVGlob,iItemSecond,iGlobLVSort,cmpstr2,200);

	if(lParamSort == 1)return strcmp((char*)cmpstr1, (char*)cmpstr2);
	else return -strcmp((char*)cmpstr1, (char*)cmpstr2);
}

////////////////////////////////////////////////////////////////////////////
//
//		Заполнение значений Options
//
////////////////////////////////////////////////////////////////////////////
void FillOptions(void){

	SendMessage(GetDlgItem(hDLGMAIN,ID_CH1),BM_SETCHECK,GetPrivateProfileInt("Options","Autostart",0,szINIpath),0);
	SendMessage(GetDlgItem(hDLGMAIN,ID_CH2),BM_SETCHECK,GetPrivateProfileInt("Options","Show",0,szINIpath),0);
	SendMessage(GetDlgItem(hDLGMAIN,ID_CH3),BM_SETCHECK,GetPrivateProfileInt("Options","Clipboard",0,szINIpath),0);
	SendMessage(GetDlgItem(hDLGMAIN,ID_CH4),BM_SETCHECK,GetPrivateProfileInt("Options","Ontop",0,szINIpath),0);

	int i=GetPrivateProfileInt("Options","Auto",0,szINIpath);
	SendMessage(GetDlgItem(hDLGMAIN,ID_CH5),BM_SETCHECK,i,0);
	EnableWindow(GetDlgItem(hDLGMAIN,ID_UD1),i);
	EnableWindow(GetDlgItem(hDLGMAIN,ID_UD2),i);
	EnableWindow(GetDlgItem(hDLGMAIN,ID_ED1),i);
	EnableWindow(GetDlgItem(hDLGMAIN,ID_ED2),i);


	char szTemp[10];
	GetPrivateProfileString("Options","Interval","",szTemp,9,szINIpath);
	SetWindowText(GetDlgItem(hDLGMAIN,ID_ED1),szTemp);

	GetPrivateProfileString("Options","Reach","",szTemp,9,szINIpath);
	SetWindowText(GetDlgItem(hDLGMAIN,ID_ED2),szTemp);

	GetPrivateProfileString("Options","Refresh","",szTemp,9,szINIpath);
	SetWindowText(GetDlgItem(hDLGMAIN,ID_ED3),szTemp);
}

void CheckAutostart(void){
	//MessageBeep(0);
	HKEY	hKey=0;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS){

		//Get checkstat
		if(GetPrivateProfileInt("Options","Autostart",0,szINIpath)){
			//always write to registry
			RegSetValueEx(hKey,"MemoryBooster", 0, REG_SZ, szProgrampath, strlen(szProgrampath)+1 );
		}
		else{
			//delete from registry
			RegDeleteValue(hKey,"MemoryBooster");
		}
		RegCloseKey(hKey);
	}
}

void AlwaysOnTop(void){
	if(!GetPrivateProfileInt("Options","Ontop",0,szINIpath)){
		SetWindowPos(hDLGMAIN, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE	| SWP_NOSIZE);
	}
	else{
		SetWindowPos(hDLGMAIN, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE	| SWP_NOSIZE);
	}
}

void OptimizeMenu(void){
	bStart=1;
	//Show Control Panel if selected
	if(GetPrivateProfileInt("Options","Show",0,szINIpath)){
		if(FindWindow(0,szTitle)){
			SetForegroundWindow(hDLGMAIN);
			beginthread(OptimizeThread,0,NULL);
		}
		else{
			gPercent=0;
			DialogBox(hInst,"DLGMAIN",hwndMain,(DLGPROC)DLGMAINProc);
		}
	}
	else beginthread(OptimizeThread,0,NULL);
}

void InitializeTab(HWND hwndDLGMAIN){
	//Initialization TabControl and ListView
	SendMessage(GetDlgItem(hDLGMAIN,ID_LV), LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	TC_ITEM tie;
	tie.mask = TCIF_TEXT;
	tie.pszText = GetLangName("29");
	TabCtrl_InsertItem(GetDlgItem(hDLGMAIN,ID_DMTAB),0, &tie);
	tie.pszText = GetLangName("30");
	TabCtrl_InsertItem(GetDlgItem(hDLGMAIN,ID_DMTAB),1, &tie);
	tie.pszText = GetLangName("31");
	TabCtrl_InsertItem(GetDlgItem(hDLGMAIN,ID_DMTAB),2, &tie);
	tie.pszText = GetLangName("32");
	TabCtrl_InsertItem(GetDlgItem(hDLGMAIN,ID_DMTAB),3, &tie);

	//Initialization ListView
	//LV_ITEM lvI;
	LV_COLUMN lvC;
	lvC.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvC.fmt  = LVCFMT_LEFT;
	lvC.cx   = 80;
	lvC.pszText = GetLangName("33");
	ListView_InsertColumn(GetDlgItem(hDLGMAIN,ID_LV), 0, &lvC);
	lvC.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvC.fmt  = LVCFMT_LEFT;
	lvC.cx   = 115;
	lvC.pszText = GetLangName("34");
	ListView_InsertColumn(GetDlgItem(hDLGMAIN,ID_LV), 1, &lvC);
	lvC.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvC.fmt  = LVCFMT_LEFT;
	lvC.cx   = 170;
	lvC.pszText = GetLangName("35");
	ListView_InsertColumn(GetDlgItem(hDLGMAIN,ID_LV), 2, &lvC);
	lvI.mask = LVIF_TEXT | LVIF_STATE | LVCF_WIDTH;
	lvI.state = 0;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;
}

void EditSaveIni(HWND hwndDLGMAIN){
	char szTemp[10];
	GetWindowText(GetDlgItem(hwndDLGMAIN,ID_ED1),szTemp,9);
	WritePrivateProfileString("Options","Interval",szTemp,szINIpath);
	GetWindowText(GetDlgItem(hwndDLGMAIN,ID_ED2),szTemp,9);
	WritePrivateProfileString("Options","Reach",szTemp,szINIpath);
	GetWindowText(GetDlgItem(hwndDLGMAIN,ID_ED3),szTemp,9);
	WritePrivateProfileString("Options","Refresh",szTemp,szINIpath);
}

void SetWinTexts(void){
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMGP1),GetLangName("36"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMGP2),GetLangName("37"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_KILL),GetLangName("38"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_REFR),GetLangName("39"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_CH1),GetLangName("40"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_CH2),GetLangName("41"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_CH3),GetLangName("42"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_CH4),GetLangName("43"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMGP3),GetLangName("44"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMGP4),GetLangName("45"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_CH5),GetLangName("46"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMT6),GetLangName("47"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMT7),GetLangName("48"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMT9),GetLangName("49"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMT8),GetLangName("50"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMGP5),GetLangName("51"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMGP6),GetLangName("52"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMT11),GetLangName("53"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_DMT12),GetLangName("54"));
	SetWindowText(GetDlgItem(hDLGMAIN,ID_OPTIMIZE),GetLangName("25"));
}
