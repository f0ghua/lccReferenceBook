# Wedit Makefile for project SHBffo
SRCDIR=d:\quelltexte\lcc\shbffo
CFLAGS=-IC:\Programme\lcc\include 
CC=c:\programme\lcc\bin\lcc.exe
LINKER=c:\programme\lcc\bin\lcclnk.exe
OBJS=\
	resource.res \
	shbffo.obj \

LIBS=ole32.lib shell32.lib uuid.lib

shbffo.dll:	$(OBJS) Makefile
	$(LINKER)  -s -dll D:\Quelltexte\LCC\SHBffo\SHBffo.def -nounderscores -o $(SRCDIR)\lcc\shbffo.dll $(OBJS) $(LIBS)

# Build OLE32.LIB
OLE32_LIB=\

ole32.obj: $(OLE32_LIB) $(SRCDIR)\ole32.lib
	$(CC) -c $(CFLAGS) $(SRCDIR)\ole32.lib

# Build resource.res
RESOURCE_RC=\
	C:\Programme\lcc\include\windows.h\
	C:\Programme\lcc\include\stdlib.h\
	C:\Programme\lcc\include\stddef.h\
	C:\Programme\lcc\include\_syslist.h\
	C:\Programme\lcc\include\string.h\
	C:\Programme\lcc\include\win.h\
	C:\Programme\lcc\include\limits.h\
	C:\Programme\lcc\include\stdarg.h\
	C:\Programme\lcc\include\ctype.h\
	C:\Programme\lcc\include\basetsd.h\
	$(SRCDIR)\res\resource.h\
	$(SRCDIR)\res\version.rc\
	C:\Programme\lcc\include\windows.h\
	C:\Programme\lcc\include\winver.h\
	$(SRCDIR)\res\version.h\

resource.res:	$(RESOURCE_RC) $(SRCDIR)\res\resource.rc
	lrc -Id:\quelltexte\lcc\shbffo -IC:\Programme\lcc\include  $(SRCDIR)\res\resource.rc

# Build SHBffo.c
SHBFFO_C=\
	C:\Programme\lcc\include\windows.h\
	C:\Programme\lcc\include\stdlib.h\
	C:\Programme\lcc\include\stddef.h\
	C:\Programme\lcc\include\_syslist.h\
	C:\Programme\lcc\include\string.h\
	C:\Programme\lcc\include\win.h\
	C:\Programme\lcc\include\limits.h\
	C:\Programme\lcc\include\stdarg.h\
	C:\Programme\lcc\include\ctype.h\
	C:\Programme\lcc\include\basetsd.h\
	C:\Programme\lcc\include\shlobj.h\
	C:\Programme\lcc\include\ole2.h\
	C:\Programme\lcc\include\objbase.h\
	C:\Programme\lcc\include\rpc.h\
	C:\Programme\lcc\include\basetsd.h\
	C:\Programme\lcc\include\rpcdce.h\
	C:\Programme\lcc\include\rpcdcep.h\
	C:\Programme\lcc\include\rpcnsi.h\
	C:\Programme\lcc\include\rpcnterr.h\
	C:\Programme\lcc\include\excpt.h\
	C:\Programme\lcc\include\winerror.h\
	C:\Programme\lcc\include\rpcasync.h\
	C:\Programme\lcc\include\rpcndr.h\
	C:\Programme\lcc\include\basetsd.h\
	C:\Programme\lcc\include\rpcnsip.h\
	C:\Programme\lcc\include\stdlib.h\
	C:\Programme\lcc\include\wtypes.h\
	C:\Programme\lcc\include\unknwn.h\
	C:\Programme\lcc\include\wtypes.h\
	C:\Programme\lcc\include\objidl.h\
	C:\Programme\lcc\include\unknwn.h\
	C:\Programme\lcc\include\cguid.h\
	C:\Programme\lcc\include\urlmon.h\
	C:\Programme\lcc\include\rpc.h\
	C:\Programme\lcc\include\rpcndr.h\
	C:\Programme\lcc\include\windows.h\
	C:\Programme\lcc\include\ole2.h\
	C:\Programme\lcc\include\unknwn.h\
	C:\Programme\lcc\include\objidl.h\
	C:\Programme\lcc\include\oleidl.h\
	C:\Programme\lcc\include\rpcbase.h\
	C:\Programme\lcc\include\servprov.h\
	C:\Programme\lcc\include\ole2.h\
	C:\Programme\lcc\include\msxml.h\
	C:\Programme\lcc\include\oaidl.h\
	C:\Programme\lcc\include\propidl.h\
	C:\Programme\lcc\include\oaidl.h\
	C:\Programme\lcc\include\oleauto.h\
	C:\Programme\lcc\include\prsht.h\
	C:\Programme\lcc\include\commctrl.h\
	C:\Programme\lcc\include\windows.h\
	C:\Programme\lcc\include\shlguid.h\
	C:\Programme\lcc\include\shellapi.h\
	C:\Programme\lcc\include\string.h\
	C:\Programme\lcc\include\stdio.h\
	C:\Programme\lcc\include\_syslist.h\
	$(SRCDIR)\shbffo.h\
	C:\Programme\lcc\include\windows.h\
	C:\Programme\lcc\include\shlobj.h\
	C:\Programme\lcc\include\shellapi.h\
	$(SRCDIR)\res\resource.h\

shbffo.obj: $(SHBFFO_C) $(SRCDIR)\shbffo.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\shbffo.c

# Build SHELL32.LIB
SHELL32_LIB=\

shell32.obj: $(SHELL32_LIB) $(SRCDIR)\shell32.lib
	$(CC) -c $(CFLAGS) $(SRCDIR)\shell32.lib

link:
	$(LINKER)  -s -dll D:\Quelltexte\LCC\SHBffo\SHBffo.def -nounderscores -o $(SRCDIR)\lcc\shbffo.dll $(OBJS) $(LIBS)

