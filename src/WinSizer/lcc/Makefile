# Wedit Makefile for project wsizer
SRCDIR=d:\lcc_sc\wsizer
CFLAGS=-ID:\lcc\db  -O  -unused
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	wsizer.res \
	wsizer.obj

LIBS=
EXE=wsizer.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\wsizer.exe $(OBJS) $(LIBS)

# Build wsizer.res
WSIZER_RC=\
	$(SRCDIR)\wsizerres.h\
	d:\lcc_sc\window_appicon.ico\

wsizer.res:	$(WSIZER_RC) $(SRCDIR)\wsizer.rc
	$(LCCROOT)\bin\lrc.exe -I$(SRCDIR) -ID:\lcc\db  $(SRCDIR)\wsizer.rc

# Build wsizer.c
WSIZER_C=\
	$(SRCDIR)\wsizerres.h\

wsizer.obj: $(WSIZER_C) $(SRCDIR)\wsizer.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\wsizer.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\wsizer.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) d:\lcc_sc\wsizer\lcc\wsizer.exe
