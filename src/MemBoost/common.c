
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples.
*       Copyright (C) 1994-1998 Microsoft Corporation.
*       All rights reserved.
*       This source code is only intended as a supplement to
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the
*       Microsoft samples programs.
\******************************************************************************/


/*

Module Name: common.c

Abstract: This module contains common apis used by tlist & kill.

*/


#include <windows.h>
#include <winperf.h>   // for Windows NT
#include <stdio.h>
#include <tlhelp32.h>  // for Windows 95
#include "common.h"

//
// manafest constants
//
#define INITIAL_SIZE        51200
#define EXTEND_SIZE         25600
#define REGKEY_PERF         "software\\microsoft\\windows nt\\currentversion\\perflib"
#define REGSUBKEY_COUNTERS  "Counters"
#define PROCESS_COUNTER     "process"
#define PROCESSID_COUNTER   "id process"
#define UNKNOWN_TASK        "unknown"


//
// Function pointer types for accessing Toolhelp32 functions dynamically.
// By dynamically accessing these functions, we can do so only on Windows
// 95 and still run on Windows NT, which does not have these functions.
//
typedef BOOL (WINAPI *PROCESSWALK)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef HANDLE (WINAPI *CREATESNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);

//
// prototypes
//
BOOL CALLBACK EnumWindowsProc(HWND hwnd, DWORD lParam);

/*

Routine Description:

    Provides an API for getting a list of tasks running at the time of the
    API call.  This function uses Toolhelp32to get the task list and is
    therefore straight WIN32 calls that anyone can call.

Arguments: dwNumTasks  - maximum number of tasks that the pTask array can hold

Return Value: Number of tasks placed into the pTask array.

*/
DWORD GetTaskList95(PTASK_LIST pTask, DWORD dwNumTasks)
{
   CREATESNAPSHOT pCreateToolhelp32Snapshot = NULL;
   PROCESSWALK    pProcess32First           = NULL;
   PROCESSWALK    pProcess32Next            = NULL;

   HANDLE         hKernel        = NULL;
   HANDLE         hProcessSnap   = NULL;
   PROCESSENTRY32 pe32           = {0};
   DWORD          dwTaskCount    = 0;


   // Guarantee to the code later on that we'll enum at least one task.
   if (dwNumTasks == 0)
      return 0;

    // Obtain a module handle to KERNEL so that we can get the addresses of
    // the 32-bit Toolhelp functions we need.
    hKernel = GetModuleHandle("KERNEL32.DLL");

    if (hKernel)
    {
        pCreateToolhelp32Snapshot = (CREATESNAPSHOT)GetProcAddress(hKernel, "CreateToolhelp32Snapshot");

        pProcess32First = (PROCESSWALK)GetProcAddress(hKernel, "Process32First");
        pProcess32Next  = (PROCESSWALK)GetProcAddress(hKernel, "Process32Next");
    }

    // make sure we got addresses of all needed Toolhelp functions.
    if (!(pProcess32First && pProcess32Next && pCreateToolhelp32Snapshot))
       return 0;


    // Take a snapshot of all processes currently in the system.
    hProcessSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == (HANDLE)-1)
        return 0;

    // Walk the snapshot of processes and for each process, get information
    // to display.
    dwTaskCount = 0;
    pe32.dwSize = sizeof(PROCESSENTRY32);   // must be filled out before use
    if (pProcess32First(hProcessSnap, &pe32))
    {
        do
        {
            LPSTR pCurChar;

            // strip path and leave executabe filename splitpath
            for (pCurChar = (pe32.szExeFile + lstrlen (pe32.szExeFile));
                 *pCurChar != '\\' && pCurChar != pe32.szExeFile;
                 --pCurChar)

            lstrcpy(pTask -> ProcessName, pCurChar);
            pTask -> flags = 0;
            pTask -> dwProcessId = pe32.th32ProcessID;

            ++dwTaskCount;   // keep track of how many tasks we've got so far
            ++pTask;         // get to next task info block.
        }
        while (dwTaskCount < dwNumTasks && pProcess32Next(hProcessSnap, &pe32));
    }
    else
        dwTaskCount = 0;    // Couldn't walk the list of processes.

    // Don't forget to clean up the snapshot object...
    CloseHandle (hProcessSnap);

    return dwTaskCount;
}

