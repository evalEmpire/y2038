#include "time64.c"
#include "t/tap.h"

#define SECS_PER_DAY     (60LL * 60LL * 24LL)
#define SECS_PER_YEAR    (SECS_PER_DAY * 365LL)
#define SECS_PER_LEAP    (SECS_PER_DAY * 366LL)

int main(void) {
	
	
	
	/*tests inside the save year range*/
	/*both tested years are the same year*/
    is_Int64( seconds_between_years_with_same_daycount( (Year)2000, (Year)2000 ),
              0LL,
              "2000 - 2000"
    );
	/*both years are leap years since they are ​divisible by 4,100 and 400*/
    is_Int64( seconds_between_years_with_same_daycount( (Year)2004, (Year)2000 ),
              SECS_PER_LEAP + 3 * SECS_PER_YEAR,
              "2004 - 2000"
    );
	/*both years are non leap years because they are not ​divisible by 4*/
    is_Int64( seconds_between_years_with_same_daycount( (Year)2005, (Year)2001 ),
              SECS_PER_YEAR * 3 + SECS_PER_LEAP,
              "2005 - 2001"
    );



	/*tests with left_year in the future beyond the safe year range*/
	/*both years are leap years since they are divisible by 4,100 and 400*/
    is_Int64( seconds_between_years_with_same_daycount( (Year)2800, (Year)2000 ),
              SECS_PER_YEAR * 606 + SECS_PER_LEAP * 194,
              "2800 - 2000"
    );
	/*both years are non leap years because they are not divisible by 4*/
    is_Int64( seconds_between_years_with_same_daycount( (Year)2801, (Year)2001 ),
              SECS_PER_YEAR * 606 + SECS_PER_LEAP * 194,
              "2801 - 2001"
    );



	/*tests with left_year in the past before the safe year range*/
	/*both years are leap years since they are divisible by 4,100 and 400*/
    is_Int64( seconds_between_years_with_same_daycount( (Year)1200, (Year)2000 ),
              (-1) * (SECS_PER_YEAR * 606 + SECS_PER_LEAP * 194),
              "1200 - 2000"
    );
	/*both years are non leap years because they are not divisible 4*/
    is_Int64( seconds_between_years_with_same_daycount( (Year)1201, (Year)2001 ),
              (-1) * (SECS_PER_YEAR * 606 + SECS_PER_LEAP * 194),
			  "1201 - 2001"
    );
	
    done_testing();
    return 0;
}
