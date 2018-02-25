#ifndef __time_gs_h__
#define __time_gs_h__

#include <time.h>

struct tm *gmtime_GS(time_t *ptt_time);
struct tm *localtime_GS(time_t *ptt_time);
time_t 	   mktime_GS(struct tm *ptm);
struct tm *msoTimeEx(double lfDate, time_t *ptt_time);

#define gmtime(pt)	  	gmtime_GS((pt))
#define localtime(pt)	localtime_GS((pt))
#define mktime(pt)		mktime_GS((pt))
#define msoTime(lfDate) msoTimeEx((lfDate), NULL)

#endif //__time_gs_h__
