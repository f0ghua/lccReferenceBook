# Wedit Makefile for project MemStat
SRCDIR=e:\lccweb\winprog\memstat
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	memstat.obj

LIBS=

memstat.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\memstat.exe $(OBJS) $(LIBS)

# Build MEMSTAT.C
MEMSTAT_C=\

memstat.obj: $(MEMSTAT_C) $(SRCDIR)\memstat.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\memstat.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\memstat.exe $(OBJS) $(LIBS)

