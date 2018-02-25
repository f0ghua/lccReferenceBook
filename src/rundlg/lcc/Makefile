# Wedit Makefile for project rundlg
SRCDIR=d:\lcc\projects\rundlg
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	rundlg.res \
	rundlg.obj

LIBS=

rundlg.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\rundlg.exe $(OBJS) $(LIBS)

# Build rundlg.res
RUNDLG_RC=\
	$(SRCDIR)\rundlgres.h\

rundlg.res:	$(RUNDLG_RC) d:\lcc\projects\rundlg\rundlg.rc
	lrc -Id:\lcc\projects\rundlg -Id:\lcc\include  d:\lcc\projects\rundlg\rundlg.rc

# Build rundlg.c
RUNDLG_C=\
	$(SRCDIR)\rundlgres.h\

rundlg.obj: $(RUNDLG_C) $(SRCDIR)\rundlg.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\rundlg.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\rundlg.exe $(OBJS) $(LIBS)

