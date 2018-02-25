# Wedit Makefile for project bnlib
SRCDIR=.
CFLAGS=-DBNINCLUDE=lbn80386.h  -O 
CC=lcc.exe
ZIPSOURCES= bn.c bn32.c bninit32.c bnprint.c bntest32.c germain.c germtest.c jacobi.c prime.h sieve.h \
lbn32.c lbnmem.c lbnppc.c legal.c prime.c sieve.c sizetest.c bn.h bn16.h bn32.h legal.h \
bnprint.h cputime.h germain.h jacobi.h kludge.h lbn.h lbn32.h lbn80386.h lbnmem.h lbnppc.h \
bnlib.rtf bnlib.hlp bnlib.hpj makefile bnlib.lib

LINKER=lcclnk.exe
OBJS=\
	sieve.obj \
	prime.obj \
	legal.obj \
	lbnmem.obj \
	lbn80386.obj \
	lbn32.obj \
	jacobi.obj \
	bnprint.obj \
	bninit32.obj \
	bn32.obj \
	germain.obj \
	bn.obj

LIBS=

all:	testbnlib.exe bnlib.lib germtest.exe

bnlib.lib:	$(OBJS)
	lcclib /out:bnlib.lib $(OBJS)

testbnlib.exe:	bnlib.lib Makefile bntest32.obj
	$(LINKER)  -subsystem console -o $(SRCDIR)\testbnlib.exe bntest32.obj bnlib.lib

germtest.exe:	bnlib.lib germtest.obj
	$(LINKER) -subsystem console -o germtest.exe germtest.obj bnlib.lib

germtest.obj:	germtest.c
	$(CC) $(CFLAGS) germtest.c

# Build SIEVE.C
SIEVE_C=\
	$(SRCDIR)\bn.h\
	$(SRCDIR)\sieve.h\
	$(SRCDIR)\kludge.h\

sieve.obj: $(SIEVE_C) $(SRCDIR)\sieve.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\sieve.c

# Build PRIME.C
PRIME_C=\
	$(SRCDIR)\bn.h\
	$(SRCDIR)\lbnmem.h\
	$(SRCDIR)\prime.h\
	$(SRCDIR)\sieve.h\
	$(SRCDIR)\kludge.h\

prime.obj: $(PRIME_C) $(SRCDIR)\prime.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\prime.c

# Build LEGAL.C
LEGAL_C=\
	$(SRCDIR)\legal.h\

legal.obj: $(LEGAL_C) $(SRCDIR)\legal.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\legal.c

# Build LBNMEM.C
LBNMEM_C=\
	$(SRCDIR)\lbn.h\
	$(SRCDIR)\lbnmem.h\
	$(SRCDIR)\kludge.h\

lbnmem.obj: $(LBNMEM_C) $(SRCDIR)\lbnmem.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\lbnmem.c

lbn80386.obj: $(SRCDIR)\lbn80386.asm
	$(CC) -c $(CFLAGS) $(SRCDIR)\lbn80386.asm

# Build LBN32.C
LBN32_C=\
	$(SRCDIR)\lbn.h\
	$(SRCDIR)\lbn32.h\
	$(SRCDIR)\lbn.h\
	$(SRCDIR)\lbnmem.h\
	$(SRCDIR)\legal.h\
	$(SRCDIR)\kludge.h\

lbn32.obj: $(LBN32_C) $(SRCDIR)\lbn32.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\lbn32.c

# Build LBN16.C
LBN16_C=\
	$(SRCDIR)\lbn.h\
	$(SRCDIR)\lbn16.h\
	$(SRCDIR)\lbn.h\
	$(SRCDIR)\lbnmem.h\
	$(SRCDIR)\legal.h\
	$(SRCDIR)\kludge.h\

lbn16.obj: $(LBN16_C) $(SRCDIR)\lbn16.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\lbn16.c

# Build JACOBI.C
JACOBI_C=\
	$(SRCDIR)\bn.h\
	$(SRCDIR)\jacobi.h\

jacobi.obj: $(JACOBI_C) $(SRCDIR)\jacobi.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\jacobi.c

# Build GERMAIN.C
GERMAIN_C=\
	$(SRCDIR)\bn.h\
	$(SRCDIR)\germain.h\
	$(SRCDIR)\jacobi.h\
	$(SRCDIR)\lbnmem.h\
	$(SRCDIR)\sieve.h\
	$(SRCDIR)\kludge.h\

germain.obj: $(GERMAIN_C) $(SRCDIR)\germain.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\germain.c

# Build BNTEST32.C
BNTEST32_C=\
	$(SRCDIR)\cputime.h\
	$(SRCDIR)\lbn32.h\
	$(SRCDIR)\lbn.h\
	$(SRCDIR)\kludge.h\

bntest32.obj: $(BNTEST32_C) $(SRCDIR)\bntest32.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\bntest32.c

# Build BNPRINT.C
BNPRINT_C=\
	$(SRCDIR)\bn.h\
	$(SRCDIR)\bnprint.h\
	$(SRCDIR)\kludge.h\

bnprint.obj: $(BNPRINT_C) $(SRCDIR)\bnprint.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\bnprint.c

# Build BNINIT32.C
BNINIT32_C=\
	$(SRCDIR)\bn.h\
	$(SRCDIR)\bn32.h\

bninit32.obj: $(BNINIT32_C) $(SRCDIR)\bninit32.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\bninit32.c

# Build BN32.C
BN32_C=\
	$(SRCDIR)\lbn.h\
	$(SRCDIR)\lbn32.h\
	$(SRCDIR)\lbn.h\
	$(SRCDIR)\lbnmem.h\
	$(SRCDIR)\bn32.h\
	$(SRCDIR)\bn.h\
	$(SRCDIR)\kludge.h\

bn32.obj: $(BN32_C) $(SRCDIR)\bn32.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\bn32.c

# Build BN.C
BN_C=\
	$(SRCDIR)\bn.h\

bn.obj: $(BN_C) $(SRCDIR)\bn.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\bn.c

clean:
	del *.obj *.exe bnlib.lib bnlib.zip

zip:	$(ZIPSOURCES)
	zip -9 bnlib.zip $(ZIPSOURCES)
