#include <locale.h>
#include <time64.h>
#include "t/tap.c"

int main(void) {
    struct TM date;
    Time64_T  time;
    char      buffer[10000];
    int       size;

    setlocale(LC_ALL, "");

    time = 60 * 60 * 24 * 15;  /* Jan 15th, 1970 ish */
    localtime64_r(&time, &date);

    size = strftime64(buffer, 10000, "%Y", &date);
    is_int( size, (int)strlen(buffer), "returned right size" );
    is_str( buffer, "1970", "%%Y" );

    size = strftime64(buffer, 10000, "%m", &date);
    is_int( size, (int)strlen(buffer), "returned right size" );
    is_str( buffer, "01", "%%m" );

    done_testing();

    return 0;
}
