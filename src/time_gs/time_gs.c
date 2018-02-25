/*
 * time_gs.c -- converts a time-value in MS-Excel format into various C-types
 *				reimplements localtime() and gmtime() to overcome problems with
 *				DST start and end dates.
 *
 * Copyright (C) 2003 Gerhard Schiller.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Written by Gerhard Schiller <gerhard.schiller@laposte.net>.
 */

#include <windows.h>
//#include <stdio.h>
#include <math.h>
#include "time_gs.h"
#include "_time_gs.h"

// next var's store timezone info and dates for a paticular year
static TIME_ZONE_INFORMATION 	tzi;
static BOOL						bFirstRun = TRUE;

static struct tm 				tmSpring;
static struct tm 				tmFall;
static time_t					ttSpring;
static time_t					ttFall;

static float         // 1     2     3     4     5     6     7     8     9    10    11    12
	alfDays[2][12] = {{31.0, 28.0, 31.0, 30.0, 31.0, 30.0, 31.0, 31.0, 30.0, 31.0, 30.0, 31.0},
					  {31.0, 29.0, 31.0, 30.0, 31.0, 30.0, 31.0, 31.0, 30.0, 31.0, 30.0, 31.0}};


/*
struct tm *gmtime_GS(time_t *ptt)

INPUT:
 a time_t representing GMT time
RETURN:
 returns a (static) pointer to a struct tm.

*/
struct tm
*gmtime_GS(time_t *ptt_time)
{
	return (time_GS(ptt_time));
}

/*
struct tm *gmtime_ex(time_t *ptt)

INPUT:
 a time_t representing GMT time
RETURN:
 returns a (static) pointer to a struct tm.

*/
struct tm
*localtime_GS(time_t *ptt_time)
{
	struct  tm 	*ptm;
	time_t		tt;
	int			year;
	int			isDst;

	tt  = *ptt_time;
	for(year = EPOCHE_C; tt > YEAR_SEC(year); ++year)
		tt -= YEAR_SEC(year);

	GetDstDates(year);
//	printf("Year=%d ", year + 1900);

//	if(*ptt_time >= ttSpring && *ptt_time <= ttFall)
	if(*ptt_time > ttSpring && *ptt_time < ttFall){
		tt = *ptt_time - ((tzi.Bias * 60) + (tzi.DaylightBias * 60));
		isDst = 1;
	}
	else{
		tt = *ptt_time -  (tzi.Bias * 60);
		isDst = 0;
	}

	ptm = time_GS(&tt);
//	ptm->tm_isdst = (*ptt_time >= ttSpring && *ptt_time <= ttFall) ? 1 : 0;
//	ptm->tm_isdst = (*ptt_time > ttSpring && *ptt_time < ttFall) ? 1 : 0;
	ptm->tm_isdst = isDst;
	return(ptm);
}

/*
struct tm *gmtime_ex(time_t *ptt)

INPUT:
 a time_t
RETURN:
 returns a (static) pointer to a struct tm.

*/
static struct tm
*time_GS(time_t *ptt_time)
{
	static struct	tm tm;
	time_t			tt;
	int				days = 0;

	tt			= *ptt_time;

	memset(&tm, 0, sizeof(tm));
	tm.tm_year  = EPOCHE_C;	// (time_t)0 -> 1.1.1970 00:00:00 GMT
	tm.tm_mon	= 0;
	tm.tm_mday	= 0;
	tm.tm_hour	= 0;
	tm.tm_min	= 0;
	tm.tm_sec	= 0;
	tm.tm_isdst	= 0;

	if(tt < Y2K_MEZ_SEC)
		return (&tm);

	while(tt >=	YEAR_SEC(tm.tm_year)){
		tt       -= YEAR_SEC(tm.tm_year);
		days	 += (int)CURYEAR(tm.tm_year);
		tm.tm_year++;
	}

	while(tt >=	DAY_SEC * (time_t)alfDays[ISLEAPYEAR(tm.tm_year)][tm.tm_mon]){
		tt         -= DAY_SEC * (time_t)alfDays[ISLEAPYEAR(tm.tm_year)][tm.tm_mon];
		tm.tm_yday += (int)alfDays[ISLEAPYEAR(tm.tm_year)][tm.tm_mon];
		tm.tm_mon++;
	}

	tm.tm_mday  = (int)(tt / DAY_SEC);
	tm.tm_yday += tm.tm_mday;
	tt         -= DAY_SEC * tm.tm_mday;
	tm.tm_mday++;

	tm.tm_hour  = (int)(tt / HOUR_SEC);
	tt         -= HOUR_SEC * tm.tm_hour;

	tm.tm_min   = (int)(tt / MIN_SEC);
	tt         -= MIN_SEC * (int)(tm.tm_min);

	tm.tm_sec   = (int)(tt);

	days += tm.tm_yday;
	days += EPOCHE_WDAY_C;
	tm.tm_wday  = days % 7;

 	return(&tm);
}

