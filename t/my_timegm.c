#include <stdlib.h>
#include "tap.c"
#include "localtime64.h"

int main(void) {
    struct tm date;
    Time64_T time = 0;

    setenv("TZ", "UTC", 1);
    gmtime64_r(&time, &date);
    is_int( (int)my_timegm(&date), (int)timegm(&date), "my_timegm(0), local before" );

    time = 60*60*16;
    gmtime64_r(&time, &date);
    is_int( (int)my_timegm(&date), (int)timegm(&date), "my_timegm(60*60*16)" );

    time = 60*60*24*364 + 60*60*23;
    gmtime64_r(&time, &date);
    is_int( (int)my_timegm(&date), (int)timegm(&date), "my_timegm(60*60*24*364 + 60*60*23)" );

    time = -1;
    gmtime64_r(&time, &date);
    is_int( (int)my_timegm(&date), (int)timegm(&date), "my_timegm(-1)" );

    time = -60*60*24*364 -60*60*23;
    gmtime64_r(&time, &date);
    is_int( (int)my_timegm(&date), (int)timegm(&date), "my_timegm(-60*60*24*364 - 60*60*23)" );

    time = 1230774010;
    gmtime64_r(&time, &date);
    is_int( (int)my_timegm(&date), (int)timegm(&date), "my_timegm(1230774010)" );

    time = 1262296406;
    gmtime64_r(&time, &date);
    is_int( (int)my_timegm(&date), (int)timegm(&date), "my_timegm(1262296406)" );

    time = 1325380799;
    gmtime64_r(&time, &date);
    is_int( (int)my_timegm(&date), (int)timegm(&date), "my_timegm(1325380799)" );

    time = 1356982397;
    gmtime64_r(&time, &date);
    is_int( (int)my_timegm(&date), (int)timegm(&date), "my_timegm(1356982397)" );

    time = -31536000;
    gmtime64_r(&time, &date);
    is_int( (int)my_timegm(&date), (int)timegm(&date), "my_timegm(-31536000)" );

    /* Negative leap year */
    time = -302216279;
    gmtime64_r(&time, &date);
    is_int( (int)my_timegm(&date), (int)timegm(&date), "my_timegm(-302216279)" );

    done_testing();
    return(0);
}
