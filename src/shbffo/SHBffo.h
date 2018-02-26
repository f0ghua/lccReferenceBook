//Header file defining functions exported from shbffo.dll and supported flags
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>

//Current LCC-Win32 headers don't define some of the following constants/functions
#ifndef BIF_EDITBOX
#define BIF_EDITBOX 0x0010
#endif
#ifndef BIF_VALIDATE
#define BIF_VALIDATE 0x0020
#endif
#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE 0x0040
#endif
#ifndef BIF_USENEWUI
#define BIF_USENEWUI (BIF_NEWDIALOGSTYLE | BIF_EDITBOX)
#endif
#ifndef BIF_BROWSEINCLUDEURLS
#define BIF_BROWSEINCLUDEURLS 0x0080
#endif
#ifndef BIF_UAHINT
#define BIF_UAHINT 0x0100
#endif
#ifndef BIF_NONEWFOLDERBUTTON
#define BIF_NONEWFOLDERBUTTON 0x0200
#endif
#ifndef BIF_BROWSEINCLUDEFILES
#define BIF_BROWSEINCLUDEFILES 0x4000
#endif
#ifndef BIF_SHAREABLE
#define BIF_SHAREABLE 0x8000
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SHGetDataFromIDList

#define SHGDFIL_FINDDATA        1
#define SHGDFIL_NETRESOURCE     2
#define SHGDFIL_DESCRIPTIONID   3

#define SHDID_ROOT_REGITEM          1
#define SHDID_FS_FILE               2
#define SHDID_FS_DIRECTORY          3
#define SHDID_FS_OTHER              4
#define SHDID_COMPUTER_DRIVE35      5
#define SHDID_COMPUTER_DRIVE525     6
#define SHDID_COMPUTER_REMOVABLE    7
#define SHDID_COMPUTER_FIXED        8
#define SHDID_COMPUTER_NETDRIVE     9
#define SHDID_COMPUTER_CDROM        10
#define SHDID_COMPUTER_RAMDISK      11
#define SHDID_COMPUTER_OTHER        12
#define SHDID_NET_DOMAIN            13
#define SHDID_NET_SERVER            14
#define SHDID_NET_SHARE             15
#define SHDID_NET_RESTOFNET         16
#define SHDID_NET_OTHER             17

typedef struct _SHDESCRIPTIONID {
    DWORD   dwDescriptionId;
    CLSID   clsid;
} SHDESCRIPTIONID, *LPSHDESCRIPTIONID;

STDAPI SHGetDataFromIDListA(IShellFolder *psf, LPCITEMIDLIST pidl, int nFormat, void *pv, int cb);
STDAPI SHGetDataFromIDListW(IShellFolder *psf, LPCITEMIDLIST pidl, int nFormat, void *pv, int cb);

#ifdef UNICODE
#define SHGetDataFromIDList  SHGetDataFromIDListW
#else
#define SHGetDataFromIDList  SHGetDataFromIDListA
#endif
#endif
////////////////////////////////////////////////////////////////////////////////////////////////

//Defines for various UI aspects of our customized browse dialog
#define BFFUI_NEWFOLDER			1
#define BFFUI_EDITBOX			2
#define BFFUI_EDITBOXREADONLY	4
#define BFFUI_DONTEXPAND		8
#define BFFUI_DONTAPPEND		16
#define BFFUI_RESIZABLE			32

