# Wedit Makefile for project SmallEdit2
SRCDIR=d:\lcc\projects\a_projects2\smalledit2
CFLAGS=-ID:\lcc\include 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	tools.obj \
	se.res \
	main.obj \
	load.obj \
	dialog.obj

LIBS=
EXE=smalledit2.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\smalledit2.exe $(OBJS) $(LIBS)

# Build TOOLS.C
TOOLS_C=\
	$(SRCDIR)\global.h\

tools.obj: $(TOOLS_C) $(SRCDIR)\tools.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\tools.c

# Build se.res
SE_RC=\
	$(SRCDIR)\resource.h\

se.res:	$(SE_RC) d:\lcc\projects\a_projects2\smalledit2\se.rc
	$(LCCROOT)\bin\lrc -Id:\lcc\projects\a_projects2\smalledit2 -ID:\lcc\include  d:\lcc\projects\a_projects2\smalledit2\se.rc

# Build MAIN.C
MAIN_C=\
	$(SRCDIR)\Resource.h\
	$(SRCDIR)\Main.h\
	$(SRCDIR)\Global.h\
	$(SRCDIR)\Menu.h\

main.obj: $(MAIN_C) $(SRCDIR)\main.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\main.c

# Build LOAD.C
LOAD_C=\
	$(SRCDIR)\Global.h\

load.obj: $(LOAD_C) $(SRCDIR)\load.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\load.c

# Build DIALOG.C
DIALOG_C=\
	$(SRCDIR)\Global.h\
	$(SRCDIR)\Dialog.h\
	$(SRCDIR)\Resource.h\

dialog.obj: $(DIALOG_C) $(SRCDIR)\dialog.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\dialog.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\smalledit2.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) d:\lcc\projects\a_projects2\smalledit2\lcc\smalledit2.exe
