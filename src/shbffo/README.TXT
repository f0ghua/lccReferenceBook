              SHBFFO (Shell_BrowseForFolder_FileOperation)
           Win32 DLL for use with the Wise Installation System
           ===================================================

         Author: Ralf Buschmann
           Date: 20 October 2001
        Contact: ralf@backmagic.de
Compiler System: Lcc-Win32 (http://www.cs.virginia.edu/~lcc-win32/)
      Platforms: Windows 95/98/ME/NT/2000/XP

I wrote this code to provide a better "directory picker" dialog for the
Wise Installation System that supports browsing the network neighborhood
and other special "shell name spaces" and, last but not least, also
looks a little more appealing than the one provided in Wise. I also
wanted to have an alternate way to copy local files with a working
progress bar in a Wise script and needed the ability to copy files
locally in a Custom Dialog script.

To accomplish this, the SHBFFO.DLL exports two functions:

BrowseForFolder: Front end to the Win32 SHBrowseForFolder() API for
                 picking a directory with various enhancements.
                 Specifically, this function permits to add a "New
                 Folder" button for creating a new folder to the dialog
                 and can make the dialog resizable. Note that these
                 features are custom implementations and are available
                 on all versions of Windows (SHBrowseForFolder natively
                 supports this only with Shell32.dll v5.0 in Windows
                 ME/2000/XP)

  FileOperation: Front end to the Win32 SHFileOperation() API (copying,
                 moving and deleting files with progress indicator).


Notes to Wise users:
====================
The 'Wise' directory contains scripts that demonstrate how to use the
DLL in a Wise script. For more information, refer to readme.txt and see comments in the scripts. The DLL can be used with the Wise Installation System versions 6 (Enterprise version only) 7, 8 and 9. Wise for Windows Installer: untested.

Notes to Lcc-Win32 users:
=========================
For information on parameters lists see shbffo.h. To use the DLL in your
own projects, just include shbffo.h and link with shbffo.lib. For an
example, see the project in the 'Testdll' directory.

Special notes:

Be careful when editing the resource file (res\resource.rc) in Weditres.
I have made the following manual modifications to the resource file:

 - added conditional #define for DS_SHELLFONT
 - changed "IDD_MAINDIALOG DIALOG" to "IDD_MAINDIALOG DIALOGEX"
 - added DS_SHELLFONT to dialog style
 - changed font name to "MS Shell Dlg"

   These modification will be removed by Weditres, make sure to put them
   back in after exiting Weditres. This ensures that at runtime, the
   dialog uses the default system font for dialog boxes (currently,
   dialogs created in Weditres use the specified font and cannot be told
   to use a "System Default" font).

 - added #include "version.rc" to have version information compiled into
   the DLL.

   This also will be removed by Weditres. Make sure to put back in.

   A backup of the original resource files/headers exists in the
   'res\backup' directory.
   
 - Whenever Wedit rebuilds the make file, you will have to fix it.
   Wedit does not correctly handle the resource file being in the 'res'
   subdirectory, you will have to insert $(SRCDIR)\ in front of every
   occurrence of 'res\resource.rc' in the makefile.


Files
=====
|
+----.\.        - the source and project files for the SHBffo project
|
+----.\Lcc      - compiler/linker output directory for the SHBffo
|                 project. Contains the DLL.
|
+----.\Res      - resource files for the SHBffo project
|
+----.\TestDll  - Lcc project (console mode) demonstrating how to use
|                 the DLL (also used to debug the DLL)
|
+----.\Wise     - scripts for the Wise installation system demonstrating
                  how to use the DLL in Wise scripts.


History
=======
16.12. 1999 - First release

13.09. 2000 - Added ability to set an arbitrary file system path as the
              root of the browse dialog box. Added support for
              SHGetFolderLocation in WinME/2K. 

20.10. 2001 - Complete rework. Added ability to add a "New Folder"
              button, make dialog resizable and include an edit box
              where users can type in a path. Various other
              enhancements.


LICENSE
=======
Permission is hereby granted to use, modify and distribute this software
for any and all purposes in both source and binary form. I only ask that
you do not remove my name as the original author of this work.

There is no charge for the original software.

NOTE: Author takes no responsibility for any damage or losses resulting
from the use of this software.
