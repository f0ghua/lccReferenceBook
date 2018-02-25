/* Gregor 1.0
 *
 * Copyright (C) 1991, 1992, 2002, 2004 Heinz van Saanen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "gregor.h"

// Utility-functions
static int daysfrombegin(const DATE d);
static int rawdelta(const DATE old, const DATE new);

// Some Date-Constants
static const DATE fday   = {1582,10,15};    // First day = 15.Oct.1582
static const DATE nodate = {9999,99,99};    // Invalid date

// Weekdays German/English
#ifdef DDMMYYYY
	static char *wdg[]={"Fr","Sa","So","Mo","Di","Mi","Do"};
#else
	static char *wde[]={"Fr","Sa","Su","Mo","Tu","We","Th"};
#endif


/*------------------------------------------------------------------------
 Purpose:       Exact difference between to dates, checking leap years etc.
 Input:         Older date, newer date
 Output:        Number of days, negative when order of old and new changed
 Error-Check:   None
------------------------------------------------------------------------*/
int deltad(const DATE old, const DATE new) {
	return (daysfrombegin(new)-daysfrombegin(old));
}


/*------------------------------------------------------------------------
 Purpose:       Calculates exact days from earliest date 15.10.1528
 Input:         DATE-record
 Output:        Number of days
 Error-Check:   None
------------------------------------------------------------------------*/
static int daysfrombegin(const DATE d) {
	int diff=0, zw, i;
	DATE h={1600,2,29};

	zw=rawdelta(fday,d);
	for(i=1600; i<=4000; i+=100) {
		h.year=i;
		if(dise(h,d)) {
			if(!checkd(h))
			diff++;
		}
		else break;
	}
	return (zw-diff);
}


/*------------------------------------------------------------------------
 Purpose:       Date Is Smaller or Equal
 Input:         DATE-records d1,d2
 Output:        True if d1<=d2 else false
 Error-Check:   None
------------------------------------------------------------------------*/
int dise(const DATE d1, const DATE d2) {
	if(d1.year<d2.year) return 1;
	if(d1.year>d2.year) return 0;
	if(d1.month<d2.month) return 1;
	if(d1.month>d2.month) return 0;
	if(d1.day<=d2.day) return 1;
	return (0);
}


/*------------------------------------------------------------------------
 Purpose:       Date Is Smaller
 Input:         DATE-records d1,d2
 Output:        True if d1<d2 else false
 Error-Check:   None
------------------------------------------------------------------------*/
int dis(const DATE d1, const DATE d2) {
	if(d1.year<d2.year) return 1;
	if(d1.year>d2.year) return 0;
	if(d1.month<d2.month) return 1;
	if(d1.month>d2.month) return 0;
	if(d1.day<d2.day) return 1;
	return (0);
}


/*------------------------------------------------------------------------
 Purpose:       Date Is Greater or Equal
 Input:         DATE-records d1,d2
 Output:        True if d1>=d2 else false
 Error-Check:   None
------------------------------------------------------------------------*/
int dige(const DATE d1, const DATE d2) {
	if(d1.year>d2.year) return (1);
	if(d1.year<d2.year) return (0);
	if(d1.month>d2.month) return (1);
	if(d1.month<d2.month) return (0);
	if(d1.day>=d2.day) return (1);
	return (0);
}


/*------------------------------------------------------------------------
 Purpose:       Date Is Greater
 Input:         DATE-records d1,d2
 Output:        True if d1>d2 else false
 Error-Check:   None
------------------------------------------------------------------------*/
int dig(const DATE d1, const DATE d2) {
	if(d1.year>d2.year) return (1);
	if(d1.year<d2.year) return (0);
	if(d1.month>d2.month) return (1);
	if(d1.month<d2.month) return (0);
	if(d1.day>d2.day) return (1);
	return (0);
}


/*------------------------------------------------------------------------
 Purpose:       Date Is Equal
 Input:         DATE-records d1,d2
 Output:        True if d1==d2 else false
 Error-Check:   None
------------------------------------------------------------------------*/
int die(const DATE d1, const DATE d2) {
	if(dtoi(d1)==dtoi(d2)) return (1); else return (0);
}


