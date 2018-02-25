#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <ctype.h>

#include "stringc.h"

#define MEMBLOCK 64

static char stringc_err[128] = {'\0'};

static void free_multiple(int n, ...);

static int ccStrSprintfBC5(stringc *dest, const char *fmt, va_list ap);
static int ccStrCheckMemSize(stringc *s, size_t size);

const char *ccStringcError(void)
{
	return stringc_err;
}

struct stringc *ccAllocStringc(const size_t len)
{
	struct stringc *str = malloc(sizeof *str);
	if( !str )
	{
		strcpy(stringc_err, "ccAllocStringc : memory error");
		return NULL;
	}

	str->data = malloc(len+1); // if len is zero we will have problems otherwise
	if( !str->data )
	{
		free(str);
		strcpy(stringc_err, "ccAllocStringc : memory error");
		return NULL;
	}

	str->data[0] = '\0';
	str->length = 0;
	str->memsize = len + 1; // if len is zero we will have problems otherwise

	return str;
}

stringc *ccAllocStringcChr(const char *src)
{
	const size_t srclen = strlen(src);
	stringc *str = ccAllocStringc(srclen+1);

	if( !str )
		return NULL;

	if( !src )
	{
		free_multiple(2, &str->data, &str);
		strcpy(stringc_err, "ccAllocStringcChr : src == NULL");
		return NULL;
	}

	strcpy(str->data, src);
	str->length = srclen;

	return str;
}

stringc *ccStrCpy(stringc *dest, const stringc *src)
{
  dest->length = 0;
  if( !ccStrCheckMemSize(dest, src->length) )
  {
	  strcpy(stringc_err, "ccStrCpy : memory error");
		return NULL;
	}
	strcpy(dest->data, src->data);
	dest->length = src->length;

	return dest;
}

stringc *ccStrCpyChr(stringc *dest, const char *src)
{
	const size_t srclen = strlen(src);
  dest->length = 0;
  if( !ccStrCheckMemSize(dest, srclen) )
  {
	 	strcpy(stringc_err, "ccStrCpyChr : memory error");
	 	return NULL;
	}
	strcpy(dest->data, src);
	dest->length = srclen;

	return dest;
}

stringc *ccStrnCpyChr(stringc *dest, const char *src, const size_t maxlen)
{
  const size_t srclen = strlen(src);

  dest->length = 0;
  if( !ccStrCheckMemSize(dest, maxlen) )
  {
    strcpy(stringc_err, "ccStrnCpyChr : memory error");
    return NULL;
  }
  strncpy(dest->data, src, maxlen);
  dest->length = (maxlen > srclen) ? srclen : maxlen;
  dest->data[dest->length] = '\0';

  return dest;
}

stringc *ccStrCat(stringc *dest, const stringc *src)
{
  if( !ccStrCheckMemSize(dest, src->length) )
  {
	  strcpy(stringc_err, "ccStrcat : memory error");
		return NULL;
	}
	strcpy(dest->data+dest->length, src->data); // strcpy is intentional
	dest->length += src->length;

	return dest;
}

stringc *ccStrCatChr(stringc *dest, const char *src)
{
	const size_t srclen = strlen(src);

  if( !ccStrCheckMemSize(dest, srclen) )
  {
		strcpy(stringc_err, "ccStrcatChr : memory error");
	  return NULL;
  }
	strcpy(dest->data+dest->length, src);  // strcpy is intentional
	dest->length += srclen;
	return dest;
}

stringc *ccStrnCatChr(stringc *dest, const char *src, const size_t maxlen)
{
  const size_t srclen = strlen(src);

  if( !ccStrCheckMemSize(dest, maxlen) )
  {
    strcpy(stringc_err, "ccStrnCatChr : memory error");
    return NULL;
  }

  strncpy(dest->data + dest->length, src, maxlen);
  dest->length += (maxlen > srclen) ? srclen : maxlen;
  dest->data[dest->length] = '\0';

  return dest;
}

char *ccStrChr(const stringc *str, const int c)
{
	return strchr(C_STR(str), c);
}

