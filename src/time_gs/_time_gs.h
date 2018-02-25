#ifndef ___time_gs_h__
#define ___time_gs_h__

#include <time.h>

#define	YEAR			365.0
#define LEAPYEAR		366.0

#define CURYEAR(y)		(((y)%4)?YEAR:LEAPYEAR)
#define ISLEAPYEAR(y)	(((y)%4)?0:1)

#define Y2K_MEZ_MSO		36526.0

#define Y2K_MEZ_SEC 	((time_t)946681200)
#define Y2K_GMT_SEC		((time_t)946684800)

#define	YEAR_SEC(y) 	((time_t)(((y)%4?365:366)*24*60*60))
#define DAY_SEC			((time_t)(24*60*60))
#define HOUR_SEC		((time_t)(60*60))
#define MIN_SEC	 		((time_t)60)

#define Y2K_WDAY		6
#define EPOCHE_WDAY_C	4
#define	EPOCHE_C		70  //C starts to count seconds on jan.1st.1970 GMT
#define	Y2K_YEARS_C		100	//we dont deal with dates before 01.01.2000

static struct tm 	*time_GS(time_t *ptt_time);
static int			IsDst(struct tm *tm);
static void			GetDstDates(int year);
static time_t		DayInMon2tm(struct tm* ptmResult, SYSTEMTIME *pstChangedate);

#endif //___time_gs_h__

