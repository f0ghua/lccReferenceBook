// tcheck.c
//
// John Findlay <john.findlay1@btinternet.com>
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

// including tmalloc.h here redefines malloc to tmalloc etc.
#include "tmalloc.h"

void test_stackmem(void)
{
	// To check stack memory for bounds errors use the following method.
	// Replace the static allocation char stackmem[10]; with a call
	// to malooc. 
#ifndef _TMALLOC_
	char stackmem[10];
#else
	char * stackmem = malloc( 10 );
#endif
	strcpy(stackmem, "Hello World!");
}

void test_strcpy(void)
{
	char stackmem[10];
	char * s = malloc( 10 );

	// this can't be checked as tmalloc does not know about the memory
	strcpy( stackmem, "Humanly" );

	// these two will be checked
	strcpy( s, "Humanly" );

	// this will be reported as a bounds error.
	// strcpy will not be called.
	strcpy( s+2, "Humanly" );

	char * ss = malloc( 16 );
	realloc(ss, 300);
	// another bounds error
	strncpy( ss, "Humanly checked ", 16 );

	free ( s );

//	free( ss ); // oops, forgot to free ss
}

void test_strcat(void)
{
	char * s = malloc( 10 );

	char ss[10];

	// this can't be checked as tmalloc does not know about the memory
	ss[0] = 0;
	strcat( ss, "Humanly" );

	// this will be checked
	strncat( s, "Humanly", 7 );

	free ( s );
}

void test_memcpy(void)
{
	char * z  = malloc( 100 );
	char buffer[32];
	char buffer1[32];

	memmove(z, buffer, 32);
	memmove(buffer, z+49, 32);

	// not a tmalloc block, can't be checked
	memmove( buffer, buffer1, 32 );

	// both these cause bounds error
	memcpy( z , z + 80, 32 );
	memcpy( z + 69, z, 32 );

	free( z );
}

void test_sprintf(void)
{
	char * a = malloc(60);

	// sprintf can't be done as a macro until
   	// variable macro args are supported, see tmalloc.h
#ifdef _TMALLOC_
	tsprintf(__FILE__, __LINE__, a, "Hello %lld %x", 12349876ll, 98765);
	tsnprintf(__FILE__, __LINE__, a, 20, "Hello %Lf %x", 122349654.98765L, 98765);
#else
	sprintf( a, "Hello %lld %x", 12349876ll, 98765);
	snprintf( a, 20, "Hello %Lf %x", 122349654.98765L, 98765);
#endif

}

void test_wcscpy(void)
{
	wchar_t * a = malloc(12);
	wchar_t w[] = L"Hello";

	// bounds error
	wcscpy( a, w );

	// bounds error
	wcsncpy( a, w, 5 );

	// ok.
	a[4] = L'\0';
	wcsncat( a, w, 1 );

	// not ok.
	char * s = calloc( 1, 6 );
	wcstombs( s, w, 6 );
}