char *ccStrrChr(const stringc *str, const int c)
{
  size_t len = str->length;
  char *posptr = &C_STR(str)[len];

  while( *posptr != c && len > 0 )
  {
    len--;
    posptr--;
  }

  return len ? posptr : NULL;
}

int ccStrStr(const stringc *str, const stringc *key)
{
	const char *ptr = strstr(str->data, key->data);

	return (ptr != NULL ? (int)(ptr - str->data) : -1);
}

stringc *ccStrTrim(stringc *dest)
{
	ptrdiff_t diff;
	char *data = dest->data;

	while( *data && isspace(*data) )
		data++;

	diff = data - dest->data;

	if( diff > 0 )
	{
		memmove(dest->data, dest->data + diff, dest->length+1 - diff);
		dest->length -= diff;
	}

	data = dest->data + dest->length - 1;
	while( data >= dest->data && isspace(*data) )
		data--;

	if( data >= dest->data )
	{
		data[1] = '\0';
		dest->length = data - dest->data + 1;
	}
	return dest;
}

int ccStrSprintf(stringc *dest, const char *fmt, ...)
{
  va_list ap;
  int n;

  va_start(ap, fmt);

#if __BORLANDC__ > 0x520 || defined(_MSC_VER)

  while( (n = _vsnprintf(dest->data, dest->memsize, fmt, ap)) == -1 )
  {
    dest->data = realloc(dest->data, dest->memsize + MEMBLOCK);
    if( !dest->data )
      return -1;
    dest->memsize += MEMBLOCK;
  }
  dest->length = n;

  va_end(ap);
  return n;

#else

  n = ccStrSprintfBC5(dest, fmt, ap);
  va_end(ap);
  return n;

#endif
}

/**
  limited printf functionality.
*/
static int ccStrSprintfBC5(stringc *dest, const char *fmt, va_list ap)
{
  size_t ret = 0;
  char buf[64];

  dest->length = 0;
  dest->data[0] = '\0';

  while( *fmt )
  {
    if( *fmt == '%' )
    {
      ++fmt;
      switch( *fmt )
      {
        case '\0' :  // last char was %
          ccStrCatChr(dest, --fmt); // add '%' and dec fmt, so that the outer loop will end
          ret++;
        break;

        case 'c' :
        {
          if( *++fmt == 's' ) // extra %cs feature
          {
            stringc *s = va_arg(ap, stringc *);
            ccStrCat(dest, s);
            ret += C_LEN(s);
          }
          else
          {
            char c = va_arg(ap, char);
            ccStrnCatChr(dest, &c, 1);
            ret++;
            fmt--;
          }
        }
        break;

        case 'd' :
        {
          int i = va_arg(ap, int);
          itoa(i, buf, 10);
          ccStrCatChr(dest, buf);
          ret += strlen(buf);
        }
        break;

        case 'l' :
          fmt++;
          if( *fmt == 'd' )
          {
            long l = va_arg(ap, long);
            itoa(l, buf, 10);
            ccStrCatChr(dest, buf);
            ret += strlen(buf);
          }
        break;

        case 's' :
        {
          char *s = va_arg(ap, char *);
          ccStrCatChr(dest, s);
          ret += strlen(s);
        }
        break;

        case 'u' :
        {
          unsigned long l = va_arg(ap, unsigned long);
          itoa(l, buf, 10);
          ccStrCatChr(dest, buf);
          ret += strlen(buf);
        }
        break;

        case '%' :
          ccStrnCatChr(dest, fmt, 1);
          ret++;
        break;

        case '*' :
        {
          long i, num = va_arg(ap, long);
          for( i = 0; i < num; i++ )
          {
            char c = ' ';
            ccStrnCatChr(dest, &c, 1);
            ret++;
          }
          fmt++;
          if( *fmt == 'c' )
          {
            char c = va_arg(ap, char);
            ccStrnCatChr(dest, &c, 1);
            ret++;
          }
          else if( *fmt == 'd' )
          {
            int i = va_arg(ap, int);
            itoa(i, buf, 10);
            ccStrCatChr(dest, buf);
            ret += strlen(buf);
          }
        }
        break;
      }
    } // end if *fmt == '%'
    else
    {
      ccStrnCatChr(dest, fmt, 1);
      ret++;
    }
    fmt++;
  }
  return (int)ret;
}

