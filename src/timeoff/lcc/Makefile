# Wedit Makefile for project TimeOff
SRCDIR=c:\win98se\desktop\timeoff
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	timeoff.res \
	timeoff.obj \

LIBS=SHELL32.LIB

timeoff.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\timeoff.exe $(OBJS) $(LIBS)

# Build timeoff.res
TIMEOFF_RC=\
	$(SRCDIR)\resource.h\

timeoff.res:	$(TIMEOFF_RC) c:\win98se\desktop\timeoff\timeoff.rc
	lrc -Ic:\win98se\desktop\timeoff -Id:\lcc\include  c:\win98se\desktop\timeoff\timeoff.rc

# Build TIMEOFF.C
TIMEOFF_C=\
	$(SRCDIR)\resource.h\

timeoff.obj: $(TIMEOFF_C) $(SRCDIR)\timeoff.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\timeoff.c

# Build SHELL32.LIB
SHELL32_LIB=\

shell32.obj: $(SHELL32_LIB) $(SRCDIR)\shell32.lib
	$(CC) -c $(CFLAGS) $(SRCDIR)\shell32.lib

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\timeoff.exe $(OBJS) $(LIBS)

