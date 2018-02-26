XMsgBox - September 2003

The XMsgBox code is designed to extend the functionality of the
MessageBox API. It provides the following features:

Timed Message box with countdown.
The message box can be set to close after a certain time. The countdown
can be displayed as part of the message text.

Custom button captions.
You can replace the normal message box button captions with text of 
your choosing.

Thread Safe.
XMsgBox is thread safe.

Optional asynchronous operation.
The XMsgBoxAsync() function will return immediately.

USAGE:
Usage notes are included in XMsgBox.h. Extensive samples and notes
are included in sample.c.

Here is the basic idea:

	xmb.hwnd = GetDesktopWindow();
	xmb.dwStyle = MB_ICONWARNING;
	xmb.szTitle = TEXT("XMsgBox()");
	xmb.szPrompt = TEXT("This message box will destruct in %T seconds.");
	xmb.dwTimerDuration = 15;
	xmb.dwTimerInterval = 1000;
	xmb.dwCountButtons = 2;
	xmb.szButtonCaptions[0] = TEXT("My button");
	xmb.szButtonCaptions[1] = TEXT("Your button");

	iRet = XMsgBox(&xmb);

Notes: The code can be compiled in unicode or ansi. The code should work on
95+/NT+ but is not extensively tested.