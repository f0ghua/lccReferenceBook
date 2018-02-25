TIME_GS.C										Dec. 19th 2003

Why:
When using localtime(), I encountered problems with the dates on which daylight saving time
starts and ends. Eg: for MEZ DST ends on the last Sunday in October, but localtime() insists
on the first Sunday in November.
So I wrote my own versions of gmtime(), localtime() and mktime(pt). 

How To Use It:
Include my headerfile "time_gs.h" instead of <time.h> (this will map the std-lib functions 
to myine) and add time_gs.c to your project.

CAVEAT:
As my functions are written, they will deal ONLY WITH DATES AFTER Jan. 1st. 2000 !
But it should not be very difficult to overcome this limitation.
Please remember, I wrote them for my very special purposes, and they are tested only against
this special requirements!
Information on when DST changes occur are obtained from the Windows-System via 
GetTimeZoneInformation().

What else:
There is another subroutine in time_gs.c: msoTimeEx(). 
As you might know, MS-Excel stores dates as days ( and fractions of days) since 
Jan. 1st 1900. msoTimeEx() takes such a date and converts it to a time_t and a struct tm.

If you want (need) to debug, improve or test my functions, there is a simple test utility: 
tz.c.
It operates on that big structure "DATETEST" and outputs to a file "hf.txt". 
There is also an excel-file (ZeitTest.xls) that helps in creating test data. 
Just expand it as necessary and copy/paste column "I" into tz.c.

If find any bugs (very likely) or improve this code, please let me know:

gerhard_schiller@laposte.net

Have fun !

	
