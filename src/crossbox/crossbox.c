#include "crossbox.h"



/**************
WM_CREATE
***************/
void init_crossbox(HDC hdc, CROSSHAIR *ch, DRAGBOX *db, COLORREF color)
{

	HBITMAP hbitmap1, hbitmap2, hbitmap3, hbitmap4;
	HBITMAP hbitmap1b, hbitmap2b, hbitmap3b, hbitmap4b;
	HPEN hpen;

	hpen=CreatePen(PS_SOLID, 1, color);
	db->boxcolor=color;

	//cross hair stuff

	//      3
	//      3
	//   444m444
	//      3
	//      3


	//vertical cross hair
	ch->hdcmem1=CreateCompatibleDC(hdc);
	hbitmap1=CreateCompatibleBitmap(hdc, 1, 600);
	SelectObject(ch->hdcmem1,hbitmap1);
	SelectObject(ch->hdcmem1,hpen);
	MoveToEx(ch->hdcmem1, 0, 0, NULL);
	LineTo(ch->hdcmem1, 0, 600);

	//horizontal cross hair
	ch->hdcmem2=CreateCompatibleDC(hdc);
	hbitmap2=CreateCompatibleBitmap(hdc, 800, 1);
	SelectObject(ch->hdcmem2,hbitmap2);
	SelectObject(ch->hdcmem2,hpen);
	MoveToEx(ch->hdcmem2, 0, 0, NULL);
	LineTo(ch->hdcmem2, 800, 0);

	//storage for what the vertical cross hair will overwrite
	ch->hdcmem3=CreateCompatibleDC(hdc);
	hbitmap3=CreateCompatibleBitmap(hdc, 1, 600);
	SelectObject(ch->hdcmem3,hbitmap3);
	SelectObject(hdc,GetStockObject(NULL_PEN));
	MoveToEx(ch->hdcmem3, 0, 0, NULL);
	LineTo(ch->hdcmem3, 0, 600);

	//storage for what the horizontal cross hair will overwrite
	ch->hdcmem4=CreateCompatibleDC(hdc);
	hbitmap4=CreateCompatibleBitmap(hdc, 800, 1);
	SelectObject(ch->hdcmem4,hbitmap4);
	MoveToEx(ch->hdcmem4, 0, 0, NULL);
	LineTo(ch->hdcmem4, 800, 0);



	//drag box stuff

	//         m44444444
	//         3        5
	//         3        5
	//         3        5
	//         3        5
	//          66666666m

	//storage for the left border (vertical)
	db->hdcmem3=CreateCompatibleDC(hdc);
	hbitmap1b=CreateCompatibleBitmap(hdc, 3, 600);
	SelectObject(db->hdcmem3,hbitmap1b);
	MoveToEx(db->hdcmem3, 0, 0, NULL);
	LineTo(db->hdcmem3, 0, 600);
	MoveToEx(db->hdcmem3, 1, 0, NULL);
	LineTo(db->hdcmem3, 1, 600);
	MoveToEx(db->hdcmem3, 2, 0, NULL);
	LineTo(db->hdcmem3, 2, 600);

	//storage for the top border (horizontal)
	db->hdcmem4=CreateCompatibleDC(hdc);
	hbitmap2b=CreateCompatibleBitmap(hdc, 800, 3);
	SelectObject(db->hdcmem4,hbitmap2b);
	MoveToEx(db->hdcmem4, 0, 0, NULL);
	LineTo(db->hdcmem4, 800, 0);
	MoveToEx(db->hdcmem4, 0, 1, NULL);
	LineTo(db->hdcmem4, 800, 1);
	MoveToEx(db->hdcmem4, 0, 2, NULL);
	LineTo(db->hdcmem4, 800, 2);

	//storage for the right border (vertical)
	db->hdcmem5=CreateCompatibleDC(hdc);
	hbitmap3b=CreateCompatibleBitmap(hdc, 3, 600);
	SelectObject(db->hdcmem5,hbitmap3b);
	MoveToEx(db->hdcmem5, 0, 0, NULL);
	LineTo(db->hdcmem5, 0, 600);
	MoveToEx(db->hdcmem5, 1, 0, NULL);
	LineTo(db->hdcmem5, 1, 600);
	MoveToEx(db->hdcmem5, 2, 0, NULL);
	LineTo(db->hdcmem5, 2, 600);

	//storage for the bottom border (horizontal)
	db->hdcmem6=CreateCompatibleDC(hdc);
	hbitmap4b=CreateCompatibleBitmap(hdc, 800, 3);
	SelectObject(db->hdcmem6,hbitmap4b);
	MoveToEx(db->hdcmem6, 0, 0, NULL);
	LineTo(db->hdcmem6, 800, 0);
	MoveToEx(db->hdcmem6, 0, 1, NULL);
	LineTo(db->hdcmem6, 800, 1);
	MoveToEx(db->hdcmem6, 0, 2, NULL);
	LineTo(db->hdcmem6, 800, 2);


	DeleteObject(hbitmap1);
	DeleteObject(hbitmap2);
	DeleteObject(hbitmap3);
	DeleteObject(hbitmap4);
	DeleteObject(hbitmap1b);
	DeleteObject(hbitmap2b);
	DeleteObject(hbitmap3b);
	DeleteObject(hbitmap4b);

	DeleteObject(hpen);

	ch->firstcross=TRUE;

}




