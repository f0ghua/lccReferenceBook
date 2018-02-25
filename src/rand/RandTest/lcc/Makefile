# Wedit Makefile for project RandTest
SRCDIR=d:\lcc\projects\rand\rand\randtest
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	randtest.obj

LIBS=rand.lib

randtest.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\randtest.exe $(OBJS) $(LIBS)

# Build RANDTEST.C
RANDTEST_C=\

randtest.obj: $(RANDTEST_C) $(SRCDIR)\randtest.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\randtest.c

link:
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\randtest.exe $(OBJS) $(LIBS)