/*
struct tm *msoTimeEx(double lfDate, time_t *ptt_time)

INPUT:
 takes a time-value in MS-Excel format,
 i.e. the number of days since Jan.0.(<- ZERO!).1900
 so that 1.0 translates to Jan.1st.1900 00:00
 and 1.5 to Jan.1st.1900 12:00

 *tt is a pointer to a time_t variable.
 Its value on entry is ignored.

OUTPUT:
 returns a (static) pointer to a "tm" structure,
 or NULL if lfDate is before 1.1.2000 00:00:00 localtime

 The coresponding C-time is stored in the *ptt_time argument.
*/
struct tm *
msoTimeEx(double lfDate, time_t *ptt_time)
{
	static struct 					tm tm;
	time_t		  					tt;
	int								days = 0;
	double							lfTmp;

	memset(&tm, 0, sizeof(tm));
	tm.tm_year  = Y2K_YEARS_C;	// 1900 + 100 = 2000
	tm.tm_mon	= 0;
	tm.tm_mday	= 0;
	tm.tm_hour	= 0;
	tm.tm_min	= 0;
	tm.tm_sec	= 0;
	tm.tm_isdst	= 0;

	if(lfDate < Y2K_MEZ_MSO)
		return (NULL);

	tt      = Y2K_GMT_SEC;
	lfDate -= Y2K_MEZ_MSO;

//	printf("%.8lf  ", lfDate);
	lfTmp  = lfDate * (24.0*60.0*60.0);
//	printf("%.8lf  ", lfTmp);
	lfTmp  = rint(lfTmp) + 0.5;
//	printf("%.8lf  ", lfTmp);
	lfDate = lfTmp / (24.0*60.0*60.0);
//	printf("%.8lf\n", lfDate);

	while(lfDate >=	CURYEAR(tm.tm_year)){
		tt       += YEAR_SEC(tm.tm_year);
		days	 += (int)CURYEAR(tm.tm_year);
		lfDate   -= CURYEAR(tm.tm_year);
		tm.tm_year++;
	}

	while(lfDate >=	alfDays[ISLEAPYEAR(tm.tm_year)][tm.tm_mon]){
		tt         += DAY_SEC * (time_t)alfDays[ISLEAPYEAR(tm.tm_year)][tm.tm_mon];
		tm.tm_yday += (int)alfDays[ISLEAPYEAR(tm.tm_year)][tm.tm_mon];
		lfDate     -= alfDays[ISLEAPYEAR(tm.tm_year)][tm.tm_mon];
		tm.tm_mon++;
	}

	tm.tm_mday  = (int)lfDate;
	tm.tm_yday += tm.tm_mday;
	tt         += DAY_SEC * tm.tm_mday;
	lfDate     -= (double)tm.tm_mday;
	tm.tm_mday++;

	tm.tm_hour  = (int)(lfDate * 24.0);
	tt         += HOUR_SEC * tm.tm_hour;
	lfDate     -= (double)tm.tm_hour / 24.0;

	tm.tm_min   = (int)(lfDate * 24.0 * 60.0);
	tt         += MIN_SEC * (int)(tm.tm_min);
	lfDate     -= (double)tm.tm_min / (24.0 * 60.0);

	tm.tm_sec   = (int)(lfDate * 24.0 * 60.0 * 60.0);
	tt         += (time_t)(tm.tm_sec);
/*
	lfDate     -= (double)tm.tm_sec / (24.0 * 60.0 * 60);

	if((lfDate * 24.0 * 60.0 * 60.0) >= 0.5)
		tm.tm_sec = (lfDate * 24.0 * 60.0 * 60.0) * -100;
*/

	days += tm.tm_yday;
	days += Y2K_WDAY;
	tm.tm_wday  = days % 7;
	tm.tm_isdst = IsDst(&tm);

	if(ptt_time)
		*ptt_time = tt + (tzi.Bias * 60) + ((tm.tm_isdst ? tzi.DaylightBias : 0) * 60);

	return(&tm);
}


