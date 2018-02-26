/***************************************************************************************/
/*                                                                                     */
/*  Program:	Example of OwnerDraw Buttons.                                          */
/*                                                                                     */
/*  Author:		Pat Bullman															   */
/*                                                                                     */
/*  Comments:   Several owner draw buttons are created and displayed.  Their details   */
/*              are stored in BUTTONDETAILS and can be modified by updating Owner.h.   */
/*                                                                                     */
/*  Usage:      Pressing the Delete button cause it to become Disabled.  Pressing the  */
/*              Insert button again enables the Delete button.                         */
/***************************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "ownbut.h"

HINSTANCE 	ghInst;
HWND 		gMainWnd;

BUTTONDETAILS gBttnDet[TOTAL_BUTTONS] =
							{BMP_INSERT_UP, ID_INSERT, 0, 0, 0, 0,
						     BMP_MODIFY_UP, ID_MODIFY, 0, 0, 0, 0,
						     BMP_DELETE_UP, ID_DELETE, 0, 0, 0, 0,
						     BMP_OK_UP,     ID_OK    , 0, 0, 0, 0};

BITMAP 	LoadOwnerDrawBitmaps(BUTTONDETAILS *BttnDetails);


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
/***************************************************************************************/
/*	Purpose:	Basic WinMain function.                                                */
/*                                                                                     */
/***************************************************************************************/
{
    WNDCLASS wc;
    MSG msg;

    ghInst = hInstance;									// Update our global variable

    wc.style            = 0;
    wc.lpfnWndProc      = MainWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = ghInst;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = LoadIcon(NULL, IDI_APPLICATION);
    wc.hbrBackground    = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "OwnerDraw";

    if(!RegisterClass(&wc))
        return(FALSE);

    gMainWnd = CreateWindow("OwnerDraw", "OwnerDraw Buttons:",
                             WS_SYSMENU | WS_CAPTION | WS_POPUP | WS_THICKFRAME |
                             WS_BORDER | WS_VISIBLE, 120, 120, 360, 280,
                             NULL, NULL, ghInst, NULL);


    if(!gMainWnd)										// Do we have a Window handle
    {
        CleanUp();										// No, then we need to clean up
        return(FALSE);									// And bail
    }

	ShowWindow(gMainWnd, nShow);						// Display our window
    UpdateWindow(gMainWnd);


    // Basic message loop
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // OK, the program is about to terminated and we need to clean up things before
    // finishing.
    CleanUp();

    return(msg.wParam);
}

LRESULT WINAPI MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
/***************************************************************************************/
/*	Purpose:	Our message loop (very simple).                                        */
/*                                                                                     */
/*	Parameter:  hWnd - Window handle;                    							   */
/*				msg - Message number;												   */
/*				wParam - First message parameter; and                                  */
/*              lParam - Second message parameter.                                     */
/***************************************************************************************/
{
	HANDLE 	hChildWin;

    switch(msg)
    {

    case WM_CREATE:										// Window is being created
        // We use this opportunity to create our OwnerDraw buttons.
        CreateOwnerdrawButtons(hWnd);
        return 0;


    case WM_COMMAND:									// Message from a Buttons

		/*******************************************************************************/
		switch(wParam)
		{

		case ID_OK:
			PostQuitMessage(0);
			break;

		case ID_DELETE:
			// If the user presses the Delete button, we'll disable it.
			hChildWin = GetDlgItem(gMainWnd, ID_DELETE);
			EnableWindow(hChildWin, FALSE);
			break;

		case ID_INSERT:
			// If the user presses the Insert button then we'll enable the DELETE button.
			hChildWin = GetDlgItem(gMainWnd, ID_DELETE);
			EnableWindow(hChildWin, TRUE);
			break;
		}
		/*******************************************************************************/
		break;

    case WM_DRAWITEM:                  					//  Handles the ownerdraw button
        return DrawButtons(lParam);

    case WM_DESTROY:
        PostQuitMessage(0);
        break;


    default:
        return(DefWindowProc(hWnd, msg, wParam, lParam));

    }

    return(FALSE);
}



int DrawButtons(LPARAM lParam)
/***************************************************************************************/
/*	Purpose:	This paints our owner draw buttons based on their current state (ie if */
/*				the button has the focus, selected etc).							   */
/*																					   */
/*	Parameter:  lParam - The LPARAM from our WinProc which contains a pointer to a     */
/*              (LPDRAWITEMSTRUCT) item.											   */
/***************************************************************************************/
{
    LPDRAWITEMSTRUCT lpDs = (LPDRAWITEMSTRUCT)lParam;
    HDC 	hDC;
	HBITMAP hBmp;
	int 	iBttn, iState;

    hDC = CreateCompatibleDC(lpDs->hDC);				// Create a memory DC
	if(!hDC)											// If we can't create a suitable
		return(FALSE);									//    DC then we return here.

	// Find the BUTTONDETAILS record.
	iBttn = GetBttnFromId(lpDs->CtlID);

	// We test itemState to determine what button image needs to be displayed.  itemState
	// is a bit field variable, the various bits being set indicate the state of the
	// button and, hence, the image required.  We test for DISABLED, FOCUS and SELECTED,
	// any other state is assumed to mean normal (or button UP).
	if(lpDs->itemState & ODS_DISABLED)					// DISABLED state
		iState = BTTN_DISABLED;

	else

		if(lpDs->itemState & ODS_FOCUS)					// If button has the FOCUS
		{
			if(lpDs->itemState & ODS_SELECTED)			// SELECTED
				iState = BTTN_DOWN;

			else										// Has FOCUS, but not SELECTED
				iState = BTTN_FOCUS;
		}

		else											// Default, assume button UP
			iState = BTTN_UP;


	// Now that we've got the gBttnDet() reference and the button state, let's do
	// the drawing.
	SelectObject(hDC, gBttnDet[iBttn].hBm[iState]);
	BitBlt(lpDs->hDC, 0, 0, lpDs->rcItem.right, lpDs->rcItem.bottom,
	 	   hDC, 0, 0, SRCCOPY);


    DeleteDC(hDC);									// We're finished with the DC
    return(TRUE);                         			// If we were successful
}