/**************
WM_LBUTTONDOWN
***************/
void lbutdown_crossbox(HDC hdc, POINT pMouse, CROSSHAIR *ch, DRAGBOX *db)
{

	ch->showcross_int = ch->showcross;  //save for lbutup
	ch->showcross=FALSE;//dont want cross to show while dragging box

	//erase cross
	if(ch->showcross_int==TRUE)
	{
		BitBlt(hdc, ch->pMouse_old.x, 0,  1,600, ch->hdcmem3, 0,0, SRCCOPY);
		BitBlt(hdc, 0, ch->pMouse_old.y,  800,1, ch->hdcmem4, 0,0, SRCCOPY);
		//ch->firstcross=TRUE;
	}


	//save the starting point upper left for dragbox
	db->pStart.x=pMouse.x;
	db->pStart.y=pMouse.y;


	//save what the left and top border of dragbox will cover up
	BitBlt(db->hdcmem3, 0,0,  3,600, hdc, db->pStart.x-1, 0, SRCCOPY);
	BitBlt(db->hdcmem4, 0,0,  800,3, hdc, 0, db->pStart.y-1, SRCCOPY);

	db->firstdragbox=TRUE;

}





/**************
WM_MOUSEMOVE
***************/
void mousemove_crossbox(HDC hdc, POINT pMouse, CROSSHAIR *ch, DRAGBOX *db, UINT wParam)
{
	//static BOOL firstcrosshair=TRUE;
	static POINT pMouse_old ;
	HPEN hpen;


	//drag box stuff
	if(wParam == MK_LBUTTON)
	{
		if(!db->firstdragbox)
		{
			//restore hdc at old x,y
			BitBlt(hdc, db->pStart.x-1, 0, 	3,600, db->hdcmem3, 0, 0, SRCCOPY);
			BitBlt(hdc, 0, db->pStart.y-1, 	800,3, db->hdcmem4, 0, 0, SRCCOPY);
			BitBlt(hdc, pMouse_old.x-1, 0,  3,600, db->hdcmem5, 0, 0, SRCCOPY);
			BitBlt(hdc, 0, pMouse_old.y-1,  800,3, db->hdcmem6, 0, 0, SRCCOPY);
		}

		//save what will be covered by the right and bottom border
		BitBlt(db->hdcmem5, 0,0,  3,600, hdc, pMouse.x-1, 0, SRCCOPY);
		BitBlt(db->hdcmem6, 0,0,  800,3, hdc, 0, pMouse.y-1, SRCCOPY);


		//draw the rect
		hpen=CreatePen(PS_SOLID, 1, db->boxcolor);
		SelectObject(hdc,hpen);
		SelectObject(hdc,GetStockObject(NULL_BRUSH));
		Rectangle(hdc, db->pStart.x, db->pStart.y, pMouse.x, pMouse.y);


		//save current mouse location as old
		pMouse_old=pMouse;

		db->firstdragbox=FALSE;

		DeleteObject(hpen);

	}//done dragbox




	//cross hair wont be drawn if making box, or if we dont want it
	else
	    {

		if(ch->showcross==FALSE) return;

		//if(!firstcrosshair)
		if(!ch->firstcross)
		{
			//restore hdc at old x,y (cover cross hairs)
			BitBlt(hdc, ch->pMouse_old.x, 0,  1,600, ch->hdcmem3, 0,0, SRCCOPY);
			BitBlt(hdc, 0, ch->pMouse_old.y,  800,1, ch->hdcmem4, 0,0, SRCCOPY);
		}

		//save what will be covered by the crosshairs
		BitBlt(ch->hdcmem3, 0,0,  1,600, hdc, pMouse.x, 0, SRCCOPY);
		BitBlt(ch->hdcmem4, 0,0,  800,1, hdc, 0, pMouse.y, SRCCOPY);


		//paint new cross hairs at new x,y
		BitBlt(hdc, pMouse.x, pMouse.y-605,  1,600, ch->hdcmem1, 0,0, SRCCOPY);//vert upper
		BitBlt(hdc, pMouse.x, pMouse.y+5,    1,600, ch->hdcmem1, 0,0, SRCCOPY);//vert lower
		BitBlt(hdc, pMouse.x-805, pMouse.y,  800,1, ch->hdcmem2, 0,0, SRCCOPY);//horiz left
		BitBlt(hdc, pMouse.x+5,   pMouse.y,  800,1, ch->hdcmem2, 0,0, SRCCOPY);//horiz right


		//save current mouse location as old
		ch->pMouse_old=pMouse;

		//firstcrosshair=FALSE;
		ch->firstcross=FALSE;

	}

}



