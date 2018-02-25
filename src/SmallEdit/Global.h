/*********************************************************************
			Global.h - Global Header File for Small Edit

	This header file contains the global declarations, structures,
	macros, and prototypes used in Small Edit.
*********************************************************************/

//	Window Macros
#define MSGBOX(hwnd,message)	MessageBox(hwnd, message, "Information", MB_OK | MB_ICONINFORMATION);
#define ERRBOX(hwnd, message)	MessageBox(hwnd, message, "Error", MB_OK | MB_ICONSTOP);


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
#define WNDCLASSNAME "SmallEditClass"
#define WND_TITLE	"Small Edit"
#define WND_WIDTH	700
#define WND_HEIGHT	440


//	Tools Information
#define NEW_FILE_NAME		"<Untitled>"
#define FILE_FILTER 		"Text Files (*.txt)\0*.txt\0Html Files (*.html;*.htm)\0*.html;*.htm\0Log Files (*.log)\0*.log\0C/C++ Files (*.c;*.cpp;*.h)\0*.c;*.cpp;*.h\0Basic Files (*.bas)\0*.bas\0All Files (*.*)\0*.*\0"


//	HTML Code Inserts
#define CODE_HTML_FRAME 	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n<html>\n<head>\n\t<title></title>\n</head>\n<BODY BACKGROUND=\"image.gif\" BGCOLOR=\"WHITE\" TEXT=\"BLACK\" LINK=\"#0000FF\" VLINK=\"#FF66FF\" ALINK=\"#FF0000\">\n</body>\n</html>"
#define CODE_HTML_META		"<meta content=\"Name\" name=\"AUTHOR\">\n<meta content=\"Message\" name=\"COPYRIGHT\">\n<meta content=\"Words\" name=\"KEYWORDS\">\n<meta content=\"Message\" name=\"DESCRIPTION\">"
#define CODE_HTML_LINK		"<link rel=\"stylesheet\" href=\"filename.css\" type=\"text/css\">"
#define CODE_HTML_QUICK		"<a href=\"http://\" target=\"\" name=\"\" onMouseOver=\"window.status=''; return true;\" onMouseOut=\"window.status=''; return true;\"></a>"
#define CODE_HTML_MODIFIED	"<script language=\"Javascript\" type=\"text/javascript\">\ndocument.write(\"Last modified \" + document.lastModified);\n</script>"
#define CODE_HTML_BROWSER	"<script language=\"Javascript\" type=\"text/javascript\">\ndocument.write(\"You are using \" + navigator.appName + \" \" + navigator.appVersion);\n</script>"
#define CODE_HTML_CURRENT	"<script language=\"Javascript\" type=\"text/javascript\">\ndocument.write(\"Url addresse is \" + document.location);\n</script>"
#define CODE_HTML_HIGHLIGHT	"<script language=\"JavaScript1.2\" type=\"text/javascript\">\nfunction disableselect(e){return false}\nfunction reEnable(){return true}\ndocument.onselectstart=new Function (\"return false\")\nif (window.sidebar){document.onmousedown=disable; selectdocument.onclick=reEnable}\n</script>"
#define CODE_HTML_FONT		"<font face=\"Arial\" color=\"black\" size=\"+3\">Display Text </font>"
#define CODE_HTML_IMAGE		"<img src=\"myimage.gif\" align=\"center\" border=\"0\" alt=\"image\">"
#define CODE_HTML_ORDERED	"<ol>\n<li>One.\n<li>Two.\n<li>Three.\n</ol>"
#define CODE_HTML_UNORDERED	"<ul>\n<li>One.\n<li>Two.\n<li>Three.\n</ul>"

//	Help Information
#define WEB_SITE		"http://backfrog.digitalrice.com"
#define VERSION_INFO	"Small Edit Version 1.00"
#define ABOUT			"\nBlack Frog Studios Small Edit - All Versions of Windows\n\nSmall Edit was designed and programmed by Sean O'Rourke.\n\nFor the latest distribution and more information visit http://backfrog.digitalrice.com.\n\nSpecial thanks goes to Jacob Navia for his C Compiler, LCC-Win32 available for free at http://www.q-software-solutions.com\n\n"
#define HELP_FILE		"SmallEdit.hlp"

//	Registry Information
#define REG_KEY	"SOFTWARE\\Black Frog\\Small Edit"
#define FONT_NAME	"Arial"
#define FONT_SIZE	16
#define FONT_ITALIC	FALSE
#define FONT_BOLD	FALSE
#define FONT_WRAP	TRUE


//	Error Messages
#define ERR_REGCLASS	"A fatal error has occured.  Small Edit could not register a window class.  Please restart Small Edit.  If the problem persists contact Black Frog Stuidos."
#define ERR_WND			"A fatal error has occured.  A window could not be created.  Please restart Small Edit.  If the problem persists contact Black Frog Stuidos."
#define ERR_CHILD		"A fatal error has occured.  A child window could not be created.  Please restart Small Edit.  If the problem persists contact Black Frog Stuidos."
#define ERR_FONT		"The font could not be created.  Small Edit will use the default system font."
#define ERR_MEM			"Small Edit has run out of available memory.  Please free more memory and try again."
#define ERR_LOAD		"An error has occured while loading a file.  Please validate you have access to the file."
#define ERR_SAVE		"An error has occured while saving a file.  Please validate you have access to the file."


//	Messages
#define MSG_NOFIND		"No more occurances of text found."
#define MSG_NTFIND		"Please specify text to find."
#define MSG_NFLINE		"Line does not exist."


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
int LoadFile(HWND,char*);
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
int FindNext(HWND);


//	Structures
typedef struct FONT{
	HFONT hFont;
	char Name[MAX_FONT_NAME];
	DWORD Size;
	DWORD Italic;
	DWORD Bold;
	DWORD Wrap;
} FONT;

typedef struct INFO{
	char FileName[MAX_FILE];
	char *Text;
} INFO;

typedef struct FIND{
	char Find[MAX_NAME];
	char Replace[MAX_NAME];
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