/*
time_t
mktime_GS(struct tm *tm)

INPUT:
 pointer to a "tm" structure, the tm_wday, tm_yday and tm_isdst
 members are ignored. All other members are expected to have

OUTPUT:
 Fills in the tm_wday, tm_yday and tm_isdst members of the *tm
 argument.
RETURNS
 returns the coresponding C-time
*/
time_t
mktime_GS(struct tm *ptm)
{
	time_t	tt;
	int		days = 0;
	int		mon;
	int		year;
	int		i;

	ptm->tm_isdst	= 0;
	ptm->tm_yday 	= 0;
	ptm->tm_wday	= 0;

	for(year = EPOCHE_C; year < ptm->tm_year; year++)
		days += (int)CURYEAR(year);

	for(mon = 0; mon < ptm->tm_mon; mon++)
		ptm->tm_yday += (int)alfDays[ISLEAPYEAR(ptm->tm_year)][mon];

	ptm->tm_yday += ptm->tm_mday;
	ptm->tm_yday--;

	days += ptm->tm_yday;
	days += EPOCHE_WDAY_C;
	ptm->tm_wday  = days % 7;
	ptm->tm_isdst = IsDst(ptm);

	tt  = 0;
	for(i = EPOCHE_C; i < ptm->tm_year; i++)
		tt += YEAR_SEC(i);
	tt += (ptm->tm_yday * DAY_SEC);
	tt += (ptm->tm_hour * HOUR_SEC);
	tt += (ptm->tm_min  * MIN_SEC);
	tt +=  ptm->tm_sec;

	tt += (tzi.Bias * 60) + ((ptm->tm_isdst ? tzi.DaylightBias : 0) * 60);
	return(tt);
}


/*
static int
IsDst(struct tm *ptm)

RETURNS 1 if daylight saving is in effect for the
local date stored in ptm, otherwise returns 0
*/
static int
IsDst(struct tm *ptm)
{
	GetDstDates(ptm->tm_year);
	if( ptm->tm_yday < tmSpring.tm_yday ||
	   (ptm->tm_yday == tmSpring.tm_yday && ptm->tm_hour <= tmSpring.tm_hour) ||
//	   (ptm->tm_yday == tmSpring.tm_yday && ptm->tm_hour < tmSpring.tm_hour && ptm->tm_min <= 59 && ptm->tm_sec <= 59)||
	    ptm->tm_yday > tmFall.tm_yday ||
	   (ptm->tm_yday == tmFall.tm_yday && ptm->tm_hour >= (tmFall.tm_hour + (tzi.DaylightBias / 60))) )
		return  0;
	else
		return  1;
}


/*
static void
GetDstDates(int year)
INPUT:
 the year, for wich the dates are requested.
OUTPUT:
 sets the global var's: tmSpring, tmFall, ttSpring, ttFall and bFirstRun

*/
static void
GetDstDates(int year)
{
 	if(bFirstRun){
		memset(&tzi, 0, sizeof(tzi));
		GetTimeZoneInformation(&tzi);
		bFirstRun = FALSE;
	}

	if(year != tmSpring.tm_year){
		memset(&tmSpring, 0, sizeof(tmSpring));
		tmSpring.tm_year = year;
		ttSpring  = DayInMon2tm(&tmSpring, &(tzi.DaylightDate));
		ttSpring += (tzi.Bias * 60);
//	   	printf("\tyear:%d, tt_GMT:%ld, YearDay:%3d, Springtime:%s", tmSpring.tm_year, ttSpring, tmSpring.tm_yday + 1, asctime(&tmSpring));

		memset(&tmFall, 0, sizeof(tmFall));
		tmFall.tm_year 	= year;
		tmFall.tm_isdst	= 1;
		ttFall  = DayInMon2tm(&tmFall, &(tzi.StandardDate));
		ttFall += ((tzi.Bias * 60) + (tzi.DaylightBias * 60));
//	  	printf("\tyear:%d, tt_GMT:%ld, YearDay:%3d, Falltime:  %s", tmFall.tm_year, ttFall, tmFall.tm_yday + 1, asctime(&tmFall));
	}
}

