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

#define DDMMYYYY        // undefine for MMDDYYYY-output-format
#define DEL '.'         // Delimiter for e.g. 30.01.2004

typedef char DSTR[11];  // String-Date-Type for "dd.mm.yyy" || "mm.dd.yyy"
typedef int DINT;       // Integer-Date-Type representing yyyymmdd
typedef struct {        // Generic Date-Type representing {yyyy,mm,dd}
	int year;
	int month;
	int day;
} DATE;

// Functions
extern int  deltad(const DATE old, const DATE new);  // int=newdate-olddate
extern int  checkd(const DATE d);                    // valid date ?
extern DATE addd(const DATE d, const int offset);    // date=date+offset
extern char *weekday(const DATE d);                  // returns weekday-string like "MO"
extern DATE today(void);                             // returns actual system-date

// Conversions
extern void dtoa(const DATE d, DSTR s);              // s=date
extern DATE atod(const DSTR s);                      // date="dd.mm.yyyy" || "mm.dd.yyyy
extern DATE itod(const DINT d);                      // date=int (int-format=yyyymmdd)
extern DINT dtoi(const DATE d);                      // int=date (int-format=yyyymmdd)

// Comparisons
extern int dise(const DATE d1, const DATE d2);       // d1<=d2 ?
extern int dis(const DATE d1, const DATE d2);        // d1<d2  ?
extern int dige(const DATE d1, const DATE d2);       // d1>=d2 ?
extern int dig(const DATE d1, const DATE d2);        // d1>d2  ?
extern int die(const DATE d1, const DATE d2);        // d1==d2 ?
extern int dine(const DATE d1, const DATE d2);       // d1!=d2 ?
