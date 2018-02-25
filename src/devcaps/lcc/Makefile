# Wedit Makefile for project Devcaps
SRCDIR=e:\lccweb\winprog\devcaps
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	devcaps.obj

LIBS= tcconio.lib

devcaps.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\devcaps.exe $(OBJS) $(LIBS)

# Build DEVCAPS.C
DEVCAPS_C=\

devcaps.obj: $(DEVCAPS_C) $(SRCDIR)\devcaps.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\devcaps.c

link:
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\devcaps.exe $(OBJS) $(LIBS)

