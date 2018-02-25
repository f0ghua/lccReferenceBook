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
	char Buffer[MAX_FONT_NAME];
	DWORD Disposition;
	DWORD BufSize = MAX_FONT_NAME;
	DWORD Type;
	DWORD Size;
	int Success = 0;

	Success = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REG_KEY, 0, "", REG_OPTION_NON_VOLATILE,
							KEY_ALL_ACCESS, NULL, &hKey, &Disposition);

	if(Success!=ERROR_SUCCESS||Disposition==REG_CREATED_NEW_KEY){
		strcpy(Font.Name, FONT_NAME);
		Font.Size = FONT_SIZE;
		Font.Italic = FONT_ITALIC;
		Font.Bold = FONT_BOLD;
		Font.Wrap = FONT_WRAP;
		return FAILURE;
	}

	RegQueryValueEx(hKey, "Font", NULL, &Type, (LPBYTE)Buffer, &BufSize);
	strcpy(Font.Name, Buffer);
	Size = sizeof(Font.Size);

	RegQueryValueEx(hKey, "Size", NULL, &Type, &Font.Size, &Size);
	Size = sizeof(Font.Bold);

	RegQueryValueEx(hKey, "Bold", NULL, &Type, &Font.Bold, &Size);
	Size = sizeof(Font.Italic);

	RegQueryValueEx(hKey, "Italic", NULL, &Type, &Font.Italic, &Size);
	Size = sizeof(Font.Wrap);

	RegQueryValueEx(hKey, "Wrap", NULL, &Type, &Font.Wrap, &Size);

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
		RegSetValueEx(hKey, "Font", 0, REG_SZ, Font.Name, (strlen(Font.Name)+1)*sizeof(char));
		RegSetValueEx(hKey, "Size", 0, REG_DWORD, &Font.Size, sizeof(Font.Size));
		RegSetValueEx(hKey, "Bold", 0, REG_DWORD, &Font.Bold, sizeof(Font.Bold));
		RegSetValueEx(hKey, "Italic", 0, REG_DWORD, &Font.Italic, sizeof(Font.Italic));
		RegSetValueEx(hKey, "Wrap", 0, REG_DWORD, &Font.Wrap, sizeof(Font.Wrap));

		RegCloseKey(hKey);
	}

	return SUCCESS;
}




