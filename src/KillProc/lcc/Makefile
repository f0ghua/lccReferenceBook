# Wedit Makefile for project KillProc
SRCDIR=d:\lcc\projects\killprocess
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	killproc.res \
	killproc.obj \
	common.obj

LIBS=

killproc.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\killproc.exe $(OBJS) $(LIBS)

# Build killproc.res
KILLPROC_RC=\
	$(SRCDIR)\killprocres.h\

killproc.res:	$(KILLPROC_RC) d:\lcc\projects\killprocess\killproc.rc
	lrc -Id:\lcc\projects\killprocess -Id:\lcc\include  d:\lcc\projects\killprocess\killproc.rc

# Build KILLPROC.C
KILLPROC_C=\
	$(SRCDIR)\killprocres.h\
	$(SRCDIR)\common.h\

killproc.obj: $(KILLPROC_C) $(SRCDIR)\killproc.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\killproc.c

# Build COMMON.C
COMMON_C=\
	$(SRCDIR)\common.h\

common.obj: $(COMMON_C) $(SRCDIR)\common.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\killproc.exe $(OBJS) $(LIBS)

