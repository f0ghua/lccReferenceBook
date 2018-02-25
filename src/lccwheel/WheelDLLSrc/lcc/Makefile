# Wedit Makefile for project wheeldll
SRCDIR=c:\win98se\desktop\lccwheel\wheeldllsrc
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	wheeldll.obj

LIBS=

wheeldll.dll:	$(OBJS) Makefile
	$(LINKER)  -s -dll -nounderscores -o $(SRCDIR)\lcc\wheeldll.dll $(OBJS) $(LIBS)

# Build WHEELDLL.C
WHEELDLL_C=\

wheeldll.obj: $(WHEELDLL_C) $(SRCDIR)\wheeldll.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\wheeldll.c

link:
	$(LINKER)  -s -dll -nounderscores -o $(SRCDIR)\lcc\wheeldll.dll $(OBJS) $(LIBS)

