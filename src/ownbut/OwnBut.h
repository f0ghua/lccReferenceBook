/* Weditres generated include file. Do NOT edit */

LRESULT WINAPI MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void 	DisplayErrorMessage(void);
int 	DrawButtons(LPARAM lParam);
int 	GetBttnFromId(UINT);
void 	CreateOwnerdrawButtons(HWND hWndParent);
void 	CleanUp();

// Define button Control Id's
#define	ID_INSERT			111
#define	ID_MODIFY			112
#define	ID_DELETE			113
#define	ID_OK				  1

// Define button states
#define	BTTN_UP			  	  0
#define	BTTN_DOWN			  1
#define	BTTN_FOCUS			  2
#define	BTTN_DISABLED		  3

// Define button bitmap images
#define	BMP_OK_UP			100
#define	BMP_OK_DOWN			101
#define	BMP_OK_FOCUS		102
#define	BMP_OK_DISABLED		103

#define	BMP_INSERT_UP		105
#define	BMP_INSERT_DOWN		106
#define	BMP_INSERT_FOCUS	107
#define	BMP_INSERT_DISABLED	108

#define	BMP_MODIFY_UP		110
#define	BMP_MODIFY_DOWN		111
#define	BMP_MODIFY_FOCUS	112
#define	BMP_MODIFY_DISABLED	113

#define	BMP_DELETE_UP     	115
#define	BMP_DELETE_DOWN		116
#define	BMP_DELETE_FOCUS	117
#define	BMP_DELETE_DISABLED	118

#define TOTAL_BUTTONS         4

typedef struct tagBUTTONDETAILS{
	int ResourceId;										// Holds button "UP" BMP image ID
    UINT CtrId;			      							// HMENU value in CreateWindow()
    HBITMAP hBm[4];										// Holds HBITMAP for each state
}BUTTONDETAILS;

