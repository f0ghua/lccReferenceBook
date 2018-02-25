// TMalloc.c
//
// John Findlay <postmaster@johnfindlay.plus.com>
//
// TMalloc version 2.0 (14th July 2003)
//
// Include tmalloc.h in all translation units that require
// testing. DO NOT INCLUDE TMALLOC.H IN THIS FILE.
//
// ----------------------------------------------------------------
//
// TMalloc was originally developed solely to check for memory
// leaks when using malloc-ed memory. Since then it has expanded
// to check for buffer over-runs for various string functions.
//
// See tmalloc.h for further comments regarding malloc and family
// functions.
//
// -----------------------------------------------------------------
// Short description of checks made when string functions are called
//
// Frequent use of the term "valid tmalloc pointer" is used - it
// means the base pointer returned from either tmalloc, tcalloc
// or trealloc.
// -----------------------------------------------------------------
// If any NULL pointer passed, 0 or NULL will be returned
// without further action.
//
// If the passed pointer is a valid tmalloc pointer and no
// bounds error will occur the run time function will be called.
//
// If not a valid tmalloc pointer but within the tmalloc
// allocated block and no bounds error will occur, the run time
// function will be called.
//
// If it's not a valid tmalloc pointer or within a valid tmalloc
// block no checks can be done - the run time function is called
// without bounds checks.
//
// If string functions are passed a pointer that does not exist
// within a tmalloc-ed memory block no checks can be preformed.
//
// example;
//
// char * a = malloc( 30 );
// strcpy( a + 40, "Hello" );
//
// This is obviously wrong but as the expression a + 40 is outside
// the boundaries of the allocated block, tmalloc string functions
// will not check it. This particular call is an error but will not
// be reported as such.
//
// However this
//
// char * a = malloc( 30 );
// strcpy( a + 26, "Hello" );
//
// Will be trapped by tmalloc checks because a + 26 is still within
// the allocated block and can be detected as a tmalloc block.
//
// --------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>

#ifdef __POCC__
 #pragma lib "user32.lib"    // for MessageBox API
#elif defined __LCC__
 #pragma lib <user32.lib>
#endif

// values to identify which function
// was used when allocating memory
#define MALLOC 1
#define CALLOC 2
#define REALLOC 3

// forward declaration of struct
typedef struct tagNODE NODE;

// NODE structure before every heap object
typedef struct tagNODE{
	NODE  * pPrev;          // previous object in heap
	NODE  * pNext;          // next object in heap
	size_t  size;           // size of block
	int     type;           // "MALLOC", "CALLOC" or "REALLOC"
	char    filename[32];   // file/module name (names will be limited to 30 chars)
	int     linenum;        // line number
	void  * pMem;           // pointer to user memory
} NODE;

//  Points to latest object in linked list of heap objects
static NODE * pHeadNode = NULL;

// static prototypes
static void AppendToLinkedList( NODE * );
static void RemoveFromLinkedList( NODE * );
static int  IsValidNode( const void * );
static void GetNodeDescription( NODE *, char * );
static NODE * IsPointerOffset( const void *, size_t * );
static void BoundsError( NODE *, char *, char *, int );
static void PointerError( char *, char *, int );

static char * ExtractFileName( char * path )
{
	static char str[] = "No File Name!";
	char * p = strrchr(path, '\\');
	if(p) 
		return (p + 1);
	else  
		return str;
}

// Error handler
static void BoundsError( NODE * pNode, char * func, char * pFile, int nLine )
{
	char str[256], str1[32], str2[260];
	sprintf( str, "%s %s %s", "Bounds error when using [", func, "] on memory allocated with tmalloc -> " );
	strncpy( str1, ExtractFileName( pFile ), 30 );
	str1[30] = 0; // terminate it
	sprintf( str + strlen(str), "%s %s %d\n\r\n\r%s", str1, "at line ",
   									 nLine, "Aborting operation!\n\n" );

	GetNodeDescription( pNode, str2 );
	strcat( str, str2 );
	MessageBox( NULL, str, "TMalloc Error. . . ", MB_OK );
}

