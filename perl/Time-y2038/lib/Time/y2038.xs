#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <math.h>

#include "time64.h"

#define myPUSHi(int)   (PUSHs(sv_2mortal(newSViv(int))));
#define myPUSHn(num)   (PUSHs(sv_2mortal(newSVnv(num))));


int about_eq(double left, double right, double epsilon) {
    double diff;

    if( left == right ) {
        return 1;
    }

    diff = fabs(left - right);

    if( diff < epsilon ) {
        return 1;
    }
    else {
        return 0;
    }
}


MODULE = Time::y2038                PACKAGE = Time::y2038
PROTOTYPES: ENABLE


void
gmtime(...)
    PROTOTYPE: ;$
    INIT:
        Time64_T when;
        struct TM *err;
        struct TM date;
    PPCODE:
        if( GIMME_V == G_VOID ) {
            warn("Useless use of gmtime() in void context");
            XSRETURN_EMPTY;
        }

        if( items == 0 ) {
            time_t small_when;
            time(&small_when);
            when = (Time64_T)small_when;
        }
        else {
            double when_float = SvNV(ST(0));
            when = (Time64_T)when_float;

            /* Check for Time64_T overflow */
            if( !about_eq((double)when, when_float, 1024.0) ) {
                warn("gmtime(%.0f) can not be represented", when_float);
                XSRETURN_EMPTY;
            }
        }

        err = gmtime64_r(&when, &date);

        if( err == NULL )
        {
            warn("gmtime(%.0f) can not be represented", (double)when);
            XSRETURN_EMPTY;
        }

        EXTEND(SP, 9);
        myPUSHi(date.tm_sec);
	myPUSHi(date.tm_min);
	myPUSHi(date.tm_hour);
	myPUSHi(date.tm_mday);
	myPUSHi(date.tm_mon);
	myPUSHn((double)date.tm_year);
	myPUSHi(date.tm_wday);
	myPUSHi(date.tm_yday);
	myPUSHi(date.tm_isdst);


void
localtime(...)
    PROTOTYPE: ;$
    INIT:
        Time64_T when;
        struct TM *err;
        struct TM date;
    PPCODE:
        if( GIMME_V == G_VOID ) {
            warn("Useless use of localtime() in void context");
            XSRETURN_EMPTY;
        }

        if( items == 0 ) {
            time_t small_when;
            time(&small_when);
            when = (Time64_T)small_when;
        }
        else {
            double when_float = SvNV(ST(0));
            when = (Time64_T)when_float;

            /* Check for Time64_T overflow */
            if( !about_eq((double)when, when_float, 1024.0) ) {
                warn("localtime(%.0f) can not be represented", when_float);
                XSRETURN_EMPTY;
            }
        }

        err = localtime64_r(&when, &date);

        if( err == NULL )
        {
            warn("localtime(%.0f) can not be represented", (double)when);
            XSRETURN_EMPTY;
        }

        EXTEND(SP, 9);
        myPUSHi(date.tm_sec);
	myPUSHi(date.tm_min);
	myPUSHi(date.tm_hour);
	myPUSHi(date.tm_mday);
	myPUSHi(date.tm_mon);
	myPUSHn((double)date.tm_year);
	myPUSHi(date.tm_wday);
	myPUSHi(date.tm_yday);
	myPUSHi(date.tm_isdst);
