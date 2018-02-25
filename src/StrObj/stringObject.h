// stringObject.h

#ifndef _STRING_OBJECT_H_
#define _STRING_OBJECT_H_

// Prototype for the string struct
typedef struct tagString String;

// Prototype for the Object constructor
String NewString(char * s = NULL);

#define FAIL 0xFFFFFFFFL      // (-1) certain string functions will return FAIL on error.

// the virtual table for the string objects
typedef struct StringVtbl {
	int  	(*StrCpy) (String & this, String & obj);
	int  	(*StrCpyc)(String & this, char * s);
	int  	(*StrCat) (String & this, String & obj);
	int  	(*StrCatc)(String & this, char * s);
	int  	(*StrChr) (String & this, unsigned char c, int start);
	int  	(*StrCmp) (String & this, String & obj);
	int  	(*StrCmpc)(String & this, char * s);
	int  	(*StrCmpi) (String & this, String & obj);
	int  	(*StrCmpci)(String & this, char * s);
	int  	(*StrLen) (String & this);
	char  * (*StrGet) (String & this);
	String  (*StrDup) (String & this);
	void 	(*StrFree)(String & this);
}StringVtbl;

struct tagString {
    StringVtbl * lpVtbl;	// the vtbl
	int     magic;			// used to verify that the struct has been initialised
	char *  user;			// pointer to array of chars to hold the C string
	int     max;			// max capacity (num chars)
	int     len;			// current length of C string
};

#endif //  _STRING_OBJECT_H_

