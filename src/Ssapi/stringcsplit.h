#ifndef __STRINGCSPLIT_H_
#define __STRINGCSPLIT_H_

/**
* ccSplit: array of stringc pointers that knows its length
*/
struct ccSplit
{
  struct stringc **array;
  size_t arraylen;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*!
   \fn struct ccSplit *ccStrSplit(const char *s, const char *splitstr);:

   \brief mimics perl's and javascript's Split function.

   Returns a ccSplit struct that represents a stringc array that
   knows its length.
   If there are two delimiter tokens following each other,
   an empty string is put in the returned array. (just like perl)
   a delimiter at the beginning or at the end also puts an extra
   item in the array.
   Example:
   "#string1#string2##string3#"      with a '#' delimiter
   This will create an array with 6 items containing the following:
   "\0", "string1", "string2", "\0", "string3", "\0"

   ccStrSplitStr reserves memory with malloc, the caller should release
   this memory with ccFreeSplitStr

   \param s: The string which should be split
   \param splitstr: A string containing all tokens that should be recognized as
                    a split token
*/

struct ccSplit *ccStrSplit(const char *s, const char *splitstr);

/*!
  \fn struct ccSplit *ccSplit(const struct stringc *s, const char *splitstr);
  \brief Behaves exactly like ccStrSplit, but expects a stringc struct
  \see ccStrSplit
*/
struct ccSplit *ccSplit(const struct stringc *s, const char *splitstr);

/*!
  \fn struct stringc *ccStrJoin(struct stringc *out, char sep, struct ccSplit *s);

  \brief Mimics perl's join() function.
  A ccSplit struct is converted into one string with a separator between every item
  Returns a stringc struct in the out parameter and by return value, so the function can
  be used in expressions.

  \param out: stringc *out, a client allocated stringc struct that will contain the resulting string
  \param sep: the separator character that will be put between every item
  \param s  : ccSplit *s, a split struct containing the strings to be joined
*/

struct stringc *ccStrJoin(struct stringc *out, char sep, struct ccSplit *s);

/*!
  \fn void ccFreeSplitStr(struct ccSplit *s);
  \brief Frees the ccSplit instance in a correct manner.
  It calls free_stringc() on all stringc structs in the array.
  Then it uses free() to free the array and then uses free() to free the ccSplit struct.

*/
void ccFreeSplitStr(struct ccSplit *s);

#ifdef __cplusplus
}
#endif // extern "C" __cplusplus

#endif

