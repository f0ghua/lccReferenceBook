# Wedit Makefile for project Magnetic
SRCDIR=d:\lcc\projects\magnetic
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	magnet.obj

LIBS=

magnetic.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\magnetic.exe $(OBJS) $(LIBS)

# Build MAGNET.C
MAGNET_C=\

magnet.obj: $(MAGNET_C) $(SRCDIR)\magnet.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\magnet.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\magnetic.exe $(OBJS) $(LIBS)