/**************
WM_NCMOUSEMOVE
***************/
void ncmousemove_crossbox(HDC hdc, CROSSHAIR *ch)
{
	if(ch->firstcross)return;

	//erase the cross before leaving the window
	BitBlt(hdc, ch->pMouse_old.x, 0,  1,600, ch->hdcmem3, 0,0, SRCCOPY);
	BitBlt(hdc, 0, ch->pMouse_old.y,  800,1, ch->hdcmem4, 0,0, SRCCOPY);

	ch->firstcross=TRUE;

}




/**************
WM_LBUTTONUP
***************/
void lbutup_crossbox(HDC hdc, POINT pMouse, DRAGBOX *db,  CROSSHAIR *ch)
{

	//prevents messing up the screen for double click in 1 spot
	if(pMouse.x==db->pStart.x && pMouse.y==db->pStart.y)return;

	//restore what the dragbox was covering up
	BitBlt(hdc, db->pStart.x-1, 0,  3,600, db->hdcmem3, 0, 0, SRCCOPY);
	BitBlt(hdc, 0, db->pStart.y-1,  800,3, db->hdcmem4, 0, 0, SRCCOPY);
	BitBlt(hdc, pMouse.x-1,0,  		3,600, db->hdcmem5, 0, 0, SRCCOPY);
	BitBlt(hdc, 0,pMouse.y-1,  		800,3, db->hdcmem6, 0, 0, SRCCOPY);

	//save the ending point lower right of the dragbox
	db->pEnd.x=pMouse.x;
	db->pEnd.y=pMouse.y;

	//turn on crosshair
	if(ch->showcross_int==TRUE) ch->showcross=TRUE;

}




/**************
WM_DELETE
***************/
void cleanup_crossbox(CROSSHAIR *ch, DRAGBOX *db )
{
	DeleteObject(ch->hdcmem1);
	DeleteObject(ch->hdcmem2);
	DeleteObject(ch->hdcmem3);
	DeleteObject(ch->hdcmem4);

	DeleteObject(db->hdcmem3);
	DeleteObject(db->hdcmem4);
	DeleteObject(db->hdcmem5);
	DeleteObject(db->hdcmem6);
}









