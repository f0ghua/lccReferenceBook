#include <windows.h>


/*notes:
-800x600 screen or smaller

-lbutup in nclient or outside window top/left results in 2^16 16bit limitation

put SetCapture() before lbutdown_crossbox()
put ReleaseCapture()  after lbutup_crossbox()
for clean box dragging outside the client area

to show the crosshair set the CROSSHAIR member showcross=TRUE
*/



struct dragbox
{
HDC hdcmem3;
HDC hdcmem4;
HDC hdcmem5;
HDC hdcmem6;

POINT pStart;	//return value
POINT pEnd;		//return value

COLORREF boxcolor;
BOOL firstdragbox;
};

typedef struct dragbox DRAGBOX;



struct crosshair
{
HDC hdcmem1;
HDC hdcmem2;
HDC hdcmem3;
HDC hdcmem4;

POINT pMouse_old;
BOOL showcross;
BOOL showcross_int;
BOOL firstcross;
};

typedef struct crosshair CROSSHAIR;

void init_crossbox(HDC hdc, CROSSHAIR *ch, DRAGBOX *db, COLORREF color);
void lbutdown_crossbox(HDC hdc, POINT pMouse, CROSSHAIR *ch, DRAGBOX *db);
void mousemove_crossbox(HDC hdc, POINT pMouse, CROSSHAIR *ch, DRAGBOX *db, UINT wParam);
void lbutup_crossbox(HDC hdc, POINT pMouse, DRAGBOX *db,  CROSSHAIR *ch);
void cleanup_crossbox(CROSSHAIR *ch, DRAGBOX *db );
void ncmousemove_crossbox(HDC hdc, CROSSHAIR *ch);


