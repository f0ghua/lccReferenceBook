#include <stdio.h>
#include "stringObject.h"

//-------------------------------------------------------
// A small string library as an example of using a
// virtual function table and creating the object
// with method-like calls.
//-------------------------------------------------------
//
// john
//

int main(void)
{
	char s[] = "Hello";

	// Create a string Object.
	// The struct is returned, not a pointer to struct.
	String my = NewString("All the world");
	printf("%s\t%d\n\n", my.StrGet(), my.StrLen());

	// StrFree free's the array of chars and sets internal vars
	my.StrFree();

	// so using NewString with an empty string makes no difference
	my = NewString();

	// Test a StrCatc on an empty string. StrCpy will be called
	// if there is no memory allocated for the string.
	my.StrCatc("The World!");
	printf("%s\t%d\n\n", my.StrGet(), my.StrLen());

	// StrCpy will will use the previous memory if
	// it's large enough.
	my.StrCpyc(s);
	printf("%s\t\t%d\n\n", my.StrGet(), my.StrLen());

	// StrCatc again, will use realloc() if necassary.
	my.StrCatc(" World!");
	printf("%s\t%d\n\n", my.StrGet(), my.StrLen());

	// Duplicate the first string
	String yr = my.StrDup();
	printf("%s\t%d\n\n", yr.StrGet(), yr.StrLen());

	my.StrCat(yr);
	printf("%s\t%d\n\n", my.StrGet(), my.StrLen());

	my.StrCatc("È");
	printf("'!' at position %d in string object 'my'.\n", my.StrChr('!', 12));

	// Destory the strings. Frees any memory and sets internal vars.
	my.StrFree();
	yr.StrFree();
	return 0;
}
