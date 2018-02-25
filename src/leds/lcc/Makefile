# Wedit Makefile for project tstledlib
SRCDIR=e:\lccweb\libs\leds
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	tstledlib.res \
	tstledlib.obj \

LIBS=e:\libs\leds\ledlib.lib

tstledlib.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\tstledlib.exe $(OBJS) $(LIBS)

# Build tstledlib.res
TSTLEDLIB_RC=\
	$(SRCDIR)\tstledlibres.h\

tstledlib.res:	$(TSTLEDLIB_RC) e:\lccweb\libs\leds\tstledlib.rc
	lrc -Ie:\lccweb\libs\leds -Id:\lcc\include  e:\lccweb\libs\leds\tstledlib.rc

# Build TSTLEDLIB.C
TSTLEDLIB_C=\
	$(SRCDIR)\tstledlibres.h\
	$(SRCDIR)\ledlib.h\

tstledlib.obj: $(TSTLEDLIB_C) $(SRCDIR)\tstledlib.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\tstledlib.c

# Build LEDLIB.LIB
LEDLIB_LIB=\

ledlib.obj: $(LEDLIB_LIB) $(SRCDIR)\ledlib.lib
	$(CC) -c $(CFLAGS) $(SRCDIR)\ledlib.lib

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\tstledlib.exe $(OBJS) $(LIBS)