/*
Routine Description:

    Provides an API for getting a list of tasks running at the time of the
    API call.  This function uses the registry performance data to get the
    task list and is therefore straight WIN32 calls that anyone can call.

Arguments:

    dwNumTasks       - maximum number of tasks that the pTask array can hold

Return Value:

    Number of tasks placed into the pTask array.

*/
DWORD GetTaskListNT( PTASK_LIST  pTask, DWORD dwNumTasks)
{
    DWORD                        rc;
    HKEY                         hKeyNames;
    DWORD                        dwType;
    DWORD                        dwSize;
    LPBYTE                       buf = NULL;
    CHAR                         szSubKey[1024];
    LANGID                       lid;
    LPSTR                        p;
    LPSTR                        p2;
    PPERF_DATA_BLOCK             pPerf;
    PPERF_OBJECT_TYPE            pObj;
    PPERF_INSTANCE_DEFINITION    pInst;
    PPERF_COUNTER_BLOCK          pCounter;
    PPERF_COUNTER_DEFINITION     pCounterDef;
    DWORD                        i;
    DWORD                        dwProcessIdTitle;
    DWORD                        dwProcessIdCounter;
    CHAR                         szProcessName[MAX_PATH];
    DWORD                        dwLimit = dwNumTasks - 1;

    // Look for the list of counters.  Always use the neutral
    // English version, regardless of the local language.  We
    // are looking for some particular keys, and we are always
    // going to do our looking in English.  We are not going
    // to show the user the counter names, so there is no need
    // to go find the corresponding name in the local language.

    lid = MAKELANGID( LANG_ENGLISH, SUBLANG_NEUTRAL );
    sprintf( szSubKey, "%s\\%03x", REGKEY_PERF, lid );
    rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                       szSubKey,
                       0,
                       KEY_READ,
                       &hKeyNames
                     );
    if (rc != ERROR_SUCCESS) {
        goto exit;
    }

    //
    // get the buffer size for the counter names
    //
    rc = RegQueryValueEx( hKeyNames,
                          REGSUBKEY_COUNTERS,
                          NULL,
                          &dwType,
                          NULL,
                          &dwSize
                        );

    if (rc != ERROR_SUCCESS) {
        goto exit;
    }

    //
    // allocate the counter names buffer
    //
    buf = (LPBYTE) malloc( dwSize );
    if (buf == NULL) {
        goto exit;
    }
    memset( buf, 0, dwSize );

    //
    // read the counter names from the registry
    //
    rc = RegQueryValueEx( hKeyNames,
                          REGSUBKEY_COUNTERS,
                          NULL,
                          &dwType,
                          buf,
                          &dwSize
                        );

    if (rc != ERROR_SUCCESS) {
        goto exit;
    }

    //
    // now loop thru the counter names looking for the following counters:
    //
    //      1.  "Process"           process name
    //      2.  "ID Process"        process id
    //
    // the buffer contains multiple null terminated strings and then
    // finally null terminated at the end.  the strings are in pairs of
    // counter number and counter name.
    //

    p = buf;
    while (*p) {
        if (p > buf) {
            for( p2=p-2; isdigit(*p2); p2--) ;
            }
        if (stricmp(p, PROCESS_COUNTER) == 0) {
            //
            // look backwards for the counter number
            //
            for( p2=p-2; isdigit(*p2); p2--) ;
            strcpy( szSubKey, p2+1 );
        }
        else
        if (stricmp(p, PROCESSID_COUNTER) == 0) {
            //
            // look backwards for the counter number
            //
            for( p2=p-2; isdigit(*p2); p2--) ;
            dwProcessIdTitle = atol( p2+1 );
        }
        //
        // next string
        //
        p += (strlen(p) + 1);
    }

    //
    // free the counter names buffer
    //
    free( buf );


    //
    // allocate the initial buffer for the performance data
    //
    dwSize = INITIAL_SIZE;
    buf = malloc( dwSize );
    if (buf == NULL) {
        goto exit;
    }
    memset( buf, 0, dwSize );


    while (TRUE) {

        rc = RegQueryValueEx( HKEY_PERFORMANCE_DATA,
                              szSubKey,
                              NULL,
                              &dwType,
                              buf,
                              &dwSize
                            );

        pPerf = (PPERF_DATA_BLOCK) buf;

        //
        // check for success and valid perf data block signature
        //
        if ((rc == ERROR_SUCCESS) &&
            (dwSize > 0) &&
            (pPerf)->Signature[0] == (WCHAR)'P' &&
            (pPerf)->Signature[1] == (WCHAR)'E' &&
            (pPerf)->Signature[2] == (WCHAR)'R' &&
            (pPerf)->Signature[3] == (WCHAR)'F' ) {
            break;
        }

        //
        // if buffer is not big enough, reallocate and try again
        //
        if (rc == ERROR_MORE_DATA) {
            dwSize += EXTEND_SIZE;
            buf = realloc( buf, dwSize );
            memset( buf, 0, dwSize );
        }
        else {
            goto exit;
        }
    }

    //
    // set the perf_object_type pointer
    //
    pObj = (PPERF_OBJECT_TYPE) ((DWORD)pPerf + pPerf->HeaderLength);

    //
    // loop thru the performance counter definition records looking
    // for the process id counter and then save its offset
    //
    pCounterDef = (PPERF_COUNTER_DEFINITION) ((DWORD)pObj + pObj->HeaderLength);
    for (i=0; i<(DWORD)pObj->NumCounters; i++) {
        if (pCounterDef->CounterNameTitleIndex == dwProcessIdTitle) {
            dwProcessIdCounter = pCounterDef->CounterOffset;
            break;
        }
        pCounterDef++;
    }

    dwNumTasks = min( dwLimit, (DWORD)pObj->NumInstances );

    pInst = (PPERF_INSTANCE_DEFINITION) ((DWORD)pObj + pObj->DefinitionLength);

    //
    // loop thru the performance instance data extracting each process name
    // and process id
    //
    for (i=0; i<dwNumTasks; i++) {
        //
        // pointer to the process name
        //
        p = (LPSTR) ((DWORD)pInst + pInst->NameOffset);

        //
        // convert it to ascii
        //
        rc = WideCharToMultiByte( CP_ACP,
                                  0,
                                  (LPCWSTR)p,
                                  -1,
                                  szProcessName,
                                  sizeof(szProcessName),
                                  NULL,
                                  NULL
                                );

        if (!rc) {
            //
	    // if we cant convert the string then use a default value
            //
            strcpy( pTask->ProcessName, UNKNOWN_TASK );
        }

        if (strlen(szProcessName)+4 <= sizeof(pTask->ProcessName)) {
            strcpy( pTask->ProcessName, szProcessName );
            strcat( pTask->ProcessName, ".exe" );
        }

        //
        // get the process id
        //
        pCounter = (PPERF_COUNTER_BLOCK) ((DWORD)pInst + pInst->ByteLength);
        pTask->flags = 0;
        pTask->dwProcessId = *((LPDWORD) ((DWORD)pCounter + dwProcessIdCounter));
        if (pTask->dwProcessId == 0) {
            pTask->dwProcessId = (DWORD)-2;
        }

        //
        // next process
        //
        pTask++;
        pInst = (PPERF_INSTANCE_DEFINITION) ((DWORD)pCounter + pCounter->ByteLength);
    }

