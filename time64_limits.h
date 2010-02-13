/* 
   Maximum and minimum inputs your system's respective time functions
   can correctly handle.  time64.h will use your system functions if
   the input falls inside these ranges and corresponding USE_SYSTEM_*
   constant is defined.
*/

#ifndef TIME64_LIMITS_H
#define TIME64_LIMITS_H

/* Max/min for localtime() */
#define SYSTEM_LOCALTIME_MAX     2147483647
#define SYSTEM_LOCALTIME_MIN    -2147483647-1

/* Max/min for gmtime() */
#define SYSTEM_GMTIME_MAX        2147483647
#define SYSTEM_GMTIME_MIN       -2147483647-1

/* Max/min for mktime() */
#define SYSTEM_MKTIME_MAX_TM_SEC        7
#define SYSTEM_MKTIME_MAX_TM_MIN        14
#define SYSTEM_MKTIME_MAX_TM_HOUR       19
#define SYSTEM_MKTIME_MAX_TM_MDAY       18
#define SYSTEM_MKTIME_MAX_TM_MON        0
#define SYSTEM_MKTIME_MAX_TM_YEAR       138
#define SYSTEM_MKTIME_MAX_TM_WDAY       1
#define SYSTEM_MKTIME_MAX_TM_YDAY       17
#define SYSTEM_MKTIME_MAX_TM_ISDST      0
#ifdef HAS_TM_TM_ZONE
    #define SYSTEM_MKTIME_MAX_TM_ZONE   "PST"
#endif
#ifdef HAS_TM_TM_GMTOFF
    #define SYSTEM_MKTIME_MAX_TM_GMTOFF -28800
#endif

#define SYSTEM_MKTIME_MIN_TM_SEC        52
#define SYSTEM_MKTIME_MIN_TM_MIN        45
#define SYSTEM_MKTIME_MIN_TM_HOUR       12
#define SYSTEM_MKTIME_MIN_TM_MDAY       13
#define SYSTEM_MKTIME_MIN_TM_MON        11
#define SYSTEM_MKTIME_MIN_TM_YEAR       1
#define SYSTEM_MKTIME_MIN_TM_WDAY       5
#define SYSTEM_MKTIME_MIN_TM_YDAY       346
#define SYSTEM_MKTIME_MIN_TM_ISDST      0
#ifdef HAS_TM_TM_ZONE
    #define SYSTEM_MKTIME_MIN_TM_ZONE   "PST"
#endif
#ifdef HAS_TM_TM_GMTOFF
    #define SYSTEM_MKTIME_MIN_TM_GMTOFF -28800
#endif

/* Max/min for timegm() */
#ifdef HAS_TIMEGM
    #define SYSTEM_TIMEGM_MAX_TM_SEC    7
    #define SYSTEM_TIMEGM_MAX_TM_MIN    14
    #define SYSTEM_TIMEGM_MAX_TM_HOUR   3
    #define SYSTEM_TIMEGM_MAX_TM_MDAY   19
    #define SYSTEM_TIMEGM_MAX_TM_MON    0
    #define SYSTEM_TIMEGM_MAX_TM_YEAR   138
    #define SYSTEM_TIMEGM_MAX_TM_WDAY   2
    #define SYSTEM_TIMEGM_MAX_TM_YDAY   18
    #define SYSTEM_TIMEGM_MAX_TM_ISDST  0
    #ifdef HAS_TM_TM_ZONE
        #define SYSTEM_TIMEGM_MAX_TM_ZONE       "UTC"
    #endif
    #ifdef HAS_TM_TM_GMTOFF
        #define SYSTEM_TIMEGM_MAX_TM_GMTOFF     0
    #endif

    #define SYSTEM_TIMEGM_MIN_TM_SEC    52
    #define SYSTEM_TIMEGM_MIN_TM_MIN    45
    #define SYSTEM_TIMEGM_MIN_TM_HOUR   20
    #define SYSTEM_TIMEGM_MIN_TM_MDAY   13
    #define SYSTEM_TIMEGM_MIN_TM_MON    11
    #define SYSTEM_TIMEGM_MIN_TM_YEAR   1
    #define SYSTEM_TIMEGM_MIN_TM_WDAY   5
    #define SYSTEM_TIMEGM_MIN_TM_YDAY   346
    #define SYSTEM_TIMEGM_MIN_TM_ISDST  0
    #ifdef HAS_TM_TM_ZONE
        #define SYSTEM_TIMEGM_MIN_TM_ZONE       "UTC"
    #endif
    #ifdef HAS_TM_TM_GMTOFF
        #define SYSTEM_TIMEGM_MIN_TM_GMTOFF     0
    #endif
#endif

#endif /* TIME64_LIMITS_H */
