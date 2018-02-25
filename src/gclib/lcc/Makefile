# Wedit Makefile for project gclib
SRCDIR=d:\lcc\projects\a_projects6\gclib
CFLAGS=-ID:\lcc\db-new  -ansic
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	typd_mlc.obj \
	stubborn.obj \
	reclaim.obj \
	ptr_chck.obj \
	os_dep.obj \
	obj_map.obj \
	new_hblk.obj \
	misc.obj \
	mark_rts.obj \
	mark.obj \
	mallocx.obj \
	malloc.obj \
	mach_dep.obj \
	headers.obj \
	finalize.obj \
	dyn_load.obj \
	dbg_mlc.obj \
	blacklst.obj \
	alloc.obj \
	allchblk.obj

LIBS=
EXE=gclib.exe

$(EXE):	$(OBJS) Makefile
	$(LCCROOT)\bin\lcclib.exe /OUT:d:\lcc\projects\a_projects6\gclib\lcc\gclib.lib $(OBJS)

# Build typd_mlc.c
TYPD_MLC_C=\
	$(SRCDIR)\include\private\gc_pmark.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\
	$(SRCDIR)\include\gc_typed.h\

typd_mlc.obj: $(TYPD_MLC_C) $(SRCDIR)\typd_mlc.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\typd_mlc.c

# Build stubborn.c
STUBBORN_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

stubborn.obj: $(STUBBORN_C) $(SRCDIR)\stubborn.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\stubborn.c

# Build reclaim.c
RECLAIM_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

reclaim.obj: $(RECLAIM_C) $(SRCDIR)\reclaim.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\reclaim.c

# Build ptr_chck.c
PTR_CHCK_C=\
	$(SRCDIR)\include\private\gc_pmark.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

ptr_chck.obj: $(PTR_CHCK_C) $(SRCDIR)\ptr_chck.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\ptr_chck.c

# Build os_dep.c
OS_DEP_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

os_dep.obj: $(OS_DEP_C) $(SRCDIR)\os_dep.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\os_dep.c

# Build obj_map.c
OBJ_MAP_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

obj_map.obj: $(OBJ_MAP_C) $(SRCDIR)\obj_map.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\obj_map.c

# Build new_hblk.c
NEW_HBLK_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

new_hblk.obj: $(NEW_HBLK_C) $(SRCDIR)\new_hblk.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\new_hblk.c

# Build misc.c
MISC_C=\
	$(SRCDIR)\include\private\gc_pmark.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

misc.obj: $(MISC_C) $(SRCDIR)\misc.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\misc.c

# Build mark_rts.c
MARK_RTS_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

mark_rts.obj: $(MARK_RTS_C) $(SRCDIR)\mark_rts.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\mark_rts.c

# Build mark.c
MARK_C=\
	$(SRCDIR)\include\private\gc_pmark.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

mark.obj: $(MARK_C) $(SRCDIR)\mark.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\mark.c

# Build mallocx.c
MALLOCX_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

mallocx.obj: $(MALLOCX_C) $(SRCDIR)\mallocx.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\mallocx.c

# Build malloc.c
MALLOC_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

malloc.obj: $(MALLOC_C) $(SRCDIR)\malloc.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\malloc.c

# Build mach_dep.c
MACH_DEP_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

mach_dep.obj: $(MACH_DEP_C) $(SRCDIR)\mach_dep.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\mach_dep.c

# Build headers.c
HEADERS_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

headers.obj: $(HEADERS_C) $(SRCDIR)\headers.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\headers.c

# Build finalize.c
FINALIZE_C=\
	$(SRCDIR)\include\private\gc_pmark.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

finalize.obj: $(FINALIZE_C) $(SRCDIR)\finalize.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\finalize.c

# Build dyn_load.c
DYN_LOAD_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

dyn_load.obj: $(DYN_LOAD_C) $(SRCDIR)\dyn_load.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\dyn_load.c

# Build dbg_mlc.c
DBG_MLC_C=\
	$(SRCDIR)\include\private\dbg_mlc.h\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

dbg_mlc.obj: $(DBG_MLC_C) $(SRCDIR)\dbg_mlc.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\dbg_mlc.c

# Build blacklst.c
BLACKLST_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

blacklst.obj: $(BLACKLST_C) $(SRCDIR)\blacklst.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\blacklst.c

# Build alloc.c
ALLOC_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\
	$(SRCDIR)\version.h\

alloc.obj: $(ALLOC_C) $(SRCDIR)\alloc.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\alloc.c

# Build allchblk.c
ALLCHBLK_C=\
	$(SRCDIR)\include\private\gc_priv.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\gc_mark.h\
	$(SRCDIR)\include\gclib.h\
	$(SRCDIR)\include\private\gcconfig.h\
	$(SRCDIR)\include\private\gc_hdrs.h\
	$(SRCDIR)\include\private\gc_locks.h\

allchblk.obj: $(ALLCHBLK_C) $(SRCDIR)\allchblk.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\allchblk.c

link:
	$(LCCROOT)\bin\lcclib.exe /OUT:d:\lcc\projects\a_projects6\gclib\lcc\gclib.lib $(OBJS)

clean:
	del $(OBJS) d:\lcc\projects\a_projects6\gclib\lcc\gclib.exe
