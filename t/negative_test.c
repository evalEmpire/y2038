#include "localtime64.h"
#include <stdio.h>
#include "tap.c"

int main(void) {
    struct tm date;
    Time64_T time = -10;

    gmtime64_r(&time, &date);
    is_int(date.tm_year, 69, "gmtime64_r(-10)");

    time = -0x00EFFFFFFFFFFFFFLL;
    gmtime64_r(&time, &date);
    is_int(date.tm_year + 1900, -2140698894, "  -2**54");

    done_testing();
    return 0;
}
