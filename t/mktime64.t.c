#include "time64.h"
#include "t/tap.c"

int mktime64_ok(Time64_T time) {
    struct TM date;

    localtime64_r(&time, &date);
    return is_Int64( mktime64(&date), time, "mktime64(%lld)", time );
}

int main(void) {
    struct TM date;
    Time64_T time;

    mktime64_ok((Time64_T)0);
    mktime64_ok((Time64_T)1);
    mktime64_ok((Time64_T)-1);
    mktime64_ok((Time64_T)60*60*24*15);
    mktime64_ok((Time64_T)60*60*24*15);
    mktime64_ok((Time64_T)60*60*24*365*143);
    mktime64_ok((Time64_T)-60*60*24*365*143);
    mktime64_ok((Time64_T)60*60*24*365*433);
    mktime64_ok((Time64_T)-60*60*24*365*433);

    /* Test timelocal64 alias to mktime64 */
    time = 12345;
    localtime64_r(&time, &date);
    is_Int64( mktime64(&date), timelocal64(&date), "timelocal64 alias" );

    done_testing();
    return 0;
}
