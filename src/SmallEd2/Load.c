#include <windows.h>
#include <string.h>

#include "Global.h"

/*------------------------------------------------------------------------
 Procedure:     LoadOptions ID:1
 Purpose:       Loads the options for the FONT structure from the
                windows registry.
 Input:         None.
 Output:        Success or failure.
 Errors:        If an error occurs the key is empty the default
                options are loaded.
------------------------------------------------------------------------*/
int LoadOptions(void){
	HKEY hKey;
	TCHAR   Buffer[MAX_FONT_NAME];
	DWORD Disposition;
	DWORD BufSize = MAX_FONT_NAME;
	DWORD Type;
	DWORD Size;
	int Success = 0;

	Success = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REG_KEY, 0, TEXT(""), REG_OPTION_NON_VOLATILE,
							KEY_ALL_ACCESS, NULL, &hKey, &Disposition);

	if(Success!=ERROR_SUCCESS||Disposition==REG_CREATED_NEW_KEY){
		lstrcpy(Font.Name, FONT_NAME);
		Font.Size = FONT_SIZE;
		Font.Italic = FONT_ITALIC;
		Font.Bold = FONT_BOLD;
		Font.Wrap = FONT_WRAP;
		return FAILURE;
	}

	RegQueryValueEx(hKey, TEXT("Font"), NULL, &Type, (LPBYTE) Buffer, &BufSize);
	lstrcpy(Font.Name, Buffer);
	Size = sizeof(Font.Size);

	RegQueryValueEx(hKey, TEXT("Size"), NULL, &Type, &Font.Size, &Size);
	Size = sizeof(Font.Bold);

	RegQueryValueEx(hKey, TEXT("Bold"), NULL, &Type, &Font.Bold, &Size);
	Size = sizeof(Font.Italic);

	RegQueryValueEx(hKey, TEXT("Italic"), NULL, &Type, &Font.Italic, &Size);
	Size = sizeof(Font.Wrap);

	RegQueryValueEx(hKey, TEXT("Wrap"), NULL, &Type, &Font.Wrap, &Size);

	RegCloseKey(hKey);
	return SUCCESS;
}

/*------------------------------------------------------------------------
 Procedure:     SaveOptions ID:1
 Purpose:       Saves the options of the FONT structure in the
                Windows registry.
 Input:         None.
 Output:        Success or failure.
 Errors:        None.
------------------------------------------------------------------------*/
int SaveOptions(void){
	HKEY hKey;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY, 0, KEY_ALL_ACCESS, &hKey)==ERROR_SUCCESS){
		RegSetValueEx(hKey, TEXT("Font"), 0, REG_SZ, Font.Name, (lstrlen(Font.Name)+1)*sizeof(TCHAR ));
		RegSetValueEx(hKey, TEXT("Size"), 0, REG_DWORD, &Font.Size, sizeof(Font.Size));
		RegSetValueEx(hKey, TEXT("Bold"), 0, REG_DWORD, &Font.Bold, sizeof(Font.Bold));
		RegSetValueEx(hKey, TEXT("Italic"), 0, REG_DWORD, &Font.Italic, sizeof(Font.Italic));
		RegSetValueEx(hKey, TEXT("Wrap"), 0, REG_DWORD, &Font.Wrap, sizeof(Font.Wrap));

		RegCloseKey(hKey);
	}

	return SUCCESS;
}




