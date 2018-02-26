/* XMsgBox code by TMouse (TMouse at NOSPAM bigfoot dot com)
 * Based loosely on Visual Basic code found at VBNet:
 * http://www.mvps.org/vbnet/index.html?code/hooks/messageboxhooktimerapi.htm
 * Feel free to distribute. */

/* This sample code demonstrates using the XMsgBox code found in
 * XMsgBox.c and XMsgBox.h */

#include <windows.h>
#include <stdio.h>

#include "xmsgbox.h"

// ==============================================================
// SAMPLE ONE - Timed msg box with custom button captions.
// ==============================================================
void SampleOne() {

	X_MSG_BOX_PARAMS xmb;
	int iRet;

	ZeroMemory(&xmb, sizeof(X_MSG_BOX_PARAMS));

	// Set message box title...
	xmb.szTitle = TEXT("XMsgBox Sample: Timed msgbox with custom buttons");

	// Set the message box style. Note we do not include a style
	// specifying buttons (eg. MB_YESNO) when we are using custom buttons. 
	// See the uType argument in the MessageBox API docs for style values...
	xmb.dwStyle = MB_ICONINFORMATION;

	// Set the length of each timer interval(in milliseconds)....
	xmb.dwTimerInterval = 1000;

	// Set the number of intervals before the message box will timeout...
	xmb.dwTimerDuration = 10;

	// Set the owner window. Used as the first argument to MessageBox API...
	xmb.hwnd = GetDesktopWindow();

	// To enable the countdown TimerProc routine
	// to update the message box, place a %T variable
	// inside the message string.
	xmb.szPrompt = TEXT("This is an XMsgBox sample. This message box will close in %T seconds."); 

	// Set the number of custom buttons 0 - 3.
	// Zero equals do not use custom buttons.
	xmb.dwCountButtons = 3;

	// Set custom  button captions...
	xmb.szButtonCaptions[0] = TEXT("Button1");
	xmb.szButtonCaptions[1] = TEXT("Button2");
	xmb.szButtonCaptions[2] = TEXT("Button3");

	// Show our XMsgBox...
	iRet = XMsgBox(&xmb);

	switch (iRet) {
		case XMB_ERROR:   /* = 0 */
			printf("Error %d occurred\n", GetLastError());
			break;
		case XMB_BUTTON1: /* = 1 */
			printf("Button One was pressed\n");
			break;
		case XMB_BUTTON2: /* = 2 */
			printf("Button two was pressed\n");
			break;
		case XMB_BUTTON3: /* = 3 */
			printf("Button three was pressed\n");
			break;
		case XMB_TIMEOUT: /* = -1 */
			printf("The message box timed out.\n");
			break;
		default:
			printf("Unexpected result!\n");
	}
}


// ==============================================================
// SAMPLE TWO - Custom buttons with no timeout
// ==============================================================
void SampleTwo() {

	X_MSG_BOX_PARAMS xmb;
	int iRet;

	ZeroMemory(&xmb, sizeof(X_MSG_BOX_PARAMS));

	xmb.szTitle = TEXT("XMsgBox Sample: Custom buttons with no timeout");

	// Note that we can set a default button while using custom buttons...
	xmb.dwStyle = MB_ICONQUESTION | MB_DEFBUTTON2;

	// We set dwTimerDuration to zero to disable the timed functionality...
	xmb.dwTimerDuration = 0;    // number of intervals to wait
	xmb.dwTimerInterval = 0;    // the length of each interval(in milliseconds).

	xmb.hwnd = NULL;

	// The message text. As timing is disabled %T will not be replaced.
	xmb.szPrompt = TEXT("What would you like to do today? We'll put a %T here for demonstration purposes.");

	// Set number of custom buttons 0 - 3.
	xmb.dwCountButtons = 2;

	// Set button captions...
	xmb.szButtonCaptions[0] = TEXT("Eat!");
	xmb.szButtonCaptions[1] = TEXT("Sleep!");

	iRet = XMsgBox(&xmb);

	if (iRet == 0) printf("The %d error occurred while displaying the message box.\n", GetLastError());
	else printf("Button Number %d was pressed by the user.\n", iRet);
}


