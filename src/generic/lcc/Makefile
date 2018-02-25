# Wedit Makefile for project generic
SRCDIR=e:\lccweb\winprog\generic
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	generic.obj

LIBS=

generic.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\generic.exe $(OBJS) $(LIBS)

# Build GENERIC.C
GENERIC_C=\

generic.obj: $(GENERIC_C) $(SRCDIR)\generic.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\generic.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\generic.exe $(OBJS) $(LIBS)