exit:
    if (buf) {
        free( buf );
    }

    RegCloseKey( hKeyNames );
    RegCloseKey( HKEY_PERFORMANCE_DATA );

    return dwNumTasks;
}

/*
Routine Description:

    Changes the process's privilege so that kill works properly.

Arguments:


Return Value:

    TRUE             - success
    FALSE            - failure

Comments:
    Always returns TRUE
*/
BOOL EnableDebugPriv95( void )
{
   return TRUE;
}

/*
Routine Description:

    Changes the process's privilege so that kill works properly.

Arguments:


Return Value:

    TRUE             - success
    FALSE            - failure
*/
BOOL EnableDebugPrivNT( void )
{
    HANDLE hToken;
    LUID DebugValue;
    TOKEN_PRIVILEGES tkp;


    //
    // Retrieve a handle of the access token
    //
    if (!OpenProcessToken(GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
            &hToken)) {
        printf("OpenProcessToken failed with %d\n", GetLastError());
        return FALSE;
    }

    //
    // Enable the SE_DEBUG_NAME privilege
    //
    if (!LookupPrivilegeValue((LPSTR) NULL,
            SE_DEBUG_NAME,
            &DebugValue)) {
        printf("LookupPrivilegeValue failed with %d\n", GetLastError());
        return FALSE;
    }

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = DebugValue;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken,
        FALSE,
        &tkp,
        sizeof(TOKEN_PRIVILEGES),
        (PTOKEN_PRIVILEGES) NULL,
        (PDWORD) NULL);

    //
    // The return value of AdjustTokenPrivileges can't be tested
    //
    if (GetLastError() != ERROR_SUCCESS) {
        printf("AdjustTokenPrivileges failed with %d\n", GetLastError());
        return FALSE;
    }

    return TRUE;
}

