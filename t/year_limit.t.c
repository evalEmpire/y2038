#include "time64.h"
#include <stdio.h>
#include "t/tap.c"

int main(void)
{
    struct TM gtime;
    Time64_T time = 0x00EFFFFFFFFFFFFFLL;
    printf("# time: %lld\n", time);
    gmtime64_r(&time, &gtime);
    printf("# sizeof time_t: %ld\n", sizeof(time_t));
    printf("# sizeof long long: %ld\n", sizeof(Time64_T));
    printf("# sizeof tm.tm_year: %ld\n", sizeof(gtime.tm_year));
    printf("# %04lld.%02d.%02d %02d:%02d:%02d %05ld %s\n",
        gtime.tm_year + 1900,
        gtime.tm_mon  + 1,
        gtime.tm_mday,
        
        gtime.tm_hour,
        gtime.tm_min,
        gtime.tm_sec
    );

    is_int( gtime.tm_year + 1900, 2140702833, "gtime.tm_year" );
    is_int( gtime.tm_mon  + 1,    12,         "gtime.tm_mon"  );

    done_testing();
    return 0;
}
