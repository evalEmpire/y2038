#include <time.h>

#ifndef LOCALTIME64_H
#    define LOCALTIME64_H

/* 64 bit time_t */
typedef long long               Time64_T;

struct tm *gmtime64_r    (const Time64_T *, struct tm *);
struct tm *localtime64_r (const Time64_T *, struct tm *);

#endif
