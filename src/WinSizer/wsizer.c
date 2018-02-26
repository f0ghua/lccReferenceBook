//       Title: Window Sizer V3.03
//  Written by: John White <yoingco@btopenworld.com>
//              http://www.btinternet.com/~yoingco/yoingco/
//  C Compiler: lcc-win32 by Jacob Navia.
// Description: Re-Sizes/Moves a Window to within the Desktop area.

#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define IDAPPLICON 710
#define MOD_ALT 1
#define MOD_CTRL 2
#define MOD_SHFT 4
#define PLATFORM_WIN32 0
#define PLATFORM_WIN32_WINDOWS 1
#define PLATFORM_WIN32_NT 2
#define PLATFORM_WIN32_CE 3

HINSTANCE handle_Instance;
HCURSOR cursor_Handle;
HWND wnd0Ptr,actw;
HWND btnPtrs[18];
RECT rect;
POINT point;
BOOL timer_status=FALSE,mouse_status=FALSE,false_true;

unsigned int milliseconds=1,mseconds=1;
unsigned long rtn=0,rtnval=0;
int timer_Handle=0,mouse_Handle=0;
int rightgap=0,bottomgap=0;
int wwidth=0,wheight=0;
int oldtop=0,oldbottom=0,oldleft=0,oldright=0;
int wintop=0,winbottom=0,winleft=0,winright=0;
int window_xlimit=800,window_ylimit=566;
char cha=0,cl=0,ml=0,tmode=0,wmode=1,mmode=1,dmode=0;
char mmbp=0,smode=1,gmode=1,rmode=1,emode=1;
char buf[50];

