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

#define OK printf("OK\n");                       // Testing-purposes
#define NOK { printf("NOK"); ok=0; getchar(); }  // Testing-purposes

#include <stdio.h>
#include <string.h>
#include "gregor.h"

// Stuff for testing
void test(void);
const DATE sday = {1961, 9,30};    // Some day for testing
const DATE fday = {1582,10,15};    // First day in calendar = 1582.Oct.15
const DATE lday = {4046,11,25};    // Last day in calendar  = 4046.Nov.25
int ok=1;



int main(void) {
	test();
	getchar();
	return (0);
}


// A little test-suite for Gregor
void test(void) {
	DINT i;
	DATE d1, d2;
	DSTR s;

	printf("Start testing\n");

	// Testing delta_date
	if(deltad(fday,lday)== 899999) OK else NOK
	if(deltad(fday,sday)== 138412) OK else NOK
	if(deltad(sday,fday)==-138412) OK else NOK

	#ifdef DDMMYYYY  // DEFAULT. Depending on #define in gregor.h
		// Testing add_date and date_to_asci in DD.MM.YYYY
		d1=addd(sday,15000);  dtoa(d1, s); if(!strcmp(s,"25.10.2002")) OK else NOK
		d1=addd(fday,899999); dtoa(d1, s); if(!strcmp(s,"25.11.4046")) OK else NOK
	#else
		// Testing add_date and date_to_asci in MM.DD.YYYY
		d1=addd(sday,15000); dtoa(d1, s);  if(!strcmp(s,"10.25.2002")) OK else NOK
		d1=addd(fday,899999); dtoa(d1, s); if(!strcmp(s,"11.25.4046")) OK else NOK
	#endif

	#ifdef DDMMYYYY  // DEFAULT. Depending on #define in gregor.h
		// Testing asci_to_date in DD.MM.YYYY
		d1=atod("30.09.1961"); if(dtoi(d1)==19610930) OK else NOK
		d1=atod("30-9-1961");  if(dtoi(d1)==19610930) OK else NOK
		d1=atod("6/9/1961");   if(dtoi(d1)==19610906) OK else NOK
		d1=atod("1.2.04");     if(dtoi(d1)==20040201) OK else NOK
	#else
		// Testing asci_to_date in MM.DD.YYYY
		d1=atod("09.30.1961"); if(dtoi(d1)==19610930) OK else NOK
		d1=atod("9-30-1961");  if(dtoi(d1)==19610930) OK else NOK
		d1=atod("9/6/1961");   if(dtoi(d1)==19610906) OK else NOK
		d1=atod("2.1.04");     if(dtoi(d1)==20040201) OK else NOK
	#endif

	// Testing check_date, int_to_date and date_to_int
	d1=sday; if(checkd(d1)) OK else NOK
	d1=itod(15821015); if( checkd(d1)) OK else NOK
	d1=itod(15821014); if(!checkd(d1)) OK else NOK
	d1=itod(40461126); if(!checkd(d1)) OK else NOK
	d1=itod(20040229); if( checkd(d1)) OK else NOK
	d1=itod(19610931); if(!checkd(d1)) OK else NOK
	d1=itod(19611330); if(!checkd(d1)) OK else NOK
	i=dtoi(sday); if(i==19610930) OK else NOK

	// Testing date_is_smaller/greater_equal
	d1=sday; d2=d1;
	if(dise(d1,d2)) OK else NOK
	if(dige(d1,d2)) OK else NOK
	d2=addd(d1,1);
	if( dise(d1,d2)) OK else NOK
	if(!dige(d1,d2)) OK else NOK

	// Testing date_is_equal/not equal
	d1=sday; d2=d1;
	if( die(d1,d2)) OK else NOK
	if(!dine(d1,d2)) OK else NOK

	// Testing date_is_smaller/greater
	d2=addd(d1,1);
	if( dis(d1,d2)) OK else NOK
	if(!dis(d2,d1)) OK else NOK
	if( dig(d2,d1)) OK else NOK
	if(!dig(d1,d2)) OK else NOK
	d1=d2;
	if(!dis(d2,d1)) OK else NOK
	if(!dig(d2,d1)) OK else NOK

	#ifdef DDMMYYYY  // DEFAULT. Depending on #define in gregor.h
		// Testing weekdays in DD.MM.YYYY
		d1=sday;           if(!strcmp(weekday(d1),"Sa")) OK else NOK
		d1=atod("6.1.04"); if(!strcmp(weekday(d1),"Di")) OK else NOK
	#else
		// Testing weekdays in MM.DD.YYYY
		d1=sday;           if(!strcmp(weekday(d1),"Sa")) OK else NOK
		d1=atod("6.1.04"); if(!strcmp(weekday(d1),"Tu")) OK else NOK
	#endif

	dtoa(today(),s);
	if(ok)
		printf("\nChecked on %s %s. Everything OK\n", weekday(today()), s);
	else
		printf("\nChecked on %s %s. Error(s) occured\n", weekday(today()), s);
	return;
}