/*
static time_t
DayInMon2tm(struct tm* ptmResult, SYSTEMTIME *pstChangedate)

DayInMon2tm() looks at the tm_year member of ptmResult (all other
members are ignorde) and sets all members of ptmResult to the
LOCAL TIME according to the info in pstChangedate.

pstChangedate is a struct SYSTEMTIME, "Day-in-month" format
(see the "Win32 SDK Reference").
*/
static time_t
DayInMon2tm(struct tm* ptmResult, SYSTEMTIME *pstChangedate)
{
	time_t	tt;
	int		iWeeks;
	int		i;

	ptmResult->tm_mon  = pstChangedate->wMonth - 1;
	ptmResult->tm_mday = 1;

	//calculate wday for 1st of month
	ptmResult->tm_wday = Y2K_WDAY;
	for(i = Y2K_YEARS_C; i < ptmResult->tm_year; i++)
		ptmResult->tm_wday += (int)CURYEAR(i);
	for(i = 0; i < (ptmResult->tm_mon - 0); i++)
		ptmResult->tm_wday += (int)alfDays[ISLEAPYEAR(ptmResult->tm_year)][i];

	ptmResult->tm_wday %= 7;
//    printf("week_day for 1st day in month:[%d], %02d.%02d.%d (%d)\n",
//		ptmResult->tm_wday, ptmResult->tm_mday, ptmResult->tm_mon+1,ptmResult->tm_year+1900, ptmResult->tm_yday);

	while(ptmResult->tm_wday != pstChangedate->wDayOfWeek){
		ptmResult->tm_mday++;
		ptmResult->tm_wday++;
		ptmResult->tm_wday %= 7;
	}
//    printf("day in week one:              [%d], %02d.%02d.%d (%d)\n",
//		ptmResult->tm_wday, ptmResult->tm_mday, ptmResult->tm_mon+1,ptmResult->tm_year+1900, ptmResult->tm_yday);

	iWeeks = alfDays[ISLEAPYEAR(ptmResult->tm_year)][ptmResult->tm_mon];
//	printf("iWeeks %d\n", iWeeks);
	iWeeks -= ptmResult->tm_mday;
	iWeeks /= 7;
//	printf("iWeeks=%d tzi.StandardDate.wDay=%d\n", iWeeks, pstChangedate->wDay);

	ptmResult->tm_mday += 7*(min(iWeeks, pstChangedate->wDay));
//	printf("ptmResult->tm_mday %d\n", ptmResult->tm_mday);

	//calculate yday for 1st of month
	ptmResult->tm_yday = 0;
	for(i = 0; i < (ptmResult->tm_mon - 0); i++){
		ptmResult->tm_yday += (int)alfDays[ISLEAPYEAR(ptmResult->tm_year)][i];
//		printf("%d:%d ", i+1, ptmResult->tm_yday);
	}
	ptmResult->tm_yday += ptmResult->tm_mday;
	ptmResult->tm_yday--;	// Jan, 1st ist day 0
//	printf("%d:%d\n", i+1, ptmResult->tm_yday);

	ptmResult->tm_hour = pstChangedate->wHour;
	ptmResult->tm_min  = pstChangedate->wMinute;
	ptmResult->tm_sec  = pstChangedate->wSecond;
//    printf("changedate:                   [%d], %02d.%02d.%d (%d)\n",
//		ptmResult->tm_wday, ptmResult->tm_mday, ptmResult->tm_mon+1,ptmResult->tm_year+1900, ptmResult->tm_yday);

	tt  = 0;
	for(i = EPOCHE_C; i < ptmResult->tm_year; i++)
		tt += YEAR_SEC(i);
	tt += (ptmResult->tm_yday * DAY_SEC);
	tt += (ptmResult->tm_hour * HOUR_SEC);
	tt += (ptmResult->tm_min  * MIN_SEC);
	tt +=  ptmResult->tm_sec;

	return(tt);
}