//Defines for type of selected object returned from BrowseForFolder
typedef enum _BFF_OBJECTTYPE {
	BFF_OBJECTTYPE_UNKNOWN,
	BFF_OBJECTTYPE_DIRECTORY,
	BFF_OBJECTTYPE_FILE,
	BFF_OBJECTTYPE_COMPUTER,
	BFF_OBJECTTYPE_DOMAIN,
	BFF_OBJECTTYPE_SHARE,
	BFF_OBJECTTYPE_NETGENERIC,
} BFF_OBJECTTYPE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     BrowseForFolder

 Purpose:       Displays a modal dialog box to browse for any Windows shell
				object. The dialog is actually the one provided by
			    SHBrowseForFolder(), but with several enhancements.

 Parameters:

				hWnd:			Parent window handle.

 				hIcon			Handle to an icon to be displayed in the dialogs caption.
								If this parameter is NULL, a default icon is displayed.

				ulFlags:		Combination of flags specifying options for the dialog
								box. See documentation of the ulFlags member of the
								BROWSEINFO structure in the Win32 SDK reference.
								Note that certain flags that are incompatible with our
								own customizations specified in ulUIFalgs, below,
								(such as BIF_EDITBOX or BIF_USENEWUI) will have no effect.

				ulUIFlags:		Combination of flags that control the appearance of
								the dialog. This can be zero, one or a combination of the
								following flags:

								BFFUI_NEWFOLDER 		Include "New Folder" button

								BFFUI_EDITBOX 			Include an edit box where the user can enter a path

								BFFUI_EDITBOXREADONLY	Make the edit box read-only (just displays the selected path)
														This flag can only be used with BFFUI_EDITBOX

								BFFUI_DONTEXPAND		Don't expand the directory tree to the path passed in lpszFolder
														when the dialog initializes

								BFFUI_DONTAPPEND		Don't append the trailing directory level of the path that was
														passed in lpszFolder to the directory selected in the dialog.
														This flag should always be specified unless you are browsing for
														a file system path in a "software installation" scenario.

								BFFUI_RESIZABLE			Make the dialog resizable

								Note: if BFFUI_EDITBOX is specified and BFFUI_EDITBOXREADONLY
								is not specified, the path returned in lpszFolder will be the
							    one specified in the edit box and will not be validated.

 				lpszTitle		Pointer to a NULL terminated string specifying
								the title of the dialog box. If this parameter
								is NULL or points to an empty string, the dialog uses
								a default title.

				lpszMsg:		Pointer to a NULL terminated string to display
								near the top of the dialog. Can be NULL.

				lpszButtonText	Button caption for the "New Folder" button. This parameter
								is ignored unless BFFUI_NEWFOLDER is specified in ulUIFlags.
								The function will fail with BFFRET_INVALIDPARAM when BFFUI_NEWFOLDER
								is specified in ulUIFlags and this parameter is NULL or a pointer
								to an empty string.

				nCSIDL:			CSIDL of a special shell folder that is used as
								the root of the browse dialog box. See CSIDL_* definitions
								in shlobj.h.

				lpszRoot:		Pointer to a NULL terminated string that contains
								a file system path that is used as the root of the
							    browse dialog box. This parameter is ignored if nCSIDL
							    is not zero.

				lpszFolder:		Pointer to a character buffer that is assumed to be
 								(MAX_PATH + 4)*2  bytes long. On input, the character buffer
								may contain a file system path used to initialize the
								directory tree in the dialog. If BFFUI_DONTEXPAND is not
							    specified in ulUIFlags, this path will be pre-selected
								in the directory tree when the dialog initializes.

								On output, the function will store the path selected in the
							    dialog in this buffer. If BFFUI_EDITBOX is specified and
							    BFFUI_EDITBOXREADONLY is not specified in ulUIFlags, this
							    will be the contents of the edit box, regardless of what is
							    actually selected in the directory tree.

								If the user clicks the Cancel button or an error occurs,
								the buffer remains unchanged. If the user selected an object
							    that is not part of the file system, this will be the display
							    name of the shell object.

				dwType			Pointer to a variable that, on return, receives a value describing
								the type of the path returned in lpszFolder. Set to NULL if you
								don't need this information. The value will be one of the following:

								BFF_OBJECTTYPE_UNKNOWN		- the type of the path is unknown
								BFF_OBJECTTYPE_DIRECTORY	- the path is a directory
								BFF_OBJECTTYPE_FILE			- the path is a file
								BFF_OBJECTTYPE_COMPUTER		- the path is a computer name
								BFF_OBJECTTYPE_DOMAIN		- the path is the name of a network domain/workgroup
								BFF_OBJECTTYPE_SHARE		- the path is a network share
								BFF_OBJECTTYPE_NETGENERIC	- the path is a generic network resource



 Return values:					BFFRET_OK			- the user clicked the OK button and the path
 													  returned in lpszFolder exists as a file system path
								BFFRET_CANCEL		- the user clicked the Cancel button

								The following indicate an fatal error:

								BFFRET_COMINITFAIL	- initialization of COM libraries failed
								BFFRET_IMALLOCFAIL	- unable to obtain the Shell's IMalloc interface
								BFFRET_ISHELLFAIL	- unable to obtain the desktop folders IShellFolder interface
								BFFRET_PATHTOOLONG	- path passed in lpszFolder is too long
							    					 (MAX_PATH characters max.)
								BFFRET_INVALIDPARAM	- lpszFolder is NULL or BFFUI_NEWFOLDER was specified
													  and lpszButtonText is NULL or points to an empty string
*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Error code defines
typedef enum _BFFRET {
	BFFRET_OK,
	BFFRET_CANCEL,
	BFFRET_COMINITFAIL,
	BFFRET_IMALLOCFAIL,
	BFFRET_ISHELLFAIL,
	BFFRET_PATHTOOLONG,
	BFFRET_INVALIDPARAM
} BFFRET;

DWORD WINAPI BrowseForFolder (HWND hWnd,
							  HICON hIcon,
  							  UINT ulFlags,
							  UINT ulUIFlags,
							  char *lpszTitle,
							  char *lpszMsg,
							  char *lpszButtonText,
							  int nCSIDL,
							  char *lpszRoot,
							  char *lpszFolder,
							  DWORD *dwType);

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*
 Procedure:     FileOperation

 Purpose:       Provides access to the Win32 "Explorer like" file operations.
				Copies, moves and deletes files. Tuned for use with the Wise
				Installation System because multiple source/destination files
				are passed as a CR/LF separated list of files.

				For more information, see documentation on SHFileOperation
				in the Platform SDK.

 Parameters:

				hWnd:		Parent window handle.

				lpszFiles:	Pointer to a NULL terminated string holding one
							or more files to be operated on. Multiple files
							must be passed as a CR/LF separated list from
							Wise. Wildcards are supported.

				lpszDest:	Pointer to a NULL terminated string holding
							a destination directory or multiple destination
							files for the file operation. Multiple destination
							files must be passed as a CR/LF separated list.
						    In the case of multiple destination files,
							the FOF_MULTIDESTFILES flags in the fFlags member
							must also be set and the number of destination files
							must be equal to the number of source files.

				wFunc:		File operation to perform. Can be FO_COPY (copy),
							FO_DELETE (delete) or FO_MOVE (move).

				fFlags:		Combination of flags that control the file operation.
							See FOF_* flags in shellapi.h.


 Return values:
 							FORET_SUCCESS	- Success
							FORET_SYSABORT	- The system aborted the operation
							FORET_USERABORT	- The user aborted the operation

							The following indicate an fatal error

							FORET_INVALID	- The source and destination file lists are not consistent/invalid
							FORET_NOMEMORY	- Out of memory
*/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Error code defines
typedef enum _FORET {
	FORET_SUCCESS,
	FORET_SYSABORT,
	FORET_USERABORT,
	FORET_INVALID,
	FORET_NOMEMORY
} FORET;

DWORD WINAPI FileOperation(HWND hWnd,
						   char *lpszFiles,
						   char *lpszDest,
						   UINT ulFunc,
						   WORD wFlags);

