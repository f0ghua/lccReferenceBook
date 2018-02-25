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

WHAT GREGOR PROVIDES

Gregor is a little Ansi-C-library dealing with the Gregorian Calender. It is capable of
"date-math" and different conversions and comparisons within the date-range from
1582-Oct-15 to 4046-Nov-25.

The internal representations are given in record, int and string. All of them are in the
genuine format yyyymmdd. Output can be provided in german-style DDMMYYYY (default)
or english-style MMDDYYYY, which is to determine in gregor.h. The conversion
functions can handle dates like "1-30-2004" or "1/30/04" etc.

Gregor should handle all special calendar-conventions like leap years et. al. and should
compile under all Ansi-C99-plattforms. A little test-suite is enclosed.