static void PointerError( char * func, char * pFile, int nLine )
{
	char str[256], str1[32];
	sprintf( str, "%s %s %s", "Null pointer when using [", func, "] -> " );
	strncpy( str1, ExtractFileName(pFile), 30 );
	str1[30] = 0; // terminate it
	sprintf( str + strlen(str), "%s %s %d\n\r\n\r%s", str1, "at line ",
   									 nLine, "Aborting operation!\n\n" );

	MessageBox( NULL, str, "TMalloc Error. . . ", MB_OK );
}

void report_heap( char * pOutFile )
{
	char buffer[200];
	FILE * fout = NULL;

	if ( pOutFile )
	{
		fout = fopen( pOutFile, "w");
		if (!fout)
		{
			MessageBox( 0, "Could not open report file!", "TMalloc Error. . . ", MB_OK );
			return;
		}
	}
	else
	{
		fout = stdout;
	}

	fprintf( fout, "\nWalking though allocated heap\n" );

	if (pHeadNode) 
	{
		fprintf( fout, "\nThe following blocks were not free-ed.\n\n" );
		NODE * pCur = pHeadNode;
		while ( pCur ) 
		{
			GetNodeDescription( pCur, buffer );
			fprintf( fout, "%s\n", buffer );
			pCur = pCur->pPrev;
		}
	}
	else
	{
		fprintf( fout, "\nNo heap to walk\n" );
	}

	if ( pOutFile && fout )
		fclose(fout);
}

static void AppendToLinkedList( NODE * pCurrent )
{
	// Add first node
	if ( !pHeadNode ) 
	{
		pCurrent->pPrev = NULL; 	// no previous node
		pCurrent->pNext = NULL; 	// no next node
	} 
	else 
	{ // else other nodes
		pCurrent->pPrev  = pHeadNode;
		pHeadNode->pNext = pCurrent;
		pCurrent->pNext  = NULL;	// no more nodes
	}
	// make current node head of list
	pHeadNode = pCurrent;
}

static void RemoveFromLinkedList( NODE * pNode )
{
	//  Remove from double linked list
	if ( !pNode->pNext && !pNode->pPrev ) 		// only one node left
	{
		pHeadNode = NULL;                    	// no nodes left
		return;
	}
	else if ( !pNode->pPrev )                	// delete first node
	{
		(pNode->pNext)->pPrev = NULL;
	}
	else if ( !pNode->pNext )            		// delete last node
	{
		pHeadNode = pNode->pPrev;
		(pNode->pPrev)->pNext = NULL;
	}
	else										// delete other nodes
	{
		(pNode->pPrev)->pNext = pNode->pNext;
		(pNode->pNext)->pPrev = pNode->pPrev;
	}
}

static NODE * IsPointerOffset( const void * pMem, size_t * offset )
{
	char * pos   = (char *)pMem;
	char * begin = 0;
	char * end   = 0;

	NODE * pNode = pHeadNode;
	// walk through the list to find the node
	while( pNode )
	{
		begin = pNode->pMem;
		end   = (char *)pNode->pMem + pNode->size;

		// Check, is the pointer within the tmalloc block
		if( pos > begin && pos < end)
		{
			*offset = pos - begin;
			break;
		}

		pNode = pNode->pPrev; // reverse traverse
		if(!pNode)
			break;
	}
	if( *offset )
		return pNode;
	else
		return NULL;
}

static int IsValidNode( const void * pMem )
{
	int fFlag = 0;

	NODE * pFindNode = (NODE*)pMem-1;

	NODE * pNode = pHeadNode;

	// Walk through the list to find the node.
	// This will only be so when the pointer is 
	// at the beginning of the memory block allocated.
	// Elsewhere [IsPointerOffset()] the pointer is 
	// checked to see if it falls within a memory block.
	while( pNode )
	{
		if ( pNode == pFindNode )
		{
		    fFlag = 1;
			break;
		}
		pNode = pNode->pPrev;
	}
    return fFlag;    // if not found return 0
}

static void GetNodeDescription( NODE * pNode, char * pBuffer )
{

	if ( pNode->pMem == &pNode[1] ) // check if node has memory
	{
		char s[8];
		if ( pNode->type == MALLOC )
		{
			strcpy( s, "MALLOC " );
		}
		else if ( pNode->type == CALLOC )
		{
			strcpy( s, "CALLOC " );
		}
		else
		{
			strcpy( s, "REALLOC" );
		}
		sprintf( pBuffer, "MEM:[%p] TYPE:[%s] SIZE:[%d] LINE:[%d] FILE:[%s] ",
				pNode->pMem, s, pNode->size, pNode->linenum, pNode->filename );
	}
	else
	{
		strcpy( pBuffer, "(bad pointer)" );
	}
}

