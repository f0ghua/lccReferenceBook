# Wedit Makefile for project crossbox
SRCDIR=d:\lcc\projects\a_projects2\crossbox
CFLAGS=-Id:\lcc\include 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	wintest.res \
	wintest.obj \
	crossbox.obj

LIBS=

crossbox.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\crossbox.exe $(OBJS) $(LIBS)

# Build wintest.res
WINTEST_RC=\

wintest.res:	$(WINTEST_RC) d:\lcc\projects\a_projects2\crossbox\wintest.rc
	$(LCCROOT)\bin\lrc -Id:\lcc\projects\a_projects2\crossbox -Id:\lcc\include  d:\lcc\projects\a_projects2\crossbox\wintest.rc

# Build WINTEST.C
WINTEST_C=\
	$(SRCDIR)\crossbox.h\

wintest.obj: $(WINTEST_C) $(SRCDIR)\wintest.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\wintest.c

# Build CROSSBOX.C
CROSSBOX_C=\
	$(SRCDIR)\crossbox.h\

crossbox.obj: $(CROSSBOX_C) $(SRCDIR)\crossbox.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\crossbox.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\crossbox.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) d:\lcc\projects\a_projects2\crossbox\lcc\crossbox.exe
