SmallEdit2 UNICODE Version 2.0 (J.Laporte 31/10/2002)

Changes made on the original.

1) UNICODE and _UNICODE compiler flags must be set to expend generic wide char functions

2) generic TCHAR instead of char

3) generic pointers to string * LPSTR

4)include <tchar.h>

5) TEXT("macro") used to expand string constants

6) malloc revisited
e.g.
(PTSTR) malloc(FileSize+1 * sizeof (TCHAR));

7) generic MaxLength sprintf used
_sntprintf(Buffer, MAX_NAME, TEXT("The text in %s has changed.\nWould you like to save it?"), Info.FileName);
instead of snprintf

8) A atoi needed in function DlgLineProc has been replaced by a
LineNum = _ttoi((TCHAR *)Buffer);

(function wtoi is lacking in compiler's library) 

9) The new Save function inserts a Unicode Utf-16 (little endian) file signature 

WORD   wByteOrderMark = 0xFEFF ;

as a header to the file.

10) The new Read function checks for Unicode Utf-16 (little endian) format (the only format 
supported in this version).
The read logic has been completly re-written.

11) File filters are limited to (*.ini) in this version.

12) Functions not needed in "Unicode Editor" have been deleted.
Entry points remain.
Putting them back is a matter of minutes.

13) Known issue :
The "bidi" algorythm for RTF and LTR mixing still needs enhancements, to avoid "dancing".
When loading Arabic or Hebrew, "Select All" and select the language on the system tray.
It needs certainly to be automatic.

 	