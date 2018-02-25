// MsgBox.h

#include <windows.h>

// Prototypes
BOOL CALLBACK OwnerDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT MsgBox(HINSTANCE hInst, HWND hwndOwner, char *szTitle, char *szMess);

// Ordinals for the predefined control classes.
#define BUTTON 	  0x0080
#define EDIT   	  0x0081
#define STATIC 	  0x0082
#define LISTBOX   0x0083
#define SCROLLBAR 0x0084
#define COMBOBOX  0x0085

#define ID_TEXT   	200

