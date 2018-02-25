// tmalloc.h
//
// John Findlay <postmaster@johnfindlay.plus.com>
//
// TMalloc version 2.0 (14th July 2003)
//
//---------------------------------------------------------
// It can be difficult to track down memory leaks when using
// malloc especially in large projects.
//
// tmalloc should be used for that purpose.
//
// Generally, call report_heap() just before your app
// terminates, this will report any allocated memory blocks
// that were not released, giving the file/module name and
// line number where the memory was allocated.
//----------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef _TMALLOC_
#define _TMALLOC_

// Function prototypes of the library
//==============================================
// do not use these, use the macros instead
//==============================================
//
// If the pointer passed to tfree() (free) is
// not a valid pointer a MessageBox is displayed
// and the operation (free) will be aborted.
void tfree( void *, char*, int );
//
// report_heap() can be used without passing a
// parameter in which case it will print to stdout
// or with a file name, it will then print
// to the specified file. Use without filename with
// console apps and with filename for Windows apps.
//
// When reporting the filename/translation unit where
// malloc, calloc or realloc was called, only the first
// 30 chars will be used.
void report_heap( char * pOutFile = NULL );
//
// tmalloc() behaves as does malloc
void * tmalloc( size_t, char*, int );
//
// tcalloc() behaves as does calloc
void * tcalloc( size_t, size_t, char*, int );
//
// trealloc() behaves as does realloc
void * trealloc( void *, size_t, char*, int );
//
// node_description() displays a MessageBox showing the
// memory address, type (malloc, calloc, realloc),
// the size of the allocated block, translation unit
// and the line number where the memory was allocated.
void node_description( void * );
//
// string functions. These functions can help when
// out-of-bounds copying is about to take place.
// If the passed memory pointer belongs to the heap
// created with tmalloc, checks are made on the size
// of the destination memory, if an over-run would
// occur the operation will be aborted and a
// MessageBox will report the problem.
// If the passed memory pointer does not belong to the
// heap created with tmalloc the call will be made
// without checks. Useful when passing memory created
// on the stack or global arrays for example.
//
// A check is also made for NULL pointers, the operation
// will be aborted and a MessageBox will report such.
char * tstrcpy( char *, const char *, char*, int );
char * tstrcat( char *, const char *, char *, int );
void * tmemcpy( void *, const void *, size_t, char *, int );
void * tmemmove( void *, const void *, size_t, char *, int );
char * tstrncat(char *, const char *, size_t, char *, int);
char * tstrncpy(char *, const char *, size_t, char *, int);

// sprintf types
int    tsprintf(char *, int, char *, const char *, ... );
int    tsnprintf(char *, int, char *, size_t, const char *, ... );

// wide character functions
wchar_t * twcscpy( wchar_t *t, const wchar_t *, char *, int );
wchar_t * twcsncpy( wchar_t *t, const wchar_t *, size_t, char *, int );
wchar_t * twcscat( wchar_t *, const wchar_t *, char *, int );
wchar_t * twcsncat( wchar_t *, const wchar_t *, size_t, char *, int );
size_t    twcstombs( char *, const wchar_t *, size_t, char *, int );

//==============================================
// Use these macros in user listing
//==============================================
// malloc, calloc, realloc and free and others are redefined
// here you may need to undef them depending on your system
#define malloc(n)        tmalloc( n, __FILE__, __LINE__ )
#define calloc(m,n)      tcalloc( m, n, __FILE__, __LINE__ )
#define free(n)          tfree( n, __FILE__, __LINE__ )
#define realloc(n, size) trealloc( n, size, __FILE__, __LINE__ )
#define strcpy(s, s1)    tstrcpy( s, s1, __FILE__, __LINE__ )
#define strcat(s, s1)    tstrcat( s, s1, __FILE__, __LINE__ )
#define memcpy(s, s1, n) tmemcpy( s, s1, n, __FILE__, __LINE__)
#define memmove(s, s1, n) tmemmove( s, s1, n, __FILE__, __LINE__)
#define strncat(s, s1, n) tstrncat( s, s1, n, __FILE__, __LINE__)
#define strncpy(s, s1, n) tstrncpy( s, s1, n, __FILE__, __LINE__)

// ---------------------------------------------------------------
// IF your compiler supports variable args in macros un-comment
// the following snprintf macro
// ---------------------------------------------------------------
//#define sprintf(s, s1, ... ) tsprintf(__FILE__, __LINE__, s, s1, ... )
//#define snprintf(s, n, s1, ... ) tsnprintf(__FILE__, __LINE__, s, n, s1, ... )
//
// ELSE use tmalloc equivalents - tsprintf, tsnprintf
//
// To use these add the __FILE and __LINE__ parameters yourself
//
// tsnprintf(__FILE__, __LINE__, s, n, s1, ... )
//
//
// wide character functions
#define wcscpy( s, s1 )     twcscpy( s, s1, __FILE__, __LINE__ )
#define wcsncpy( s, s1, n ) twcsncpy( s, s1, n, __FILE__, __LINE__ )
#define wcscat( s, s1 )     twcscat( s, s1, __FILE__, __LINE__ )
#define wcsncat( s, s1, n)  twcsncat( s, s1, n, __FILE__, __LINE__ )
#define wcstombs( s, s1, n) twcstombs( s, s1, n, __FILE__, __LINE__ )

#endif //_TMALLOC_

