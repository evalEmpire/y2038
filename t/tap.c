#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "time64.h"

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

int ok(const int test, const char *message, ...) {
    va_list args;

    Test_Count++;

    va_start(args, message);

    printf("%s %d ", (test ? "ok" : "not ok"), Test_Count);
    vprintf(message, args);
    printf("\n");

    va_end(args);

    if( !test ) {
        diag("Failed test");
        diag(message, args);
    }

    return test;
}

int is_int(const int have, const int want, const char *message, ...) {
    int test = (have == want);
    va_list args;
    va_start(args, message);

    ok( test, message, args );

    if( !test ) {
        diag("have: %d", have);
        diag("want: %d", want);
    }

    va_end(args);

    return test;
}


int is_ll(const long long have, const long long want, const char *message, ...) {
    int test = (have == want);
    va_list args;
    va_start(args, message);

    ok( test, message, args );

    if( !test ) {
        diag("have: %lld", have);
        diag("want: %lld", want);
    }

    va_end(args);

    return test;
}


int is_Int64(const Int64 have, const Int64 want, const char *name) {
    int test = (have == want);
    ok( test, name );

    if( !test ) {
        diag("have: %lld", have);
        diag("want: %lld", want);
    }

    return test;
}

int is_not_null(void *arg, const char *name) {
    return( ok( arg != NULL, name ) );
}

int tm_ok(const struct TM *have,
          const int year, const int mon, const int mday,
          const int hour, const int min, const int sec)
{
    int ok = 1;

    ok *= is_ll((Year)have->tm_year, (Year)year,  "tm.year");
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
