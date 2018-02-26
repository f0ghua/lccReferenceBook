A rudimentary crash report logger meant to aid tracking down bugs in programs
compiled without debug information. It is based on sample code from Matt
Pietrek's "Under the Hood" column in the April 1997 issue of Microsoft Systems
Journal.

Date: 10/31/2003
Author: Mike Caetano
License: Whatever follows from the Pietrek code plus those portions of code
taken from the MS Platform SDK plus a general "use at your own risk" - and of
course, some credit for the added value of my effort would be nice too! :)

// ---------------------------------------------------------------------------
What is this, how do I use it, why would I use it and what does it do?

This code is a rudimentary crash report logger based on sample code from Matt
Pietrek's "Under the Hood" column in the April 1997 issue of Microsoft Systems
Journal. It is intended for use with a program compiled in "release" mode, that
is, a program compiled without debugging information. It has been designed to
work with programs compiled with lcc-win32, that is, 32 bit Windows programs
intended to run on x86 compatible CPUs. It is not crafted to work on computers
with more than one CPU. It might work on computers with multiple CPUs, but I
haven't tested it under those conditions. I've checked that all of the Win32 API
functions used are available on every version of Windows from Windows 95 to
Windows XP, but I've only tested it on Windows 2000. If you find any problems
with it on other versions of Windows, please let me know (prionx87@hotmail.com).

To use this code, add the ueh.c source file to your project and include the
ueh.h header file in the file containing main or WinMain. Then place a call to
the InitializeExceptionHandler function immediately after the opening brace of
either of those functions. Select the "generate a map file" option on the linker
tab of the project configuration dialog in wedit. This is important. The
generated map file is the only way that the information contained in the crash
report can be related back to the original source code for the faulting program.
This code will work with the debugging level set to -g2, but if the debugging
level is set to -g3 or -g4 the exception handling code supplied by lcc-win32
will override this code.

