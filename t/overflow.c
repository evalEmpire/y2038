#include <errno.h>
#include "localtime64.h"
#include "tap.c"

int main(void) {
    Time64_T time = 0x0FFFFFFFFFFFFFFFLL;;
    struct tm date;
    struct tm *error;

#ifndef EOVERFLOW
    skip_all( "EOVERFLOW not defined on this system" );
#else
    if( sizeof(date.tm_year) > 4 )
        skip_all( "tm_year is too large to overflow (yay!)" );

    error = gmtime64_r(&time, &date);

    ok( error == NULL,          "gmtime64_r() returned null on overflow" );
    is_int( errno, EOVERFLOW,   "  errno set to EOVERFLOW" );

    done_testing();
#endif

    return 0;
}
 