// ==============================================================
// SAMPLE THREE - Timed message box with normal button captions.
// ==============================================================
void SampleThree() {

	X_MSG_BOX_PARAMS xmb;
	int iRet;

	ZeroMemory(&xmb, sizeof(X_MSG_BOX_PARAMS));

	xmb.szTitle = TEXT("XMsgBox Sample: Timed Message Box with normal buttons");

	// As we are not using custom buttons we should specify the buttons
	// to use in dwStyle. In this case we desire yes/no buttons.
	xmb.dwStyle = MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2;

	// We set the timer interval to 500 (half a second) to provide a faster countdown.
	xmb.dwTimerInterval = 500;

	// We set the duration to 30. This means the message box will display
	// for fifteen seconds (30 * 500 milliseconds)...
	xmb.dwTimerDuration = 30;

	xmb.hwnd = GetDesktopWindow();

	// Set the prompt with %T where intervals till timeout should be substituted...
	xmb.szPrompt = TEXT("Do you think XMsgBox() will be useful?\nIf you do not select an answer before the countdown reaches 0, no will be assumed.\nCountdown: %T");

	// Set number of custom buttons 0 - 3.
	// We set this to zero to indicate we are not using custom buttons.
	// Buttons will be based on dwStyle.
	xmb.dwCountButtons = 0;

	iRet = XMsgBox(&xmb);

	// Because we are not using custom buttons the normal result from MessageBox
	// will be returned, unless the message box timed out, in which case XMB_TIMEOUT(-1)
	// will be returned.
	switch (iRet) {
		case XMB_ERROR: /* = 0 */
			printf("The %d error occurred\n", GetLastError());
			break;
		case IDYES:
			printf("You picked Yes! I'm glad to hear that!\n");
			break;
		case IDNO:
		case XMB_TIMEOUT: /* In this sample timeout equals NO */
			printf("Damn and blast! You picked no!\n");
			break;
	}
}


// ==============================================================
// SAMPLE FOUR - Demonstration of using XMsgBoxAsync
// ==============================================================
void SampleFour() {

	X_MSG_BOX_PARAMS xmb;
	HANDLE hThread;
	DWORD dwThreadId;
	DWORD dwProcessingCount = 0;

	ZeroMemory(&xmb, sizeof(X_MSG_BOX_PARAMS));

	xmb.szTitle = TEXT("XMsgBoxAsync Sample");
	xmb.szPrompt = TEXT("Your hard disk is now being formatted.\nPress cancel to abort...");
	xmb.dwStyle = MB_ICONEXCLAMATION;
	xmb.dwCountButtons = 1;
	xmb.szButtonCaptions[0] = "Cancel";

	// NOTE: xmb must be valid for the lifetime of the message box.

	// Show the message box...
	XMsgBoxAsync(&xmb, &hThread, &dwThreadId);

	// Now continue work...

	while(TRUE) {

		// do some processing...
		Sleep(500);
		dwProcessingCount++;
		printf("Processing (%d of 15)...\n", dwProcessingCount);
		
		if (dwProcessingCount >= 15) {
			// We are finished so close the message box.

			printf("Processing is complete.\n");
			// Posting a WM_QUIT message to the thread will make
			// the message box close...
			PostThreadMessage(dwThreadId, WM_QUIT, 0, 0);

			break;
		}
		else if (WaitForSingleObject(hThread, 0) == WAIT_OBJECT_0) {
			// If the thread has finished the user has
			// dismissed the message box...
			printf("User pressed cancel. Processing was aborted.\n");

			// We can get the result if needed.
			// --> GetExitCodeThread(hThread, &dwExitCode);

			// In this case we know the user must have pressed cancel.
			break;
		}
	}

	CloseHandle(hThread);
}


// ==============================================================
// SAMPLE FIVE - Using XMsgBox in a multi threaded manner
// ==============================================================
DWORD CALLBACK SampleFive(LPVOID lpParam) {

	X_MSG_BOX_PARAMS xmb;

	ZeroMemory(&xmb, sizeof(X_MSG_BOX_PARAMS));

	xmb.hwnd = GetDesktopWindow();
	xmb.dwStyle = MB_ICONWARNING;
	xmb.szTitle = TEXT("Multi Threaded Example");
	xmb.szPrompt = TEXT("T-%T The rocket is due to launch at T-%T.");
	xmb.dwTimerDuration = ((DWORD) lpParam + 1) * 8;
	xmb.dwTimerInterval =  ((DWORD) lpParam + 1) * 1000;
	xmb.dwCountButtons = 3;
	xmb.szButtonCaptions[0] = TEXT("Launch!");
	xmb.szButtonCaptions[1] = TEXT("Destruct!");
	xmb.szButtonCaptions[2] = TEXT("RUN!");

	printf("Multi threaded example %d returned %d\n", (DWORD) lpParam, XMsgBox(&xmb));

	return 0;
}


// ==============================================================
int main() {

	int x;
	HANDLE hThreads[3];

	SampleOne();   // Timed with custom buttons
	SampleTwo();   // Custom buttons with no timeout
	SampleThree(); // Timed message box with normal button captions
	SampleFour();  // Sample of using XMsgBoxAsync

	// Sample Five - multi threaded example..
	for (x=0; x < 3; x++) {
		hThreads[x] = CreateThread(NULL, 0, SampleFive, (LPVOID) x, 0, NULL);
		Sleep(100);
	}

	// Wait for thread examples to finish and cleanup...
	WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);
	for (x=0; x < 3; x++) CloseHandle(hThreads[x]);

	printf("Press ENTER to continue...");
	getchar();

	return 0;
}