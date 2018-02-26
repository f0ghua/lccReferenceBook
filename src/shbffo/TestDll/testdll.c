/* --- The following code comes from c:\programme\lcc\lib\wizard\textmode.tpl. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <shlobj.h>
#pragma pack(push, 1)
#include <shellapi.h>
#pragma pack(pop)
#include "..\shbffo.h"

void Usage(char *szProgramName)
{
	printf("%s usage:\n\n", szProgramName);
	printf("%s [-BFF] [-FO]\n\n", szProgramName);
	printf("\t-BFF tests BrowseForFolder, -FO tests FileOperation.\n");
}

HWND GetConsoleHwnd(void)
{
	char szConsoleTitle[255];
	char szProcessId[255];
	HWND hWnd;

	GetConsoleTitle(szConsoleTitle, sizeof(szConsoleTitle));
	sprintf(szProcessId, "%lu", GetCurrentProcessId());
	SetConsoleTitle(szProcessId);
	Sleep(400);
	hWnd = FindWindow(NULL, szProcessId);
	SetConsoleTitle(szConsoleTitle);

	return hWnd;
}


int main(int argc,char *argv[])
{
	int nReturn;
	DWORD dwType = 0;
	char szFolder[(MAX_PATH + 4) * 2];
	char szSource[MAX_PATH + 1], szTarget[MAX_PATH + 1];

	if (argc == 1)
	{
		Usage(argv[0]);
		return 9;
	}

	if (!stricmp(argv[1], "-BFF"))
	{
		//Test BrowseForFolder
		ZeroMemory(szFolder, sizeof(szFolder));
		if (argc == 3)
			strcpy(szFolder, argv[2]);
		nReturn = BrowseForFolder(GetConsoleHwnd(),
								  NULL,
								  0,
								  BFFUI_NEWFOLDER | BFFUI_EDITBOX | BFFUI_RESIZABLE | BFFUI_DONTAPPEND,
								  "Select destination directory",
								  "Select the directory to install the software into",
								  "&New Folder",
								  0,
								  NULL,
								  szFolder,
								  &dwType
								  );
		if (nReturn == BFFRET_OK)
		{
			printf("You selected\n\n\t%s\n\n", szFolder);

			switch (dwType)
			{
				case BFF_OBJECTTYPE_UNKNOWN:
					printf("The type of the path is unknown\n");
					break;
				case BFF_OBJECTTYPE_DIRECTORY:
					printf("The path is a directory\n");
					break;
				case BFF_OBJECTTYPE_FILE:
					printf("The path is a file\n");
					break;
				case BFF_OBJECTTYPE_COMPUTER:
					printf("The path is a computer name\n");
					break;
				case BFF_OBJECTTYPE_DOMAIN:
					printf("The path is the name of a network domain/workgroup\n");
					break;
				case BFF_OBJECTTYPE_SHARE:
					printf("The path is a network share\n");
					break;
				case BFF_OBJECTTYPE_NETGENERIC:
					printf("The path is a generic network resource\n");
					break;
				default:
					printf("Unexpected type\n");
					break;
			}
		}
		else if (nReturn == BFFRET_CANCEL)
			printf("Cancelled\n");
		else
			printf("Error %lu\n", nReturn);

	}
	else if (!stricmp(argv[1], "-FO"))
	{
		strcpy(szSource, "C:\\autoexec.bat\r\nC:\\config.sys");
		strcpy(szTarget, "E:\\Temp");

		nReturn = FileOperation(GetConsoleHwnd(),
								szSource,
								szTarget,
								FO_COPY,
								0
								);
		switch (nReturn)
		{
			case FORET_SUCCESS:
				printf("Success\n");
				break;
			case FORET_SYSABORT:
				printf("The system aborted the operation\n");
				break;
			case FORET_USERABORT:
				printf("The user aborted the operation\n");
				break;
			case FORET_INVALID:
				printf("The source and destination file lists are not consistent/invalid\n");
				break;
			case FORET_NOMEMORY:
				printf("Out of memory\n");
				break;
			default:
				printf("Unexpected return code %lu\n",nReturn);
		}
	}
	else
		printf("Unknown option %s.\n", argv[1]);

	return nReturn;
}
