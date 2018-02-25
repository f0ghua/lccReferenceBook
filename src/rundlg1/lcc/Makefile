# Wedit Makefile for project RunDlg1
SRCDIR=d:\lcc\projects\rundlg1
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	rundlg1.res \
	rundlg1.obj \
	msgbox.obj

LIBS=

rundlg1.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\rundlg1.exe $(OBJS) $(LIBS)

# Build rundlg1.res
RUNDLG1_RC=\

rundlg1.res:	$(RUNDLG1_RC) d:\lcc\projects\rundlg1\rundlg1.rc
	lrc -Id:\lcc\projects\rundlg1 -Id:\lcc\include  d:\lcc\projects\rundlg1\rundlg1.rc

# Build RUNDLG1.C
RUNDLG1_C=\
	$(SRCDIR)\msgbox.h\

rundlg1.obj: $(RUNDLG1_C) $(SRCDIR)\rundlg1.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\rundlg1.c

# Build MSGBOX.C
MSGBOX_C=\
	$(SRCDIR)\msgbox.h\

msgbox.obj: $(MSGBOX_C) $(SRCDIR)\msgbox.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\msgbox.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\rundlg1.exe $(OBJS) $(LIBS)