/*------------------------------------------------------------------------
 Purpose:       Date Is Not Equal
 Input:         DATE-records d1,d2
 Output:        True if d1!=d2 else false
 Error-Check:   None
------------------------------------------------------------------------*/
int dine(const DATE d1, const DATE d2) {
	if(dtoi(d1)!=dtoi(d2)) return (1); else return (0);
}


/*------------------------------------------------------------------------
 Purpose:       Adds a number of days to a date
 Input:         DATE-record, offset
 Output:        Calculated date
 Error-Check:   Negative offset && date>25.11.4046
------------------------------------------------------------------------*/
DATE addd(const DATE d, const int offset) {
	DATE h=d;
	int i;

	if(offset<=0) return (h);
	for(i=1; i<=offset; i++) {
		if(h.year==4046 && h.month==11 && h.day>=25) {
			puts("Date out of range");
			getchar();
			return (nodate);
		}
		h.day++;
		if(!checkd(h)) {
			h.day=1;
			h.month++;
			if(!checkd(h)) {
				h.month=1;
				if(h.year<4046) h.year++;
			}
		}
	}
	return (h);
}


/*------------------------------------------------------------------------
 Purpose:       Returns weekday
 Input:         DATE-records
 Output:        Calculated weekday
 Error-Check:   None
------------------------------------------------------------------------*/
extern char *weekday(const DATE d) {
	int dval, x, y;

	if(d.month<=2) {
		x=0;
		y=d.year-1;
	}
	else {
		x=(int)(0.4*d.month+2.3);
		y=d.year;
	}
	dval=365*d.year+31*(d.month-1)+d.day+(int)(y/4.0)-x;
	#ifdef DDMMYYYY
		return *(wdg+dval%7);
	#else
		return *(wde+dval%7);
	#endif
}


/*------------------------------------------------------------------------
 Purpose:       Returns actual system-date
 Input:         None
 Output:        Today
 Error-Check:   None
------------------------------------------------------------------------*/
extern DATE today(void) {
	time_t today;
	struct tm sysdate;
	DSTR s;

	time(&today);
	sysdate=*localtime(&today);
	#ifdef DDMMYYYY
		strftime(s,11,"%d.%m.%Y",&sysdate);
	#else
		strftime(s,11,"%m.%d.%Y",&sysdate);
	#endif
	return atod(s);
}


/*------------------------------------------------------------------------
 Purpose:       Raw difference of to dates, without special leap years
 Input:         Older date, newer date
 Output:        Number of days, negative when d2 and d1 change place
 Error-Check:   None
------------------------------------------------------------------------*/
static int rawdelta(const DATE old, const DATE new) {
	int olddays,z,x;

	z=old.year;
	if(old.month<=2) {
		x=0;
		z--;
	}
	else x=(int)(0.4*old.month+2.3);
	olddays=365*old.year+31*(old.month-1)+old.day+(int)(z/4.0)-x;

	z=new.year;
	if(new.month<=2) {
		x=0;
		z--;
	}
	else x=(int)(0.4*new.month+2.3);
	return (365*new.year+31*(new.month-1)+new.day+(int)(z/4.0)-x-olddays);
}


/*------------------------------------------------------------------------
 Purpose:       Converts DATE to string, fills in leading '0'
 Input:         DATE-record, datestring
 Output:        None, DSTR string is altered
 Error-Check:   None
------------------------------------------------------------------------*/
void dtoa(const DATE d, DSTR s) {
	if(d.day<10 && d.month<10) {
		#ifdef DDMMYYYY
			sprintf(s,"0%d%c0%d%c%d",d.day,DEL,d.month,DEL,d.year);
		#else
			sprintf(s,"0%d%c0%d%c%d",d.month,DEL,d.day,DEL,d.year);
		#endif
		return;
	}
	if(d.day<10) {
		#ifdef DDMMYYYY
			sprintf(s,"0%d%c%d%c%d",d.day,DEL,d.month,DEL,d.year);
		#else
			sprintf(s,"0%d%c%d%c%d",d.month,DEL,d.day,DEL,d.year);
		#endif
		return;
	}
	if(d.month<10) {
		#ifdef DDMMYYYY
			sprintf(s,"%d%c0%d%c%d",d.day,DEL,d.month,DEL,d.year);
		#else
			sprintf(s,"%d%c0%d%c%d",d.day,DEL,d.month,DEL,d.year);
		#endif
		return;
	}
	#ifdef DDMMYYYY
		sprintf(s,"%d%c%d%c%d",d.day,DEL,d.month,DEL,d.year);
	#else
		sprintf(s,"%d%c%d%c%d",d.month,DEL,d.day,DEL,d.year);
	#endif
	return;
}


