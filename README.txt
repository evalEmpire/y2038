This is an implementation of localtime_r() and gmtime_r() which solves the year 2038 bug on systems where time_t is only 32 bits.  The latest version can be found at http://y2038.googlecode.com/

It makes use of the system's native 32 bit localtime_r() to perform time zone and daylight savings time calculations and thus does *not* ship it's own time zone table.

localtime64.h contains two public functions, localtime64_r() and gmtime64_r().  They are implementations of localtime_r() and gmtime_r().

struct tm *gmtime64_r    (const long long *in_time, struct tm *gm_tm)
struct tm *localtime64_r (const long long *in_time, struct tm *local_tm)

To install, simply copy localtime64.h into your project and make use of the functions.

To test, run "make test".  It will check the results against tables of known good 64 bit results between 2008 and 2700, in several time zones.  If anything goes wrong it will display the differences


Limitations, Issues, etc...
---------------------------

localtime64_r() gets its time zone and daylight savings time information by mappping the future year back to a similar one between 2010 and 2037, safe for localtime_r().  The calculations are accurate according to current time zone and daylight savings information, but may become inaccurate if a change is made that takes place after 2010.

Future versions will simply use the system gmtime_r() and localtime_r() if the given time is within a safe range.

Future versions will probe for a 64 bit safe system localtime_r() and gmtime_r() and use that.

The maximum date is still limited by your tm struct.  Most 32 bit systems use a signed integer tm_year which means the practical upper limit is the year 2147483647 which is somewhere around 2**54.


Portability
-----------
I would like to add some configuration detection stuff in the future, but for now all I can do is document the assumptions...

Not all systems have localtime_r() or gmtime_r().  This code is easily adaptable to use the non-reentrant versions.

timegm() is a non-standard function.  Future versions will ship with its own copy.

This code assumes that long longs are 64 bit integers which is technically in violation of the C standard.  This can be changed in localtime64.h by changing the Time64_T typedef.
