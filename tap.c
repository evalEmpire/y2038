#include <stdlib.h>
#include <stdio.h>

int Test_Count = 0;

void skip_all(const char *reason) {
    printf("1..0 # Skip %s\n", reason);
    exit(0);
}

int ok(const int test, const char *name) {
    printf("%s %s\n", (test ? "ok" : "not ok"), name);

    if( !test ) {
        fprintf(stderr, "Failed test '%s'.\n", name);
    }

    Test_Count++;

    return test;
}

int is_int(const int have, const int want, const char *name) {
    int test = (have == want);
    ok( test, name );

    if( !test ) {
        fprintf(stderr, "have: %d\nwant: %d\n", have, want);
    }

    return test;
}

void done_testing(void) {
    printf("1..%d\n", Test_Count);
}
