#include "time64.h"
#include "t/tap.h"
#define IS_LEAP_ABS(n)  (!((n) % 400) || (!((n) % 4) && ((n) % 100) && ((n) > 0)))

int mktime64_ok(Time64_T time);

int mktime64_ok(Time64_T time) {
    struct TM date;

    localtime64_r(&time, &date);
    return is_Int64( mktime64(&date), time, "mktime64(%"PRId64")", time );
}

int main(void) {
    struct TM date;
    Time64_T time;
    Year year;
    const short length_of_year[2] = { 365, 366 };
#ifdef TIME_64_DEBUG
    char result[60];
#endif

    /* Some basic round trip mktime64 tests */
    mktime64_ok((Time64_T)0);
    mktime64_ok((Time64_T)1);
    mktime64_ok((Time64_T)-1);
    mktime64_ok((Time64_T)60*60*24*15);
    mktime64_ok((Time64_T)-60*60*24*15);
    mktime64_ok((Time64_T)60*60*24*365*143);
    mktime64_ok((Time64_T)-60*60*24*365*143);
    mktime64_ok((Time64_T)60*60*24*365*433);
    mktime64_ok((Time64_T)-60*60*24*365*433);
    mktime64_ok((Time64_T)573942500); /* a leap year */
    mktime64_ok((Time64_T)-2147483647);
    mktime64_ok((Time64_T)2147483647);


    /* Test timelocal64 alias to mktime64 */
    time = 12345;
    localtime64_r(&time, &date);
    is_Int64( mktime64(&date), timelocal64(&date), "timelocal64 alias" );

    /* Test that mktime64 accepts and corrects out of bound dates */
    /* The original values of the tm_wday and tm_yday components of the
     * structure are ignored, and the original values of the other components
     * are not restricted to the ranges described in <time.h>.
     * http://www.opengroup.org/onlinepubs/009695399/functions/mktime.html
     */
    year = 1981;
    /* Thu Apr  9 12:28:20 1981 */
    date.tm_sec  = 20;
    date.tm_min  = 28;
    date.tm_hour = 11;  /* avoid overflow to prev or next day on other timezones */
    date.tm_mon  = 3;
    date.tm_mday = 9;
    date.tm_year = (year_t)(year - 1900);
    time = mktime64(&date);
#ifdef TIME_64_DEBUG
    diag ("time = %lld", time); /* 355660100LL */
    diag ("time = %s", asctime64_r(&date, result));
#endif 
    for (; year < 2400;
           year++, time += length_of_year[IS_LEAP_ABS(year)] * 60 * 60 * 24)
    {
      date.tm_sec  = 20;
      date.tm_min  = 28;
      date.tm_year = (year_t)(year - 1900);
      date.tm_mon  = 2;   /* Mar 40 == Apr 9, avoid Feb leap day */
      date.tm_mday = 40;
      date.tm_wday = 9;   /* deliberately wrong week day */
      date.tm_yday = 487; /* and wrong year day */

      /* Upon successful completion, the values of the tm_wday and tm_yday
       * components of the structure shall be set appropriately, and the other
       * components are set to represent the specified time since the Epoch,
       * but with their values forced to the ranges indicated in the <time.h>
       * entry; the final value of tm_mday shall not be set until tm_mon and
       * tm_year are determined.
       * http://www.opengroup.org/onlinepubs/009695399/functions/mktime.html
       */
      is_Int64( mktime64(&date), time, "mktime64(%"PRId64") oob, year:%ld",
                time, (long)year );
      /*diag ("time = %s", asctime64_r(&date, result));*/
    }
    
    time = 12347288900LL;     /* Sun Apr  9 12:28:20 2361 PST */
    localtime64_r(&time, &date);
#ifdef TIME_64_DEBUG
    diag ("time = %s", asctime64_r(&date, result));
#endif
    /*date.tm_hour = 12;*/
    date.tm_year = (year_t)(2361-1900);
    date.tm_mon  = 2;         /* Mar 37 == Apr 9 */
    date.tm_mday = 40;
    date.tm_wday = 9;         /* deliberately wrong week day */
    date.tm_yday = 487;       /* and wrong year day */

    is_Int64( mktime64(&date), time, "mktime64(%"PRId64") oob, year:2361",
              time);
#ifdef TIME_64_DEBUG
    diag ("time = %s", asctime64_r(&date, result));
#endif
    is_int( date.tm_mon,  3, "tm_mon corrected" );
    is_int( date.tm_mday, 9, "tm_mday corrected" );
    is_int( date.tm_yday, 98,"tm_yday corrected" );
    is_int( date.tm_wday, 0, "tm_wday corrected" );

    done_testing();
    return 0;
}
