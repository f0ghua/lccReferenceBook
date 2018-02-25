// registry.c
#include <windows.h>
#include "registry.h"

#define KEY HKEY_CURRENT_USER
static char szRegSubKey[] = "Software\\";
static char szRegName[]   = "FindFile\\";
static char szCurrentKey[MAX_PATH+100];

/*----------------------------------------------------------------------------

  FUNCTION	: ReadRegistryString(char *szKey, char *szSubKey, int szReturnBuffer)

  PURPOSE	: Reads a int registry entry for this app.

  RETURN   	: int value or default value supplied by caller (cbReturnBuffer)

  PARAMS   	: *szKey    	 - e.g. "WindowPos"
  			: *szSubKey 	 - e.g. "left"
			: cbReturnBuffer - default value returned if no reg entry

-----------------------------------------------------------------------------*/
DWORD ReadRegistryString(char *szKey, char *szSubKey, char *pszDefault, char * szReturnBuffer)
{
	HKEY  hKey;
	DWORD dwType;

	DWORD size = MAX_PATH+100;

	// make whole key string
	strcpy(szCurrentKey, szRegSubKey);
	strcat(szCurrentKey, szRegName);
	strcat(szCurrentKey, szKey);

	if (ERROR_SUCCESS == RegOpenKeyEx(KEY, szCurrentKey, 0, KEY_QUERY_VALUE, &hKey))
	{
		dwType = REG_SZ;

		if (ERROR_SUCCESS == RegQueryValueEx(hKey, szSubKey, 0, &dwType, (PBYTE)szReturnBuffer, &size))
		{
			RegCloseKey(hKey);
			return (size);
		}
		else
		{
			strcpy(szReturnBuffer, pszDefault);
			return 0;
		}
	}
	else
	{
		strcpy(szReturnBuffer, pszDefault);
		return 0;
	}
}

char * ReadRegistryDepends(void)
{
	static char szdepends[MAX_PATH+100];

	HKEY  hKey;
	DWORD dwType;
	unsigned long size = MAX_PATH+100;

	#define KEY1 HKEY_LOCAL_MACHINE

	// make whole key string
	strcpy(szCurrentKey, szRegSubKey);
	strcat(szCurrentKey, "Microsoft\\Shared Tools\\depends.exe");

	if (ERROR_SUCCESS == RegOpenKeyEx(KEY1, szCurrentKey, 0, KEY_QUERY_VALUE, &hKey))
	{
		dwType = REG_SZ;

		if (ERROR_SUCCESS == RegQueryValueEx(hKey, "Path", 0, &dwType, (PBYTE)szdepends, &size))
		{
			RegCloseKey(hKey);
			return szdepends;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

/*----------------------------------------------------------------------------

  FUNCTION	: WriteRegistryString(char *szKey, char *szSubKey, char *pszStr)

  PURPOSE	: Writes a string registry entry for this app. If entry does not
			  exist creates a new one.

  RETURN   	: TRUE

  PARAMS   	: *szKey     - e.g. "MRU"
			: *szSubKey  - e.g. "1st"
			: *pszStr	 - the string

----------------------------------------------------------------------------*/
BOOL WriteRegistryString(char *szKey, char *szSubKey, char *pszStr, char *szdefault)
{
	DWORD dwdisp;
	HKEY  hkey;

	// make whole key string
	strcpy(szCurrentKey, szRegSubKey);
	strcat(szCurrentKey, szRegName);
	strcat(szCurrentKey, szKey);

	// The RegCreateKeyEx function creates the specified key. If the key
	// already exists in the registry, the function opens it.
	if (ERROR_SUCCESS != RegCreateKeyEx(KEY, szCurrentKey, 0, 0,
						REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hkey, &dwdisp))
	{
		return FALSE;
	}

	if (*pszStr != 0)
		RegSetValueEx(hkey, szSubKey, 0, REG_SZ, (PBYTE)pszStr, strlen(pszStr));
	else
		RegSetValueEx(hkey, szSubKey, 0, REG_SZ, (PBYTE)szdefault, strlen(szdefault));

	RegCloseKey(hkey);

  return TRUE;

}

/*----------------------------------------------------------------------------

  FUNCTION	: ReadRegistryInt(char *szKey, char *szSubKey, int cbReturnBuffer)

  PURPOSE	: Reads a int registry entry for this app.

  RETURN   	: int value or default value supplied by caller (cbReturnBuffer)

  PARAMS   	: *szKey    	 - e.g. "WindowPos"
			: *szSubKey 	 - e.g. "left"
			: cbReturnBuffer - default value returned if no reg entry

----------------------------------------------------------------------------*/
DWORD ReadRegistryInt(char *szKey, char *szSubKey, int cbReturnBuffer)
{
	HKEY  hKey;
	DWORD IntBuffer, cbValue, dwType, result;

	// make whole key string
	strcpy(szCurrentKey, szRegSubKey);
	strcat(szCurrentKey, szRegName);
	strcat(szCurrentKey, szKey);

	if (RegOpenKeyEx(KEY, szCurrentKey,	0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		cbValue = 4;
		dwType  = REG_DWORD;

		if (RegQueryValueEx(hKey, szSubKey, 0, &dwType, (BYTE*)&IntBuffer, &cbValue) == 0)
			result = IntBuffer;
		else
			result = cbReturnBuffer;

		RegCloseKey(hKey);

		return (result);
	}
	else
		return cbReturnBuffer;
}

/*----------------------------------------------------------------------------

  FUNCTION	: WriteRegistryInt(char *szKey, char *szSubKey, int n)

  PURPOSE	: Writes a int registry entry for this app. If entry does not
			  exist creates a new one.

  RETURN   	: TRUE

  PARAMS   	: *szKey     - e.g. "WindowPos"
 			: *szSubKey - e.g. "left"
			: n			 - the int value to write

----------------------------------------------------------------------------*/
BOOL WriteRegistryInt(char *szKey, char *szSubKey, int n)
{
	DWORD dwdisp;
	HKEY  hkey;

	// make whole key string
	strcpy(szCurrentKey, szRegSubKey);
	strcat(szCurrentKey, szRegName);
	strcat(szCurrentKey, szKey);

	// The RegCreateKeyEx function creates the specified key. If the key
	// already exists in the registry, the function opens it.
	if (ERROR_SUCCESS != RegCreateKeyEx(KEY, szCurrentKey, 0, 0,
						REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hkey, &dwdisp))
	{
		return FALSE;
	}

	RegSetValueEx(hkey, szSubKey, 0, REG_BINARY, (BYTE*)&n, sizeof(int));
	RegCloseKey(hkey);

  return TRUE;

}



