int Test_Count = 0;

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
