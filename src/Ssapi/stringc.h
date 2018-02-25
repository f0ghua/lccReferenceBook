/*!
 * File: stringc.h
 *
 * @version 1.0
 * @date 2002-06-05
 * @author Servé Laurijssen
 *
 * stringc structure declaration and functions to process stringc
 * also contains general string handling functions that operate on char *
 */


#ifndef __STRINGC_H_
#define __STRINGC_H_

#include <stddef.h>
#include <stdio.h>

/*!
 
  struct stringc:
 
  stringc is designed for safe handling of text in C
  The members should not be accessed directly! If there are
  functions missing add them to the library instead of producing
  them in the client code.
 
  The functions that operate on struct stringc follow the C stdlib convention
  but are prefixed with 'cc' and get a capital letter on every syllable
 
 e.g: strcat(char *, const char *) becomes ccStrCat(stringc *, const stringc *)
      strchr(const char *, const int) becomes ccStrChr(const stringc *, const int) 

 if a function needs to operate on stringc and char *, Chr is added
 to the end of the function.
 e.g: strcpy(char *, const char *) becomes ccStrCpyChr(stringc *, const char *)
       strcat(char *, const char *) becomes ccStrCatChr(stringc *, const char *)
*/

typedef struct stringc
{
	char *data;
	size_t length;
	size_t memsize;
} stringc;

#define C_STR(STR) (((struct stringc *)STR)->data)
#define C_LEN(STR) (((struct stringc *)STR)->length)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*!
  stringc_error()
  if an error occurs in any of the stringc functions, a description can be retreived with this function
*/
const char *ccStringcError(void);

/*!
  ccAllocStringc(len): function allocates a stringc struct and reserves len chars memory.
  If memory allocation fails, ccAllocStringc returns NULL
  \param len size_t unsigned int, amount of memory to allocate
*/
stringc *ccAllocStringc(const size_t len);

/*!
	\fn stringc *ccAllocStringcChr(const char *src);
	\brief Function allocates a stringc struct and reserves enough memory to hold the source string.

  If memory allocation fails, ccAllocStringcChr returns NULL. The space allocated includes the terminating NULL character.
  \param src const char *, pointer to memory to copy into the stringc
	\return a pointer to the newly created string or NULL.
*/
stringc *ccAllocStringcChr(const char *src);

/*!
	\fn void free_stringc(stringc *str);
  \brief function that frees all memory associated with a stringc pointer

  \param str pointer to stringc struct to free
*/
void free_stringc(stringc *str);

/*!
 ccStrCpy(): function behaves like strcpy, but you don't have to worry about memory anymore.
 ccLib reserves more memory if there's not enough available
 \param dest Stringc struct to copy the string into
 \param src  Stringc struct to copy the data from
*/
stringc *ccStrCpy(stringc *dest, const stringc *src);

/*!
 ccStrCpyChr(): function behaves like C-stdlib strcpy, but you don't have to worry about memory anymore.
 ccLib reserves more memory if there's not enough available
 \param dest Stringc struct to copy the string into
 \param src  const char *to copy the data from
*/
stringc *ccStrCpyChr(stringc *dest, const char *src);

/*!
 ccStrnCpyChr(): function behaves like C-stdlib strncpy, but you don't have to worry about memory anymore.
 ccLib reserves more memory if there's not enough available
 ccStrnCpyChr() copies maxlen characters into dest
 \param dest Stringc struct to copy the string into
 \param src  const char *to copy the data from
 \param maxlen the maximum numbers of characters copied into dest
*/
stringc *ccStrnCpyChr(stringc *dest, const char *src, const size_t maxlen);

/*!
 ccStrCat(): function behaves like C-stdlib strcat, but you don't have to worry about memory anymore.
 ccLib reserves more memory if there's not enough available
 adds the source string to the dest string
 \param dest
 \param src
*/
stringc *ccStrCat(stringc *dest, const stringc *src);

/*!
 ccStrCatChr(): function behaves like C-stdlib strcat, but you don't have to worry about memory anymore.
 ccLib reserves more memory if there's not enough available
 adds the source string to the dest string
 \param dest
 \param src
*/
stringc *ccStrCatChr(stringc *dest, const char *src);

