/* pivotal_gmtime_r - a replacement for gmtime/localtime/mktime
                      that works around the 2038 bug on 32-bit
                      systems. (Version 3)

   Copyright (C) 2005  Paul Sheer

   Redistribution and use in source form, with or without modification,
   is permitted provided that the above copyright notice, this list of
   conditions, the following disclaimer, and the following char array
   are retained.

   Redistribution and use in binary form must reproduce an
   acknowledgment: 'With software provided by http://2038bug.com/' in
   the documentation and/or other materials provided with the
   distribution, and wherever such acknowledgments are usually
   accessible in Your program.

   This software is provided "AS IS" and WITHOUT WARRANTY, either
   express or implied, including, without limitation, the warranties of
   NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
   PURPOSE. THE ENTIRE RISK AS TO THE QUALITY OF THIS SOFTWARE IS WITH
   YOU. Under no circumstances and under no legal theory, whether in
   tort (including negligence), contract, or otherwise, shall the
   copyright owners be liable for any direct, indirect, special,
   incidental, or consequential damages of any character arising as a
   result of the use of this software including, without limitation,
   damages for loss of goodwill, work stoppage, computer failure or
   malfunction, or any and all other commercial damages or losses. This
   limitation of liability shall not apply to liability for death or
   personal injury resulting from copyright owners' negligence to the
   extent applicable law prohibits such limitation. Some jurisdictions
   do not allow the exclusion or limitation of incidental or
   consequential damages, so this exclusion and limitation may not apply
   to You.

*/

const char pivotal_gmtime_r_stamp[] =
    "pivotal_gmtime_r. Copyright (C) 2005  Paul Sheer. Terms and "
    "conditions apply. Visit http://2038bug.com/ for more info.";

