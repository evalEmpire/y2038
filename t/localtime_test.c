#include <stdio.h>
#include <time.h>
#include "time64.h"

int main(void)
{
    struct TM d;
    long long time = 1202380093;

    d.tm_year = 90;

    while(d.tm_year < 800) {
        localtime64_r(&time, &d);
        printf("%lld %d %d %d %d %d %d %d %d %d\n",
                time,
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
        
        time += 60 * 60 * 11 + 1;
    }
    return(0);
}
