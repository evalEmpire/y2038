/* A little program to test the limits of your system's time functions */

#include <time.h>
#include <stdio.h>
#include <math.h>

time_t Time_Zero = 0;

void check_gmtime_max (void)
{
    struct tm *date;

    /* Let's not bother checking anything smaller than 2**30-1 */
    time_t time = 1073741823;
    time_t last_time = 0;
    time_t time_change;
    int i;

    for (i = 30; i <= 63; i++) {
        date = gmtime(&time);

        /* gmtime() broke or tm_year overflowed */
        if(date == NULL || date->tm_year < 69)
          break;

        last_time = time;
        time += time + 1;

        /* time_t overflowed */
        if( time < last_time )
            break;
    }

    /* Binary search for the exact failure point */
    time = last_time;
    time_change = last_time / 2;

    do {
        time += time_change;

        date = gmtime(&time);

        /* gmtime() broke or tm_year overflowed or time_t overflowed */
        if(date == NULL || date->tm_year < 69 || time < last_time) {
            time = last_time;
            time_change = time_change / 2;
        }
        else {
            last_time = time;
        }
    } while(time_change > 0);

    printf("%20s %.0f\n", "gmtime max", difftime(last_time, Time_Zero));
}


void check_gmtime_min (void)
{
    struct tm *date;
    time_t time = -1;
    time_t last_time = 0;
    time_t time_change;
    int i;

    for (i = 1; i <= 63; i++) {
        date = gmtime(&time);

        /* gmtime() broke or tm_year underflowed */
        if(date == NULL || date->tm_year > 70)
            break;

        last_time = time;
        time += time;

        /* time_t underflowed */
        if( time > last_time )
            break;
    }

    /* Binary search for the exact failure point */
    time = last_time;
    time_change = last_time / 2;

    do {
        time -= time_change;

        date = gmtime(&time);

        /* gmtime() broke or tm_year overflowed or time_t overflowed */
        if(date == NULL || date->tm_year < 69 || time < last_time) {
            time = last_time;
            time_change = time_change / 2;
        }
        else {
            last_time = time;
        }
    } while(time_change > 0);

    printf("%20s %.0f\n", "gmtime min", difftime(last_time, Time_Zero));
}


void check_localtime_max (void)
{
    struct tm *date;

    /* Let's not bother checking anything smaller than 2**30-1 */
    time_t time = 1073741823;
    time_t last_time = 0;
    int i;

    for (i = 30; i <= 63; i++) {
        date = localtime(&time);

        /* gmtime() broke or tm_year overflowed */
        if(date == NULL || date->tm_year < 69)
            break;

        last_time = time;
        time += time + 1;

        /* time_t overflowed */
        if( time < last_time )
            break;
    }

    printf("%20s %.0f\n", "localtime max", difftime(last_time, Time_Zero));
}


void check_localtime_min (void)
{
    struct tm *date;
    time_t time = -1;
    time_t last_time = 0;
    int i;

    for (i = 1; i <= 63; i++) {
        date = localtime(&time);

        /* gmtime() broke or tm_year underflowed */
        if(date == NULL || date->tm_year > 70)
            break;

        last_time = time;
        time += time;

        /* time_t underflowed */
        if( time > last_time )
            break;
    }

    printf("%20s %.0f\n", "localtime min", difftime(last_time, Time_Zero));
}


int main(void) {
    check_gmtime_max();
    check_gmtime_min();
    check_localtime_max();
    check_localtime_min();

    return 0;
}

