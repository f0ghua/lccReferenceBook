# Wedit Makefile for project SimpleDlg
SRCDIR=e:\lccweb\winprog\dialog\simple
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	simple.res \
	simple.obj

LIBS=

simpledlg.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\simpledlg.exe $(OBJS) $(LIBS)

# Build simple.res
SIMPLE_RC=\
	d:\lcc\projects\dialogonly\simple\simpleres.h\

simple.res:	$(SIMPLE_RC) e:\lccweb\winprog\dialog\simple\simple.rc
	lrc -Ie:\lccweb\winprog\dialog\simple -Id:\lcc\include  e:\lccweb\winprog\dialog\simple\simple.rc

# Build SIMPLE.C
SIMPLE_C=\
	$(SRCDIR)\simpleres.h\

simple.obj: $(SIMPLE_C) $(SRCDIR)\simple.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\simple.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\simpledlg.exe $(OBJS) $(LIBS)

