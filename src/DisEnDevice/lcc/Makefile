# Wedit Makefile for project DisEnDevice
SRCDIR=g:\lcc\projects\a_projects8\disendevice
CFLAGS=
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	setupdi.obj \
	enable.res \
	enable.obj \
	debug.obj \
	config.obj

LIBS=SETUPAPI.lib
EXE=disendevice.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\disendevice.exe $(OBJS) $(LIBS)

# Build setupdi.c
SETUPDI_C=\
	$(SRCDIR)\common.h\
	$(SRCDIR)\resource.h\

setupdi.obj: $(SETUPDI_C) $(SRCDIR)\setupdi.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\setupdi.c

# Build enable.res
ENABLE_RC=\
	$(SRCDIR)\resource.h\
	$(SRCDIR)\ntverp.h\

enable.res:	$(ENABLE_RC) $(SRCDIR)\enable.rc
	$(LCCROOT)\bin\lrc.exe -I$(SRCDIR)  $(SRCDIR)\enable.rc

# Build enable.c
ENABLE_C=\
	$(SRCDIR)\common.h\
	$(SRCDIR)\resource.h\

enable.obj: $(ENABLE_C) $(SRCDIR)\enable.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\enable.c

# Build debug.c
DEBUG_C=\
	$(SRCDIR)\common.h\
	$(SRCDIR)\resource.h\

debug.obj: $(DEBUG_C) $(SRCDIR)\debug.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\debug.c

# Build config.c
CONFIG_C=\
	$(SRCDIR)\common.h\
	$(SRCDIR)\resource.h\

config.obj: $(CONFIG_C) $(SRCDIR)\config.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\config.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\disendevice.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) g:\lcc\projects\a_projects8\disendevice\lcc\disendevice.exe
