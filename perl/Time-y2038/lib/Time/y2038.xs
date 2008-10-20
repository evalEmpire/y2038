#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "time64.h"

#define myPUSHi(int)   (PUSHs(sv_2mortal(newSViv(int))));
#define myPUSHn(num)   (PUSHs(sv_2mortal(newSVnv(num))));


MODULE = Time::y2038                PACKAGE = Time::y2038
PROTOTYPES: ENABLE

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
            when = (Time64_T)SvNV(ST(0));
        }

        err = localtime64_r(&when, &date);

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
            when = (Time64_T)SvNV(ST(0));
        }

        err = gmtime64_r(&when, &date);

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