/* Terminating a process causes the following:

	1.	All of the object handles opened by the process are closed.
	2.	All of the threads in the process terminate their execution.
	3.	The state of the process object becomes signaled, satisfying
		any threads that had been waiting for the process to terminate.
	4.	The states of all threads of the process become signaled,
		satisfying any threads that had been waiting for the threads to terminate.
	5.	The termination status of the process changes from STILL_ACTIVE
		to the exit value of the process.

	Terminating a process does not cause child processes to be terminated.

	Terminating a process does not necessarily remove the process
	object from the system. A process object is deleted when the last
	handle to the process is closed. */

/* Also
   The TerminateProcess function is used to unconditionally cause a
   process to exit. Use it only in extreme circumstances. The state
   of global data maintained by dynamic-link libraries (DLLs) may
   be compromised if TerminateProcess is used rather than ExitProcess.
*/
BOOL KillProcess( PTASK_LIST tlist )
{
	HANDLE hProcess;

	hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, tlist->dwProcessId );
	if (hProcess) {

		if (!TerminateProcess( hProcess, 1 )) {
			CloseHandle( hProcess );
			return FALSE;
		}

		CloseHandle( hProcess );
		if (tlist->hwnd)
			PostMessage( tlist->hwnd, WM_CLOSE, 0, 0 );

		return TRUE;

	}else{
		return FALSE;
	}

}

// enumerate all windows
void GetWindowTitles(PTASK_LIST_ENUM te)
{
    EnumWindows( EnumWindowsProc, (LPARAM) te );
}

/*
Routine Description:

    Callback function for window enumeration.

Arguments:

    hwnd             - window handle
    lParam           - ** not used **

Return Value:

    TRUE  - continues the enumeration

*/
BOOL CALLBACK EnumWindowsProc(HWND hwnd, DWORD lParam)
{
    DWORD             pid = 0;
    DWORD             i;
    CHAR              buf[TITLE_SIZE];
    PTASK_LIST_ENUM   te = (PTASK_LIST_ENUM)lParam;
    PTASK_LIST        tlist = te->tlist;
    DWORD             numTasks = te->numtasks;

    // get the processid for this window
    if (!GetWindowThreadProcessId( hwnd, &pid )) {
        return TRUE;
    }

    //
    // look for the task in the task list for this window
    //
    for (i=0; i<numTasks; i++) {
        if (tlist[i].dwProcessId == pid) {
            tlist[i].hwnd = hwnd;
            //
	    // we found the task so lets try to get the
            // window text
            //
            if (GetWindowText( tlist[i].hwnd, buf, sizeof(buf) )) {
                //
		// got it, so lets save it
                //
                strcpy( tlist[i].WindowTitle, buf );
            }
            break;
        }
    }

    //
    // continue the enumeration
    //
    return TRUE;
}


