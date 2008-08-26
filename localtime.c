#include <time.h>
#include <stdio.h>

int main () {
    struct tm d;
    time_t time = 1202380093;
    localtime_r(&time, &d);
    printf("%d %d %d %d %d %d %d %d %d %d\n",
            (int)time,
            d.tm_sec,
            d.tm_min,
            d.tm_hour,
            d.tm_mday,
            d.tm_mon,
            d.tm_year,
            d.tm_wday,
            d.tm_yday,
            d.tm_isdst
    );
    
    return(0);
}