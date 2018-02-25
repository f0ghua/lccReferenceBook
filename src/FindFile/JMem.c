// jmem.c
#include <windows.h>
#include <stdlib.h>
#include "jmem.h"

// allocating and free-ing many thousands of memory blocks
// can take an age, these routines make it more reasonable.

#define JMEMLIBVERS "Memory Manage Library Version 1.01"

#define ALIGN 4     // align on 4 byte boundry

void jmem_libvers(void)
{
	MessageBox( GetActiveWindow(), JMEMLIBVERS, "Version", MB_OK );
}

int get_block( HMEM hmem )
{
	return hmem->curblock+1;
}

int get_numblocks( HMEM hmem )
{
	return hmem->numblocks+1;
}

HMEM add_blocks( HMEM hmem, int num )
{
	int i;
	HMEM tmp = realloc( hmem, sizeof(struct tagJMem) + (hmem->numblocks + num)* sizeof(char*));
	if (tmp)
	{
		hmem = tmp;
		for ( i = hmem->numblocks; i<=(hmem->numblocks+num); i++)
		{
			hmem->block[i] = NULL;
		}
		hmem->numblocks += num;
		return hmem;
	}
	else
	{
		return NULL;
	}
}

HMEM InitJMem( int blocksize, int numblocks )
{
	HMEM hmem = calloc( 1, sizeof(struct tagJMem) + numblocks * sizeof(char*) );
	if ( hmem == NULL )
		return NULL;

	hmem->blocksize	= blocksize;
	hmem->numblocks	= numblocks -1;
	hmem->curblock  = 0;
	hmem->curoffset = 0;

	hmem->block[hmem->curblock] = calloc( 1, hmem->blocksize );

	if (hmem->block[hmem->curblock] == NULL )
		return NULL;
	else
		return hmem;
}

void * jmalloc( HMEM hmem, int size )
{
	char * pnewmem;

	hmem->curoffset = (hmem->curoffset + ALIGN - 1) & ~(ALIGN - 1);

	if( (hmem->curoffset + size) > hmem->blocksize )
	{
		if ( hmem->curblock >= hmem->numblocks )
			return NULL;

		 pnewmem = calloc( 1, hmem->blocksize );

		if ( pnewmem == NULL )
			return NULL;

		hmem->curblock++;
		hmem->block[hmem->curblock] = pnewmem;
		hmem->curoffset = size;

	}
	else
	{
		pnewmem = hmem->block[hmem->curblock] + hmem->curoffset;
		hmem->curoffset += size;
	}
	return pnewmem;
}

void jfree( HMEM hmem )
{
	for( int i =0; i<=(hmem->curblock); i++)
	{
		if ( hmem->block[i] ) 
			free( hmem->block[i] );
	}
	free( hmem );
}