void node_description( void * pMem )
{
	size_t offset;
	char buf[260];
	char msg[] = "TMalloc Node Description. . . ";
	char msg1[] = "TMalloc Node Description. . . Pointer is offset!";

	NODE * pNode;
	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
		GetNodeDescription( pNode, buf );
		MessageBox( NULL, buf, msg, MB_OK );
	}
	else if(IsPointerOffset( pMem, &offset ))
	{
		pNode = (NODE*)pMem-1;
		char * p = (char *)pNode - offset;
		pNode = (NODE*)p;
		GetNodeDescription( pNode, buf );
		sprintf(buf+strlen(buf), "\n\nPointer is offset by +%d Bytes", offset);
		MessageBox( NULL, buf, msg1, MB_OK );
	}
	else
	{
		MessageBox( NULL, "Not a valid tmalloc pointer!", msg, MB_OK );
	}
	return;
}

// Wrapper for CRT free()
void tfree( void * pMem, char * pFile, int nLine )
{
	size_t offset;
	char str[256], str1[32];

	// free accepts a NULL pointer.
	if (!pMem)
		return;

	if ( IsValidNode( pMem ) ) 
	{
		NODE * pNode = (NODE*)pMem-1;
		RemoveFromLinkedList( pNode );
		free(pNode);
	}
	else
	{
		// search to see it the ptr is within a previously
		// allocated block
		if( IsPointerOffset( pMem, &offset ) )
		{
			strcpy(str, "Pointer is not at the start of allocated block and cannot be passed to free! -> ");
			strncpy(str1, ExtractFileName(pFile), 30);
			str1[30] = 0; // terminate it
			sprintf( str + strlen(str), "%s %s %d\n\r\n\r%s", str1, "at line ",
   									 nLine, "Aborting operation!" );
			MessageBox( NULL, str, "TMalloc Error. . . ", MB_OK );

		}
		else
		{
			strcpy(str, "Trying to free from invalid pointer from -> ");
			strncpy(str1, ExtractFileName(pFile), 30);
			str1[30] = 0; // terminate it
			sprintf( str + strlen(str), "%s %s %d\n\r\n\r%s", str1, "at line ",
   									 nLine, "Aborting operation!" );
			MessageBox( NULL, str, "TMalloc Error. . . ", MB_OK );
		}
	}
}

// Wrapper for CRT malloc()
void * tmalloc( size_t size, char * pFile, int nLine )
{
	void * ret;
	NODE * pNode;
	pNode = malloc( sizeof(NODE) + size );
	if ( pNode ) 
	{
		AppendToLinkedList( pNode );
		pNode->size      = size;
		pNode->type      = MALLOC;
		strncpy( pNode->filename, ExtractFileName( pFile ), 30 );
		pNode->filename[30] = 0; // terminate it
		pNode->linenum   = nLine;

		// The user mem is 'pNode + sizeof(NODE)'
		// it will be on a 4 byte boundary
		pNode->pMem = pNode+1;
		ret = pNode->pMem;
	}
	else 
	{
		ret = NULL;
	}

	return ret;
}

// Wrapper for CRT calloc()
void * tcalloc( size_t nmemb, size_t size, char * pFile, int nLine )
{
	void * ret;
	NODE * pNode;
	size_t nsize = nmemb * size;
	pNode = calloc( 1, sizeof(NODE) + nsize );
	if ( pNode ) 
	{
		AppendToLinkedList( pNode );
		pNode->size      = nsize;
		pNode->type      = CALLOC;
		pNode->linenum   = nLine;

		// The user mem is 'pNode + sizeof(NODE)'
		// it will be on a 4 byte boundary
		pNode->pMem = pNode+1;
		strncpy( pNode->filename, ExtractFileName(pFile), 30 );
		pNode->filename[30] = 0; // terminate it
		ret = pNode->pMem;
	}
	else 
	{
		ret = NULL;
	}
	return ret;
}

