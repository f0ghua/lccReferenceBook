# Wedit Makefile for project TinyPad
SRCDIR=d:\lcc\projects\a_projects1\tinypad
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	file.obj \
	misc.obj \
	options.obj \
	search.obj \
	tinypad.obj \
	tinypad.res

LIBS=SHELL32.LIB version.lib

tinypad.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\tinypad.exe $(OBJS) $(LIBS)

# Build FILE.C
FILE_C=\

file.obj: $(FILE_C) $(SRCDIR)\file.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\file.c

# Build MISC.C
MISC_C=\
	$(SRCDIR)\tinypadres.h\

misc.obj: $(MISC_C) $(SRCDIR)\misc.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\misc.c

# Build OPTIONS.C
OPTIONS_C=\
	$(SRCDIR)\tinypadres.h\

options.obj: $(OPTIONS_C) $(SRCDIR)\options.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\options.c

# Build SEARCH.C
SEARCH_C=\
	$(SRCDIR)\tinypadres.h\

search.obj: $(SEARCH_C) $(SRCDIR)\search.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\search.c

# Build TINYPAD.C
TINYPAD_C=\
	$(SRCDIR)\tinypadres.h\

tinypad.obj: $(TINYPAD_C) $(SRCDIR)\tinypad.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\tinypad.c

# Build tinypad.res
TINYPAD_RC=\
	$(SRCDIR)\tinypadres.h\

tinypad.res:	$(TINYPAD_RC) d:\lcc\projects\a_projects1\tinypad\tinypad.rc
	lrc -Id:\lcc\projects\a_projects1\tinypad -Id:\lcc\include  d:\lcc\projects\a_projects1\tinypad\tinypad.rc

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\tinypad.exe $(OBJS) $(LIBS)

