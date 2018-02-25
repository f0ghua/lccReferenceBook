# Wedit Makefile for project SubClassBut
SRCDIR=d:\lcc\projects\subclassbut
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	subclassbutt.obj \
	subclassbut.res

LIBS=

subclassbut.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\subclassbut.exe $(OBJS) $(LIBS)

# Build SUBCLASSBUTT.C
SUBCLASSBUTT_C=\
	$(SRCDIR)\SubClassButton.h\

subclassbutt.obj: $(SUBCLASSBUTT_C) $(SRCDIR)\subclassbutt.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\subclassbutt.c

# Build subclassbut.res
SUBCLASSBUT_RC=\
	$(SRCDIR)\SubClassButton.h\

subclassbut.res:	$(SUBCLASSBUT_RC) d:\lcc\projects\subclassbut\subclassbut.rc
	lrc -Id:\lcc\projects\subclassbut -Id:\lcc\include  d:\lcc\projects\subclassbut\subclassbut.rc

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\subclassbut.exe $(OBJS) $(LIBS)

