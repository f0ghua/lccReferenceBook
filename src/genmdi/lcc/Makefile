# Wedit Makefile for project GenMDI
SRCDIR=e:\lccweb\winprog\mdi
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	genmdi.res \
	genmdi.obj

LIBS=

genmdi.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\genmdi.exe $(OBJS) $(LIBS)

# Build genmdi.res
GENMDI_RC=\
	$(SRCDIR)\GenMDI.h\

genmdi.res:	$(GENMDI_RC) e:\lccweb\winprog\mdi\genmdi.rc
	lrc -Ie:\lccweb\winprog\mdi -Id:\lcc\include  e:\lccweb\winprog\mdi\genmdi.rc

# Build GENMDI.C
GENMDI_C=\
	$(SRCDIR)\GenMDI.h\

genmdi.obj: $(GENMDI_C) $(SRCDIR)\genmdi.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\genmdi.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\genmdi.exe $(OBJS) $(LIBS)

