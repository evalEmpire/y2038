#include "localtime64.h"
#include <stdio.h>
#include "tap.c"

int main(void) {
    struct tm date;
    Time64_T time = -10;

    gmtime64_r(&time, &date);
    is_int(date.tm_year, 69, "gmtime64_r(-10)");

    /* -2140698894-01-13 14:56:01 */
    time = -0x00EFFFFFFFFFFFFFLL;
    gmtime64_r(&time, &date);
    is_int(date.tm_year + 1900, -2140698894, "  -2**54 year");
    is_int(date.tm_mon  + 1,    1,           "    month");
    is_int(date.tm_mday,        13,          "    day");
    is_int(date.tm_hour,        14,          "    hour");
    is_int(date.tm_min,         56,          "    min");
    is_int(date.tm_sec,         1,           "    sec");

    done_testing();
    return 0;
}