The code in ueh.c performs similar tasks as the code supplied by lcc-win32 when
the -g3 or -g4 settings are used, except that the crash information is written
to a file rather than displayed in a popup. This makes it easier to track down
the bug that caused the fault because the information remains available for use
at a later time. With a popup, that information disappears as soon as the popup
is dismissed. In situations where you have shared your projects with others,
this makes it easier for them to report bugs to you. They can simply email you
the log file without having to fuss with too much of an explanation as to what
went wrong. This makes it easier for you too, because you don't have to grill
them for information about their computer or the faulting information supplied
by the operating system's default exception handler. It's all there in the log.
Of course, the log file won't tell you everything about the fault that there is
to know. If the fault occured in response to some action that the user took, the
user will still need to tell you what they did to evoke the fault ("I pressed
this button and then the program died..."). Nevertheless, having an exception
report available will make finding and fixing the bug easier.

How does this code do what it does?

The InitializeExceptionHandler function does three things. It sets up the print
buffer for the crash report and records basic information about the execution
environment of the program to this print buffer. This includes the program
launch time, the operating system version and information about the CPU. The
function then uses the name of the program file to derive a name for the report
log file. Lastly, this function directs the operating system to suppress the
default failure popup box that would otherwise be displayed in response to an
exception. Then using the SetUnhandledExceptionFilter API, this function
replaces the default operating system provided "unhandled exception handler"
(ueh) with the the local CatchAll function.

With this in place, if an unexpected exception occurs in the program while it's
running outside of a debugger, the operating system will invoke the CatchAll
function passing it a pointer to a structure containing information about the
state of the program at the time of the exception. An unexpected exception is an
exception that was not foreseen during development and guarded against via a
try-except block or similar mechanism. In more common parlance, an unexpected
exception is a bug and an unhandled exception handler could be termed a "bug
catcher". That is why I named the handler function "CatchAll", it catches all
the bugs - at least that's what it's supposed to do.

The CatchAll function has the same signature as the UnhandledExceptionFilter API
function. Both of these functions can also be referred to as "top-level
exception filter" functions. They provide a "filter" on the exception, returning
a value to the operating system that indicates whether the process should
continue running, should be shut down or whether the operating system should
continue searching for an exception handler for this exception. CatchAll is
crafted to inform the operating system that the exception has been handled, but
this value is never returned because CatchAll invokes the ExitProcess function
shutting down the program itself. But I'm getting ahead of myself here. CatchAll
does a lot of things before shutting down the faulting process.

When CatchAll receives information about a fault the first thing it does is to
pass that information along to the GenerateExceptionReport function. (Actually,
if you look at the code for CatchAll you'll see that the first items in the
function are related to debugging CatchAll. I'll cover that later, but before I
do that I want to explain what CatchAll is designed to do in response to an
exception.) The GenerateExceptionReport function fills the print buffer with the
exception information after transforming it into a more useable form. Then
CatchAll calls the LogExceptionReport function which writes the print buffer out
to a file all at once.

In Matt Pietrek's original code, the log file was written to with every
invocation of the ExceptionPrint function using the FILE_FLAG_WRITE_THROUGH flag
to ensure a disk write. This produced a lot of hard drive noise that I felt
could possibly create the impression with the user that the fault was worse than
it might actually be. I found that perfectly acceptable for demonstration code,
but felt that a more professional implementation would seek to minimize the
possibility of creating a negative impression on the user. It's bad enough that
the program faulted, let alone to add to it by inducing disk "scratching"
sounds! ("Help! Your software is killing my computer!...")

Therefore, to reduce the disk writing noise, I implemented a print buffer that
the ExceptionPrint function could incrementally write to in ordinary printf
fashion. Because the viability of the process is questionable following an
exception, I intentionally sought to limit the size of the print buffer to one
page of memory (4096 bytes), however, this increases to two pages when compiled
with UNICODE defined (yes, this code is Unicode compatible). Further, to
guarantee that that the memory for the print buffer would be available, I
declared the buffer variable as static and initialized it to zero to ensure that
space for it would be allocated within the executable file itself.

Lastly, to guarantee that the print buffer would not be overrun, I implemented
tracking measures in the ExceptionPrint function to prevent writing to a full
print buffer. When the print buffer nears it's capacity, ExceptionPrint prints a
warning message to the buffer and sets the counter that trace the remaining
space in the buffer to zero to  prevent further writes. When the
LogExceptionReport function is called, it opens the log file, sets the file
pointer to append, writes the entire static print buffer to the file at once and
then closes the file. Because I still use the FILE_FLAG_WRITE_THROUGH flag to
ensure a disk write, there might still be some disk noise generated, but it is
considerably less than before.

After the log file is written, CatchAll displays a message box to inform the
user that an error has occured and that there are details available in the log
file. You should change this message to suit the needs of your application. For
example, you could ask the user to send the log file to you as an email
attachment. You could also add additional code to format an email message with
the log file attached, open an smtp connection and ask the user to send you the
report. I leave that up to you.

The last thing that CatchAll does is to call ExitProcess. This provides a clean
shutdown to the process, ensuring that the entry points of all dlls attached to
the process are called with the DLL_PROCESS_DETACH message and that all object
handles are closed and any threads in the process are terminated cleanly as
well. Consult the documentation on ExitProcess for details. Also note that in
some situations calling ExitProcess from a dll can lead to additional exceptions
and system errors. So, if you intend to use to use this code in a dll, you might
want to comment out the call to ExitProcess.

A closer look at GenerateExceptionReport

Let's take a closer look at the GenerateExceptionReport function and the various
items included in the exception log report. The first thing that the
GenerateExceptionReport function does is to record the exception code. The
exception code indicates the type of fault and thus provides a clue as to what
when wrong and how to fix it. I can't begin to explain here what the various
exception codes mean, that knowledge can truly only be acquired through
experience. However, to make it easier to make sense of the fault,
GenerateExceptionReport attempts to provide a name for the most common faults
and if that fails an operating system supplied description of the fault using
the FormatMessage API.

GenerateExceptionReport then attempts to provide information about the location
of the fault: the address of the instruction that produced the fault, the
section and offset in that section in the module where the instruction is
located and the name of that module as well. Following that,
GenerateExceptionReport records the time that the fault occurred. This is
actually the time when this portion of the code is reached, but that's close
enough to the fault time that it remains suitable for use. The time is recorded
in UTC (Coordinated Universal Time), the same as the launch time and both of
these values can be used to determine how long the program was running before
the exception occurred.

After printing the faulting information, GenerateExceptionReport prints the
information pointed to by the ContextRecord member of the ExceptionPointers
structure. This information pertains to the state of the CPU at the time of the
exception. For the x86 family of CPUs this includes the control registers, the
integer registers and the segment registers. For more information about the x86
registers, consult the relevant documentation from Intel or AMD. Suffice it to
say, the CPU registers mark the spot where your program does what it does. This
is the point of program execution, where the CPU translates the bits of your
program into the actions that you want it to do - or as in the case of
exceptions, the actions that you don't want it to do. The information in this
section of the report can reveal values stored in local function variables and
that can help in determining the cause of the crash.

Once the primary values from the CONTEXT structure are reported,
GenerateExceptionReport invokes the WalkX86CallStack function that walks the
call stack and reports information about it. The call stack is a chain of stack
references that correspond with the sequence of function calls in your program
that led up to the exception. The stack is a special area of process memory that
the operating system assigns to each thread in the process. Each thread has it's
own stack separate and distinct from other threads in the process. The thread
uses it's stack to store the arguments passed to functions and more importantly
the addresses that these functions are supposed to return to after completing
operation. Functions compiled without optimization use the Ebp register to store
a reference to the position of top of the stack just after the function is
called. This "stack frame pointer" is then used to more easily reference the
return address stored on the stack for the particular function call. When
compiling with optimization, the compiler omits the instructions setting the
"stack frame pointer" for some functions. In those cases, the information
returned by the call stack walk is not as accurate as it is in cases where
optimization was not used. Nevertheless, some information is better than no
information.

The call stack section of the report provides information that can be used in
conjunction with the map file generated for the program to identify the function
that contains the faulting code. Let's examine a snippet from a call stack walk:


Address   Frame     Function  Logical Address  Module
004012E1  0012FF04  00401396  0001:000002E1    C:\PROJECTS\uetest\lcc\uetest.exe


The first column of the first item in the list contains the address where the
fault occured. For subsequent items on the list, the first column contains the
return address of the function. The return address of the function can be used
in turn to determine the actual address of the function. That is the value found
in the third column. In some cases, the actual function address cannot easily be
determined. In those situations this field will contain zeroes.

When the function address is available, it can be used to search for the name of
the function in the map file. For example, a search for the string "401396"
(leading zeroes should be omitted) in the map file for uetest.exe leads us to
the line "401396    _f4". This tells us that a function named "f4" was called
prior to the exception. If this happens to be the first item from the stack
walk, chances are very good that the bug resides inside the function "f4" and
that is where to look to fix the problem. If the function address from the first
item in the stack walk cannot be found in the map file, continue the search
using the function address from the next item on the stack walk and so on until
a match is found. Then search for the bug in the code proceeding forward from
that function. The function addresses in the stack walk may not always produce
an exact identification of the faulting function, but they definitely help to
narrow the search.

The other columns in the stack walk are not as useful with lcc-win32 map files.
The Frame column can help to identify how far into the execution flow a function
was called. The Logical Address column contains information more useful with
MSVC generated map files and is a vestige of Matt Pietrek's original code. The
Module column identifies the module containing the instruction that produced the
exception. Generally, the module name will be the same as the program name, but
in some cases the module name might be that of a system dll, such as
KERNEL32.dll or CRTDLL.DLL. That does not mean that there is a bug in those
files. More likely it means that your code provided bad information in a call to
a function in those files. Invoke the AccessViolationInK32 or
AccessViolationInCRT functions in uetest.c to experiment with this kind of
situation.

After the call stack information is printed, GenerateExceptionReport prints
information pertaining to the top and bottom of the stack for the thread that
produced the fault. The bottom of the stack is established before the thread
begins execution and remains constant for the duration of the thread, but the
top of the stack grows during the thread as more stack space is needed. The
stack grows in one page (4096 bytes) increments. This information is helpful in
determining when stack addresses are present in the raw stack data. This
information can also aid in determining how far into the execution flow the
program had reached before it faulted.

Immediately following the thread stack limits, GenerateExceptionReport prints
the actual values stored on the stack, upto 1 kilobyte worth. These values are
printed as hexadecimal DWORDs to make it easier to correlate them with other
information in the report. The various stack addresses are printed along the
left at 24 byte intervals and the contents are printed to the right at six
hexadecimal DWORDs per line (6x4=24). A close examination of the raw stack
values reveals function call return addresses, stack frame pointers and in
between those the possible values of function arguments (for arguments passed by
value) or pointer addresses (for arguments passed by reference or as pointers).
In cases where the program was compiled with optimization, this raw stack data
can be used to compenstate for the inaccuracies present in the call stack walk.
Also note that on Windows 2000 (and possibly NT and XP),  the value 7FFDF000 may
appear at different places in the stack. This is the address of the "Process
Environment Block" or PEB structure and it is usually stored in the Ebx
register. When this value is found on the stack, it indicates that a function
"pushed" Ebx in order to use it as a temporary storage location. In most cases,
for the purposes of finding your bug, these values can be ignored as possible
function arguments.

The last items printed by GenerateExceptionReport, if space remains in the print
buffer, is derived from the FloatSave member of the CONTEXT structure. This
information pertains to the floating point unit (FPU) of the CPU and includes
the floating point stack as well as the FPU tag, status and control words. The
startup code produced by lcc-win32 routes exceptions generated by the FPU to a
special trap frame that prevents them from ever reaching CatchAll. At least,
that's how it appears to me. During testing I wasn't able to catch any floating
point related exceptions using CatchAll, even though tracking faulty fpu
instructions using the Machine State dialog from wedit (invoked by the CPU
option on the Debug menu) indicated that the float exception flag was set (see
the FloatDivideByZero function in uetest.c). To overcome this during testing I
developed the FloatSaveRegisters function, which uses special assembler
instructions to push various common constants onto the the FPU register stack
and then triggers a breakpoint exception to invoke CatchAll.

Debugging CatchAll-GenerateExceptionReport and related functions

According to the documentation for RaiseException, when an exception is raised,
the exception dispatcher of the operating system goes through the following
steps:

	1. The system first attempts to notify the process's debugger, if any.

	2. If the process is not being debugged, or if the associated debugger
	does not handle the exception, the system attempts to locate a frame-
	based exception handler by searching the stack frames of the thread in
	which the exception occurred.

	3. The system searches the current stack frame first, then proceeds
	backward through preceding stack frames. If no frame-based handler can
	be found, or no frame-based handler handles the exception, the system
	makes a second attempt to notify the process's debugger.

	4. If the process is not being debugged, or if the associated debugger
	does not handle the exception, the system provides default handling
	based on the exception type. For most exceptions, the default action is
	to call the ExitProcess function.

This first response - notifying the process's debugger, if any - can make
debugging an unhandled exception filter function quite a chore. When running the
test program under the wedit debugger, the debugger would break into
purposefully thrown exceptions at the point where they were thrown rather than
halting at a breakpoint set in the filter or one of the subsequently called
functions the way I wanted it too. Initially, this meant that I had to test the
filter outside of the debugger, in the dark so to speak. I did this by compiling
the project, running the test program directly from Windows Explorer and
examining the resulting log file when one was produced, which wasn't always the
case. When an exception filter produces an exception, the operating system
responds by immediately terminating the program. This meant that when there were
bugs in the filter, no report file was generated and that meant in turn that I
had to scan through the entire complex of CatchAll-GenerateExceptionReport code
by hand searching for the problem.

This started me thinking that there had to be a better way. To cut to the chase,
it so happens that the signature of the top-level exception filter (CatchAll,
UnhandledExceptionFilter) corresponds with the filter function that can be used
in the __except clause of a __try-__except structured exception handling (seh)
frame. So, I set up an seh frame using CatchAll(GetExceptionInformation()) as
the filter and then purposefully inserted some faulty code into the try block to
trigger this local exception handler. Because the wedit debugger is crafted to
work with these seh frames, I was able to set a breakpoint in CatchAll and use
the debugger to break into the handler at any point that I wanted. It wasn't
possible to do this in wedit until a few months ago when Jacob updated the seh
facilities of lcc-win32 and implemented the GetExceptionInformaton routine
(Thanks Jacob!). Among other things, the ability to break into CatchAll enabled
me to improve the ExceptionPrint function so that it would not print to a full
print buffer.

To explain the debugging code found in CatchAll, let's start with the debugging
code found in InitializeExceptionHandler. The first thing to note is the _DEBUG
preprocessor token. This token is defined if the program is compiled with
debugging information included. The fact that debug information is included in
the file, however, is not reason enough to skip setting the error mode or
setting CatchAll to filter exceptions. We still want the crash handler to kick
in when debug information is present and the program is not being run in a
debugger. However, when the program is running in the debugger, setting the
error mode and setting CatchAll makes little sense. This is where the
IsDebuggerAttached function comes into play.

This function is available as part of the Win32 API under the name
IsDebuggerPresent. However, IsDebuggerPresent is not available on Windows 95, so
using that function safely would require checking the operating system version,
setting up a function pointer using GetProcAddress and so on - and ultimately
that's a lot of work for such a small function, so rather than go through all
that I just wrote a local declspec naked version of it. And, because the
usefulness of the debugger declines dramatically when used on programs lacking
debug information, I placed the function definition and every call to it inside
#ifdef _DEBUG preprocessor directives. What this means is that if you want to
debug CatchAll or related functions, you'll have to set the debug level to -g2 -
which is easy because it's the default.

At any rate, the IsDebuggerAttached function returns 0 when the program is not
being debugged and 1 when it is. So, when the program is being debugged,
InitializeExceptionHandler returns before setting the error mode and the
unhandled exception filter. And in the function named f4 in uetest.c, program
flow is routed to an seh frame with CatchAll(GetExceptionInformation()) as the
filter function and a call to an "access violation" producing routine in the try
block. My experiments revealed that the wedit debugger passed access violations
along to CatchAll but would not pass other kinds of exceptions to it, and
instead would break into the program at the location of the exception generating
code. In situations where the program is not being debugged, the f4 function
skips the seh frame entirely and calls one of the other exception generating
functions.

To finally return to the CatchAll function. If ueh.c is being debugged,  a
hardcoded breakpoint is invoked to halt the debugger early in the operation of
the CatchAll function. If you want to enhance the various parts of the complex
of CatchAll-GenerateExceptionReport code, you can comment out this breakpoint
and manually place your own using the F2 option in wedit. I should also note
that I have not been able to get the int3 assembler instruction to work well as
a hardcoded breakpoint on Windows 95 based machines. There are other means of
producing break points, I've listed a few possibilities near the BreakPoint
function in uetest.c, but I haven't tested them on the Windows 95 family of
operating systems. If you run into terrible problems, let me know.

As a final thought in regard to the code in this file - ueh.c. There are places
where the code could be improved. For starters, the functions DumpSystemVersion
and DumpCPUInfo were taken from examples given in the MS Platform SDK and
although they have been slightly modified, I think more work could be done with
them. In addition, reducing the number of calls to ExceptionPrint would likely
provide overall benefit to the crash handler. And of course, improving the user
friendliness of the message popup portion of the code is another possiblity.

Lastly, I should comment on the various functions in uetest.c. The functions f1,
f2, f3, f4 are spun out like that merely to generate a call stack to examine.
The each have 4 arguments to provide an idea of how arguments are passed to
function by way of the thread stack. Otherwise, these functions do nothing. The
remaining functions were purposefully designed to produce various exceptions to
allow for testing CatchAll-GenerateExceptionReport in response to a variety of
exceptions. I could just as easily have used the RaiseException API to do the
same thing, but when taking that approach the location of every exception always
turns out to be that of RaiseException and I didn't want that. So I dug around
the web and found a few examples of various bugs and then dug around the Intel
instruction set reference and came up with a few more. And these functions are
fruits of that digging. If you know of ways to generate the exceptions that
aren't included among these functions, please let me know.

Consult the comments in the code for details regarding particular issues in the
code.


