// jmem.h

#ifndef _JMEM_H_
#define _JMEM_H_

typedef struct tagJMem{
	unsigned long	blocksize;
	unsigned long	numblocks;
	unsigned long	curblock;
	unsigned long	curoffset;
	char          * block[1];
}*HMEM;

HMEM   InitJMem( int blocksize, int numblocks );
void * jmalloc( HMEM hmem, int size );
void   jfree( HMEM hmem );
int    get_block( HMEM hmem );
int    get_numblocks( HMEM hmem );
HMEM   add_blocks( HMEM hmem, int num );
void   jmem_libvers( void );

#endif // _JMEM_H_
