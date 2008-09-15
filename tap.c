#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

int Test_Count = 0;


int diag(const char *message, ...) {
    va_list args;
    va_start(args, message);

    fprintf(stderr, "# ");
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");

    va_end(args);

    return(0);
}


void skip_all(const char *reason) {
    printf("1..0 # Skip %s\n", reason);
    exit(0);
}

int ok(const int test, const char *name) {
    Test_Count++;

    printf("%s %d %s\n", (test ? "ok" : "not ok"), Test_Count, name);

    if( !test ) {
        diag("Failed test '%s'.", name);
    }

    return test;
}

int is_int(const int have, const int want, const char *name) {
    int test = (have == want);
    ok( test, name );

    if( !test ) {
        diag("have: %d", have);
        diag("want: %d", want);
    }

    return test;
}

int is_not_null(void *arg, const char *name) {
    return( ok( arg != NULL, name ) );
}

int tm_ok(const struct tm *have,
          const int year, const int mon, const int mday,
          const int hour, const int min, const int sec)
{
    int ok = 1;

    ok *= is_int(have->tm_year, year,  "tm.year");
    ok *= is_int(have->tm_mon,  mon,   "   month");
    ok *= is_int(have->tm_mday, mday,  "   day");
    ok *= is_int(have->tm_hour, hour,  "   hour");
    ok *= is_int(have->tm_min,  min,   "   min");
    ok *= is_int(have->tm_sec,  sec,   "   sec");

    return ok;
}
    

void done_testing(void) {
    printf("1..%d\n", Test_Count);
}
