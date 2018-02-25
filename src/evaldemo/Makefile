# Wedit Makefile for project evaldemo
SRCDIR=e:\bot\rqbeta\eval11\evaldemo
CFLAGS=-Id:\bcx\lcc\include  -O  -A  -ansic
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	evalwrap.obj \
	evalkern.obj \
	evaldemo.res \
	evaldemo.obj

LIBS=
EXE=evaldemo.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\evaldemo.exe $(OBJS) $(LIBS)

# Build evalwrap.c
EVALWRAP_C=\
	$(SRCDIR)\evaldefs.h\
	$(SRCDIR)\evalkern.h\

evalwrap.obj: $(EVALWRAP_C) $(SRCDIR)\evalwrap.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\evalwrap.c

# Build evalkern.c
EVALKERN_C=\
	$(SRCDIR)\evaldefs.h\
	$(SRCDIR)\evalkern.h\

evalkern.obj: $(EVALKERN_C) $(SRCDIR)\evalkern.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\evalkern.c

# Build evaldemo.res
EVALDEMO_RC=\
	$(SRCDIR)\evaldemo.h\

evaldemo.res:	$(EVALDEMO_RC) $(SRCDIR)\evaldemo.rc
	$(LCCROOT)\bin\lrc.exe -I$(SRCDIR) -Id:\bcx\lcc\include  $(SRCDIR)\evaldemo.rc

# Build evaldemo.c
EVALDEMO_C=\
	$(SRCDIR)\evaldemo.h\
	$(SRCDIR)\evaldefs.h\

evaldemo.obj: $(EVALDEMO_C) $(SRCDIR)\evaldemo.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\evaldemo.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\evaldemo.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) e:\bot\rqbeta\eval11\evaldemo\evaldemo.exe
