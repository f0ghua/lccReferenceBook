//
// test_tmalloc.c
//
// John Findlay
//
#include <stdio.h>
#include <stdlib.h>  // for malloc and friends
#include "tmalloc.h" // tmalloc should be included after stdlib.h
#include "tcheck.h"  // test calls in tcheck.c

//----------------------------------------------------------------
// Make project with 'test_tmalloc.c', 'tmalloc.c' and 'tcheck.c'
// The name of translation unit is stored and will be truncated to 30 chars.
//------------------------------------------
//  Including tmalloc.h redefines malloc and friends and a few other calls.
//
//  malloc()   -> tmalloc()
//  calloc()   -> tcalloc()
//  realloc()  -> trealloc()
//  free()     -> tfree()
//
//  String functions
//  strcpy()   -> tstrcpy()
//  strcat()   -> tstrcat()
//  memcpy()   -> tmemcpy()
//  memmove()  -> tmemmove()
//  strncat()  -> tstrncat()
//  strncpy()  -> tstrncpy()
//
//  Wide Character functions
//	wcscpy()   -> twcscpy()
//	wcscat()   -> twcscat()
//	wcsncpy()  -> twcsncpy()
//	wcsncat()  -> twcsncat()
//  wcstombs() -> twcstombs()
//
//  These next two can't be replaced with macro's so use
//  the preprocessor, see tcheck.c
//  -----------------------------------------------------
//  sprintf()   xx tsprintf()
//  snprintf()  xx tsnprintf()
//
// When testing is finished, do not include tmalloc.h then all
// functions will be normal again.
//
// Include tmalloc.h for each translation unit that uses these
// functions if you want them checked while debugging.
//
// John Findlay <postmaster@johnfindlay.plus.com>
//
//-------------------------------------------
int main( void )
{
//  Routines situated in tcheck.c
	test_strcpy();
	test_memcpy();
	test_strcat();
	test_sprintf();
	test_wcscpy();

	char buf[90];
	free(buf);	// an error because the memory was not allocated with tmalloc

	test_stackmem(); // see routine in tcheck.c

	// Final heap report showing what has not been free-ed.
	// The memory address, type of allocation, size of allocated memory,
	// line number where allocation was made, and the translation unit
	// are displayed, or if no console, written to specified file.
	report_heap();

    return 0;

}