void CreateOwnerdrawButtons(HWND hWndParent)
/***************************************************************************************/
/*	Purpose:	Creates our ownerdraw buttons.  We have four buttons in this demo. The */
/*              first three are spaced along the left of the window, while the fourth  */
/*              sits at the bottom right.                                              */
/*                                                                                     */
/*	Parameter:  hWndParent - The window onto which our owner draw buttons were         */
/*              created (parent window).                                               */
/***************************************************************************************/
{
	int		i, iRight, iBottom, iTop = 0, iSpace, iHeight;
	int		iMargin = 10;
	RECT	rect;
	BITMAP  bm;


	// We calculate the sace requirements so that the bitmaps are evenly spaced
	// vertically down the left of the window.
	GetClientRect(hWndParent, &rect);					// Get window details
	iHeight = rect.bottom;	    						// Store window height


	// OK, let's draw the left buttons.  They are left aligned to iMargin and we use
	// our above calculated iSpace to position the buttons.
    for (i = 0; i < 3; i++)
    {

		bm = LoadOwnerDrawBitmaps(&gBttnDet[i]);		// Get image & bitmap details

		// The following calculates the top position for the button so that the buttons
		// are equally spaced.
		iSpace = (iHeight - (bm.bmHeight * (3 - i))) / (4 - i);
		iTop  += iSpace;

		CreateWindow("Button", NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      iMargin, iTop , bm.bmWidth, bm.bmHeight,
                      hWndParent, (HMENU) gBttnDet[i].CtrId, ghInst, NULL);

		// Add button height to get start for the calculation of next button position.
		iTop += bm.bmHeight;
		iHeight -= (iSpace + bm.bmHeight);
    }


	// Now, let's do our OK button.  We want it to be situated at the bottom right of
	// the window.  Details for the OK button are stored in gBttnDet[3].
	bm      = LoadOwnerDrawBitmaps(&gBttnDet[3]);
	iRight  = rect.right - bm.bmWidth - iMargin;
	iBottom = rect.bottom - bm.bmHeight - iSpace;

    CreateWindow("Button", NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                  iRight, iBottom, bm.bmWidth, bm.bmHeight,
                  hWndParent, (HMENU) gBttnDet[3].CtrId, ghInst, NULL);
}

int GetBttnFromId(UINT ControlId)
/***************************************************************************************/
/*	Purpose:	Details about OwnerDraw buttons, including bitmap handles, are stored  */
/*              in gBttnDet().  This function returns the gBttnDet() element number    */
/*              where we can find details for the button identified by ControlId.      */
/*                                                                                     */
/*  Comments:   We assume that ControlId can be found in gBttnDet().                   */
/*                                                                                     */
/*	Parameter:  (UINT) ControlId - The Control Id (HMENU) to search for.               */
/*                                                                                     */
/*  Returns:    Integer - The gBttnDet() element number where ControlId was found.     */
/***************************************************************************************/
{
	for (int i = 0; i < TOTAL_BUTTONS; i++)
	{
		if (gBttnDet[i].CtrId == ControlId)
			return i;
	}

	return 0;											// We should never get here!
}

BITMAP LoadOwnerDrawBitmaps(BUTTONDETAILS *BttnDet)
/***************************************************************************************/
/*	Purpose:	Loads the bitmaps used in our ownerdraw buttons.  Each button has four */
/*              states, ranging from BTTN_UP to BTTN_DISABLED.  The images for each    */
/*              state are consequectively numbered (in the resource file) from BTTN_UP */
/*              to BTTN_DISABLED.                                                      */
/*                                                                                     */
/*  Returns:    A BITMAP object.                                                       */
/***************************************************************************************/
{
	BITMAP  bm;

	// Load images into our global array.
	for (int i = BTTN_UP; i <= BTTN_DISABLED; i++)
	{
		BttnDet->hBm[i] = LoadImage(ghInst, MAKEINTRESOURCE(BttnDet->ResourceId + i),
                  					IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );

	}

	// Once we have loaded all the images for the various states, we use the BTTN_UP
	// image to get details about our bitmaps, which is returned to the caller.
	GetObject(BttnDet->hBm[BTTN_UP], sizeof(BITMAP), &bm);
	return bm;
}

void CleanUp()
/***************************************************************************************/
/*	Purpose:	This cleans up any resources loaded or objects created by our program. */
/***************************************************************************************/
{

	// Delete bitmap hadles stored in BUTTONDETAILS array.
	for (int index = 0; index < TOTAL_BUTTONS; index++)
	{
		for (int i = BTTN_UP; i <= BTTN_DISABLED; i++)

			if (gBttnDet[index].hBm[i])
				DeleteObject(gBttnDet[index].hBm[i]);
	}
}
