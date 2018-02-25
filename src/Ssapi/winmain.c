/*---------------------------------------------------------------------
Author     : Servé Laurijssen 2003
Project    : SSafeScript
Module     :
File       : winmain.c
Created    : 2-8-2003
Modified   : 2-8-2003
--------------------------------------------------------------------- */


#include <windows.h>
#include <stdio.h>
#include <stringc.h>
#include <stdlib.h>
#include <stringcsplit.h>
#include "ssapi.h"

#define ARRSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define MAXLINE 512

const char * COMMANDS[] = { "PROJECT", "GET", "CHECKOUT", "CHECKIN" };

typedef int (*COMMANDFUNC)(struct ccSplit *);

static int intndlePROJECTCommand	(struct ccSplit *cmdline);
static int HandleGETCommand				(struct ccSplit *cmdline);
static int HandleCHECKOUTCommand	(struct ccSplit *cmdline);
static int HandleCHECKINCommand		(struct ccSplit *cmdline);
static 		 COMMANDFUNC GetCommand(stringc *cmd);

static int  ExecuteScript(FILE *fp);
static void Cleanup(void);

wchar_t *A2W(wchar_t *ws, stringc *s);

struct
{
	IVSSDatabase *db;
} SS = {0};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
	int ret = EXIT_FAILURE;

	if (strlen(lpCmdLine) > 0)
	{
		FILE *fp = fopen(lpCmdLine, "r");
		if (fp)
		{
			CoInitialize(NULL);
			if (ExecuteScript(fp))
				ret = EXIT_SUCCESS;
			Cleanup();
			CoUninitialize();
		}
		else
			MessageBox(NULL, "Could not find the sourcesafe script", "Error", MB_OK | MB_ICONERROR);

	}
	else
		MessageBox(NULL, "Please specify the sourcesafe script to execute\n"
										 "on the command line", "Error", MB_OK | MB_ICONERROR);

	return ret;
}

static int ExecuteScript(FILE *fp)
{
	char buf[MAXLINE];
	struct ccSplit *split = NULL;
	int line = 0;
	int ret = 1;

	while (fgets(buf, MAXLINE, fp))
	{
		line++;
		buf[strcspn(buf, "\r\n")] = '\0';

		split = ccStrSplit(buf, ",");

		if (split && split->arraylen > 0)
		{
			for (int i = 0; i < split->arraylen; i++)
				ccStrTrim(split->array[i]);

			if (strcspn(C_STR(split->array[0]), "#") == 0)
				continue;

			COMMANDFUNC func = GetCommand(split->array[0]);

			if (func)
			{
				if (!func(split))
				{
					if (SS.db)
					{
						snprintf(buf, MAXLINE, "An error occurred while processing %s at line %d.",
										 C_STR(split->array[0]), line);
						MessageBox(NULL, buf, "Error", MB_OK | MB_ICONERROR);
					}
					else
					{
						snprintf(buf, MAXLINE, "An error occurred while processing %s at line %d. No project is open",
										 C_STR(split->array[0]), line);
						MessageBox(NULL, buf, "Error", MB_OK | MB_ICONERROR);
					}
					ret = 0;
					break;
				}
			}
			else
			{
				snprintf(buf, MAXLINE, "Invalid command (%s) at line %d.\n"
															 "The program cannot continue", C_STR(split->array[0]), line);
				MessageBox(NULL, buf, "Error", MB_OK | MB_ICONERROR);
				ret = 0;
				break;
			}
		}
		ccFreeSplitStr(split);
	}
	fclose(fp);

	return ret;
}

static int HandlePROJECTCommand(struct ccSplit *cmdline)
{
	int ret = 0;

	if (cmdline->arraylen == 4)
	{
		wchar_t project[MAXLINE/2];
		wchar_t username[MAXLINE/2];
		wchar_t password[MAXLINE/2];

		A2W(project, cmdline->array[1]);
		A2W(username, cmdline->array[2]);
		A2W(password, cmdline->array[3]);

		if (SS.db)
		{
			SS.db->Release();
			SS.db = NULL;
		}
		if (SUCCEEDED(CoCreateInstance(&CLSID_VSSDatabase, 0, CLSCTX_INPROC,
																	 &IID_IVSSDatabase, (void **)&SS.db)))
		{
			if (SUCCEEDED(SS.db->Open(project, username, password)))
			{
				ret = 1;
			}
		}
	}
	else
		MessageBox(NULL, "Invalid PROJECT command.", "Error", MB_OK | MB_ICONERROR);

	return ret;
}