LRESULT CALLBACK wnd0Procedure(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

BOOL InitApplication(void);
void timer_off(TCHAR);
void timer_on(void);
void timer_code(void);
void mouse_off(void);
void mouse_on(void);
void mouse_code(void);
void show_messagedlg(char);
void recheck_check(void);
BOOL check_wsize(void);
BOOL get_rectangle(void);
BOOL get_wsize(void);
BOOL set_mousepos(void);
BOOL kill_window(void);
void cleanup(void);
void check_limits(void);
void get_desktop(void);
void cad98(char);
void get_winver(void);
void do_winver(void);
HWND Create_JWW0_Window(void);
HWND Create_Option(int,int);
HWND Create_Tick(int,int);
HWND Create_Number(int,int,int,int,char *);
HWND Create_Button(int,int,int,int,char *);
BOOL Create_ToolTip(char,char *);
void load_settings(char *);
void save_settings(char *);
void create_cursor(void);

BOOL InitApplication(void)
{
WNDCLASS wc0;
memset(&wc0,0,sizeof(WNDCLASS));
wc0.style=CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_BYTEALIGNCLIENT |CS_BYTEALIGNWINDOW;
wc0.lpfnWndProc=(WNDPROC)wnd0Procedure;
wc0.hInstance=handle_Instance;
wc0.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
wc0.lpszClassName="JWW0_Class";
wc0.lpszMenuName=NULL;
wc0.hCursor=LoadCursor(NULL,IDC_ARROW);
wc0.hIcon=LoadIcon(handle_Instance,MAKEINTRESOURCE(IDAPPLICON));
if (!RegisterClass(&wc0))
return FALSE;
return TRUE;
}

void show_messagedlg(char status)
{
char stgbuf[100];
if (dmode==1)
{
EnableWindow(wnd0Ptr,FALSE);
if (status==0)
lstrcpy(stgbuf, "Timer (Monitoring) is OFF");
else if (status==1)
lstrcpy(stgbuf, "Timer (Monitoring) is ON");
else if (status==2)
lstrcpy(stgbuf, "Timer (Monitoring) is OFF\n\nQuitting Window Sizer");
else if (status==3)
lstrcpy(stgbuf, "Mouse (Monitoring) is OFF");
else if (status==4)
lstrcpy(stgbuf, "Mouse (Monitoring) is ON");
MessageBox(wnd0Ptr,stgbuf,"  Window Sizer V3.03",MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL);
EnableWindow(wnd0Ptr,TRUE);
}
}

BOOL check_wsize(void)
{
BOOL moved=FALSE;
if (wwidth>window_xlimit)
{
wwidth=window_xlimit;
SetWindowPos(actw, 0, 0, 0, wwidth, wheight, SWP_NOMOVE|SWP_NOZORDER);
moved=TRUE;
}
if (wheight>window_ylimit)
{
wheight=window_ylimit;
SetWindowPos(actw, 0, 0, 0, wwidth, wheight, SWP_NOMOVE|SWP_NOZORDER);
moved=TRUE;
}
if (winleft<0)
{
MoveWindow(actw, 0, wintop, wwidth, wheight, TRUE);
moved=TRUE;
}
if (wintop<0)
{
MoveWindow(actw, winleft, 0, wwidth, wheight, TRUE);
moved=TRUE;
}
if (winright>window_xlimit)
{
rightgap=winright-window_xlimit;
if (winleft>=rightgap)
{
MoveWindow(actw, winleft-rightgap, wintop, wwidth, wheight, TRUE);
moved=TRUE;
}
else
{
SetWindowPos(actw, 0, winleft, wintop, wwidth-rightgap, wheight, SWP_NOMOVE|SWP_NOZORDER);
moved=TRUE;
}
}
if (winbottom>window_ylimit)
{
bottomgap=winbottom-window_ylimit;
if (wintop>=bottomgap)
{
MoveWindow(actw, winleft, wintop-bottomgap, wwidth, wheight, TRUE);
moved=TRUE;
}
else
{
SetWindowPos(actw, 0, winleft, wintop, wwidth, wheight-bottomgap, SWP_NOMOVE|SWP_NOZORDER);
moved=TRUE;
}
}
return moved;
}

BOOL get_rectangle(void)
{
long flong=0;
flong=GetWindowRect(actw,&rect);
if (flong==TRUE)
{
winleft=rect.left;
wintop=rect.top;
winbottom=rect.bottom;
winright=rect.right;
wwidth=winright-winleft;
wheight=winbottom-wintop;
return 0;
}
return 1;
}

void recheck_wsize(void)
{
false_true=check_wsize();
if (false_true==TRUE)
{
false_true=get_rectangle();
if (false_true==FALSE)
{
false_true=check_wsize();
if (false_true==TRUE)
{
false_true=get_rectangle();
if (false_true==FALSE)
{
false_true=check_wsize();
if (false_true==TRUE)
{

}
}
}
}
}
cl=0;
}

BOOL get_wsize(void)
{
long flong=0;
actw=0;
cl=1;
oldtop=wintop;
oldleft=winleft;
oldright=winright;
oldbottom=winbottom;
rtn=GetCursorPos(&point);
if (rtn==TRUE)
{
if (cha==0)
{
cha=1;
actw=WindowFromPoint(point);
}
else if (cha==1)
{
cha=2;
point.x=0;
point.y=0;
actw=WindowFromPoint(point);
}
else if (cha==2)
{
cha=3;
actw=GetForegroundWindow();
}
else if (cha==3)
{
cha=4;
actw=GetActiveWindow();
}
else if (cha==4)
{
cha=0;
point.x=0;
point.y=-5;
actw=WindowFromPoint(point);
}
}
if (actw)
{
rtn=GetClassName(actw,buf,50);
if (rtn)
{
rtn=lstrcmp(buf,"SysListView32");
if (rtn==0)
{
cl=0;
return 0;
}
rtn=lstrcmp(buf,"SysTabControl32");
if (rtn==0)
{
cl=0;
return 0;
}
rtn=lstrcmp(buf,"TrayClockWClass");
if (rtn==0)
{
cl=0;
return 0;
}
rtn=lstrcmp(buf,"TrayNotifyWnd");
if (rtn==0)
{
cl=0;
return 0;
}
rtn=lstrcmp(buf,"cIMDIFrame");
if (rtn==0)
{
cl=0;
return 0;
}
rtn=lstrcmp(buf,"ToolbarWindow32");
if (rtn==0)
{
cl=0;
return 0;
}
rtn=lstrcmp(buf,"ReBarWindow32");
if (rtn==0)
{
cl=0;
return 0;
}
rtn=lstrcmp(buf,"Shell_TrayWnd");
if (rtn==0)
{
cl=0;
return 0;
}
rtn=lstrcmp(buf,"Button");
if (rtn==0)
{
cl=0;
return 0;
}
}
rtn=GetWindowRect(actw,&rect);
if (rtn==TRUE)
{
winleft=rect.left;
wintop=rect.top;
winbottom=rect.bottom;
winright=rect.right;
wwidth=winright-winleft;
wheight=winbottom-wintop;
rtn=GetWindowLong(actw,GWL_STYLE);
if (rtn)
{
if (rtn & (WS_SIZEBOX|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_MINIMIZE|WS_MAXIMIZE|WS_POPUP|WS_CHILD|WS_BORDER|WS_DLGFRAME))
{
flong=(rtn & WS_SIZEBOX);
if (flong==WS_SIZEBOX)
{
recheck_wsize();
return 0;
}
flong=(rtn & WS_MINIMIZEBOX);
if (flong==WS_MINIMIZEBOX)
{
recheck_wsize();
return 0;
}
flong=(rtn & WS_MAXIMIZEBOX);
if (flong==WS_MAXIMIZEBOX)
{
recheck_wsize();
return 0;
}
flong=(rtn & WS_MINIMIZE);
if (flong==WS_MINIMIZE)
{
recheck_wsize();
return 0;
}
flong=(rtn & WS_MAXIMIZE);
if (flong==WS_MAXIMIZE)
{
recheck_wsize();
return 0;
}
flong=(rtn & WS_POPUP);
if (flong==WS_POPUP)
{
recheck_wsize();
return 0;
}
flong=(rtn & WS_CHILD);
if (flong==WS_CHILD)
{
recheck_wsize();
return 0;
}
flong=(rtn & WS_BORDER);
if (flong==WS_BORDER)
{
recheck_wsize();
return 0;
}
flong=(rtn & WS_DLGFRAME);
if (flong==WS_DLGFRAME)
{
recheck_wsize();
return 0;
}
rtn=GetClassName(actw,buf,50);
if (rtn)
{
rtn=lstrcmp(buf,"Notepad");
if (rtn==0)
{
recheck_wsize();
return 0;
}
rtn=lstrcmp(buf,"OpusApp");
if (rtn==0)
{
recheck_wsize();
return 0;
}
rtn=lstrcmp(buf,"IEFrame");
if (rtn==0)
{
recheck_wsize();
return 0;
}
rtn=lstrcmp(buf,"#32770");
if (rtn==0)
{
recheck_wsize();
return 0;
}
}
}
}
}
}
cl=0;
return 0;
}

BOOL set_mousepos(void)
{
RECT mrect;
POINT mpoint;
int right=800,bottom=600;
ml=1;
if (gmode==0)
{
right=window_xlimit;
bottom=window_ylimit;
}
else
{
if (rmode==0)
{
right=640;
bottom=480;
}
else if (rmode==2)
{
right=1024;
bottom=768;
}
}
rtn=GetCursorPos(&mpoint);
if (rtn==TRUE)
{
if (mpoint.x<0)
{
mpoint.x=0;
rtn=SetCursorPos(mpoint.x,mpoint.y);
if (rtn==TRUE)
{

}
}
if (mpoint.x>right)
{
mpoint.x=right;
rtn=SetCursorPos(mpoint.x,mpoint.y);
if (rtn==TRUE)
{

}
}
if (mpoint.y<0)
{
mpoint.y=0;
rtn=SetCursorPos(mpoint.x,mpoint.y);
if (rtn==TRUE)
{

}
}
if (mpoint.y>bottom)
{
mpoint.y=bottom;
rtn=SetCursorPos(mpoint.x,mpoint.y);
if (rtn==TRUE)
{

}
}
rect.left=0;
rect.top=0;
rect.right=right;
rect.bottom=bottom;
rtn=ClipCursor(&mrect);
if (rtn==TRUE)
{

}
}
ml=0;
return 0;
}

void mouse_on(void)
{
if (mouse_status==FALSE)
{
mouse_Handle=SetTimer(0,0,milliseconds,(TIMERPROC)mouse_code);
if (mouse_Handle)
{
mouse_status=TRUE;
show_messagedlg(4);
}
}
}

void mouse_off(void)
{
if (mouse_status==TRUE)
{
rtn=KillTimer(NULL,mouse_Handle);
if (rtn==TRUE)
{
mouse_status=FALSE;
show_messagedlg(3);
}
}
}

void mouse_code(void)
{
if (smode==1)
{
if (ml==0)
set_mousepos();
}
else
set_mousepos();
}

void timer_on(void)
{
if (timer_status==FALSE)
{
timer_Handle=SetTimer(0,0,milliseconds,(TIMERPROC)timer_code);
if (timer_Handle)
{
timer_status=TRUE;
show_messagedlg(1);
}
}
}

void timer_off(TCHAR off_status)
{
if (timer_status==TRUE)
{
rtn=KillTimer(NULL,timer_Handle);
if (rtn==TRUE)
{
timer_status=FALSE;
if (off_status==0)
show_messagedlg(0);
}
}
if (off_status==2)
show_messagedlg(2);
}

void timer_code(void)
{
if (smode==1)
{
if (cl==0)
get_wsize();
}
else
get_wsize();
}

void cad98(char status)
{
if (status==0)
{
rtn=SystemParametersInfo(SPI_SCREENSAVERRUNNING,0,&rtnval,0);
if (rtn==TRUE)
{

}
else
{
tmode=1;
SendMessage(btnPtrs[0],BM_SETCHECK,BST_CHECKED,0);
}
}
else
{
rtn=SystemParametersInfo(SPI_SCREENSAVERRUNNING,1,&rtnval,0);
if (rtn==TRUE)
{

}
else
{
tmode=0;
SendMessage(btnPtrs[0],BM_SETCHECK,BST_UNCHECKED,0);
}
}
}

void create_cursor(void)
{
// 1 = TRANSPARENT
BYTE ANDmask[] =
{
0xF9,0xFF,0xFF,0xFF,
0xF0,0xFF,0xFF,0xFF,
0xF0,0xFF,0xFF,0xFF,
0xF0,0xFF,0xFF,0xFF,
0xF0,0xFF,0xFF,0xFF,
0xF0,0xFF,0xFF,0xFF,
0xF0,0xFF,0xFF,0xFF,
0xF0,0xFF,0xFF,0xFF,
0xF0,0x3F,0xFF,0xFF,
0xF0,0x07,0xFF,0xFF,
0xF0,0x03,0xFF,0xFF,
0xF0,0x00,0xFF,0xFF,
0x90,0x00,0x7F,0xFF,
0x00,0x00,0x7F,0xFF,
0x00,0x00,0x7F,0xFF,
0x80,0x00,0x7F,0xFF,
0xC0,0x00,0x7F,0xFF,
0xC0,0x00,0x7F,0xFF,
0xE0,0x00,0x7F,0xFF,
0xE0,0x00,0xFF,0xFF,
0xF0,0x00,0xFF,0xFF,
0xF0,0x00,0xFF,0xFF,
0xF8,0x01,0xFF,0xFF,
0xF8,0x01,0xFF,0xFF,
0xFC,0x03,0xFF,0xFF,
0xFC,0x03,0xFF,0xFF,
0xFC,0x03,0xFF,0xFF,
0xFC,0x03,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,
};

// 1 = WHITE
BYTE XORmask[] =
{
0x00,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,
0x06,0xC0,0x00,0x00,
0x06,0xD8,0x00,0x00,
0x06,0xD8,0x00,0x00,
0x06,0xDB,0x00,0x00,
0x66,0xDB,0x00,0x00,
0x77,0xFB,0x00,0x00,
0x37,0xFF,0x00,0x00,
0x1F,0xFF,0x00,0x00,
0x1F,0xFF,0x00,0x00,
0x0F,0xFF,0x00,0x00,
0x0F,0xFE,0x00,0x00,
0x07,0xFE,0x00,0x00,
0x07,0xFE,0x00,0x00,
0x03,0xFC,0x00,0x00,
0x03,0xFC,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
};

cursor_Handle=CreateCursor(handle_Instance,6,0,32,32,ANDmask,XORmask);
}

void get_winver(void)
{
OSVERSIONINFO osvi;
osvi.dwOSVersionInfoSize=sizeof(osvi);
rtn=GetVersionEx(&osvi);
if (rtn==TRUE)
{
if (osvi.dwPlatformId==PLATFORM_WIN32_WINDOWS)
{
// Windows 95, 98 and ME.
if (osvi.dwMajorVersion==4)
{
if (osvi.dwMinorVersion==10)
{
// Windows 98.
if (osvi.dwBuildNumber==1998)
mmbp=11;
else if ((osvi.dwBuildNumber>1998) && (osvi.dwBuildNumber<2183))
mmbp=12;
else if (osvi.dwBuildNumber>2182)
mmbp=13;
else
mmbp=201;
}
else if (osvi.dwMinorVersion<10)
{
// Windows 95.
if (osvi.dwBuildNumber==950)
mmbp=1;
else if ((osvi.dwBuildNumber>950) && (osvi.dwBuildNumber<1081))
mmbp=2;
else if (osvi.dwBuildNumber>1080)
mmbp=3;
else
mmbp=200;
}
else if (osvi.dwMinorVersion==90)
{
// Windows ME.
if (osvi.dwBuildNumber==3000)
mmbp=21;
else
mmbp=202;
}
else
mmbp=228;
}
else
mmbp=227;
}
else if (osvi.dwPlatformId==PLATFORM_WIN32_NT)
{
// Windows NT, XP and 2000.
if (osvi.dwMajorVersion==3)
{
// Windows NT 3.51.
if (osvi.dwMinorVersion==51)
{
if (osvi.dwBuildNumber==1057)
mmbp=31;
else
mmbp=203;
}
else
mmbp=226;
}
else if (osvi.dwMajorVersion==4)
{
// Windows NT 4.
if (osvi.dwMinorVersion==0)
{
if (osvi.dwBuildNumber==1381)
mmbp=41;
else
mmbp=204;
}
else
mmbp=225;
}
else if (osvi.dwMajorVersion==5)
{
// Windows 2000 / XP.
if (osvi.dwMinorVersion==0)
{
if (osvi.dwBuildNumber==2195)
// Windows 2000/2000Pro.
mmbp=51;
else
mmbp=205;
}
else if (osvi.dwMinorVersion==1)
// XP.
mmbp=61;
else
mmbp=223;
}
else
mmbp=224;
}
else if (osvi.dwPlatformId==PLATFORM_WIN32_CE)
{
// Pocket Phone, Small Devices, etc.

}
else
mmbp=229;
}
else
mmbp=230;
}

BOOL kill_window(void)
{
HWND killw=0;
char genbuf[50];
killw=GetForegroundWindow();
if ((killw) && (killw!=wnd0Ptr))
{
rtn=GetClassName(killw,genbuf,50);
if (rtn)
{
rtn=lstrcmp(genbuf,"SysListView32");
if (rtn==0)
return 0;
rtn=lstrcmp(genbuf,"SysTabControl32");
if (rtn==0)
return 0;
rtn=lstrcmp(genbuf,"TrayClockWClass");
if (rtn==0)
return 0;
rtn=lstrcmp(genbuf,"TrayNotifyWnd");
if (rtn==0)
return 0;
rtn=lstrcmp(genbuf,"cIMDIFrame");
if (rtn==0)
return 0;
rtn=lstrcmp(genbuf,"ToolbarWindow32");
if (rtn==0)
return 0;
rtn=lstrcmp(genbuf,"ReBarWindow32");
if (rtn==0)
return 0;
rtn=lstrcmp(genbuf,"Shell_TrayWnd");
if (rtn==0)
return 0;
rtn=lstrcmp(genbuf,"Button");
if (rtn==0)
return 0;
}
if (emode<2)
EnableWindow(killw,FALSE);
else
EnableWindow(killw,TRUE);
if ((emode>0) && (emode<3))
{
rtn=DestroyWindow(killw);
if (rtn==TRUE)
{

}
else
{
rtn=CloseWindow(killw);
if (rtn==TRUE)
{

}
}
}
}
return 0;
}

void cleanup(void)
{
mouse_off();
timer_off(2);
false_true=UnregisterHotKey(wnd0Ptr,0);
false_true=UnregisterHotKey(wnd0Ptr,1);
false_true=UnregisterHotKey(wnd0Ptr,2);
false_true=UnregisterHotKey(wnd0Ptr,3);
false_true=UnregisterHotKey(wnd0Ptr,4);
}

void get_desktop(void)
{
RECT dtrect;
HWND desktop=0;
desktop=GetDesktopWindow();
rtn=GetWindowRect(desktop,&dtrect);
if (rtn==TRUE)
{
window_xlimit=dtrect.right;
if (dtrect.right==640)
{
window_ylimit=446;
rmode=0;
rtn=SetWindowText(btnPtrs[10],"0640");
if (rtn==TRUE)
{

}
rtn=SetWindowText(btnPtrs[11],"0446");
if (rtn==TRUE)
{

}
}
else if (dtrect.right==1024)
{
window_ylimit=734;
rmode=2;
rtn=SetWindowText(btnPtrs[10],"1024");
if (rtn==TRUE)
{

}
rtn=SetWindowText(btnPtrs[11],"0734");
if (rtn==TRUE)
{

}
}
else
{
window_xlimit=800;
window_ylimit=566;
rmode=1;
rtn=SetWindowText(btnPtrs[10],"0800");
if (rtn==TRUE)
{

}
rtn=SetWindowText(btnPtrs[11],"0566");
if (rtn==TRUE)
{

}
}
}
}

void check_limits(void)
{
char stgbuf[10];
rtn=GetWindowText(btnPtrs[10],stgbuf,10);
if (rtn!=4)
{
if (rmode==0)
{
window_xlimit=640;
rtn=SetWindowText(btnPtrs[10],"0640");
if (rtn==TRUE)
{

}
}
else if (rmode==1)
{
window_xlimit=800;
rtn=SetWindowText(btnPtrs[10],"0800");
if (rtn==TRUE)
{

}
}
else if (rmode==2)
{
window_xlimit=1024;
rtn=SetWindowText(btnPtrs[10],"1024");
if (rtn==TRUE)
{

}
}
}
rtn=GetWindowText(btnPtrs[11],stgbuf,10);
if (rtn!=4)
{
if (rmode==0)
{
window_ylimit=446;
rtn=SetWindowText(btnPtrs[11],"0446");
if (rtn==TRUE)
{

}
}
else if (rmode==1)
{
window_ylimit=566;
rtn=SetWindowText(btnPtrs[11],"0566");
if (rtn==TRUE)
{

}
}
else if (rmode==2)
{
window_ylimit=1024;
rtn=SetWindowText(btnPtrs[11],"0734");
if (rtn==TRUE)
{

}
}
}
rtn=GetWindowText(btnPtrs[12],stgbuf,10);
if (rtn!=4)
{
milliseconds=1;
rtn=SetWindowText(btnPtrs[12],"0001");
if (rtn==TRUE)
{

}
}
rtn=GetWindowText(btnPtrs[13],stgbuf,10);
if (rtn!=4)
{
mseconds=1;
rtn=SetWindowText(btnPtrs[13],"0001");
if (rtn==TRUE)
{

}
}
}

void save_settings(char *filename)
{
FILE *settings;
int character=0;
char stgbuf[17];
settings=fopen(filename,"wb");
if (settings!=NULL) {
rtn=GetWindowText(btnPtrs[10],stgbuf,5);
if (rtn)
{
rtn=fputs(stgbuf,settings);
if (rtn!=EOF)
{
rtn=GetWindowText(btnPtrs[11],stgbuf,5);
if (rtn)
{
rtn=fputs(stgbuf,settings);
if (rtn!=EOF)
{
rtn=GetWindowText(btnPtrs[12],stgbuf,5);
if (rtn)
{
rtn=fputs(stgbuf,settings);
if (rtn!=EOF)
{
rtn=GetWindowText(btnPtrs[13],stgbuf,5);
if (rtn)
{
rtn=fputs(stgbuf,settings);
if (rtn!=EOF)
{
character=fputc(tmode,settings);
if (character!=EOF)
{
character=fputc(wmode,settings);
if (character!=EOF)
{
character=fputc(mmode,settings);
if (character!=EOF)
{
character=fputc(dmode,settings);
if (character!=EOF)
{
character=fputc(smode,settings);
if (character!=EOF)
{
character=fputc(gmode,settings);
if (character!=EOF)
{
character=fputc(emode,settings);
if (character!=EOF)
{
character=fputc(0,settings);
if (character!=EOF)
{
character=fputc(0,settings);
if (character!=EOF)
{
character=fputc(0,settings);
if (character!=EOF)
{
character=fputc(0,settings);
if (character!=EOF)
{
character=fputc(0,settings);
if (character!=EOF)
{
character=fputc(0,settings);
if (character!=EOF)
{
character=fputc(0,settings);
if (character!=EOF)
{
character=fputc(0,settings);
if (character!=EOF)
{
character=fputc(23,settings);
if (character!=EOF)
{

}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
character=fclose(settings);
if (character!=EOF)
{

}
else
{
// File did not Close.
}
}
else
{
// file did not open.
}
}

void load_settings(char *filename)
{
FILE *settings;
int character=0,fsize=0,position=0,number=0;
char error=0;
char stgbuf[17];
// Open the File.
settings=fopen(filename,"rb");
if (settings!=NULL)
{
// Get the file Size.
character=fgetc(settings);
while (character!=EOF)
{
position++;
character=fgetc(settings);
}
fsize=position;
if (fsize==32)
{
// Calculate/Convert the data.
rtn=fseek(settings,0,SEEK_SET);
if (rtn==0)
{
if (fgets(stgbuf,5,settings))
{
number=atoi(stgbuf);
if (rmode==0)
{
if ((number>99) && (number<641))
{
window_xlimit=number;
rtn=SetWindowText(btnPtrs[10],stgbuf);
if (rtn==TRUE)
{

}
}
else
{
window_xlimit=640;
rtn=SetWindowText(btnPtrs[10],"0640");
if (rtn==TRUE)
{

}
}
}
else if (rmode==1)
{
if ((number>99) && (number<801))
{
window_xlimit=number;
rtn=SetWindowText(btnPtrs[10],stgbuf);
if (rtn==TRUE)
{

}
}
else
{
window_xlimit=800;
rtn=SetWindowText(btnPtrs[10],"0800");
if (rtn==TRUE)
{

}
}
}
else if (rmode==2)
{
if ((number>99) && (number<1025))
{
window_xlimit=number;
rtn=SetWindowText(btnPtrs[10],stgbuf);
if (rtn==TRUE)
{

}
}
else
{
window_xlimit=1024;
rtn=SetWindowText(btnPtrs[10],"1024");
if (rtn==TRUE)
{

}
}
}
}
else
{
error=1;
}
}
else
{
error=1;
}
rtn=fseek(settings,4,SEEK_SET);
if (rtn==0)
{
if (fgets(stgbuf,5,settings))
{
number=atoi(stgbuf);
if (rmode==0)
{
if ((number>99) && (number<447))
{
window_ylimit=number;
rtn=SetWindowText(btnPtrs[11],stgbuf);
if (rtn==TRUE)
{

}
}
else
{
window_ylimit=446;
rtn=SetWindowText(btnPtrs[11],"0446");
if (rtn==TRUE)
{

}
}
}
else if (rmode==1)
{
if ((number>99) && (number<567))
{
window_ylimit=number;
rtn=SetWindowText(btnPtrs[11],stgbuf);
if (rtn==TRUE)
{

}
}
else
{
window_ylimit=566;
rtn=SetWindowText(btnPtrs[11],"0566");
if (rtn==TRUE)
{

}
}
}
else if (rmode==2)
{
if ((number>99) && (number<735))
{
window_ylimit=number;
rtn=SetWindowText(btnPtrs[11],stgbuf);
if (rtn==TRUE)
{

}
}
else
{
window_ylimit=734;
rtn=SetWindowText(btnPtrs[11],"0734");
if (rtn==TRUE)
{

}
}
}
}
else
{
error=1;
}
}
else
{
error=1;
}
rtn=fseek(settings,8,SEEK_SET);
if (rtn==0)
{
if (fgets(stgbuf,5,settings))
{
number=atoi(stgbuf);
if ((number>0) && (number<9001))
{
milliseconds=number;
rtn=SetWindowText(btnPtrs[12],stgbuf);
if (rtn==TRUE)
{

}
}
else
{
milliseconds=1;
rtn=SetWindowText(btnPtrs[12],"0001");
if (rtn==TRUE)
{

}
}
}
else
{
error=1;
}
}
else
{
error=1;
}
rtn=fseek(settings,12,SEEK_SET);
if (rtn==0)
{
if (fgets(stgbuf,5,settings))
{
number=atoi(stgbuf);
if ((number>0) && (number<9001))
{
mseconds=number;
rtn=SetWindowText(btnPtrs[13],stgbuf);
if (rtn==TRUE)
{

}
}
else
{
mseconds=1;
rtn=SetWindowText(btnPtrs[13],"0001");
if (rtn==TRUE)
{

}
}
}
else
{
error=1;
}
}
else
{
error=1;
}
rtn=fseek(settings,16,SEEK_SET);
if (rtn==0)
{

}
else
{
error=1;
}
tmode=fgetc(settings);
if ((tmode<0) || (tmode>1))
{
tmode=0;
}
wmode=fgetc(settings);
if ((wmode<0) || (wmode>1))
{
wmode=1;
}
mmode=fgetc(settings);
if ((mmode<0) || (mmode>1))
{
mmode=1;
}
dmode=fgetc(settings);
if ((dmode<0) || (dmode>1))
{
dmode=0;
}
smode=fgetc(settings);
if ((smode<0) || (smode>1))
{
smode=1;
}
gmode=fgetc(settings);
if ((gmode<0) || (gmode>1))
{
gmode=1;
}
emode=fgetc(settings);
if ((emode<0) || (emode>3))
{
emode=1;
}
}
else
{
// Incorrect file size.
}
character=fclose(settings);
if (character!=EOF)
{

}
else
{
// File did not Close.
}
}
else
{
// file don`t exist.
}
}

BOOL Create_ToolTip(char ttNumber,char *textTT)
{
INITCOMMONCONTROLSEX iccex;
TOOLINFO ti;
HWND handle_ToolTip;
iccex.dwICC=ICC_WIN95_CLASSES;
iccex.dwSize=sizeof(INITCOMMONCONTROLSEX);
InitCommonControlsEx(&iccex);
handle_ToolTip=CreateWindowEx(WS_EX_TOPMOST,TOOLTIPS_CLASS,NULL,
WS_POPUP|TTS_NOPREFIX|TTS_ALWAYSTIP,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
wnd0Ptr,NULL,handle_Instance,NULL);
if (handle_ToolTip)
{
SetWindowPos(handle_ToolTip,HWND_TOPMOST,0,100,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
ti.cbSize=sizeof(TOOLINFO);
ti.uFlags=TTF_SUBCLASS;
ti.hwnd=wnd0Ptr;
ti.hinst=handle_Instance;
ti.uId=0;
ti.lpszText=textTT;
if (ttNumber<17)
{
ti.rect.left=12;
ti.rect.right=188;
if (ttNumber==0)
{
ti.rect.top=10;
ti.rect.bottom=23;
}
else if (ttNumber==1)
{
ti.rect.top=30;
ti.rect.bottom=43;
}
else if (ttNumber==2)
{
ti.rect.top=50;
ti.rect.bottom=63;
}
else if (ttNumber==3)
{
ti.rect.top=70;
ti.rect.bottom=83;
}
else if (ttNumber==4)
{
ti.rect.top=90;
ti.rect.bottom=103;
}
else if (ttNumber==5)
{
ti.rect.top=110;
ti.rect.bottom=123;
}
else if (ttNumber==6)
{
ti.rect.top=140;
ti.rect.bottom=152;
}
else if (ttNumber==7)
{
ti.rect.top=160;
ti.rect.bottom=172;
}
else if (ttNumber==8)
{
ti.rect.top=180;
ti.rect.bottom=192;
}
else if (ttNumber==9)
{
ti.rect.top=200;
ti.rect.bottom=212;
}
else if (ttNumber==10)
{
ti.rect.top=229;
ti.rect.bottom=250;
}
else if (ttNumber==11)
{
ti.rect.top=259;
ti.rect.bottom=280;
}
else if (ttNumber==12)
{
ti.rect.top=297;
ti.rect.bottom=318;
}
else if (ttNumber==13)
{
ti.rect.top=327;
ti.rect.bottom=348;
}
}
else
{
ti.rect.top=387;
ti.rect.bottom=394;
if (ttNumber==17)
{
ti.rect.left=10;
ti.rect.right=56;
}
else if (ttNumber==18)
{
ti.rect.left=75;
ti.rect.right=120;
}
else if (ttNumber==19)
{
ti.rect.left=137;
ti.rect.right=183;
}
}
SendMessage(handle_ToolTip,TTM_ADDTOOL,0,(LPARAM)(LPTOOLINFO) &ti);
SendMessage(handle_ToolTip,TTM_SETMAXTIPWIDTH,0,300);
return TRUE;
}
return FALSE;
}

HWND Create_JWW0_Window(void)
{
RECT rect;
HWND desktop=0;
int x=300,y=92;
desktop=GetDesktopWindow();
rtn=GetWindowRect(desktop,&rect);
if (rtn==TRUE)
{
if (rect.right==640)
{
x=220;
y=32;
}
else if (rect.right==1024)
{
x=412;
y=176;
}
}
return CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,"JWW0_Class"," Window Sizer V3.03",
WS_CAPTION|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,x,y,200,416,NULL,NULL,handle_Instance,NULL);
}

HWND Create_Tick(int x,int y)
{
return CreateWindow("BUTTON","",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BS_NOTIFY,
x,y,13,13,wnd0Ptr,NULL,NULL,NULL);
}

HWND Create_Option(int x,int y)
{
return CreateWindow("BUTTON","",WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|BS_NOTIFY,
x,y,13,12,wnd0Ptr,NULL,NULL,NULL);
}

HWND Create_Number(int x,int y,int w,int h,char *numstg)
{
return CreateWindow("EDIT",numstg,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT|ES_NUMBER,
x,y,w,h,wnd0Ptr,NULL,NULL,NULL);
}

HWND Create_Button(int x,int y,int w,int h,char *title)
{
return CreateWindow("BUTTON",title,
WS_CHILD|WS_VISIBLE|BS_CENTER|BS_VCENTER|BS_NOTIFY|BS_PUSHLIKE|BS_TEXT,
x,y,w,h,wnd0Ptr,NULL,NULL,NULL);
}

LRESULT CALLBACK wnd0Procedure(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
HDC hdc;
PAINTSTRUCT ps;
RECT srect;
HBRUSH sbrush;
HGDIOBJ old1font,old2font,cur1font,cur2font;
COLORREF colour;
int hsize=0,wsize=0,number=0;
char stgbuf[10];

switch (msg)
{

case WM_PAINT:
colour=GetSysColor(COLOR_MENU);
hdc=BeginPaint(wnd0Ptr,&ps);
if (hdc)
{
rtn=GetClientRect(wnd0Ptr,&srect);
if (rtn)
{
rtn=SetBkMode(hdc,OPAQUE);
if (rtn)
{
rtn=SetBkColor(hdc,colour);
if (rtn!=CLR_INVALID)
{
rtn=SetTextColor(hdc,0x00FFFFFF);
if (rtn!=CLR_INVALID)
{
hsize=-MulDiv(4,GetDeviceCaps(hdc,LOGPIXELSX),72);
wsize=-MulDiv(4,GetDeviceCaps(hdc,LOGPIXELSY),72);
cur1font=CreateFont(hsize,wsize,1,1,FW_NORMAL,0,0,0,1,0,0,0,0,"MS Sans Serif");
if (cur1font)
{
hsize=-MulDiv(6,GetDeviceCaps(hdc,LOGPIXELSX),72);
wsize=-MulDiv(6,GetDeviceCaps(hdc,LOGPIXELSY),72);
cur2font=CreateFont(hsize,wsize,1,1,FW_NORMAL,0,0,0,1,0,0,0,0,"MS Sans Serif");
if (cur2font)
{
sbrush=CreateSolidBrush(colour);
if (sbrush)
{
rtn=FillRect(hdc,&srect,sbrush);
if (rtn==TRUE)
{
old1font=SelectObject(hdc,cur1font);
if ((old1font) && ((long)old1font!=GDI_ERROR))
{
rtn=TextOut(hdc,28,10,"Disable CTRL+ALT+DEL",20);
if (rtn)
{
rtn=TextOut(hdc,28,30,"Window Sizer On",15);
if (rtn)
{
rtn=TextOut(hdc,28,50,"Mouse Mover On",14);
if (rtn)
{
rtn=TextOut(hdc,28,70,"Show Off/On Messages",20);
if (rtn)
{
rtn=TextOut(hdc,28,90,"Timer/s in Safe Mode",20);
if (rtn)
{
rtn=TextOut(hdc,28,110,"Whole Desktop - Mouse Mover",27);
if (rtn)
{
rtn=TextOut(hdc,28,139,"Kill Window - Disable Only",26);
if (rtn)
{
rtn=TextOut(hdc,28,159,"Kill Window - Disable/Minimize",30);
if (rtn)
{
rtn=TextOut(hdc,28,179,"Kill Window - Minimize Only",27);
if (rtn)
{
rtn=TextOut(hdc,28,199,"Kill Window - Enable Only",25);
if (rtn)
{
old2font=SelectObject(hdc,cur2font);
if ((old2font) && ((long)old2font!=GDI_ERROR))
{
rtn=TextOut(hdc,55,231,"Desktop Width Area",18);
if (rtn)
{
rtn=TextOut(hdc,55,261,"Desktop Height Area",19);
if (rtn)
{
rtn=TextOut(hdc,55,299,"Window Sizer Timer",18);
if (rtn)
{
rtn=TextOut(hdc,55,329,"Mouse Mover Timer",17);
if (rtn)
{
rtn=SetBkColor(hdc,0x00000000);
if (rtn!=CLR_INVALID)
{
rtn=MoveToEx(hdc,0,0,NULL);
if (rtn)
{
rtn=LineTo(hdc,200,0);
if (rtn)
{
rtn=MoveToEx(hdc,0,131,NULL);
if (rtn)
{
rtn=LineTo(hdc,200,131);
if (rtn)
{
rtn=MoveToEx(hdc,0,220,NULL);
if (rtn)
{
rtn=LineTo(hdc,200,220);
if (rtn)
{
rtn=MoveToEx(hdc,0,288,NULL);
if (rtn)
{
rtn=LineTo(hdc,200,288);
if (rtn)
{
rtn=MoveToEx(hdc,0,356,NULL);
if (rtn)
{
rtn=LineTo(hdc,200,356);
if (rtn)
{

}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
rtn=DeleteObject(sbrush);
if (rtn)
{

}
}
rtn=DeleteObject(cur2font);
if (rtn)
{

}
}
rtn=DeleteObject(cur1font);
if (rtn)
{
SelectObject(hdc,old1font);
}
}
}
}
}
}
EndPaint(wnd0Ptr,&ps);
}
return 0;

case WM_SETFOCUS:
case WM_KILLFOCUS:
check_limits();
return 0;

case WM_COMMAND:
if (HIWORD(wParam)==BN_CLICKED)
{
check_limits();
if ((HWND)lParam==btnPtrs[0])
{
rtn=SendMessage(btnPtrs[0],BM_GETCHECK,0,0);
if (rtn==BST_UNCHECKED)
{
tmode=0;
if (mmbp==13)
cad98(0);
}
else
{
tmode=1;
if (mmbp==13)
cad98(1);
}
return 0;
}
else if ((HWND)lParam==btnPtrs[1])
{
rtn=SendMessage(btnPtrs[1],BM_GETCHECK,0,0);
if (rtn==BST_UNCHECKED)
{
wmode=0;
timer_off(0);
}
else
{
wmode=1;
timer_on();
}
return 0;
}
else if ((HWND)lParam==btnPtrs[2])
{
rtn=SendMessage(btnPtrs[2],BM_GETCHECK,0,0);
if (rtn==BST_UNCHECKED)
{
mmode=0;
mouse_off();
}
else
{
mmode=1;
mouse_on();
}
return 0;
}
else if ((HWND)lParam==btnPtrs[3])
{
rtn=SendMessage(btnPtrs[3],BM_GETCHECK,0,0);
if (rtn==BST_UNCHECKED)
dmode=0;
else
dmode=1;
return 0;
}
else if ((HWND)lParam==btnPtrs[4])
{
rtn=SendMessage(btnPtrs[4],BM_GETCHECK,0,0);
if (rtn==BST_UNCHECKED)
smode=0;
else
smode=1;
return 0;
}
else if ((HWND)lParam==btnPtrs[5])
{
rtn=SendMessage(btnPtrs[5],BM_GETCHECK,0,0);
if (rtn==BST_UNCHECKED)
gmode=0;
else
gmode=1;
return 0;
}
else if ((HWND)lParam==btnPtrs[6])
{
emode=0;
return 0;
}
else if ((HWND)lParam==btnPtrs[7])
{
emode=1;
return 0;
}
else if ((HWND)lParam==btnPtrs[8])
{
emode=2;
return 0;
}
else if ((HWND)lParam==btnPtrs[9])
{
emode=3;
return 0;
}
else if ((HWND)lParam==btnPtrs[14])
{
EnableWindow(wnd0Ptr,FALSE);
ShowWindow(wnd0Ptr,SW_HIDE);
return 0;
}
else if ((HWND)lParam==btnPtrs[15])
{
check_limits();
save_settings("WSSettings");
return 0;
}
else if ((HWND)lParam==btnPtrs[16])
{
EnableWindow(wnd0Ptr,FALSE);
ShowWindow(wnd0Ptr,SW_HIDE);
cleanup();
PostQuitMessage(0);
return 0;
}
}
else if (HIWORD(wParam)==EN_SETFOCUS)
{
check_limits();
if ((HWND)lParam==btnPtrs[10])
{

}
else if ((HWND)lParam==btnPtrs[11])
{

}
else if ((HWND)lParam==btnPtrs[12])
{

}
else if ((HWND)lParam==btnPtrs[13])
{

}
return 0;
}
else if (HIWORD(wParam)==EN_MAXTEXT)
{
check_limits();
if ((HWND)lParam==btnPtrs[10])
{

}
else if ((HWND)lParam==btnPtrs[11])
{

}
else if ((HWND)lParam==btnPtrs[12])
{

}
else if ((HWND)lParam==btnPtrs[13])
{

}
return 0;
}
else if (HIWORD(wParam)==EN_CHANGE)
{
if ((HWND)lParam==btnPtrs[10])
{
rtn=GetWindowText(btnPtrs[10],stgbuf,10);
if (rtn==4)
{
number=atoi(stgbuf);
if (rmode==0)
{
if ((number>99) && (number<641))
window_xlimit=number;
else
{
window_xlimit=640;
rtn=SetWindowText(btnPtrs[10],"0640");
if (rtn)
{

}
}
}
else if (rmode==1)
{
if ((number>99) && (number<801))
window_xlimit=number;
else
{
window_xlimit=800;
rtn=SetWindowText(btnPtrs[10],"0800");
if (rtn)
{

}
}
}
else if (rmode==2)
{
if ((number>99) && (number<1025))
window_xlimit=number;
else
{
window_xlimit=1024;
rtn=SetWindowText(btnPtrs[10],"1024");
if (rtn)
{

}
}
}
}
}
else if ((HWND)lParam==btnPtrs[11])
{
rtn=GetWindowText(btnPtrs[11],stgbuf,10);
if (rtn==4)
{
number=atoi(stgbuf);
if (rmode==0)
{
if ((number>99) && (number<447))
window_ylimit=number;
else
{
window_ylimit=446;
rtn=SetWindowText(btnPtrs[11],"0446");
if (rtn)
{

}
}
}
else if (rmode==1)
{
if ((number>99) && (number<567))
window_ylimit=number;
else
{
window_ylimit=566;
rtn=SetWindowText(btnPtrs[11],"0566");
if (rtn)
{

}
}
}
else if (rmode==2)
{
if ((number>99) && (number<735))
window_ylimit=number;
else
{
window_ylimit=734;
rtn=SetWindowText(btnPtrs[11],"0734");
if (rtn)
{

}
}
}
}
}
else if ((HWND)lParam==btnPtrs[12])
{
rtn=GetWindowText(btnPtrs[12],stgbuf,10);
if (rtn==4)
{
number=atoi(stgbuf);
if ((number>0) && (number<9001))
milliseconds=number;
else
{
milliseconds=1;
rtn=SetWindowText(btnPtrs[12],"0001");
if (rtn)
{

}
}
}
}
else if ((HWND)lParam==btnPtrs[13])
{
rtn=GetWindowText(btnPtrs[13],stgbuf,10);
if (rtn==4)
{
number=atoi(stgbuf);
if ((number>0) && (number<9001))
mseconds=number;
else
{
mseconds=1;
rtn=SetWindowText(btnPtrs[13],"0001");
if (rtn)
{

}
}
}
}
return 0;
}
break;

case WM_DISPLAYCHANGE:
get_desktop();

case WM_SHOWWINDOW:
if (wParam==TRUE)
{

}
break;

case WM_HOTKEY:
if (wParam==0)
{
cleanup();
PostQuitMessage(0);
break;
}
else if (wParam==1)
timer_on();
else if (wParam==2)
timer_off(0);
else if (wParam==3)
{
EnableWindow(wnd0Ptr,TRUE);
ShowWindow(wnd0Ptr,SW_SHOW);
}
else if (wParam==4)
kill_window();

default:
return DefWindowProc(hwnd,msg,wParam,lParam);
}
return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
MSG msg;
HANDLE mutex_handle;
char counter=0;
handle_Instance=hInstance;
char txt0[ ]="Disable CTRL+ALT+DEL keys.\n\nCurrently only for Win98 SE.";
char txt1[ ]="Switch Window Sizer Off/On.\n\nON - Re-Size and/or Move a window\n        to within the Desktop Area.";
char txt2[ ]="Switch Mouse Mover Off/On.\n\nON - Keep the mouse pointer\n        within the Desktop Area.";
char txt3[ ]="Switch Messages Off/On.\n\nON - Show Notification Messages.";
char txt4[ ]="Switch Timer/s Safety Off/On.\n\nON - Do not re-run a Timer until\n        its function has finished.";
char txt5[ ]="Change Mouse Mover`s Desktop Area.\n\nOFF - Use custom desktop area.\n ON - Use whole desktop area.";
char txt6[ ]="Disable, only, the currently selected window.";
char txt7[ ]="Disable and Minimize the currently selected window.";
char txt8[ ]="Minimize, only, the currently selected window.";
char txt9[ ]="Enable, only, the currently selected window.";
char txt10[ ]="Enter a 4 Digit, custom, Desktop Width.\n\n 640 x 480 - Range 0100 to 0640\n 800 x 600 - Range 0100 to 0800\n1024 x 768 - Range 0100 to 1024";
char txt11[ ]="Enter a 4 Digit, custom, Desktop Height.\n\n 640 x 480 - Range 0100 to 0446\n 800 x 600 - Range 0100 to 0566\n1024 x 768 - Range 0100 to 0734";
char txt12[ ]="Enter a 4 Digit, Window Sizer, Timer value.\n\nMilliseconds Range: 0001 to 9000";
char txt13[ ]="Enter a 4 Digit, Mouse Mover, Timer value.\n\nMilliseconds Range: 0001 to 9000";
char txt17[ ]="Hide this G.U.I";
char txt18[ ]="Save these Settings";
char txt19[ ]="Quit Window Sizer";
mutex_handle=CreateMutex(NULL,TRUE,"Window Sizer");
if (mutex_handle)
{
rtn=GetLastError();
if (rtn!=ERROR_ALREADY_EXISTS)
{
rtn=InitApplication();
if (rtn==1)
{
get_winver();
create_cursor();
wnd0Ptr=Create_JWW0_Window();
if (wnd0Ptr)
{
btnPtrs[0]=Create_Tick(10,10);
if (btnPtrs[0])
{
btnPtrs[1]=Create_Tick(10,30);
if (btnPtrs[1])
{
btnPtrs[2]=Create_Tick(10,50);
if (btnPtrs[2])
{
btnPtrs[3]=Create_Tick(10,70);
if (btnPtrs[3])
{
btnPtrs[4]=Create_Tick(10,90);
if (btnPtrs[4])
{
btnPtrs[5]=Create_Tick(10,110);
if (btnPtrs[5])
{
btnPtrs[6]=Create_Option(10,140);
if (btnPtrs[6])
{
btnPtrs[7]=Create_Option(10,160);
if (btnPtrs[7])
{
btnPtrs[8]=Create_Option(10,180);
if (btnPtrs[8])
{
btnPtrs[9]=Create_Option(10,200);
if (btnPtrs[9])
{
btnPtrs[10]=Create_Number(10,229,40,21,"0800");
if (btnPtrs[10])
{
btnPtrs[11]=Create_Number(10,259,40,21,"0566");
if (btnPtrs[11])
{
btnPtrs[12]=Create_Number(10,297,40,21,"0001");
if (btnPtrs[12])
{
btnPtrs[13]=Create_Number(10,327,40,21,"0001");
if (btnPtrs[13])
{
btnPtrs[14]=Create_Button(10,365,46,21,"HIDE");
if (btnPtrs[14])
{
btnPtrs[15]=Create_Button(74,365,46,21,"SAVE");
if (btnPtrs[15])
{
btnPtrs[16]=Create_Button(137,365,46,21,"QUIT");
if (btnPtrs[16])
{
if (cursor_Handle)
{
if (SetClassLong(wnd0Ptr,GCL_HCURSOR,(LONG)cursor_Handle))
{
for (counter=0;counter<19;counter++)
{
if (SetClassLong(btnPtrs[counter],GCL_HCURSOR,(LONG)cursor_Handle))
{

}
}
}
}
if (Create_ToolTip(0,txt0))
{
if (Create_ToolTip(1,txt1))
{
if (Create_ToolTip(2,txt2))
{
if (Create_ToolTip(3,txt3))
{
if (Create_ToolTip(4,txt4))
{
if (Create_ToolTip(5,txt5))
{
if (Create_ToolTip(6,txt6))
{
if (Create_ToolTip(7,txt7))
{
if (Create_ToolTip(8,txt8))
{
if (Create_ToolTip(9,txt9))
{
if (Create_ToolTip(10,txt10))
{
if (Create_ToolTip(11,txt11))
{
if (Create_ToolTip(12,txt12))
{
if (Create_ToolTip(13,txt13))
{
if (Create_ToolTip(17,txt17))
{
if (Create_ToolTip(18,txt18))
{
if (Create_ToolTip(19,txt19))
{
SendMessage(btnPtrs[10],EM_SETLIMITTEXT,4,0);
SendMessage(btnPtrs[11],EM_SETLIMITTEXT,4,0);
SendMessage(btnPtrs[12],EM_SETLIMITTEXT,4,0);
SendMessage(btnPtrs[13],EM_SETLIMITTEXT,4,0);
get_desktop();
load_settings("WSSettings");
if (tmode)
SendMessage(btnPtrs[0],BM_SETCHECK,BST_CHECKED,0);
if (dmode)
SendMessage(btnPtrs[3],BM_SETCHECK,BST_CHECKED,0);
if (smode)
SendMessage(btnPtrs[4],BM_SETCHECK,BST_CHECKED,0);
if (gmode)
SendMessage(btnPtrs[5],BM_SETCHECK,BST_CHECKED,0);
if (emode==0)
SendMessage(btnPtrs[6],BM_SETCHECK,BST_CHECKED,0);
else if (emode==1)
SendMessage(btnPtrs[7],BM_SETCHECK,BST_CHECKED,0);
else if (emode==2)
SendMessage(btnPtrs[8],BM_SETCHECK,BST_CHECKED,0);
else if (emode==3)
SendMessage(btnPtrs[9],BM_SETCHECK,BST_CHECKED,0);
if (wmode)
{
SendMessage(btnPtrs[1],BM_SETCHECK,BST_CHECKED,0);
timer_on();
}
if (mmode)
{
SendMessage(btnPtrs[2],BM_SETCHECK,BST_CHECKED,0);
mouse_on();
}

// Allocate the Quit Window Sizer key.
rtn=RegisterHotKey(wnd0Ptr,0,MOD_CTRL|MOD_ALT,81);
if (rtn==0)
{
rtn=RegisterHotKey(wnd0Ptr,0,MOD_CTRL|MOD_ALT,88);
if (rtn==0)
{
MessageBox(NULL,"Could not Allocate a Quit HotKey","  Window Sizer V3.03",MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL);
}
}

// Allocate the Timer Enable key.
rtn=RegisterHotKey(wnd0Ptr,1,MOD_CTRL|MOD_ALT,69);
if (rtn==0)
{
rtn=RegisterHotKey(wnd0Ptr,1,MOD_CTRL|MOD_ALT,79);
if (rtn==0)
{
MessageBox(NULL,"Could not Allocate an Enable HotKey","  Window Sizer V3.03",MB_OK|MB_ICONINFORMATION);
}
}

// Allocate the Timer Disable key.
rtn=RegisterHotKey(wnd0Ptr,2,MOD_CTRL|MOD_ALT,68);
if (rtn==0)
{
rtn=RegisterHotKey(wnd0Ptr,2,MOD_CTRL|MOD_ALT,70);
if (rtn==0)
{
MessageBox(NULL,"Could not Allocate a Disable HotKey","  Window Sizer V3.03",MB_OK|MB_ICONINFORMATION);
}
}

// Allocate the Timer G.U.I key.
rtn=RegisterHotKey(wnd0Ptr,3,MOD_CTRL|MOD_ALT,71);
if (rtn==0)
{
rtn=RegisterHotKey(wnd0Ptr,3,MOD_CTRL|MOD_ALT,87);
if (rtn==0)
{
MessageBox(NULL,"Could not Allocate a G.U.I HotKey","  Window Sizer V3.03",MB_OK|MB_ICONINFORMATION);
}
}

// Allocate the Kill Window key.
rtn=RegisterHotKey(wnd0Ptr,4,MOD_CTRL|MOD_ALT,75);
if (rtn==0)
{
rtn=RegisterHotKey(wnd0Ptr,4,MOD_CTRL|MOD_ALT,67);
if (rtn==0)
{
MessageBox(NULL,"Could not Allocate a Kill Window HotKey","  Window Sizer V3.03",MB_OK|MB_ICONINFORMATION);
}
}

while (GetMessage(&msg,NULL,0,0))
{
TranslateMessage(&msg);
DispatchMessage(&msg);
}

if (cursor_Handle)
DestroyCursor(cursor_Handle);
if (mutex_handle)
ReleaseMutex(mutex_handle);

return msg.wParam;

}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
if (mutex_handle)
ReleaseMutex(mutex_handle);
}
else
{
MessageBox(NULL,"Window Sizer  -  Already Running!","  Window Sizer V3.03",MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL);
return 1;
}
}
else
{
MessageBox(NULL,"Could Not CreateMutex()","  Window Sizer V3.03",MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL);
return 1;
}
return 0;
}

