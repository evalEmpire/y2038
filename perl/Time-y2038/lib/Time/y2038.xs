#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "time64.h"

#define myPUSHi(int)   (PUSHs(sv_2mortal(newSViv(int))));
#define myPUSHn(num)   (PUSHs(sv_2mortal(newSVnv(num))));


MODULE = Time::y2038                PACKAGE = Time::y2038
PROTOTYPES: ENABLE

void
localtime(time)
    const Time64_T time
    PROTOTYPE: ;$
    INIT:
        struct TM *err;
        struct TM date;
    PPCODE:
        err = localtime64_r(&time, &date);

        if( err == NULL ) {
            warn("localtime() could not represent the time");
            XSRETURN_EMPTY;
        }

        EXTEND(SP, 9);
        myPUSHi(date.tm_sec);
	myPUSHi(date.tm_min);
	myPUSHi(date.tm_hour);
	myPUSHi(date.tm_mday);
	myPUSHi(date.tm_mon);
	myPUSHn(date.tm_year);
	myPUSHi(date.tm_wday);
	myPUSHi(date.tm_yday);
	myPUSHi(date.tm_isdst);


void
gmtime(time)
    const Time64_T time
    PROTOTYPE: ;$
    INIT:
        struct TM *err;
        struct TM date;
    PPCODE:
        err = gmtime64_r(&time, &date);

        if( err == NULL ) {
            warn("gmtime() could not represent the time");
            XSRETURN_EMPTY;
        }

        EXTEND(SP, 9);
        myPUSHi(date.tm_sec);
	myPUSHi(date.tm_min);
	myPUSHi(date.tm_hour);
	myPUSHi(date.tm_mday);
	myPUSHi(date.tm_mon);
	myPUSHn(date.tm_year);
	myPUSHi(date.tm_wday);
	myPUSHi(date.tm_yday);
	myPUSHi(date.tm_isdst);
