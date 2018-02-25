# Wedit Makefile for project rand
SRCDIR=d:\lcc\projects\rand\rand
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	mt19937.obj

LIBS=

rand.lib:	$(OBJS) Makefile
	d:\lcc\bin\lcclib.exe /OUT:d:\lcc\projects\rand\rand\lcc\rand.lib $(OBJS)

# Build MT19937.C
MT19937_C=\

mt19937.obj: $(MT19937_C) $(SRCDIR)\mt19937.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\mt19937.c

link:
	d:\lcc\bin\lcclib.exe /OUT:d:\lcc\projects\rand\rand\lcc\rand.lib $(OBJS)

