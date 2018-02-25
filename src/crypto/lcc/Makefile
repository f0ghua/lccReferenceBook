# Wedit Makefile for project Crypto
SRCDIR=d:\lcc\projects\crypto
CFLAGS=-Id:\lcc\include 
CC=d:\lcc\bin\lcc.exe
LINKER=d:\lcc\bin\lcclnk.exe
OBJS=\
	crypto.obj

LIBS=

crypto.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\crypto.exe $(OBJS) $(LIBS)

# Build CRYPTO.C
CRYPTO_C=\

crypto.obj: $(CRYPTO_C) $(SRCDIR)\crypto.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\crypto.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\lcc\crypto.exe $(OBJS) $(LIBS)

