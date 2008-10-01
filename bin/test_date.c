#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int check_date(struct tm *date, time_t time, char *name) {
    if( date == NULL ) {
        printf("%s(%lld) returned null\n", name, (long long)time);
        return 1;
    }
    else {
        printf("%s(%lld): %s\n", name, (long long)time, asctime(date));
        return 0;
    }
}


int main(int argc, char *argv[]) {
    long long number;
    time_t time;
    struct tm *localdate;
    struct tm *gmdate;

    if( argc <= 1 ) {
        printf("usage: %s <time>\n", argv[0]);
        return 1;
    }

    number = strtoll(argv[1], NULL, 0);
    time = (time_t)number;

    printf("input: %lld, time: %lld\n", number, (long long)time);
    if( time != number ) {
        printf("time_t overflowed\n");
        return 0;
    }

    localdate = localtime(&time);
    gmdate    = gmtime(&time);

    check_date(localdate, time, "localtime");
    check_date(gmdate,    time, "gmtime");

    return 0;
}