// Wrapper for CRT realloc()
void * trealloc( void * pOld, size_t size, char * pFile, int nLine )
{
	NODE * pNode = (NODE*)pOld-1;
	NODE * pNewNode = NULL;

	// realloc works like malloc if NULL pointer is passed
	if ( !pOld )
		return tmalloc( size, pFile, nLine );

	// realloc works like free if size is zero
	if ( !size )
	{
		tfree( pOld, pFile, nLine );
		return NULL;
	}

	if ( IsValidNode( pOld ) ) 
	{
		// Try to reallocate block
		// The user mem will be at 'pNode + sizeof(NODE)'

		RemoveFromLinkedList( pNode ); // remove it first

		pNewNode = realloc( pNode, sizeof(NODE) + size );

		// if failed return
		if ( !pNewNode )
			return NULL;

		AppendToLinkedList( pNewNode ); // add it
		
		// pNode will have been free-ed by realloc if pNode != pNewNode
		pNewNode->size      = size;
		pNewNode->type      = REALLOC;
		pNewNode->linenum   = nLine;

		// The user mem is 'pNode + sizeof(NODE)'
		// it will be on a 4 byte boundary
		pNewNode->pMem      = pNewNode+1;
		strncpy( pNewNode->filename, ExtractFileName(pFile), 30 );
		pNewNode->filename[30] = 0;
		return pNewNode->pMem;
	}
	else
	{
		char str[256], str1[32];
		strcpy( str, "Trying to realloc from invalid pointer from -> " );
		strncpy( str1, ExtractFileName( pFile ), 30 );
		str1[30] = 0;
		sprintf( str + strlen(str), "%s %s %d\n\r\n\r%s", str1, "at line ",
	   									 nLine, "Aborting operation!" );
		MessageBox( NULL, str, "TMalloc Error. . . ", MB_OK );
		return NULL;
	}
}


// -----------------------------------------------
// Functions that copy memory, i.e. strcpy etc.
// -----------------------------------------------
// In these functions IsValidNode() checks that
// the dest memory is a 'valid tmalloc pointer', if
// not, IsPointerOffset() checks if the pointer is
// within the allocated block and will do the necessary
// checks before using the run time function. If the
// allocated block is not a 'valid tmalloc pointer'
// and not within the allocated block the run time
// function is called without checks and the return
// value is passed to the caller.
// -----------------------------------------------

// Wrapper for CRT strcpy()
// Copies a string.
char * tstrcpy( char * dest, const char * src, char * pFile, int nLine )
{
	if( !dest || !src )
	{
		PointerError( "strcpy", pFile, nLine );
		return NULL;
	}

	size_t offset = 0;
	NODE * pNode;
	void * pMem = dest;

	// chekc if the pointer is valid
	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
	}
	else
	{
		// if the pointer is not within a previously allocated
		// block we can't do any checks so do it anyway.
		if( !(pNode = IsPointerOffset( pMem, &offset )) )
			return strcpy( dest, src );
	}

	// ok so far, check the length. offset can be the offset from
	// the start of the allocated block if found in IsPointerOffset().
	// If not found offset will be '0'
	int len = strlen( src ) + offset;
	if(	len > pNode->size -1 )
	{
		BoundsError( pNode, "strcpy", pFile, nLine );
		return NULL;
	}
	else
	{
		return strcpy( dest, src );
	}
}

// Wrapper for CRT strcat()
// Appends a string.
char * tstrcat( char * dest, const char * src, char * pFile, int nLine )
{
	if( !dest || !src )
	{
		PointerError( "strcat", pFile, nLine );
		return NULL;
	}

	size_t offset = 0;
	NODE * pNode;
	void * pMem = dest;

	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
	}
	else
	{
		// is pointer within the allocated memory block
		if( !(pNode = IsPointerOffset( pMem, &offset )) )
			return strcat( dest, src );
	}

	// check the length
	int len = strlen(src) + strlen(dest) + offset;
	if(	len > pNode->size -1)
	{
		BoundsError( pNode, "strcat", pFile, nLine );
		return NULL;
	}
	else
	{
		return strcat( dest, src );
	}
}

// Wrapper for CRT strncat()
// Appends a string.
char * tstrncat(char * restrict dest, const char * restrict src, size_t length, char * pFile, int nLine)
{
	if( !dest || !src )
	{
		PointerError( "strncat", pFile, nLine );
		return NULL;
	}

	size_t offset = 0;
	NODE * pNode;
	void * pMem = dest;

	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
	}
	else
	{
		// is pointer within the allocated memory block
		if( !(pNode = IsPointerOffset( pMem, &offset )) )
			return strncat( dest, src, length );
	}

	// check the length
	int len = strlen(src) + length + offset;
	if(	len > pNode->size -1 )
	{
		BoundsError( pNode, "strncat", pFile, nLine );
		return NULL;
	}
	else
	{
		return strncat( dest, src, length );
	}
}