int ccStrCmp(const stringc *str, const stringc *str2)
{
  return strcmp(str->data, str2->data);
}

int ccStrCmpChr(const stringc *str, const char *str2)
{
  return strcmp(str->data, str2);
}

stringc *ccSubStr(const stringc *str, size_t start, size_t count)
{
  if( start >= C_LEN(str) )
    return NULL;

  if( start + count >= C_LEN(str) )
    return ccAllocStringcChr(C_STR(str)+start);
  else
  {
    stringc *ret = ccAllocStringc(count);
    if( ret == NULL )
      return NULL;

    ccStrnCpyChr(ret, C_STR(str)+start, count);
    return ret;
  }
}

stringc *ccStrReplace(stringc *str, const char *findstr, const char *replace)
{
  const size_t findlen = strlen(findstr);
  struct stringc *temp = ccAllocStringc(C_LEN(str));
  const char *p        = C_STR(str), *oldp = C_STR(str);

  if( !temp )
    return NULL;

  while( (p = strstr(p, findstr)) != NULL )
  {
    ccStrnCatChr(temp, oldp, p - oldp);
    ccStrCatChr (temp, replace);

    p+=findlen;
    oldp = p;
  }
  ccStrCatChr(temp, oldp);

  ccStrCpy(str, temp);
  free_stringc(temp);

  return str;
}

void free_stringc(stringc *str)
{
	free(str->data);
	free(str);
}

static void free_multiple(int n, ...)
{
	va_list ap;

	va_start(ap, n);

	while( n-- > 0 )
	{
		char **arg = va_arg(ap, char **);
		free((char *)*arg);
		*arg = NULL;
	}

	va_end(ap);
}

stringc *ccStrInsert(stringc *out, size_t offset, stringc *insert, size_t count)
{
	if (offset <= C_LEN(out) && ccStrCheckMemSize(out, count))
	{
		size_t n = C_LEN(out) - offset + 1;
		stringc *tmp = ccAllocStringc(n);
		if (!tmp)
			return NULL;
		ccStrCpyChr(tmp, C_STR(out) + offset);

		count = min(count, C_LEN(insert));
		strncpy(C_STR(out) + offset, C_STR(insert), count);
		C_LEN(out) = count + offset;
		ccStrCat(out, tmp);

		free_stringc(tmp);

	}
	return out;
}

char *ccstrndup(const char *s, const size_t n)
{
  char *p;

  if( n >= strlen(s) )
    return strdup(s);

  if( (p = malloc(n+1)) == NULL )
    return NULL;

  strncpy(p, s, n);
  p[n] = '\0';

  return p;
}

/***
  Don't use ccStrTokChr within threads
***/
const char *ccstrtok(char *s, const char *splitstr)
{
  static size_t lastlength   = 0;
  static char *startptr			 = NULL;
  static char *currentstring = NULL;
  static char *currentptr    = NULL;
	char				*sptr;

  if( s ) // create the tokenized string if s != NULL
  {
    sptr    = currentstring = startptr = s;
    currentptr    = currentstring;
    lastlength    = strlen(s);
    while( (sptr = strpbrk(sptr, splitstr)) != NULL ) // sptr points to s, it puts a'\0' where a delimiter is
      *sptr++ = '\0';

		currentptr = &s[strlen(s)];
    return startptr;
  }
  else
  {
    sptr = currentptr;

    if( sptr >= &startptr[lastlength] )
      return NULL;

		++currentptr;
    currentptr = &currentptr[strlen(currentptr)];
    return sptr+1;
  }
}

/*
  checks if stringc has enough memory available to add size characters
  If s has 10 chars allocated and 5 in the string and ccStrCheckExtraSize
  is called with 1 for size, the function checks if (s->length + size < s->memsize).
  If there's not enough available, more memory is allocated.
*/
static int ccStrCheckMemSize(stringc *s, size_t size)
{
	char *ptr;
  if( s->memsize > s->length + size )
    return 1;

  // more memory is needed
  if( (ptr = realloc(s->data, s->memsize + size + MEMBLOCK)) == NULL )
    return 0;
	s->data = ptr;
  s->memsize += size + MEMBLOCK;
  return 1;
}
