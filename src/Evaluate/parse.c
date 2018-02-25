/******************************************************************
					Parse.c - Expression Parser

	In order to parse and evaluate a string call Evaluate(Str)
	where Str is the string to be evaluated.  For example,

	Evaluate("15/3");

	Evaluate returns a double of the evaluated string, in this case
	5.  All memory management is conducted in the library all you have
	to worry about is passing a valid expression.

	If DEBUG is defined in parse.h then for any errors that occur a
	message will be displayed to the console window.  This is useful
	for debugging purposes.  If an error occurs, Evaluate returns
	P_ERR, defined in parse.h.

	Designed and Programmed by Sean O'Rourke.
	http://backfrog.digitalrice.com
******************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include "parse.h"

/******************************************************************
	* Memory Manager *

	AllocMem and the macro FreeMem make up the memory manager.  Any
	time memory	is allocated in the library it is done through this
	function.  This allows errors and memory leaks to be tracked
	easily.

	AllocMem allocates memory in the size of Amt.  It also clears the
	memory to zero and returns the pointer to that memory.
******************************************************************/
static void *AllocMem(unsigned int Amt)
{
	void *Mem = malloc(Amt);
	if(Mem==NULL){
		#ifdef DEBUG
			printf("Memory Allocation error.");
		#endif
		return NULL;
	}
	memset(Mem, 0, Amt);
	return Mem;
}

/******************************************************************
	* Flag String *

	FlagString takes the source string src and parses it into flags
	based on parenthesis, operators, and values.  Memory is allocated
	for a new string, Str, which contains the tokens of src and is
	then returned.
******************************************************************/
static char *FlagString(char *src)
{
	unsigned int srcPos = 0;
	int Brackets = 0, LastOp = FALSE;
	char *Str = AllocMem(strlen(src)*P_FACTOR);
	if(Str==NULL){
		return NULL;
	}
	for(srcPos=0;srcPos<strlen(src);srcPos++){
		if(src[srcPos]=='('){
			Brackets++;
			sprintf(Str, "%s<o%i>", Str, Brackets);
			LastOp = FALSE;
		}
		else if(src[srcPos]==')'){
			LastOp = FALSE;
			Brackets--;
			if(Brackets<0){
				#ifdef DEBUG
					printf("Brackets error.");
				#endif
				FreeMem(Str);
				return NULL;
			}
			sprintf(Str, "%s<c%i>", Str, Brackets+1);
		}
		else if(isspace(src[srcPos]))
			continue;
		else if(src[srcPos]=='^'){
			LastOp = FALSE;
			sprintf(Str, "%s<op1 ^>", Str);
		}
		else if(src[srcPos]=='*' || src[srcPos]=='/'){
			LastOp = FALSE;
			sprintf(Str, "%s<op2 %c>", Str, src[srcPos]);
		}
		else if(src[srcPos]=='-' || src[srcPos]=='+'){
			if(LastOp==FALSE && src[srcPos]=='-'){
				srcPos++;
				sprintf(Str, "%s<val -%g>", Str, atof(&src[srcPos]));
				while(src[srcPos]=='.' || isdigit(src[srcPos]) || src[srcPos]=='E' || src[srcPos]=='e')
					srcPos++;
				srcPos--;
				LastOp = TRUE;
			}
			else{
				sprintf(Str, "%s<op3 %c>", Str, src[srcPos]);
				LastOp = FALSE;
			}
		}
		else if(src[srcPos]=='.' || isdigit(src[srcPos])){
			LastOp = TRUE;
			sprintf(Str, "%s<val %g>", Str, atof(&src[srcPos]));
			while(src[srcPos]=='.' || isdigit(src[srcPos]) || src[srcPos]=='E' || src[srcPos]=='e')
				srcPos++;
			srcPos--;
		}
		else{
			#ifdef DEBUG
				printf("Undefined symbol.");
			#endif
			FreeMem(Str);
			return NULL;
		}
	}
	if(Brackets!=0){
		#ifdef DEBUG
			printf("Brackets error");
		#endif
		FreeMem(Str);
		return NULL;
	}
	return Str;
}

/******************************************************************
	* Tool Functions *

	The tool functions are made up by CountTags, StrTrim, and
	ReplaceStrings.

	CountTags counts the number of tags in a given string Str.  Each
	open bracket consists of one tag.

	StrTrim will destructively eliminate redundant spaces from
	the given string.

	ReplaceStrings takes a pointer to the value, a end value, q,
	and a start value, r.  It places the value tag of Value into
	the start position and gets rid of everything until the end tag
	near q.
******************************************************************/
static unsigned int CountTags(const char *Str)
{
	const char *p = Str;
	unsigned int Count = 0;
	while(p[0]!='\0'){
		if(p[0]=='<')
			Count++;
		p++;
	}
	return Count;
}

static int StrTrim(char *str)
{
    char *src = str,*dst = str;

    while (isspace(*src))
        src++;
    do {
        while (*src && !isspace(*src))
            *dst++ = *src++;
        if (*src) {
            *dst++ = *src++;
            while (isspace(*src))
                src++;
        }
    } while (*src);
    if (dst != src && isspace(dst[-1]))
        dst--;
    *dst = 0;
    return dst - src;
}