// Wrapper for CRT strncpy()
// Copies characters from one string to another.
char * tstrncpy(char * restrict dest, const char * restrict src, size_t length, char * pFile, int nLine)
{
	if( !dest || !src )
	{
		PointerError( "strncpy", pFile, nLine );
		return NULL;
	}

	size_t offset = 0;
	NODE * pNode;
	void * pMem = dest;

	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
	}
	else
	{
		// is pointer within the allocated memory block
		if( !(pNode = IsPointerOffset( pMem, &offset )) )
	 		return strncpy( dest, src, length );
	}

	// check the length
	int len = length + offset;
	if(	len > pNode->size -1 )
	{
		BoundsError( pNode, "strncpy", pFile, nLine );
		return NULL;
	}
	else
	{
		return strncpy( dest, src, length );
	}
}

// Wrapper for CRT memcpy()
// Copies characters between two memory buffers.
void * tmemcpy( void * dest, const void * src, size_t length, char * pFile, int nLine )
{
	if( !dest || !src )
	{
		PointerError( "memcpy", pFile, nLine );
		return NULL;
	}

	size_t dest_offset = 0;
	size_t src_offset  = 0;
	int bDest = 0, bSrc = 0;
	NODE * pNodeDest = NULL, * pNodeSrc = NULL;

	// is either of the pointers a base address of tmalloc memory block
	if ( (bDest = IsValidNode( dest ) ) != 0 )
		pNodeDest = (NODE*)dest-1;

	if ( (bSrc = IsValidNode( src )) != 0 )
		pNodeSrc = (NODE*)src-1;


	// if not, is the address within a tmalloc memory block
	if(!pNodeDest)
		pNodeDest = IsPointerOffset( dest, &dest_offset );

	if(!pNodeSrc)
		pNodeSrc = IsPointerOffset( src, &src_offset );

	// ------------------------------------------------------------
	// First case - dest and src are within the same tmalloc block
	// ------------------------------------------------------------
	if( pNodeDest && ( pNodeDest == pNodeSrc ) )
	{
		if( length + dest_offset <= pNodeDest->size && length + src_offset <= pNodeSrc->size )
		{
			return memcpy( dest, src, length );
		}
		else
		{
			BoundsError( pNodeDest, "memmove", pFile, nLine );
			return NULL;
		}
	}

	// -----------------------------------------------------------
	// Next case, destination is within a tmalloc block, source
	// is not within a tmalloc block
	// -----------------------------------------------------------
	if ( bDest && !pNodeSrc )
	{
		if( length + dest_offset <= pNodeDest->size )
			return memcpy( dest, src, length );
		else
		{
			BoundsError( pNodeDest, "memmove", pFile, nLine );
			return NULL;
		}
	}

	// -----------------------------------------------------------
	// Next case, destination is not within a tmalloc block, source
	// is within a tmalloc block
	// -----------------------------------------------------------
	if ( !pNodeDest && ( bSrc || pNodeSrc ) )
	{
		if( length + src_offset <= pNodeSrc->size )
			return memmove( dest, src, length );
		else
		{		
			BoundsError( pNodeDest, "memmove", pFile, nLine );
			return NULL;
		}
	}

	// -----------------------------------------------------------
	// ELSE case
	// -----------------------------------------------------------
	return memcpy( dest, src, length );
}

