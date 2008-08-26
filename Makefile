.PHONY : test

all : localtime_test

localtime_test : test.c localtime64.h
	gcc -g -Wall test.c -o localtime_test

year_limit_test : year_limit_test.c localtime64.h
	gcc -g -Wall year_limit_test.c -o year_limit_test

test : localtime_test year_limit_test
	TZ=Canada/Eastern ./localtime_test | bzip -9 > eastern_test.out.bz2
	bzdiff -u eastern_test.out.bz2 localtime.out.bz2 | less -F
	TZ=Australia/West ./localtime_test | bzip -9 > oz_test.out.bz2
	bzdiff -u oz_test.out.bz2 oztime.out.bz2 | less -F
	./year_limit_test
