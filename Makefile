.PHONY : test

test : test.c gmtime.h
	gcc -g -Wall test.c -o test
	TZ=Canada/Eastern ./test | bzip -9 > test.out.bz2
	bzdiff -u test.out.bz2 localtime.out.bz2 | less
