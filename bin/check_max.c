/* A little program to test the limits of your system's time functions */

#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

struct tm Test_TM;

time_t Time_Max;
time_t Time_Min;

time_t Time_Zero = 0;


char *dump_date(const struct tm *date) {
    char *dump = malloc(80 * sizeof(char));
    sprintf(
        dump,
        "{ %d, %d, %d, %d, %d, %d }",
        date->tm_sec, date->tm_min, date->tm_hour, date->tm_mday, date->tm_mon, date->tm_year
    );

    return dump;
}


/* Visual C++ 2008's difftime() can't do negative times */
double my_difftime(time_t left, time_t right) {
    double diff = (double)left - (double)right;
    return diff;
}

time_t check_date_max( struct tm * (*date_func)(const time_t *), const char *func_name ) {
    struct tm *date;
    time_t time        = Time_Max;
    time_t good_time   = 0;
    time_t time_change = Time_Max;

    /* Binary search for the exact failure point */
    do {
        printf("# Trying %s(%.0f) max...", func_name, my_difftime(time, Time_Zero));
        date = (*date_func)(&time);

        time_change /= 2;

        /* date_func() broke or tm_year overflowed or time_t overflowed */
        if(date == NULL || date->tm_year < 69 || time < good_time) {
            printf(" failed\n");
            time -= time_change;
        }
        else {
            printf(" success\n");
            good_time = time;
            time += time_change;
        }
    } while(time_change > 0 && good_time < Time_Max);

    printf("%s_max %.0f\n", func_name, my_difftime(good_time, Time_Zero));
    return(good_time);
}


time_t check_date_min( struct tm * (*date_func)(const time_t *), const char *func_name ) {
    struct tm *date;
    time_t time        = Time_Min;
    time_t good_time   = 0;
    time_t time_change = Time_Min;

    /* Binary search for the exact failure point */
    do {
        printf("# Trying %s(%.0f) min...", func_name, my_difftime(time, Time_Zero));
        date = (*date_func)(&time);

        time_change /= 2;

        /* date_func() broke or tm_year overflowed or time_t overflowed */
        if(date == NULL || date->tm_year > 70 || time > good_time) {
            printf(" failed\n");
            time -= time_change;
        }
        else {
            printf(" success\n");
            good_time = time;
            time += time_change;
        }
    } while((time_change > 0) && (good_time > Time_Min));

    printf("%s_min %.0f\n", func_name, my_difftime(good_time, Time_Zero));
    return(good_time);
}


time_t check_to_time_max( time_t (*to_time)(struct tm *), const char *func_name,
                          struct tm * (*to_date)(const time_t *) )
{
    time_t round_trip;
    time_t time        = Time_Max;
    time_t good_time   = 0;
    struct tm *date;
    struct tm *good_date;
    time_t time_change = Time_Max;

    /* Binary search for the exact failure point */
    do {
        printf("# Trying %s(%.0f) max...", func_name, my_difftime(time, Time_Zero));
        date = (*to_date)(&time);
        round_trip = (*to_time)(date);

        time_change /= 2;

        /* date_func() broke or tm_year overflowed or time_t overflowed */
        if(time != round_trip) {
            printf(" failed\n");
            time -= time_change;
        }
        else {
            printf(" success\n");
            good_time = time;
            good_date = date;
            time += time_change;
        }
    } while(time_change > 0 && good_time < Time_Max);

    printf("%s_max %.0f %s\n", func_name, my_difftime(good_time, Time_Zero), dump_date(good_date));
    return(good_time);
}


time_t check_to_time_min( time_t (*to_time)(struct tm *), const char *func_name,
                          struct tm * (*to_date)(const time_t *) )
{
    time_t round_trip;
    time_t time        = Time_Min;
    time_t good_time   = 0;
    struct tm *date;
    struct tm *good_date;
    time_t time_change = Time_Min;

    /* Binary search for the exact failure point */
    do {
        printf("# Trying %s(%.0f) min...", func_name, my_difftime(time, Time_Zero));
        date = (*to_date)(&time);
        round_trip = (*to_time)(date);

        time_change /= 2;

        /* date_func() broke or tm_year overflowed or time_t overflowed */
        if(time != round_trip) {
            printf(" failed\n");
            time -= time_change;
        }
        else {
            printf(" success\n");
            good_time = time;
            good_date = date;
            time += time_change;
        }
    } while((time_change > 0) && (good_time > Time_Min));

    printf("%s_min %.0f %s\n", func_name, my_difftime(good_time, Time_Zero), dump_date(good_date));
    return(good_time);
}


void guess_time_limits_from_types(void) {
    if( sizeof(time_t) == 4 ) {
        /* y2038 bug, out to 2**31-1 */
        Time_Max =  2147483647;
        Time_Min = -2147483647 - 1;     /* "C90 doesn't have negative constants, only
                                           positive ones that have been negated." */
    }
    else if( sizeof(time_t) >= 8 ) {
        /* The compiler might warn about overflowing in the assignments
           below.  Don't worry, these won't get run in that case */
        if( sizeof(Test_TM.tm_year) == 4 ) {
            /* y2**31-1 bug */
            Time_Max =  67768036160140799LL;
            Time_Min = -67768036191676800LL;
        }
        else {
            /* All the way out to 2**63-1 */
            Time_Max =  9223372036854775807LL;
            Time_Min = -9223372036854775807LL;
        }
    }
    else {
        printf("Weird sizeof(time_t): %ld\n", sizeof(time_t));
    }
}


int main(void) {
    time_t gmtime_max;
    time_t gmtime_min;
    time_t localtime_max;
    time_t localtime_min;

    guess_time_limits_from_types();

    gmtime_max = check_date_max(gmtime, "gmtime");
    gmtime_min = check_date_min(gmtime, "gmtime");

    localtime_max = check_date_max(localtime, "localtime");
    localtime_min = check_date_min(localtime, "localtime");

#ifdef HAS_TIMEGM
    Time_Max = gmtime_max;
    Time_Min = gmtime_min;
    check_to_time_max(timegm, "timegm", gmtime);
    check_to_time_min(timegm, "timegm", gmtime);
#endif

    Time_Max = localtime_max;
    Time_Min = localtime_min;
    check_to_time_max(mktime, "mktime", localtime);
    check_to_time_min(mktime, "mktime", localtime);

    return 0;
}