// Wrapper for CRT memmove()
// Copies characters between two overlapping memory buffers.
void * tmemmove( void * dest, const void * src, size_t length, char * pFile, int nLine )
{
	if( !dest || !src )
	{
		PointerError( "memmove", pFile, nLine );
		return NULL;
	}

	size_t dest_offset = 0;
	size_t src_offset  = 0;
	int bDest = 0, bSrc = 0;
	NODE * pNodeDest = NULL, * pNodeSrc = NULL;

	// is either of the pointers a base address of tmalloc memory block
	if ( (bDest = IsValidNode( dest )) != 0 )
		pNodeDest = (NODE*)dest-1;

	if ( (bSrc = IsValidNode( src )) != 0 )
		pNodeSrc = (NODE*)src-1;

	// if not, is the address within a tmalloc memory block
	if(!pNodeDest)
		pNodeDest = IsPointerOffset( dest, &dest_offset );

	if(!pNodeSrc)
		pNodeSrc = IsPointerOffset( src, &src_offset );

	// ------------------------------------------------------------
	// First case - dest and src are within tmalloc blocks
	// ------------------------------------------------------------
	if( pNodeDest && ( pNodeDest == pNodeSrc ) )
	{
		if( length + dest_offset <= pNodeDest->size && length + src_offset <= pNodeSrc->size )
		{
			return memmove( dest, src, length );
		}
		else
		{
			BoundsError( pNodeDest, "memmove", pFile, nLine );
			return NULL;
		}
	}

	// -----------------------------------------------------------
	// Next case, destination is within a tmalloc block, source
	// is not within a tmalloc block
	// -----------------------------------------------------------
	if ( bDest && !pNodeSrc )
	{
		if( length + dest_offset <= pNodeDest->size )
			return memmove( dest, src, length );
		else
		{
			BoundsError( pNodeDest, "memmove", pFile, nLine );
			return NULL;
		}
	}

	// -----------------------------------------------------------
	// Next case, destination is not within a tmalloc block, source
	// is within a tmalloc block
	// -----------------------------------------------------------
	if ( !pNodeDest && ( bSrc || pNodeSrc ) )
	{
		if( length + src_offset <= pNodeSrc->size )
			return memmove( dest, src, length );
		else
		{
			BoundsError( pNodeDest, "memmove", pFile, nLine );
			return NULL;
		}
	}

	// -----------------------------------------------------------
	// ELSE case
	// -----------------------------------------------------------
	return memmove( dest, src, length );
}

// size of printf
int sopf( const char *fmt, va_list ap )
{
	// measure the required size including
	// the null termination
	return vsnprintf( NULL, 0, fmt, ap );
}

// Wrapper for CRT sprintf()
// Writes formatted data to a string.
int tsprintf( char * pFile, int nLine, char * dest, const char * format, ... )
{
	if( !dest )
	{
		PointerError( "sprintf", pFile, nLine );
		return 0;
	}

	// get the requested size of fornmated string
	va_list pArgs;
	va_start( pArgs, format );
	int len = sopf( format, pArgs );
	va_end( pArgs );

	size_t offset = 0;

	NODE * pNode;
	void * pMem = dest;

	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
	}
	else
	{
		if( !(pNode = IsPointerOffset( pMem, &offset )) )
		{
			int ret;
			va_start( pArgs, format );
			ret = vsprintf( dest, format, pArgs );
			va_end( pArgs );
			return ret;
		}
	}

	len += offset;			// offset maybe set in IsPointerOffset()

	if(	len > pNode->size -1 )
	{
		BoundsError( pNode, "sprintf", pFile, nLine );
		return 0;
	}
	else
	{
		int ret;
		va_start( pArgs, format );
		ret = vsprintf( dest, format, pArgs );
		va_end( pArgs );
		return ret;
	}
}

// Wrapper for CRT snprintf()
// Writes formatted data to a string.
int tsnprintf( char * pFile, int nLine, char * dest, size_t maxlength, const char * format, ... )
{
	if( !dest )
	{
		PointerError( "snprintf", pFile, nLine );
		return 0;
	}

	// get the requested size of fornmated string
	va_list pArgs;
	va_start( pArgs, format );
	int len = sopf( format, pArgs );
	va_end( pArgs );

	size_t offset = 0;

	NODE * pNode;
	void * pMem = dest;

	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
	}
	else
	{
		if( !(pNode = IsPointerOffset( pMem, &offset )) )
		{
			int ret;
			va_start( pArgs, format );
			ret = vsnprintf( dest, maxlength, format, pArgs );
			va_end( pArgs );
			return ret;
		}
	}

	if(	len + offset > pNode->size -1 )
	{
		BoundsError( pNode, "snprintf", pFile, nLine );
		return 0;
	}
	else
	{
		int ret;
		va_start( pArgs, format );
		ret = vsnprintf( dest, maxlength, format, pArgs );
		va_end( pArgs );
		return ret;
	}
}

//=========================================================
// Wide Character functions
//=========================================================

