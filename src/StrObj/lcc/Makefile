# Wedit Makefile for project strObject
SRCDIR=d:\lcc\projects\a_projects5\stringobject
CFLAGS=-Id:\lcc\include  -g2 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	test.obj \
	stringobject.obj

LIBS=
EXE=strobject.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -subsystem console -o $(SRCDIR)\lcc\strobject.exe $(OBJS) $(LIBS)

# Build test.c
TEST_C=\
	$(SRCDIR)\stringObject.h\

test.obj: $(TEST_C) $(SRCDIR)\test.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\test.c

# Build stringobject.c
STRINGOBJECT_C=\
	$(SRCDIR)\stringObject.h\

stringobject.obj: $(STRINGOBJECT_C) $(SRCDIR)\stringobject.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\stringobject.c

link:
	$(LINKER)  -subsystem console -o $(SRCDIR)\lcc\strobject.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) d:\lcc\projects\a_projects5\stringobject\lcc\strobject.exe
