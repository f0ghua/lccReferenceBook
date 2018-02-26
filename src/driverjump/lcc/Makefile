# Wedit Makefile for project DJ
SRCDIR=d:\lcc\projects\drvjmp
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	dj.res \
	dj.obj

LIBS=shell32.lib

dj.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\dj.exe $(OBJS) $(LIBS)

# Build dj.res
DJ_RC=\
	$(SRCDIR)\dj.h\

dj.res:	$(DJ_RC) d:\lcc\projects\drvjmp\dj.rc
	lrc -Id:\lcc\projects\drvjmp -Id:\lcc\include  d:\lcc\projects\drvjmp\dj.rc

# Build DJ.C
DJ_C=\
	$(SRCDIR)\dj.h\

dj.obj: $(DJ_C) $(SRCDIR)\dj.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\dj.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\dj.exe $(OBJS) $(LIBS)

