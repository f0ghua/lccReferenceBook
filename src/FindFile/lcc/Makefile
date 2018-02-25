# Wedit Makefile for project FindFile
SRCDIR=d:\lcc\findfile
CFLAGS= -O 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	dialogs.obj \
	find.obj \
	findfile.obj \
	findfile.res \
	idragdrop.obj \
	jmem.obj \
	olepicture.obj \
	registry.obj \
	shell.obj \
	utils.obj \

LIBS=ole32.lib olepro32.lib shell32.lib shlwapi.lib urlmon.lib uuid.lib wlib.lib version.lib
EXE=findfile.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -stack-reserve 20000000 -o $(SRCDIR)\lcc\findfile.exe $(OBJS) $(LIBS)

# Build dialogs.c
DIALOGS_C=\
	$(SRCDIR)\findfileres.h\
	$(SRCDIR)\dialogs.h\
	$(SRCDIR)\find.h\
	$(SRCDIR)\jmem.h\
	$(SRCDIR)\shell.h\
	$(SRCDIR)\find.h\
	$(SRCDIR)\findfile.h\
	$(SRCDIR)\findfileres.h\

dialogs.obj: $(DIALOGS_C) $(SRCDIR)\dialogs.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\dialogs.c

# Build find.c
FIND_C=\
	$(SRCDIR)\find.h\
	$(SRCDIR)\jmem.h\
	$(SRCDIR)\dialogs.h\
	$(SRCDIR)\find.h\
	$(SRCDIR)\findfileres.h\

find.obj: $(FIND_C) $(SRCDIR)\find.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\find.c

# Build findfile.c
FINDFILE_C=\
	$(SRCDIR)\find.h\
	$(SRCDIR)\jmem.h\
	$(SRCDIR)\findfileres.h\
	$(SRCDIR)\olepicture.h\
	$(SRCDIR)\registry.h\
	$(SRCDIR)\jmem.h\
	$(SRCDIR)\shell.h\
	$(SRCDIR)\find.h\
	$(SRCDIR)\dialogs.h\
	$(SRCDIR)\find.h\
	$(SRCDIR)\idragdrop.h\
	$(SRCDIR)\utils.h\
	$(SRCDIR)\findfile.h\
	$(SRCDIR)\findfileres.h\
	$(SRCDIR)\ButtonEx.h\

findfile.obj: $(FINDFILE_C) $(SRCDIR)\findfile.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\findfile.c

# Build findfile.res
FINDFILE_RC=\

findfile.res:	$(FINDFILE_RC) $(SRCDIR)\findfile.rc
	$(LCCROOT)\bin\lrc.exe -I$(SRCDIR)  $(SRCDIR)\findfile.rc

# Build idragdrop.c
IDRAGDROP_C=\
	$(SRCDIR)\IDragDrop.h\
	$(SRCDIR)\Shell.h\
	$(SRCDIR)\find.h\
	$(SRCDIR)\jmem.h\

idragdrop.obj: $(IDRAGDROP_C) $(SRCDIR)\idragdrop.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\idragdrop.c

# Build jmem.c
JMEM_C=\
	$(SRCDIR)\jmem.h\

jmem.obj: $(JMEM_C) $(SRCDIR)\jmem.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\jmem.c

# Build olepicture.c
OLEPICTURE_C=\

olepicture.obj: $(OLEPICTURE_C) $(SRCDIR)\olepicture.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\olepicture.c

# Build registry.c
REGISTRY_C=\
	$(SRCDIR)\registry.h\

registry.obj: $(REGISTRY_C) $(SRCDIR)\registry.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\registry.c

# Build shell.c
SHELL_C=\
	$(SRCDIR)\shell.h\
	$(SRCDIR)\find.h\
	$(SRCDIR)\jmem.h\
	$(SRCDIR)\dialogs.h\
	$(SRCDIR)\find.h\
	$(SRCDIR)\findfileres.h\

shell.obj: $(SHELL_C) $(SRCDIR)\shell.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\shell.c

# Build utils.c
UTILS_C=\
	$(SRCDIR)\findfileres.h\
	$(SRCDIR)\find.h\
	$(SRCDIR)\jmem.h\
	$(SRCDIR)\utils.h\
	$(SRCDIR)\dialogs.h\
	$(SRCDIR)\find.h\
	$(SRCDIR)\findfile.h\
	$(SRCDIR)\findfileres.h\

utils.obj: $(UTILS_C) $(SRCDIR)\utils.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\utils.c

link:
	$(LINKER)  -s -subsystem windows -stack-reserve 20000000 -o $(SRCDIR)\lcc\findfile.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) findfile.exe
