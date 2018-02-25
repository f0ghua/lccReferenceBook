#include "windows.h"
#include "stdio.h"
#include "time_gs.h"
#include "_time_gs.h"

void show_time(FILE *fpo, char *expl, time_t t, struct tm *ptm);
void dotime(int year, int mon, int day, int hour, int min);

typedef struct _DATETEST {
	char	*str;
	double	val;
} DATETEST, *PDATETEST;

DATETEST dt[] = {
{ "01.01.2000 00:00:00", 36526.000000 },
{ "31.12.2000 23:59:59", 36891.999988 },
{ "01.01.2001 00:00:00", 36892.000000 },
{ "31.12.2001 23:59:59", 37256.999988 },
{ "30.03.2003 01:00:00", 37710.041667 },
{ "30.03.2003 01:59:00", 37710.082639 },
{ "30.03.2003 02:00:00", 37710.083333 },
{ "30.03.2003 03:00:00", 37710.125000 },
{ "30.03.2003 03:01:00", 37710.125694 },
{ "30.03.2003 04:00:00", 37710.166667 },
{ "26.10.2003 01:00:00", 37920.041667 },
{ "26.10.2003 01:59:00", 37920.082639 },
{ "26.10.2003 02:00:00", 37920.083333 },
{ "26.10.2003 02:01:00", 37920.084028 },
{ "26.10.2003 02:59:00", 37920.124306 },
{ "26.10.2003 03:00:00", 37920.125000 },
{ "26.10.2003 03:01:00", 37920.125694 },
{ "26.10.2003 04:00:00", 37920.166667 },
{ "28.03.2004 01:00:00", 38074.041667 },
{ "28.03.2004 01:59:00", 38074.082639 },
{ "28.03.2004 02:00:00", 38074.083333 },
{ "28.03.2004 03:00:00", 38074.125000 },
{ "28.03.2004 03:01:00", 38074.125694 },
{ "28.03.2004 04:00:00", 38074.166667 },
{ "31.10.2004 01:00:00", 38291.041667 },
{ "31.10.2004 01:59:00", 38291.082639 },
{ "31.10.2004 02:00:00", 38291.083333 },
{ "31.10.2004 02:01:00", 38291.084028 },
{ "31.10.2004 02:59:00", 38291.124306 },
{ "31.10.2004 03:00:00", 38291.125000 },
{ "31.10.2004 03:01:00", 38291.125694 },
{ "31.10.2004 04:00:00", 38291.166667 },
{"", -1.0} };

int
main(void)
{
	TIME_ZONE_INFORMATION 	tzi;
	PDATETEST				pdt;
	FILE					*fpo;
	struct tm				tm, *ptm;
	time_t					t;
	char					buf[256];

	fpo = fopen("hf.txt", "w");

	memset(&tzi, 0, sizeof(tzi));
	GetTimeZoneInformation(&tzi);
	wsprintf(buf, "%ls\n%ls\nBias=%d\nStandardBias=%d\nDaylightBias=%d\n",
										tzi.StandardName,
										tzi.DaylightName,
										tzi.Bias,
										tzi.StandardBias,
										tzi.DaylightBias);
	fputs(buf, fpo);

	wsprintf(buf, "DaylightDate: Month:%02d WeekDay=%d Week:%d Time:%02d:%02d:%02d\n",
										tzi.DaylightDate.wMonth,
										tzi.DaylightDate.wDayOfWeek,
										tzi.DaylightDate.wDay,
										tzi.DaylightDate.wHour,
										tzi.DaylightDate.wMinute,
										tzi.DaylightDate.wSecond);
	fputs(buf, fpo);

	wsprintf(buf, "StandardDate: Month:%02d WeekDay=%d Week:%d Time:%02d:%02d:%02d\n",
										tzi.StandardDate.wMonth,
										tzi.StandardDate.wDayOfWeek,
										tzi.StandardDate.wDay,
										tzi.StandardDate.wHour,
										tzi.StandardDate.wMinute,
										tzi.StandardDate.wSecond);
	fputs(buf, fpo);
	fputs("\n", fpo);

	t   = Y2K_GMT_SEC;
	ptm = gmtime(&t);
	show_time(fpo, "gmtime(Y2K_GMT_SEC)   ", t, ptm);

	memset(&tm, 0, sizeof(tm));
	tm.tm_year = Y2K_YEARS_C;
	tm.tm_mon  = 0;
	tm.tm_mday = 1;
	tm.tm_hour = 0;
	tm.tm_min  = 0;
	tm.tm_sec  = 0;
	t   = mktime(&tm);
	ptm = localtime(&t);
	show_time(fpo, "localtime(Y2K_YEARS_C)", t, ptm);

	ptm = msoTimeEx(Y2K_MEZ_MSO, &t);
	show_time(fpo, "msoTimeEx(Y2K_MEZ_MSO)", t, ptm);
	fputs("\n", fpo);

	pdt = dt;
	while(*(pdt->str)){
		ptm = msoTimeEx(pdt->val, &t);
		if(ptm){
			show_time(fpo, pdt->str, t, ptm);

			ptm = localtime(&t);
			show_time(fpo, "localtime()        ", t, ptm);

			t   = mktime(ptm);
			ptm = localtime(&t);
			show_time(fpo, "mktime() 	       ", t, ptm);

			ptm = gmtime(&t);
			show_time(fpo, "gmtime()           ", t, ptm);

			fputs("\n", fpo);
		}
		else
			fprintf(fpo, "%s, %lf\n", pdt->str, pdt->val);
		pdt++;
	}
	return 0;
}

void
show_time(FILE *fpo, char *expl, time_t t, struct tm *ptm)
{
	fprintf(fpo, "%s time_t=%10ld, %02d.%02d.%4d %02d:%02d:%02d, weekday=%d yearday=%3d, DST=%d\n",
		expl,
		t,
		ptm->tm_mday,
		ptm->tm_mon + 1,
		ptm->tm_year + 1900,
		ptm->tm_hour,
		ptm->tm_min,
		ptm->tm_sec,
		ptm->tm_wday,
		ptm->tm_yday + 1,
		ptm->tm_isdst);
}
