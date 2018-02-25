# Wedit Makefile for project moon
SRCDIR=d:\lcc\projects\moon
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	stringx.obj \
	moon.res \
	moon.obj

LIBS=shell32.lib weditres.lib

moon.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\moon.exe $(OBJS) $(LIBS)

# Build STRINGX.C
STRINGX_C=\

stringx.obj: $(STRINGX_C) $(SRCDIR)\stringx.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\stringx.c

# Build moon.res
MOON_RC=\
	$(SRCDIR)\moon.h\

moon.res:	$(MOON_RC) d:\lcc\projects\moon\moon.rc
	lrc -Id:\lcc\projects\moon -Id:\lcc\include  d:\lcc\projects\moon\moon.rc

# Build MOON.C
MOON_C=\
	$(SRCDIR)\stringx.h\

moon.obj: $(MOON_C) $(SRCDIR)\moon.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\moon.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\moon.exe $(OBJS) $(LIBS)

