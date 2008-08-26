#include "localtime64.h"
#include <stdio.h>

int Tests_Run = 0;

void done_testing() {
    printf("1..%d\n", Tests_Run);
}

int is_num( int have, int want, char *desc ) {
    int  ok = (have == want);

    printf("%s %d - %s\n", (ok ? "ok" : "not ok"), ++Tests_Run, desc);

    if( ok ) {
        return 1;
    }
    else {
        printf("# have: %d", have);
        printf("# want: %d", want);
        return 0;
    }
}

int main(int argc, char *argv[])
{
    struct tm gtime;
    long long time = 0x00EFFFFFFFFFFFFFLL;
    printf("# time: %lld\n", time);
    gmtime64_r(&time, &gtime);
    printf("# sizeof time_t: %ld\n", sizeof(time_t));
    printf("# sizeof long long: %ld\n", sizeof(long long));
    printf("# sizeof tm.tm_year: %ld\n", sizeof(gtime.tm_year));
    printf("# %04d.%02d.%02d %02d:%02d:%02d %05ld %s\n",
        gtime.tm_year + 1900,
        gtime.tm_mon  + 1,
        gtime.tm_mday,
        
        gtime.tm_hour,
        gtime.tm_min,
        gtime.tm_sec,
        
        (gtime.tm_gmtoff / (60 * 60)) * 100,
        gtime.tm_zone
    );

    is_num( gtime.tm_year + 1900, 2140702833, "gtime.tm_year" );
    is_num( gtime.tm_mon  + 1,    12,         "gtime.tm_mon"  );

    done_testing();

    return 0;
}
