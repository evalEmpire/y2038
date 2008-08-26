.PHONY : test

OPTIMIZE = -g
CC       = gcc
CCFLAGS  = -Wall
INCLUDE  = -I.
COMPILE  = $(CC) $(OPTIMIZE) $(INCLUDE) $(CCFLAGS)

all : t/localtime_test

t/localtime_test : t/localtime_test.c localtime64.h
	$(COMPILE) t/localtime_test.c -o t/localtime_test

t/year_limit_test : t/year_limit_test.c localtime64.h
	$(COMPILE) t/year_limit_test.c -o t/year_limit_test

test : localtime_test year_limit_test

localtime_test: t/localtime_test
	TZ=Canada/Eastern t/localtime_test | bzip -9 > t/eastern_test.out.bz2
	bzdiff -u t/eastern_test.out.bz2 t/localtime.out.bz2 | less -F
	TZ=Australia/West t/localtime_test | bzip -9 > t/oz_test.out.bz2
	bzdiff -u t/oz_test.out.bz2 t/oztime.out.bz2 | less -F

year_limit_test: t/year_limit_test
	@prove --exec '' t/year_limit_test

clean:
	-rm 	t/year_limit_test 	\
	   	t/localtime_test	\
		t/eastern_test.out.bz2	\
		t/oz_test.out.bz2
