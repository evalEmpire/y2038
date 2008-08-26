.PHONY : test

all : localtime_test

localtime_test : test.c gmtime64.h
	gcc -g -Wall test.c -o localtime_test

test : localtime_test
	TZ=Canada/Eastern ./localtime_test | bzip -9 > test.out.bz2
	bzdiff -u test.out.bz2 localtime.out.bz2 | less
