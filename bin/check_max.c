/* A little program to test the limits of your system's time functions */

#include <time.h>
#include <stdio.h>
#include <math.h>

struct tm Test_TM;

time_t Time_Max;
time_t Time_Min;

time_t Time_Zero = 0;


/* Visual C++ 2008's difftime() can't do negative times */
double my_difftime(time_t left, time_t right) {
	double diff = (double)left - (double)right;
	return diff;
}

void check_date_max( struct tm * (*date_func)(const time_t *), char *func_name ) {
    struct tm *date;
    time_t time        = 0;
    time_t last_time   = 0;
    time_t time_change = Time_Max;

    /* Binary search for the exact failure point */
    do {
        time += time_change;

        date = (*date_func)(&time);

        /* date_func() broke or tm_year overflowed or time_t overflowed */
        if(date == NULL || date->tm_year < 69 || time < last_time) {
            time = last_time;
            time_change = time_change / 2;
        }
        else {
            last_time = time;
        }
    } while(time_change > 0);

    printf("%s_max %.0f\n", func_name, my_difftime(last_time, Time_Zero));
}


void check_date_min( struct tm * (*date_func)(const time_t *), char *func_name ) {
    struct tm *date;
    time_t time        = 0;
    time_t last_time   = 0;
    time_t time_change = Time_Min;

    /* Binary search for the exact failure point */
    do {
        time += time_change;

        date = (*date_func)(&time);

        /* gmtime() broke or tm_year overflowed or time_t overflowed */
        if(date == NULL || date->tm_year > 70 || time > last_time) {
            time = last_time;
            time_change = time_change / 2;
        }
        else {
            last_time = time;
        }
    } while(time_change < 0);

    printf("%s_min %.0f\n", func_name, my_difftime(last_time, Time_Zero));
}


void guess_time_limits_from_types(void) {
    if( sizeof(time_t) == 4 ) {
        /* y2038 bug, out to 2**31-1 */
        Time_Max =  2147483647;
        Time_Min = -2147483648;
    }
    else if( sizeof(time_t) >= 8 ) {
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
    guess_time_limits_from_types();
    check_date_max(gmtime, "gmtime");
    check_date_max(localtime, "localtime");
    check_date_min(gmtime, "gmtime");
    check_date_min(localtime, "localtime");

    return 0;
}
