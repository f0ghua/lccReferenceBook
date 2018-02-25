
//DEMONSTRATE HOW TO USE LEDLIB
//JWH - DECEMBER 2000
//
//Compile AS IS with LCC-WIN32:
//  Add ledlib.lib library in the linker tab of Wedit

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include "tstledlibres.h"

#include "ledlib.h"

#define NLEDS 24
HLED tl1[NLEDS];
HLED tl2[NLEDS];
HLED tl3[NLEDS/2];
HLED l1,l2,l3;

static int InitializeApp(HWND hDlg,WPARAM wParam, LPARAM lParam)
{
    //Init LIB (MANDATORY)
	SetDlgItemText(hDlg,ID_VERSION,ledlib_start());

	//Create some leds
	for(int ii=0;ii<NLEDS;ii++)
	  tl1[ii]=ledlib_create_init(hDlg,56+LEDLIB_SMALL_GAP*ii,45,LED_HVUMETRE,LED_RED,LED_OFF);
	for(int ii=0;ii<NLEDS;ii++)
	  tl2[ii]=ledlib_create_init(hDlg,56+LEDLIB_SMALL_GAP*ii,60,LED_HVUMETRE,LED_GREEN,LED_OFF);
	for(int ii=0;ii<NLEDS/2;ii++)
	  tl3[ii]=ledlib_create_init(hDlg,200,105-LEDLIB_SMALL_GAP*ii,LED_VVUMETRE,ii<6?LED_GREEN:ii<9?LED_YELLOW:LED_RED,LED_OFF);

    l1=ledlib_create_init(hDlg,100,95,LED_SQUARE,LED_BLUE,LED_ON);
    l2=ledlib_create_init(hDlg,120,95,LED_RECT,LED_GREEN,LED_ON);
    l3=ledlib_create_init(hDlg,110,110,LED_ROUND,LED_YELLOW,LED_ON);

	return 1;
}

static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
static int i1=0, i2=-1 ,i3=-1;
static int ai=1;
static int il3=0;

	switch (msg) {

	case WM_INITDIALOG:
		InitializeApp(hwndDlg,wParam,lParam);
		SetTimer(hwndDlg,1,125,NULL);
		return TRUE;

		case WM_PAINT:
    		ledlib_draw_tab(tl1,NLEDS);
			ledlib_draw_tab(tl2,NLEDS);
			ledlib_draw_tab(tl3,NLEDS/2);
			ledlib_draw(l1);
			ledlib_draw(l2);
			ledlib_draw(l3);
			return 0;

	case WM_TIMER:
	      //The blinking yellow
          if(! (++il3 % 5)) ledlib_draw_swap_mode(l3);

		  //The horizontals
		  ledlib_draw_swap_mode(tl1[i1]);
		  ledlib_draw_swap_mode(tl2[i1]);
		  if(i2>=0) {
		   	ledlib_draw_swap_mode(tl1[i2]);
			ledlib_draw_swap_mode(tl2[i2]);
		  }
		  if(++i1==NLEDS) i1=0;
		  if(++i2==NLEDS) i2=0;

          //The vertical
		  if(ai==1) {
		    if(i3==(NLEDS/2)-1) {
		      ai=-1;
		      ledlib_draw_swap_mode(tl3[i3]);
			  return 0;
			}
		  } else
		    if(i3==0) {
		      ai=1;
		      ledlib_draw_swap_mode(tl3[i3]);
			  return 0;
			}

	      i3+=ai;
		  ledlib_draw_swap_mode(tl3[i3]);

		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDOK:
				PostMessage(hwndDlg,WM_CLOSE,0,0);
   				return 1;
		}
		break;

	case WM_CLOSE:
		KillTimer(hwndDlg,1);
		EndDialog(hwndDlg,0);
		ledlib_end();  //DESTROY ANY CREATED LEDS
		return 0;

	}
	return FALSE;
}


int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;

	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc = DefDlgProc;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hinst;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszClassName = "tstledlib";
	RegisterClass(&wc);

	return DialogBox(hinst, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC) DialogFunc);

}