/*

On 32-bit machines, with 'now' passed as NULL, pivotal_gmtime_r() gives
the same result as gmtime() (i.e. gmtime() of the GNU C library 2.2) for
all values of time_t positive and negative. See the gmtime() man page
for more info.

It is intended that you pass 'now' as the current time (as previously
retrieved with a call such as time(&now);). In this case,
pivotal_gmtime_r() returns the correct broken down time in the range of
    now - 2147483648 seconds
through to
    now + 2147483647 seconds

For example, on 10-Jan-2008, pivotal_gmtime_r() will return the correct
broken down time format from 23-Dec-1939 through 29-Jan-2076.

For all values of 'now' before Jan-23-2005 and after Jan-19-2038,
pivotal_gmtime_r() will return the correct broken down time format from
exactly 01-Jan-1970 through to 07-Feb-2106.

In other words, if, for example, pivotal_gmtime_r() is used in a program
that needs to convert time values of 25 years into the future and 68
years in the past, the program will operate as expected until the year
2106-25=2081. This will be true even on 32-bit systems.

Note that "Jan-23-2005" is the date of the authoring of this code.

Programmers who have available to them 64-bit time values as a 'long
long' type can use gmtime64_r() instead, which correctly converts the
time even on 32-bit systems. Whether you have 64-bit time values
will depend on the operating system.

Both functions are 64-bit clean and should work as expected on 64-bit
systems. They have not yet been tested on 64-bit systems however.

The localtime() equivalent functions do both a POSIX localtime_r() and
gmtime_r() and work out the time zone offset from their difference. This
is inefficient but gives the correct timezone offset and daylight
savings time adjustments.

The function prototypes are:

    long long pivot_time_t (const time_t * now, long long *t);
    long long mktime64 (struct tm *t);
    struct tm *localtime64_r (const long long *t, struct tm *p);
    struct tm *pivotal_localtime_r (const time_t * now, const time_t * t, struct tm *p);
    struct tm *gmtime64_r (const long long *t, struct tm *p);
    struct tm *pivotal_gmtime_r (const time_t * now, const time_t * t, struct tm *p);

pivot_time_t() takes a 64-bit time that may have had its top 32-bits set
to zero, and adjusts it so that it is in the range explained above. You
can use pivot_time_t() to convert any time that may be incorrect.
pivot_time_t() returns its argument unchanged if either now is NULL or
sizeof(time_t) is not 4.

mktime64() is a 64-bit equivalent of mktime().

localtime64_r() is a 64-bit equivalent of localtime_r().

pivotal_localtime_r() is 32-bit equivalent of localtime_r() with
pivoting.

gmtime64_r() is a 64-bit equivalent of gmtime_r().

pivotal_gmtime_r() is a 32-bit equivalent of gmtime_r() with pivoting.

Note that none of these functions handle leap seconds.

RATIONALE: The purpose of pivotal_gmtime_r() is as a replacement for the
functions gmtime(), localtime() and their corresponding reentrant
versions gmtime_r() and localtime_r().

pivotal_gmtime_r() is meant for 32-bit systems that must still correctly
convert 32-bit time into broken down time format through the year 2038.
Since most programs tend to operate within a range of time no more than
68 years in the future or the past, it is possible to determine the
correct interpretation of a 32-bit time value in spite of the wrap that
occurs in the year 2038.

Many databases are likely to store time in 32-bit format and not be
easily upgradeable to 64-bit. By using pivot_time_t(), these time values
can be correctly used.


Changes:
    06-Feb-2005  v3.0:  Some optimizations.
			mktime() no-longer zeros tm struct.

*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static const int days_in_month[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

static const int julian_days_by_month[2][12] = {
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
};

static const int length_of_year[2] = { 365, 366 };

#define LEAP_CHECK(n)	((!(((n) + 1900) % 400) || (!(((n) + 1900) % 4) && (((n) + 1900) % 100))) != 0)
#define WRAP(a,b,m)	((a) = ((a) <  0  ) ? ((b)--, (a) + (m)) : (a))


void _check_tm(struct tm *tm)
{
    /* Don't forget leap seconds */
    assert(tm->tm_sec  >= 0 && tm->tm_sec <= 61);
    assert(tm->tm_min  >= 0 && tm->tm_min <= 59);
    assert(tm->tm_hour >= 0 && tm->tm_hour <= 23);
    assert(tm->tm_mday >= 1 && tm->tm_mday <= 31);
    assert(tm->tm_mon  >= 0 && tm->tm_mon  <= 11);
    assert(tm->tm_wday >= 0 && tm->tm_wday <= 6);
    assert(tm->tm_yday >= 0 && tm->tm_yday <= 365);
    assert(   tm->tm_gmtoff >= -24 * 60 * 60
           && tm->tm_gmtoff <=  24 * 60 * 60);
}

int _cycle_offset(int year)
{
    int start_year = 2000;
    int year_diff  = year - start_year;
    int cycle_offset = 0;
    cycle_offset += year_diff / 100;
    cycle_offset -= year_diff / 400;
    
    return cycle_offset;
}

/* For a given year after 2038, pick the latest possible matching
   year in the 28 year calendar cycle.
   XXX Correct for 100/400 year boundries
*/
#define SOLAR_CYCLE_LENGTH 28
int _safe_year(int year)
{
    int safe_year = 2016;
    int year_cycle;
    
    year_cycle = (year + _cycle_offset(year)) % SOLAR_CYCLE_LENGTH;
    
    if( year_cycle <= 21 )
        safe_year += year_cycle;
    else
        safe_year =  safe_year - (SOLAR_CYCLE_LENGTH - year_cycle);

    assert(safe_year <= 2037 && safe_year >= 2009);
    
    /* printf("year: %d, safe_year: %d\n", year, safe_year); */

    return safe_year;
}

