# Wedit Makefile for project testdll
SRCDIR=d:\quelltexte\lcc\shbffo\testdll
CFLAGS=-Ic:\programme\lcc\include  -g2 
CC=c:\programme\lcc\bin\lcc.exe
LINKER=c:\programme\lcc\bin\lcclnk.exe
OBJS=\
	testdll.obj

LIBS=shbffo.lib tcconio.lib

testdll.exe:	$(OBJS) Makefile
	$(LINKER)  -subsystem console -o $(SRCDIR)\lcc\testdll.exe $(OBJS) $(LIBS)

# Build testdll.c
TESTDLL_C=\
	$(SRCDIR)\..\shbffo.h\

testdll.obj: $(TESTDLL_C) $(SRCDIR)\testdll.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\testdll.c

link:
	$(LINKER)  -subsystem console -o $(SRCDIR)\lcc\testdll.exe $(OBJS) $(LIBS)