/*!
 ccStrnCatChr(): function behaves like C-stdlib strncat, but you don't have to worry about memory anymore.
 ccLib reserves more memory if there's not enough available
 ccStrnCatChr() concatenates maxlen characters into dest
 \param dest Stringc struct to copy the string into
 \param src  const char *to copy the data from
 \param maxlen the maximum numbers of characters copied into dest
*/
stringc *ccStrnCatChr(stringc *dest, const char *src, const size_t maxlen);

/*!
 ccStrTrim: function removes all whitespace from dest and returns itself.
 isspace is internally used to determine if a character is whitespace.
 To change the characters that are considered whitespace, use the C function
 setlocale with the LC_CTYPE parameter. Default is '\t', ' ' and '\n'
 \param dest The string to be trimmed
*/
stringc *ccStrTrim(stringc *dest);

/*!
  ccStrSprintf: function behaves like C-stdlib sprintf, but memory is added
  as needed. 
  Extra formatstring feature: %cs adds a stringc struct to the destination stringc
  \param dest The destination string to send to
  \param fmt  The format string as with standard C. If good old Borland C 5 is used
              only %%d %%u %%ld %%c %%s %*c %*d and %% are supported (and %cs for stringc)    
*/
int ccStrSprintf(stringc *dest, const char *fmt, ...);

/*!
 ccStrChr: function behaves like C-stdlib strchr.
 It finds the first occurence of c in str and returns a pointer to it.
 If c is not found, it returns NULL
 \param str const stringc * to search
 \param c const int c is the character to find 
*/
char *ccStrChr(const stringc *str, const int c);

/*!
 ccStrrChr: function behaves like C-stdlib strrchr.
 It finds the last occurence of c in str and returns a pointer to it.
 If c is not found, it returns NULL
*/
char *ccStrrChr(const stringc *str, const int c);

/*!
  ccStrStr: function behaves like C-stdlib strstr
  it searches str for the first occurence of key
  and returns the index if it is found
*  If key is not found, it returns -1 
/
int ccStrStr(const stringc *str, const stringc *key);

/*!
  ccStrCmp: function behaves like C-stdlib strcmp
  it compares two stringc structs. returns zero when equal
  -1 when str is less than str2 and 1 when str is greater than str2   

*/
int ccStrCmp(const stringc *str, const stringc *str2);

/*!
  ccStrCmpChr: function behaves like C-stdlib strcmp
  it compares a stringc struct and a char *. returns zero when equal
  -1 when str is less than str2 and 1 when str is greater than str2

*/
int ccStrCmpChr(const stringc *str, const char *str2);

/*!
  ccSubStr: function returns a substring allocated with ccAllocString
  or NULL when a memory error occurs. The substring is a string starting
  at index 'start' until 'count' characters. If count points beyond the end
  the substring will start at 'start' until the end.
  \param const struct stringc * str   : The string to return a substring from
  \param size_t start           start : The start index
  \param size_t count           count : The number of characters to copy
*/
stringc *ccSubStr(const stringc *str, size_t start, size_t count);

/*!
	\fn stringc *ccStrReplace(stringc *str, const char *findstr, const char *replace);
  \brief The function replaces ALL occurences of findstr into replace string.
  \param str : the string to change
  \param findstr : The substring of str to find.
	\param replace: The string to replace findstr into.
*/
stringc *ccStrReplace(stringc *str, const char *findstr, const char *replace);

/*!
 \fn stringc *ccStrInsert(stringc *out, stringc *insert, size_t offset, size_t count);
 \brief Inserts count characters of stringc insert into out, starting at offset.

 If offset is bigger than the length of out, nothing is inserted.
 If count is bigger than C_LEN(insert), only C_LEN(insert) characters are inserted.
 An offset of 0 puts it before the first character of out.
*/

stringc *ccStrInsert(stringc *out, size_t offset, stringc *insert, size_t count);

/*!
   ccstrndup: function duplicates n characters of a string or the complete string
   if n > strlen(s).
   \param s The string to duplicate a substring from
   \param n Number of characters to duplicate, starting from s
*/
char *ccstrndup(const char *s, const size_t n);

/*!
*/
const char *ccstrtok(char *s, const char *splitstr);

#ifdef __cplusplus
}
#endif // extern "C" __cplusplus

#endif // __STRINGC_H_