long long pivot_time_t (const time_t * now, long long *_t)
{
    long long t;
    t = *_t;
    if (now && sizeof (time_t) == 4) {
        time_t _now;
        _now = *now;
        if (_now < 1106500000 /* Jan 23 2005 - date of writing */ )
            _now = 2147483647;
        if ((long long) t + ((long long) 1 << 31) < (long long) _now)
            t += (long long) 1 << 32;
    }
    return t;
}

static struct tm *_gmtime64_r (const time_t * now, long long *_t, struct tm *p)
{
    int v_tm_sec, v_tm_min, v_tm_hour, v_tm_mon, v_tm_wday, v_tm_tday;
    int leap;
    long long t;
    long m;
    
    p->tm_gmtoff = 0;
    p->tm_isdst  = 0;
    p->tm_zone   = "UTC";
    
    t = pivot_time_t (now, _t);
    v_tm_sec = ((long long) t % (long long) 60);
    t /= 60;
    v_tm_min = ((long long) t % (long long) 60);
    t /= 60;
    v_tm_hour = ((long long) t % (long long) 24);
    t /= 24;
    v_tm_tday = t;
    WRAP (v_tm_sec, v_tm_min, 60);
    WRAP (v_tm_min, v_tm_hour, 60);
    WRAP (v_tm_hour, v_tm_tday, 24);
    if ((v_tm_wday = (v_tm_tday + 4) % 7) < 0)
        v_tm_wday += 7;
    m = (long) v_tm_tday;
    if (m >= 0) {
        p->tm_year = 70;
        leap = LEAP_CHECK (p->tm_year);
        while (m >= (long) length_of_year[leap]) {
            m -= (long) length_of_year[leap];
            p->tm_year++;
            leap = LEAP_CHECK (p->tm_year);
        }
        v_tm_mon = 0;
        while (m >= (long) days_in_month[leap][v_tm_mon]) {
            m -= (long) days_in_month[leap][v_tm_mon];
            v_tm_mon++;
        }
    } else {
        p->tm_year = 69;
        leap = LEAP_CHECK (p->tm_year);
        while (m < (long) -length_of_year[leap]) {
            m += (long) length_of_year[leap];
            p->tm_year--;
            leap = LEAP_CHECK (p->tm_year);
        }
        v_tm_mon = 11;
        while (m < (long) -days_in_month[leap][v_tm_mon]) {
            m += (long) days_in_month[leap][v_tm_mon];
            v_tm_mon--;
        }
        m += (long) days_in_month[leap][v_tm_mon];
    }
    p->tm_mday = (int) m + 1;
    p->tm_yday = julian_days_by_month[leap][v_tm_mon] + m;
    p->tm_sec = v_tm_sec, p->tm_min = v_tm_min, p->tm_hour = v_tm_hour,
        p->tm_mon = v_tm_mon, p->tm_wday = v_tm_wday;
    
    _check_tm(p);

    return p;
}

struct tm *gmtime64_r (const long long *_t, struct tm *p)
{
    long long t;
    t = *_t;
    return _gmtime64_r (NULL, &t, p);
}

static struct tm *_localtime64_r (const time_t * now, long long *_t, struct tm *local_tm)
{
    time_t t;
    struct tm gm_tm;
    int orig_year;
    int month_diff;

    _gmtime64_r(now, _t, &gm_tm);
    orig_year = gm_tm.tm_year;

    if (gm_tm.tm_year > (2037 - 1900))
        gm_tm.tm_year = _safe_year(gm_tm.tm_year + 1900) - 1900;

    t = timegm(&gm_tm);
    localtime_r(&t, local_tm);

    local_tm->tm_year = orig_year;
    month_diff = local_tm->tm_mon - gm_tm.tm_mon;
    
    if( month_diff == 11 )
        local_tm->tm_year--;

    if( month_diff == -11 )
        local_tm->tm_year++;

    _check_tm(local_tm);
    
    return local_tm;
}

struct tm *localtime64_r (const long long *_t, struct tm *p)
{
    long long tl;
    tl = *_t;
    return _localtime64_r (NULL, &tl, p);
}