// Wrapper for CRT wcscpy()
// Copies characters from one string to another.
wchar_t * twcscpy( wchar_t * dest, const wchar_t * src, char * pFile, int nLine )
{
	if( !dest || !src )
	{
		PointerError( "wcscpy", pFile, nLine );
		return NULL;
	}

	size_t offset = 0;
	NODE * pNode;
	void * pMem = dest;

	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
	}
	else
	{
		if( !(pNode = IsPointerOffset( pMem, &offset )) )
		{
			return wcscpy( dest, src );
		}
	}

	// check the length (wide chars)
	int len = wcslen( src ) * 2 + offset;
	if(	len > pNode->size -2 )
	{
		BoundsError( pNode, "wcscpy", pFile, nLine );
		return NULL;
	}
	else
	{
		return wcscpy( dest, src );
	}
}

// Wrapper for CRT wcsncpy()
// Copies characters from one string to another.
wchar_t * twcsncpy( wchar_t * dest, const wchar_t * src, size_t length, char * pFile, int nLine )
{
	if( !dest || !src )
	{
		PointerError( "wcsncpy", pFile, nLine );
		return NULL;
	}

	size_t offset = 0;
	NODE * pNode;
	void * pMem = dest;

	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
	}
	else
	{
		if( !(pNode = IsPointerOffset( pMem, &offset )) ){
	 		return wcsncpy( dest, src, length );
		}
	}

	// check the length (wide chars)
	int len = length * 2 + offset;
	if(	len > pNode->size -2 )
	{
		BoundsError( pNode, "wcsncpy", pFile, nLine );
		return NULL;
	}
	else
	{
		return wcsncpy( dest, src, length );
	}
}

// Wrapper for CRT wcscat()
// Appends a string.
wchar_t * twcscat( wchar_t * dest, const wchar_t * src, char * pFile, int nLine )
{
	if( !dest || !src )
	{
		PointerError( "wcscat", pFile, nLine );
		return NULL;
	}

	size_t offset = 0;
	NODE * pNode;
	void * pMem = dest;

	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
	}
	else
	{
		if( !(pNode = IsPointerOffset( pMem, &offset )) )
		{
			return wcscat( dest, src );
		}
	}

	// check the length
	int len = (wcslen(src) * 2) + (wcslen(dest) * 2) + offset;
	if(	len > pNode->size -2)
	{
		BoundsError( pNode, "wcscat", pFile, nLine );
		return NULL;
	}
	else
	{
		return wcscat( dest, src );
	}
}

// Wrapper for CRT wcsncpy()
// Appends a string.
wchar_t * twcsncat( wchar_t * restrict dest, const wchar_t * restrict src, size_t length, char * pFile, int nLine )
{
	if( !dest || !src )
	{
		PointerError( "wcsncat", pFile, nLine );
		return NULL;
	}

	size_t offset = 0;
	NODE * pNode;
	void * pMem = dest;

	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
	}
	else
	{
		if( !(pNode = IsPointerOffset( pMem, &offset )) )
		{
		 	return wcsncat( dest, src, length );
		}
	}

	// check the length (wide chars)
	int len = (wcslen(dest) * 2) + (length * 2) + offset;
	if(	len > pNode->size -2 )
	{
		BoundsError( pNode, "wcsncat", pFile, nLine );
		return NULL;
	}
	else
	{
		return wcsncat( dest, src, length );
	}
}

// Wrapper for CRT wcstombs()
// Converts a wide character string to a multibyte string.
size_t twcstombs( char * mbstr, const wchar_t * wcstr, size_t length, char * pFile, int nLine )
{
	if( !mbstr || !wcstr )
	{
		PointerError( "wcstombs", pFile, nLine );
		return 0;
	}

	size_t offset = 0;
	NODE * pNode;
	void * pMem = mbstr;

	if ( IsValidNode( pMem ) ) 
	{
		pNode = (NODE*)pMem-1;
	}
	else
	{
		if( !(pNode = IsPointerOffset( pMem, &offset )) ){
			return wcstombs( mbstr, wcstr, length );
		}
	}

	// check the length (chars)
	int len = length + offset;
	if(	len > pNode->size -1 )
	{
		BoundsError( pNode, "wcstombs", pFile, nLine );
		return 0;
	}
	else
	{
		return wcstombs( mbstr, wcstr, length );
	}
}

