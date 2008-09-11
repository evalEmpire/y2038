#include <stdlib.h>
#include <stdio.h>

int Test_Count = 0;

void skip_all(const char *reason) {
    printf("1..0 # Skip %s\n", reason);
    exit(0);
}

int ok(const int test, const char *name) {
    Test_Count++;

    printf("%s %d %s\n", (test ? "ok" : "not ok"), Test_Count, name);

    if( !test ) {
        fprintf(stderr, "Failed test '%s'.\n", name);
    }

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

int diag(const char *message) {
    fprintf(stderr, "# %s\n", message);
    return(0);
}

void done_testing(void) {
    printf("1..%d\n", Test_Count);
}
