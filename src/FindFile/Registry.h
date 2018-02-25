// registry.h


#ifndef _REGISTRY_H_
#define _REGISTRY_H_

DWORD  ReadRegistryString(char *pszKey, char *pszSubKey, char *pszDefault, char *pszReturnBuffer);
BOOL   WriteRegistryString(char *szKey, char *szSubKey, char *pszStr, char *szdefault);
DWORD  ReadRegistryInt(char *pszKey, char *pszSubKey, int cbReturnBuffer);
BOOL   WriteRegistryInt(char *szKey, char *szSubKey, int n);
char * ReadRegistryDepends(void);

#endif // _REGISTRY_H_
