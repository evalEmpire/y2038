/* Test the internal _safe_year() function which does the critical mapping
   to make the localtime trick work
*/

#include "tap.c"
#include "localtime64.c"


void _year_to_TM(const Int64 year, struct TM *date) {
    Time64_T time;

    date->tm_year = year - 1900;
    date->tm_mon  = 0;
    date->tm_mday = 1;
    date->tm_hour = 0;
    date->tm_min  = 0;
    date->tm_sec  = 0;

    time = timegm64(date);
    gmtime64_r(&time, date);
}


void _year_to_tm(const Int64 year, struct tm *ret) {
    Time64_T time;
    struct TM date;

    date.tm_year = year - 1900;
    date.tm_mon  = 0;
    date.tm_mday = 1;
    date.tm_hour = 0;
    date.tm_min  = 0;
    date.tm_sec  = 0;

    time = timegm64(&date);
    gmtime64_r(&time, &date);
    copy_TM_to_tm(&date, ret);
}


void test_safe_year(Int64 orig_year) {
    int safe_year = _safe_year( orig_year );
    struct tm safe_tm;
    struct TM orig_tm;

    _year_to_tm((Int64)safe_year, &safe_tm);
    _year_to_TM(orig_year, &orig_tm);
    is_int( orig_tm.tm_year, orig_year - 1900, "_year_to_nyd(orig)" );
    is_int( safe_tm.tm_year, safe_year - 1900, "_year_to_nyd(safe)" );

    ok(1, "orig_year: %lld, safe_year: %d", orig_year, safe_year);
    is_int( safe_tm.tm_wday, orig_tm.tm_wday,                           "  tm_wday" );
    is_int( IS_LEAP( safe_year - 1900 ), IS_LEAP( orig_year - 1900 ),   "  ISLEAP()" );

    safe_year--;
    orig_year--;
    _year_to_tm((Int64)safe_year, &safe_tm);
    _year_to_TM(orig_year, &orig_tm);
    is_int( safe_tm.tm_wday, orig_tm.tm_wday,                           "  previous tm_wday" );
    is_int( IS_LEAP( safe_year - 1900 ), IS_LEAP( orig_year - 1900 ),   "  previous ISLEAP()" );

    safe_year += 2;
    orig_year += 2;
    _year_to_tm((Int64)safe_year, &safe_tm);
    _year_to_TM(orig_year, &orig_tm);
    is_int( safe_tm.tm_wday, orig_tm.tm_wday,                           "  next tm_wday" );
    /* Don't care about the next leap status */
}

int main(void) {
    int year;

    /* Boundry tests */
    test_safe_year(1);
    test_safe_year(0);
    test_safe_year(-1);
    

    for( year = 1599;  year <= 2401;  year++ ) {
        test_safe_year(year);
    }

    for( year = -401;  year <= 401;  year++ ) {
        test_safe_year(year);
    }

    done_testing();

    return(0);
}