static void ReplaceString(double *Value, char *q, char *r)
{
	char Buffer[MAX_DIGIT+6];
	char *s;
	sprintf(Buffer, "<val %g>", *Value);
	s = r;
	while(s<q || s[0]!='>'){
		s[0] = ' ';
		s++;
	}
	s[0] = ' ';
	strncpy(r, Buffer, strlen(Buffer));
	StrTrim(r+strlen(Buffer));
	FreeMem(Value);
}

/******************************************************************
	* Calculate *

	Calculate takes a string which must not contain any parenthesis.
	It determines, based on the order of operation, the highest
	priority operation and performs it replacing the two values and
	the operation flag with the new value flag.  It then recursively
	calls it self until there are no more operations to perform
	and returns the value that is remaining.

	If an error occurs Calculate returns NULL.
******************************************************************/
static double *Calculate(char *Str)
{
	char *p, *q, *r;
	char Op = 0;
	double *Value = AllocMem(sizeof(double));
	if(Value==NULL)
		return NULL;

	#ifdef DEBUG
		printf("%s\n", Str);
	#endif

	if(CountTags(Str)==1)	{
		*Value = atof(&Str[5]);
		return Value;
	}

	p = strstr(Str, "<op1");
	if(p!=NULL){
		q = p;
		p--;
		while(p[0]!='<')
			p--;
		r = p;
		p += 5;
		*Value = atof(p);
		q++;
		while(q[0]!='<')
			q++;
		q += 5;
		*Value = pow(*Value, atof(q));
		ReplaceString(Value, q, r);
		return Calculate(Str);
	}

	p = strstr(Str, "<op2");
	if(p!=NULL){
		Op = *(p + 5);
		q = p;
		p--;
		while(p[0]!='<')
			p--;
		r = p;
		p += 5;
		*Value = atof(p);
		q++;
		while(q[0]!='<')
			q++;
		q += 5;
		if(Op=='*')
			*Value *= atof(q);
		else{
			if(atof(q)==0){
				#ifdef DEBUG
					printf("Divide by zero error.  ");
				#endif
				FreeMem(Value);
				return NULL;
			}
			*Value /= atof(q);
		}
		ReplaceString(Value, q, r);
		return Calculate(Str);
	}

	p = strstr(Str, "<op3");
	if(p!=NULL){
		Op = *(p + 5);
		q = p;
		p--;
		while(p[0]!='<')
			p--;
		r = p;
		p += 5;
		*Value = atof(p);
		q++;
		while(q[0]!='<')
			q++;
		q += 5;
		if(Op=='+')
			*Value += atof(q);
		else
			*Value -= atof(q);
		ReplaceString(Value, q, r);
		return Calculate(Str);
	}
	return NULL;
}

/******************************************************************
	* Parse *

	Parse takes a string that has been already flagged and determines
	if it has any paranthesis.  If paranthesis are detect Parse uses
	Calculate to replace each one with the appropriate value and
	continues until no parenthesis are left.  When this happens Parse
	returns the value Calculate returns.

	If an error occurs Parse returns NULL.
******************************************************************/
static double *Parse(char *Str)
{
	char *Start, *End, *Open;
	int Again = FALSE, High = 0;
	double* Value;

	for(int StrPos=0;StrPos<strlen(Str);StrPos++){
		if(Str[StrPos]=='<'){
			StrPos++;
			if(Str[StrPos]=='o' && isdigit(Str[StrPos+1])){
				StrPos++;
				if(atoi(&Str[StrPos])>=High){
					High = atoi(&Str[StrPos]);
					Open = &Str[StrPos-2];
					while(Str[StrPos]!='>')
						StrPos++;
					Start = &Str[++StrPos];
					Again = TRUE;
				}
			}
			else if(Str[StrPos]=='c'){
				StrPos++;
				if(atoi(&Str[StrPos])==High){
					End = &Str[StrPos-2];
					while(Str[StrPos]!='>')
						StrPos++;
				}
			}
		}
	}
	if(Again==TRUE){
		char *tmp = AllocMem(End-Start+2);
		if(tmp==NULL)
			return NULL;
		strncpy(tmp, Start, End-Start);
		tmp[End-Start+1] = '\0';
		Value = Parse(tmp);
		FreeMem(tmp);
		if(Value==NULL)
			return NULL;
		ReplaceString(Value, End, Open);
		return Parse(Str);
	}
	else {
		return Calculate(Str);
	}
}

/******************************************************************
	* Evaluate *

	Evaluate is the main function in the library.  Call Evaluate
	and pass the string to be evaluated.  If an error occurs then
	P_ERR is returned.
******************************************************************/
double Evaluate(char *src)
{
	double *Value;
	double Answer = 0;
	if(src==NULL || strlen(src)==0)
		return 0;
	char *Str = FlagString(src);
	if(Str==NULL)
		return P_ERR;

	#ifdef DEBUG
		printf("%s\n", Str);
	#endif

	Value = Parse(Str);
	if(Value==NULL){
		#ifdef DEBUG
			printf("Parse error.");
		#endif

		FreeMem(Str);
		return P_ERR;
	}
	Answer = *Value;
	FreeMem(Value);
	FreeMem(Str);
	return Answer;
}
