# Wedit Makefile for project memorybooster
SRCDIR=d:\lcc\projects\a_projects7\memorybooster
CFLAGS= -O 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	common.obj \
	memorybooster.obj \
	memorybooster.res \

LIBS=shell32.lib
EXE=memorybooster.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\memorybooster.exe $(OBJS) $(LIBS)

# Build common.c
COMMON_C=\
	$(SRCDIR)\common.h\

common.obj: $(COMMON_C) $(SRCDIR)\common.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common.c

# Build memorybooster.c
MEMORYBOOSTER_C=\
	$(SRCDIR)\common.h\
	$(SRCDIR)\memoryboosterres.h\

memorybooster.obj: $(MEMORYBOOSTER_C) $(SRCDIR)\memorybooster.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\memorybooster.c

# Build memorybooster.res
MEMORYBOOSTER_RC=\

memorybooster.res:	$(MEMORYBOOSTER_RC) $(SRCDIR)\memorybooster.rc
	$(LCCROOT)\bin\lrc.exe -I$(SRCDIR)  $(SRCDIR)\memorybooster.rc

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\memorybooster.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) D:\lcc\projects\a_projects7\MemoryBooster\lcc\memorybooster.exe
