#include <locale.h>
#include <time64.h>
#include "t/tap.c"

void strftime64_ok(struct TM *date, const char *format, char *expect) {
    char      buffer[1000];
    size_t    size;

    size = strftime64(buffer, 1000, format, date);
    is_int( (int)size, (int)strlen(buffer), "returned right size" );
    is_str( buffer, expect, format );
}

int main(void) {
    struct TM date;
    Time64_T  time;

    setlocale(LC_ALL, "");

    time = 60 * 60 * 24 * 15;  /* Jan 15th, 1970 ish */
    localtime64_r(&time, &date);

    strftime64_ok(&date, "%Y", "1970");
    strftime64_ok(&date, "%m", "01" );

    done_testing();

    return 0;
}
