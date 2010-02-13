/* A little program to test the limits of your system's time functions */

#include "time64_config.h"
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

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

    return(good_time);
}


struct tm * check_to_time_max( time_t (*to_time)(struct tm *), const char *func_name,
                          struct tm * (*to_date)(const time_t *) )
{
    time_t round_trip;
    time_t time        = Time_Max;
    time_t good_time   = 0;
    struct tm *date;
    struct tm *good_date = malloc(sizeof(struct tm));
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
            memcpy(good_date, date, sizeof(struct tm));
            time += time_change;
        }
    } while(time_change > 0 && good_time < Time_Max);

    return(good_date);
}


struct tm * check_to_time_min( time_t (*to_time)(struct tm *), const char *func_name,
                          struct tm * (*to_date)(const time_t *) )
{
    time_t round_trip;
    time_t time        = Time_Min;
    time_t good_time   = 0;
    struct tm *date;
    struct tm *good_date = malloc(sizeof(struct tm));
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
            memcpy(good_date, date, sizeof(struct tm));
            time += time_change;
        }
    } while((time_change > 0) && (good_time > Time_Min));

    return(good_date);
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


/* Dump a tm struct as a json fragment */
char * tm_as_json(const struct tm* date) {
    char *date_json = malloc(sizeof(char) * 512);
#ifdef HAS_TM_TM_ZONE
    char zone_json[32];
#endif
#ifdef HAS_TM_TM_GMTOFF
    char gmtoff_json[32];
#endif

    sprintf(date_json,
            "\"tm_sec\": %d, \"tm_min\": %d, \"tm_hour\": %d, \"tm_mday\": %d, \"tm_mon\": %d, \"tm_year\": %d, \"tm_wday\": %d, \"tm_yday\": %d, \"tm_isdst\": %d",
            date->tm_sec, date->tm_min, date->tm_hour, date->tm_mday,
            date->tm_mon, date->tm_year, date->tm_wday, date->tm_yday, date->tm_isdst
    );

#ifdef HAS_TM_TM_ZONE
    sprintf(zone_json, ", \"tm_zone\": \"%s\"", date->tm_zone);
    strcat(date_json, zone_json);
#endif
#ifdef HAS_TM_TM_GMTOFF
    sprintf(gmtoff_json, ", \"tm_gmtoff\": %ld", date->tm_gmtoff);
    strcat(date_json, gmtoff_json);
#endif

    return date_json;
}


int main(void) {
    time_t gmtime_max;
    time_t gmtime_min;
    time_t localtime_max;
    time_t localtime_min;
#ifdef HAS_TIMEGM
    struct tm* timegm_max;
    struct tm* timegm_min;
#endif
    struct tm* mktime_max;
    struct tm* mktime_min;

    guess_time_limits_from_types();

    gmtime_max = check_date_max(gmtime, "gmtime");
    gmtime_min = check_date_min(gmtime, "gmtime");

    localtime_max = check_date_max(localtime, "localtime");
    localtime_min = check_date_min(localtime, "localtime");

#ifdef HAS_TIMEGM
    Time_Max = gmtime_max;
    Time_Min = gmtime_min;
    timegm_max = check_to_time_max(timegm, "timegm", gmtime);
    timegm_min = check_to_time_min(timegm, "timegm", gmtime);
#endif

    Time_Max = localtime_max;
    Time_Min = localtime_min;
    mktime_max = check_to_time_max(mktime, "mktime", localtime);
    mktime_min = check_to_time_min(mktime, "mktime", localtime);

    printf("# system time.h limits, as JSON\n");
    printf("{\n");

    printf("    \"gmtime\": { \"max\": %.0f, \"min\": %0.f },\n",
           my_difftime(gmtime_max, Time_Zero),
           my_difftime(gmtime_min, Time_Zero)
    );

    printf("    \"localtime\": { \"max\": %.0f, \"min\": %0.f },\n",
           my_difftime(localtime_max, Time_Zero),
           my_difftime(localtime_min, Time_Zero)
    );

    printf("    \"mktime\": {\n");
    printf("        \"max\": { %s },\n", tm_as_json(mktime_max));
    printf("        \"min\": { %s }\n", tm_as_json(mktime_min));
    printf("    }\n");

#ifdef HAS_TIMEGM
    printf("    ,\n");
    printf("    \"timegm\": {\n");
    printf("        \"max\": { %s },\n", tm_as_json(timegm_max));
    printf("        \"min\": { %s }\n", tm_as_json(timegm_min));
    printf("    }\n");
#endif

    printf("}\n");

    return 0;
}
