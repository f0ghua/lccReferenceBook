// stringObject.c
//
//-------------------------------------------------------
// A small string library as an example of using a
// virtual function table and creating the object
// with method-like calls.
//-------------------------------------------------------
//
// john
//

#include <malloc.h>
#include <string.h>
#include "stringObject.h"

#define MAGIC 0x62943283L

// Local prototype's for the string object functions.
// Using references to struct.
static int 	   StrCpy(String & this, String & obj);
static int 	   StrCpyc(String & this, char * s);
static int     StrCat(String & this, String & obj);
static int     StrCatc(String & this, char * s);
static int     StrChr(String & this, unsigned char c, int start);
static int 	   StrCmp(String & this, String & obj);
static int 	   StrCmpc(String & this, char * s);
static int 	   StrCmpi(String & this, String & obj);
static int 	   StrCmpci(String & this, char * s);
static int     StrLen(String & this);
static char *  StrGet(String & this);
static String  StrDup(String & this);
static void    StrFree(String & this);

// make the virtual table
StringVtbl lpVtbl = {
	StrCpy, StrCpyc, StrCat, StrCatc, StrChr, StrCmp, StrCmpc,
	StrCmpi, StrCmpci, StrLen, StrGet, StrDup, StrFree
};

// Will make a new sting object. If no char * is passed
// no memory will be allocated.
String NewString(char * s = NULL)
{
	String this;

	// Init the vars
	this.lpVtbl = &lpVtbl;			// address of the virtual table
	this.magic  = MAGIC;			// can be used to check if valid
	this.user   = NULL;				// array of chars to hold the string
	this.len    = 0;				// current length of C string
	this.max    = 0;				// max capacity
	if(s){
		this.StrCpyc(s);
	}
	return this;					// copy the struct to the caller.
}

// Duplicate this
static String StrDup(String & this)
{
	return NewString(this.user);
}

static void StrFree(String & this)
{
	if(this.magic != MAGIC){
		return;
	}

   if(this.user) free(this.user);
   this.len  = this.max = 0;
   this.user = NULL;
}

// The length of the string is stored in this.len when
// using StrCpy or StrCat.
static int StrLen(String & this)
{
	if(this.magic == MAGIC){
		return this.len;
	}else{
		return FAIL;
	}
}

static int StrChr(String & this, unsigned char c, int start)
{
	// more checks could be done here
	if(this.magic != MAGIC || this.len == 0){
		return FAIL;
	}
	int pos = start;
	unsigned char * s = this.user + pos;
	while(*s){
		if(*s++ == c){
			return pos;
		}
		pos++;
	}
	return FAIL;
}

static int StrCmp(String & this, String & obj)
{
	// more checks could be done here
	if(this.magic != MAGIC || this.len == 0){
		return FAIL;
	}
	return strcmp(this.user, obj.user);
}

static int StrCmpi(String & this, String & obj)
{
	// more checks could be done here
	if(this.magic != MAGIC || this.len == 0){
		return FAIL;
	}
	return strcmpi(this.user, obj.user);
}

static int StrCmpc(String & this, char * s)
{
	if(this.magic != MAGIC || this.len == 0){
		return FAIL;
	}
	return strcmp(this.user, s);
}

static int StrCmpci(String & this, char * s)
{
	if(this.magic != MAGIC || this.len == 0){
		return FAIL;
	}
	return strcmpi(this.user, s);
}

static int StrCpy(String & this, String & obj)
{
	char * tmp;
	if(this.magic != MAGIC){
		return FAIL;
	}

	int len = strlen(obj.user);
	if(this.max < len){
		if(this.user)
			free(this.user);
		tmp = malloc(len + 1);
		if(!tmp){
			return FAIL;
		}else{
			this.user = tmp;
			this.len = this.max = len;
			strcpy(this.user, obj.user);
			return this.len;
		}
	}else{
		this.len = len;
		strcpy(this.user, obj.user);
		return this.len;
	}
}

static int StrCpyc(String & this, char * s)
{
	char * tmp;
	if(this.magic != MAGIC){
		return FAIL;
	}

	int len = strlen(s);
	if(this.max < len){
		if(this.user)
			free(this.user);
		tmp = malloc(len + 1);
		if(!tmp){
			return FAIL;
		}else{
			this.user = tmp;
			this.len = this.max = len;
			strcpy(this.user, s);
			return this.len;
		}
	}else{
		this.len = len;
		strcpy(this.user, s);
		return this.len;
	}
}

static int StrCat(String & this, String & obj)
{
	if(this.magic != MAGIC){
		return FAIL;
	}

	char * tmp;
	// if there is no string memory, use StrCpy
	if(!this.user){
		return this.StrCpyc(obj.user);
	}

	int len = obj.len;
	int total = this.len + len;
	if(this.max < total){
		tmp = realloc( this.user, total + 1 );
		if(!tmp){
			return FAIL;
		}else{
			this.user = tmp;
			this.len  = this.max = total;
			strcat(this.user, obj.user);
			return this.len;
		}
	}else{
		this.len += len;
		strcat(this.user, obj.user);
		return this.len;
	}
}

static int StrCatc(String & this, char * s)
{
	if(this.magic != MAGIC){
		return FAIL;
	}

	char * tmp;
	// if there is no string memory, use StrCpy
	if(!this.user){
		return this.StrCpyc(s);
	}

	int len = strlen(s);
	int total = this.len + len;
	if(this.max < total){
		tmp = realloc( this.user, total + 1 );
		if(!tmp){
			return FAIL;
		}else{
			this.user = tmp;
			this.len  = this.max = total;
			strcat(this.user, s);
			return this.len;
		}
	}else{
		this.len += len;
		strcat(this.user, s);
		return this.len;
	}
}

// StrGet returns the pointer to the array of chars
static char * StrGet(String & this)
{
	if(this.magic == MAGIC){
		return this.user;
	}else{
		return NULL;
	}
}

