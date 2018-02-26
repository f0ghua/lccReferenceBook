# Wedit Makefile for project OwnBut
SRCDIR=d:\lcc\projects\ownbut
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	ownbut.res \
	ownbut.obj

LIBS=

ownbut.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\ownbut.exe $(OBJS) $(LIBS)

# Build ownbut.res
OWNBUT_RC=\

ownbut.res:	$(OWNBUT_RC) d:\lcc\projects\ownbut\ownbut.rc
	lrc -Id:\lcc\projects\ownbut -Id:\lcc\include  d:\lcc\projects\ownbut\ownbut.rc

# Build OWNBUT.C
OWNBUT_C=\

ownbut.obj: $(OWNBUT_C) $(SRCDIR)\ownbut.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\ownbut.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\ownbut.exe $(OBJS) $(LIBS)

