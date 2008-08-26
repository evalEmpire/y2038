/* localtime64.h is Copyright (C) 2007  Michael G Schwern

   It is a modification on and improvement of code written by 
   Paul Sheer from 2038bug.com.  His copyright, conditions and 
   disclaimer are reproduced below.
*/

/* Copyright (C) 2005  Paul Sheer

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

Programmers who have available to them 64-bit time values as a 'long
long' type can use localtime64_r() and gmtime64_r() which correctly
converts the time even on 32-bit systems. Whether you have 64-bit time 
values will depend on the operating system.

localtime64_r() is a 64-bit equivalent of localtime_r().

gmtime64_r() is a 64-bit equivalent of gmtime_r().

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

/* 28 year calendar cycle between 2010 and 2037 */
static const int safe_years[28] = {
    2016, 2017, 2018, 2019,
    2020, 2021, 2022, 2023,
    2024, 2025, 2026, 2027,
    2028, 2029, 2030, 2031,
    2032, 2033, 2034, 2035,
    2036, 2037, 2010, 2011,
    2012, 2013, 2014, 2015
};

static const int dow_year_start[28] = {
    5, 0, 1, 2,     /* 2016 - 2019 */
    3, 5, 6, 0,
    1, 3, 4, 5,
    6, 1, 2, 3,
    4, 6, 0, 1,
    2, 4, 5, 6,     /* 2036, 2037, 2010, 2011 */
    0, 2, 3, 4      /* 2012, 2013, 2014, 2015 */
};


#define IS_LEAP(n)	((!(((n) + 1900) % 400) || (!(((n) + 1900) % 4) && (((n) + 1900) % 100))) != 0)
#define WRAP(a,b,m)	((a) = ((a) <  0  ) ? ((b)--, (a) + (m)) : (a))

int _is_exception_century(long long year)
{
    int is_exception = ((year % 100 == 0) && !(year % 400 == 0));
    /* printf("is_exception_century: %s\n", is_exception ? "yes" : "no"); */

    return(is_exception);
}

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

    if( !IS_LEAP(tm->tm_year) ) {
        /* no more than 365 days in a non_leap year */
        assert( tm->tm_yday <= 364 );

        /* and no more than 28 days in Feb */
        if( tm->tm_mon == 1 ) {
            assert( tm->tm_mday <= 28 );
        }
    }
}

/* The exceptional centuries without leap years cause the cycle to
   shift by 16
*/
int _cycle_offset(long long year)
{
    const long long start_year = 2000;
    long long year_diff  = year - start_year - 1;
    long long exceptions  = year_diff / 100;
    exceptions     -= year_diff / 400;

    assert( year >= 2001 );

    /* printf("year: %d, exceptions: %d\n", year, exceptions); */

    return exceptions * 16;
}

/* For a given year after 2038, pick the latest possible matching
   year in the 28 year calendar cycle.
*/
#define SOLAR_CYCLE_LENGTH 28
int _safe_year(long long year)
{
    int safe_year;
    long long year_cycle = year + _cycle_offset(year);

    /* Change non-leap xx00 years to an equivalent */
    if( _is_exception_century(year) )
        year_cycle += 11;
    
    year_cycle %= SOLAR_CYCLE_LENGTH;
    
    safe_year = safe_years[year_cycle];

    assert(safe_year <= 2037 && safe_year >= 2010);
    
    /*
    printf("year: %d, year_cycle: %d, safe_year: %d\n",
           year, year_cycle, safe_year);
    */

    return safe_year;
}

struct tm *gmtime64_r (const long long *in_time, struct tm *p)
{
    int v_tm_sec, v_tm_min, v_tm_hour, v_tm_mon, v_tm_wday, v_tm_tday;
    int leap;
    long m;
    long long time = *in_time;
    
    p->tm_gmtoff = 0;
    p->tm_isdst  = 0;
    p->tm_zone   = "UTC";
    
    v_tm_sec =  time % 60;
    time /= 60;
    v_tm_min =  time % 60;
    time /= 60;
    v_tm_hour = time % 24;
    time /= 24;
    v_tm_tday = time;
    WRAP (v_tm_sec, v_tm_min, 60);
    WRAP (v_tm_min, v_tm_hour, 60);
    WRAP (v_tm_hour, v_tm_tday, 24);
    if ((v_tm_wday = (v_tm_tday + 4) % 7) < 0)
        v_tm_wday += 7;
    m = (long) v_tm_tday;
    if (m >= 0) {
        p->tm_year = 70;
        leap = IS_LEAP (p->tm_year);
        while (m >= (long) length_of_year[leap]) {
            m -= (long) length_of_year[leap];
            p->tm_year++;
            leap = IS_LEAP (p->tm_year);
        }
        v_tm_mon = 0;
        while (m >= (long) days_in_month[leap][v_tm_mon]) {
            m -= (long) days_in_month[leap][v_tm_mon];
            v_tm_mon++;
        }
    } else {
        p->tm_year = 69;
        leap = IS_LEAP (p->tm_year);
        while (m < (long) -length_of_year[leap]) {
            m += (long) length_of_year[leap];
            p->tm_year--;
            leap = IS_LEAP (p->tm_year);
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


struct tm *localtime64_r (const long long *time, struct tm *local_tm)
{
    time_t safe_time;
    struct tm gm_tm;
    long long orig_year;
    int month_diff;

    gmtime64_r(time, &gm_tm);
    orig_year = gm_tm.tm_year;

    if (gm_tm.tm_year > (2037 - 1900))
        gm_tm.tm_year = _safe_year(gm_tm.tm_year + 1900) - 1900;

    safe_time = timegm(&gm_tm);
    localtime_r(&safe_time, local_tm);

    local_tm->tm_year = orig_year;
    month_diff = local_tm->tm_mon - gm_tm.tm_mon;

    /*  When localtime is Dec 31st previous year and
        gmtime is Jan 1st next year.
    */
    if( month_diff == 11 ) {
        local_tm->tm_year--;
    }

    /*  When localtime is Jan 1st, next year and
        gmtime is Dec 31st, previous year.
    */
    if( month_diff == -11 ) {
        local_tm->tm_year++;
    }

    /* GMT is Jan 1st, xx01 year, but localtime is still Dec 31st 
       in a non-leap xx00.  There is one point in the cycle
       we can't account for which the safe xx00 year is a leap
       year.  So we need to correct for Dec 31st comming out as
       the 366th day of the year.
    */
    if( !IS_LEAP(local_tm->tm_year) && local_tm->tm_yday == 365 )
        local_tm->tm_yday--;

    _check_tm(local_tm);
    
    return local_tm;
}
