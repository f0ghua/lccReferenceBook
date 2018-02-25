# Wedit Makefile for project SmallEdit
SRCDIR=d:\lcc\projects\a_projects2\smalledit
CFLAGS=-Id:\lcc\include 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	dialog.obj \
	load.obj \
	main.obj \
	se.res \
	tools.obj

LIBS=

smalledit.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\smalledit.exe $(OBJS) $(LIBS)

# Build DIALOG.C
DIALOG_C=\
	$(SRCDIR)\Global.h\
	$(SRCDIR)\Dialog.h\
	$(SRCDIR)\Resource.h\

dialog.obj: $(DIALOG_C) $(SRCDIR)\dialog.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\dialog.c

# Build LOAD.C
LOAD_C=\
	$(SRCDIR)\Global.h\

load.obj: $(LOAD_C) $(SRCDIR)\load.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\load.c

# Build MAIN.C
MAIN_C=\
	$(SRCDIR)\Resource.h\
	$(SRCDIR)\Main.h\
	$(SRCDIR)\Global.h\
	$(SRCDIR)\Menu.h\

main.obj: $(MAIN_C) $(SRCDIR)\main.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\main.c

# Build se.res
SE_RC=\
	$(SRCDIR)\resource.h\

se.res:	$(SE_RC) d:\lcc\projects\a_projects2\smalledit\se.rc
	$(LCCROOT)\bin\lrc -Id:\lcc\projects\a_projects2\smalledit -Id:\lcc\include  d:\lcc\projects\a_projects2\smalledit\se.rc

# Build TOOLS.C
TOOLS_C=\
	$(SRCDIR)\global.h\

tools.obj: $(TOOLS_C) $(SRCDIR)\tools.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\tools.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\smalledit.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) d:\lcc\projects\a_projects2\smalledit\lcc\smalledit.exe
