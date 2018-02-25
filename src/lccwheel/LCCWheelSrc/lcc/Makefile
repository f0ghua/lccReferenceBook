# Wedit Makefile for project lccwheel
SRCDIR=c:\win98se\desktop\lccwheel\lccwheelsrc
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	lccwheel.res \
	lccwheel.obj

LIBS=shell32.lib

lccwheel.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\lccwheel.exe $(OBJS) $(LIBS)

# Build lccwheel.res
LCCWHEEL_RC=\
	$(SRCDIR)\resource.h\

lccwheel.res:	$(LCCWHEEL_RC) c:\win98se\desktop\lccwheel\lccwheelsrc\lccwheel.rc
	lrc -Ic:\win98se\desktop\lccwheel\lccwheelsrc -Id:\lcc\include  c:\win98se\desktop\lccwheel\lccwheelsrc\lccwheel.rc

# Build LCCWHEEL.C
LCCWHEEL_C=\
	$(SRCDIR)\resource.h\

lccwheel.obj: $(LCCWHEEL_C) $(SRCDIR)\lccwheel.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\lccwheel.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\lccwheel.exe $(OBJS) $(LIBS)

