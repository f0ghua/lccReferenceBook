#include <stdlib.h>
#include <string.h>
#include "stringcsplit.h"
#include "stringc.h"

static int s_ccSplitCount(const char *s, const char *splitstr);

static int s_ccSplitCount(const char *s, const char *splitstr)
{
  int count = 1;

  while( (s = strpbrk(s, splitstr)) != NULL )
  {
    s++;
    count++;
  }
  return count;
}

struct ccSplit *ccStrSplit(const char *s, const char *splitstr)
{
  const char *ptr;
  char *dup;
  size_t i;        
  struct ccSplit *ret = malloc(sizeof(struct ccSplit));

  if( !ret )
    return NULL;

  dup = strdup(s); // duplicate of s, because strtok puts '\0' at the tokenpositions
  if( !dup )
  {
    free(ret);
    return NULL;
  }

  ret->arraylen = s_ccSplitCount(s, splitstr);
  ret->array = malloc(ret->arraylen * sizeof(struct stringc *));
  if( !ret->array )
  {
		free(dup);
    free(ret);
    return NULL;
  }
  ptr = ccstrtok(dup, splitstr); // ret->arraylen > 0, so no NULL check
  for( i = 0; ptr && i < ret->arraylen; i++ )
  {
    ret->array[i] = ccAllocStringcChr(ptr);
    ptr = ccstrtok(NULL, splitstr);
  }                
  free(dup);

  return ret;
}

struct ccSplit *ccSplit(const struct stringc *s, const char *splitstr)
{
  return ccStrSplit(C_STR(s), splitstr);
}

struct stringc *ccStrJoin(struct stringc *out, char sep, struct ccSplit *s)
{
  size_t i;

  for( i = 0; i < s->arraylen-1; i++ )
  {
    ccStrCat(out, s->array[i]);
    ccStrnCatChr(out, &sep, 1);
  }
  ccStrCat(out, s->array[i]);

  return out;
}

void ccFreeSplitStr(struct ccSplit *s)
{
  size_t i;
  for( i = 0; i < s->arraylen; i++ )
    free_stringc(s->array[i]);

  free(s->array);
  free(s);
}

