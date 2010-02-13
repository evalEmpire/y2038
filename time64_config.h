/* Configuration
   -------------
   Define as appropriate for your system.
   Sensible defaults provided.
*/


#ifndef TIME64_CONFIG_H
#    define TIME64_CONFIG_H

/* Debugging
   TIME_64_DEBUG
   Define if you want debugging messages
*/
/* #define TIME_64_DEBUG */


/* INT_64_T
   A 64 bit integer type to use to store time and others.
   Must be defined.
*/
#define INT_64_T                long long


/* USE_TM64
   Should we use a 64 bit safe replacement for tm?  This will
   let you go past year 2 billion but the struct will be incompatible
   with tm.  Conversion functions will be provided.
*/
/* #define USE_TM64 */

#ifdef USE_TM64
    #define TM64_ASCTIME_FORMAT "%.3s %.3s%3d %.2d:%.2d:%.2d %lld\n"
#else
    #define TM64_ASCTIME_FORMAT "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n"
#endif


/* Availability of system functions.

   HAS_GMTIME_R
   Define if your system has gmtime_r()

   HAS_LOCALTIME_R
   Define if your system has localtime_r()

   HAS_TIMEGM
   Define if your system has timegm(), a GNU extension.
*/
#define HAS_GMTIME_R
#define HAS_LOCALTIME_R
/* #define HAS_TIMEGM */


/* Details of non-standard tm struct elements.

   HAS_TM_TM_GMTOFF
   True if your tm struct has a "tm_gmtoff" element.
   A BSD extension.

   HAS_TM_TM_ZONE
   True if your tm struct has a "tm_zone" element.
   A BSD extension.
*/
/* #define HAS_TM_TM_GMTOFF */
/* #define HAS_TM_TM_ZONE */


/* USE_SYSTEM_LOCALTIME
   USE_SYSTEM_GMTIME
   USE_SYSTEM_MKTIME
   USE_SYSTEM_TIMEGM
   Should we use the system functions if the time is inside their range?
   Your system localtime() is probably more accurate, but our gmtime() is
   fast and safe.
*/
#define USE_SYSTEM_LOCALTIME
/* #define USE_SYSTEM_GMTIME */
#define USE_SYSTEM_MKTIME
/* #define USE_SYSTEM_TIMEGM */


/* 
   Maximum and minimum inputs your system's respective time functions
   can correctly handle.  time64.h will use your system functions if
   the input falls inside these ranges and corresponding USE_SYSTEM_*
   constant is defined.
*/
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

#endif /* TIME64_CONFIG_H */
