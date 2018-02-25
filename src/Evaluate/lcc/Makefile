# Wedit Makefile for project evaluate
SRCDIR=c:\lcc\projects\evaluate
CFLAGS=-IC:\lcc\include  -O  -A 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	parse.obj

LIBS=
EXE=evaluate.lib

$(EXE):	$(OBJS) Makefile
	$(LCCROOT)\bin\lcclib.exe /OUT:c:\lcc\projects\evaluate\lcc\evaluate.lib $(OBJS)

# Build parse.c
PARSE_C=\
	$(SRCDIR)\parse.h\

parse.obj: $(PARSE_C) $(SRCDIR)\parse.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\parse.c

link:
	$(LCCROOT)\bin\lcclib.exe /OUT:c:\lcc\projects\evaluate\lcc\evaluate.lib $(OBJS)

clean:
	del $(OBJS) c:\lcc\projects\evaluate\lcc\evaluate.lib
