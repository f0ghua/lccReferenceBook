/*********************************************************************
			Global.h - Global Header File for Small Edit

	This header file contains the global declarations, structures,
	macros, and prototypes used in Small Edit.
*********************************************************************/

#include <tchar.h>


//	Window Macros
#define MSGBOX(hwnd,message)	MessageBox(hwnd, message, TEXT("Information"), MB_OK | MB_ICONINFORMATION);
#define ERRBOX(hwnd,message)	MessageBox(hwnd, message, TEXT("Error"), MB_OK | MB_ICONSTOP);


//	General Definitions
#define TRUE	1
#define FALSE	0

#define SUCCESS	TRUE
#define FAILURE	FALSE


//	Find and Replace
#define FOWARD		1
#define BACKWARD	2


//	Limit Definitions
#define MAX_FILE		512
#define MAX_FONT_NAME	256
#define MAX_NAME		1024
#define MAX_EDIT_CHAR	2000000000


//	Printer Information
#define PRINTER_INDENT			150
#define PRINTER_HEADER			200
#define PRINTER_PAGE_LESS_LINES	6


//	Window Information
#define WNDCLASSNAME TEXT("SmallEditClass")
#define WND_TITLE	TEXT("Small Edit")
#define WND_WIDTH	700
#define WND_HEIGHT	440


//	Tools Information
#define NEW_FILE_NAME		TEXT("<Untitled>")
#define FILE_FILTER 		TEXT("Unicode Files (*.uni)\0*.uni\0")


//	Help Information
#define WEB_SITE		TEXT("http://backfrog.digitalrice.com")
#define VERSION_INFO	TEXT("Small Edit Version 2.0 - UNICODE enhancements by Jacques Laporte (laporte.j@wanadoo.fr)")
#define ABOUT			TEXT("\nBlack Frog Studios Small Edit - Windows NT\n\nSmall Edit was designed and programmed by Sean O'Rourke.\nUNICODE support by J.Laporte\n(laporte.j@wanadoo.fr)\nFor the latest distribution and more information visit http://backfrog.digitalrice.com.\n\nSpecial thanks goes to Jacob Navia for his C Compiler, LCC-Win32 available for free at http://www.q-software-solutions.com\n\n")
#define HELP_FILE		TEXT("SmallEdit.hlp")


//	Registry Information
#define REG_KEY	TEXT("SOFTWARE\\Black Frog\\Small Edit")
#define FONT_NAME	TEXT("Arial")
#define FONT_SIZE	16
#define FONT_ITALIC	FALSE
#define FONT_BOLD	FALSE
#define FONT_WRAP	TRUE


//	Error Messages
#define ERR_REGCLASS	TEXT("A fatal error has occured.  Small Edit could not register a window class.  Please restart Small Edit.  If the problem persists contact Black Frog Stuidos.")
#define ERR_WND			TEXT("A fatal error has occured.  A window could not be created.  Please restart Small Edit.  If the problem persists contact Black Frog Stuidos.")
#define ERR_CHILD		TEXT("A fatal error has occured.  A child window could not be created.  Please restart Small Edit.  If the problem persists contact Black Frog Stuidos.")
#define ERR_FONT		TEXT("The font could not be created.  Small Edit will use the default system font.")
#define ERR_MEM			TEXT("Small Edit has run out of available memory.  Please free more memory and try again.")
#define ERR_LOAD		TEXT("An error has occured while loading a file.  Please validate you have access to the file.")
#define ERR_UNICODE		TEXT("An error has occured while loading a file.  Check the Unicode format.")
#define ERR_SAVE		TEXT("An error has occured while saving a file.  Please validate you have access to the file.")


//	Messages
#define MSG_NOFIND		TEXT("No more occurances of text found.")
#define MSG_NTFIND		TEXT("Please specify text to find.")
#define MSG_NFLINE		TEXT("Line does not exist.")


//	Prototypes
//Main.c
LRESULT CALLBACK WndMainProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK RichEditProc(HWND,UINT,WPARAM,LPARAM);


//Load.c
int LoadOptions(void);
int SaveOptions(void);

//Tools.c
int New(HWND);
int Open(HWND);
int LoadFile(HWND,TCHAR *);
int Save(HWND);
int SaveAs(HWND);
int InsertDT(HWND,int);
int LineCount(HWND);
int GetSize(HWND);
int CharChange(HWND,int);
int Print(HWND);
int SelectFontDlg(HWND);
int ScrollTop(HWND,int);
int SelectAll(HWND);
int WordWrap(HWND);




//Dialog.c
BOOL CALLBACK DlgAboutProc(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK DlgFindProc(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK DlgReplaceProc(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK DlgLineProc(HWND,UINT,WPARAM,LPARAM);
int wtoi(const wchar_t *string);
int FindNext(HWND);
DWORD CharSet;


//	Structures
typedef struct FONT{
	HFONT hFont;
	TCHAR Name[MAX_FONT_NAME];
	DWORD Size;
	DWORD Italic;
	DWORD Bold;
	DWORD Wrap;
} FONT;

typedef struct INFO{
	TCHAR FileName[MAX_FILE];
	TCHAR *Text;
} INFO;

typedef struct FIND{
	TCHAR Find[MAX_NAME];
	TCHAR Replace[MAX_NAME];
	int bReplace;
	int bConfirm;
	int bCase;
	int Direction;
	int bWord;
	CHARRANGE cr;
} FIND;


//	Global Variables
extern INFO Info;
extern FONT Font;
extern HANDLE hEdit;
extern FIND Find;
