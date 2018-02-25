# Wedit Makefile for project gclibtest
SRCDIR=d:\lcc\projects\a_projects6\gclibtest
CFLAGS=-ID:\lcc\db-new 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	gclib.obj

LIBS=gclib.lib
EXE=gclibtest.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\gclibtest.exe $(OBJS) $(LIBS)

# Build gclib.c
GCLIB_C=\

gclib.obj: $(GCLIB_C) $(SRCDIR)\gclib.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\gclib.c

link:
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\gclibtest.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) d:\lcc\projects\a_projects6\gclibtest\lcc\gclibtest.exe
