# Wedit Makefile for project newlook
SRCDIR=d:\lcc\projects\a_projects5\newlook
CFLAGS=-Id:\lcc\include 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	ncpaint.obj \
	newlook.obj \
	newlook.res

LIBS=
EXE=newlook.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\newlook.exe $(OBJS) $(LIBS)

# Build ncpaint.c
NCPAINT_C=\

ncpaint.obj: $(NCPAINT_C) $(SRCDIR)\ncpaint.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\ncpaint.c

# Build newlook.c
NEWLOOK_C=\
	$(SRCDIR)\newlookres.h\

newlook.obj: $(NEWLOOK_C) $(SRCDIR)\newlook.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\newlook.c

# Build newlook.res
NEWLOOK_RC=\
	$(SRCDIR)\newlookres.h\

newlook.res:	$(NEWLOOK_RC) $(SRCDIR)\newlook.rc
	$(LCCROOT)\bin\lrc.exe -I$(SRCDIR) -Id:\lcc\include  $(SRCDIR)\newlook.rc

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\newlook.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) d:\lcc\projects\a_projects5\newlook\lcc\newlook.exe