/*------------------------------------------------------------------------
 Purpose:       Converts string to date
 Input:         DATE-record
 Output:        None, DSTR string is altered
 Error-Check:   None
------------------------------------------------------------------------*/
DATE atod(const DSTR s) {
	DATE d;

	#ifdef DDMMYYYY
		if(isdigit(*(s))&&isdigit(*(s+1))) {
			d.day=10*(*s++-48);
			d.day+=*s++-48;
			s++;
		}
		else {
			d.day=*s++-48;
			s++;
		}
	#else
		if(isdigit(*(s))&&isdigit(*(s+1))) {
			d.month=10*(*s++-48);
			d.month+=*s++-48;
			s++;
		}
		else {
			d.month=*s++-48;
			s++;
		}
	#endif

	#ifdef DDMMYYYY
		if(isdigit(*(s))&&isdigit(*(s+1))) {
			d.month=10*(*s++-48);
			d.month+=*s++-48;
			s++;
		}
		else {
			d.month=*s++-48;
			s++;
		}
	#else
		if(isdigit(*(s))&&isdigit(*(s+1))) {
			d.day=10*(*s++-48);
			d.day+=*s++-48;
			s++;
		}
		else {
			d.day=*s++-48;
			s++;
		}
	#endif

	if(isdigit(*(s+2))) {
		d.year=1000*(*s++-48);
		d.year+=100*(*s++-48);
		d.year+=10*(*s++-48);
		d.year+=*s++-48;
	}
	else {
		d.year=10*(*s++-48);
		d.year+=*s++-48;
		d.year+=2000;
	}
	return (d);
}


/*------------------------------------------------------------------------
 Purpose:       Converts DATE to integer
 Input:         DATE-record
 Output:        Integer in yyyymmdd-format
 Error-Check:   None
------------------------------------------------------------------------*/
DINT dtoi(const DATE d) {
	return (d.year*10000+d.month*100+d.day);
}


/*------------------------------------------------------------------------
 Purpose:       Integer to date
 Input:         Integer representing a date in yyyymmdd
 Output:        DATE-record
 Error-Check:   None
------------------------------------------------------------------------*/
DATE itod(const DINT d) {
	int i=d;
	DATE h;
	h.year=i/10000;
	i-=h.year*10000;
	h.month=i/100;
	h.day=i-h.month*100;
	return (h);
}


/*------------------------------------------------------------------------
 Purpose:       Checks for correct date
 Input:         DATE-record
 Output:        True=1 False=0
 Error-Check:   None
------------------------------------------------------------------------*/
int checkd(const DATE d) {
	int ok=1;
	if(d.year<1582) return (0);
	if(d.year==1582) if(d.month<11 && d.day<15) return (0);
	if(d.year>4046) return (0);
	if(d.year==4046) if(d.month>10 && d.day>25) return (0);
	if(d.month<1) return (0);
	if(d.month>12) return (0);
	if(d.day<1) return (0);
	switch(d.month) {
		case  1: if(d.day>31) ok=0; break;
		case  2: if(d.day>29) ok=0; break;
		case  3: if(d.day>31) ok=0; break;
		case  4: if(d.day>30) ok=0; break;
		case  5: if(d.day>31) ok=0; break;
		case  6: if(d.day>30) ok=0; break;
		case  7: if(d.day>31) ok=0; break;
		case  8: if(d.day>31) ok=0; break;
		case  9: if(d.day>30) ok=0; break;
		case 10: if(d.day>31) ok=0; break;
		case 11: if(d.day>30) ok=0; break;
		case 12: if(d.day>31) ok=0; break;
	}
	if(d.month==2 && d.day==29 && d.year%4!=0) return (0);
	if(d.month==2 && d.day==29)
		if((d.year/100)%4!=0 && d.year%100==0) return (0);
	return (ok);
}