static int HandleGETCommand(struct ccSplit *cmdline)
{
	int ret = 0;

	if (SS.db && cmdline->arraylen == 2)
	{
		IVSSItem *item;
		wchar_t getwhat[MAXLINE];

		A2W(getwhat, cmdline->array[1]);
		if (SUCCEEDED(SS.db->get_VSSItem(getwhat, 0, &item)))
		{
			BSTR localspec;
			if (SUCCEEDED(item->get_LocalSpec(&localspec)) && SysStringLen(localspec) > 0)
			{
				if (SUCCEEDED(item->Get(&localspec, VSSFLAG_TIMEMOD | VSSFLAG_RECURSYES)))
					ret = 1;
			}
			else
				MessageBox(NULL, "The project has no working dir set. "
		 											"Do this first with the SourceSafe manager", "", MB_OK);
			SysFreeString(localspec);
			item->Release();
		}
	}
	return ret;
}

static int HandleCHECKOUTCommand(struct ccSplit *cmdline)
{
	int ret = 0;

	if (SS.db && (cmdline->arraylen == 2 || cmdline->arraylen == 3))
	{
		IVSSItem *item;
		wchar_t getwhat[MAXLINE];

		A2W(getwhat, cmdline->array[1]);
		if (SUCCEEDED(SS.db->get_VSSItem(getwhat, 0, &item)))
		{
			BSTR localspec;
			if (SUCCEEDED(item->get_LocalSpec(&localspec)) && SysStringLen(localspec) > 0)
			{
				*getwhat = L'\0';
				if (cmdline->arraylen == 3)
					A2W(getwhat, cmdline->array[2]);
				if (SUCCEEDED(item->Checkout(getwhat, localspec, 0)))
					ret = 1;
			}
			else
				MessageBox(NULL, "The project has no working dir set. Do this first with the SourceSafe manager", "", MB_OK);
			SysFreeString(localspec);
			item->Release();
		}
	}
	return ret;
}

static int HandleCHECKINCommand(struct ccSplit *cmdline)
{
	int ret = 0;

	if (SS.db && (cmdline->arraylen == 2 || cmdline->arraylen == 3))
	{
		IVSSItem *item;
		wchar_t putwhat[MAXLINE];

		A2W(putwhat, cmdline->array[1]);
		if (SUCCEEDED(SS.db->get_VSSItem(putwhat, 0, &item)))
		{
			BSTR localspec;
			if (SUCCEEDED(item->get_LocalSpec(&localspec)) && SysStringLen(localspec) > 0)
			{
				*putwhat = L'\0';
				if (cmdline->arraylen == 3)
					A2W(putwhat, cmdline->array[2]);
				if (SUCCEEDED(item->Checkin(putwhat, localspec, 0)))
					ret = 1;
			}
			else
				MessageBox(NULL, "The project has no working dir set. Do this first with the SourceSafe manager", "", MB_OK);
			SysFreeString(localspec);
			item->Release();
		}
	}

	return ret;
}

static COMMANDFUNC GetCommand(stringc *cmd)
{
	char *s = strupr(C_STR(cmd));
	if (strcmp(s, COMMANDS[0]) == 0)
		return HandlePROJECTCommand;
	if (strcmp(s, COMMANDS[1]) == 0)
		return HandleGETCommand;
	if (strcmp(s, COMMANDS[2]) == 0)
		return HandleCHECKOUTCommand;
	if (strcmp(s, COMMANDS[3]) == 0)
		return HandleCHECKINCommand;

	return NULL;
}

wchar_t *A2W(wchar_t *ws, stringc *s)
{
	mbstowcs(ws, C_STR(s), C_LEN(s)+1);

	return ws;
}


static void Cleanup(void)
{
	if (SS.db)
		SS.db->Release();
}
