.PHONY : test

OPTIMIZE = -g
INCLUDE  = -I.
CC       = gcc
WARNINGS = -Wall -ansi -pedantic -Wno-long-long -Wextra -Wdeclaration-after-statement -Wendif-labels -Wconversion
CFLAGS   = $(WARNINGS) $(OPTIMIZE) $(INCLUDE)
COMPILE  = $(CC) $(CFLAGS)
LINK     = $(COMPILE)

all : time64.o

time64.o : time64_config.h time64.h time64.c Makefile

t/bench : t/bench.c time64.o
	$(LINK) time64.o t/bench.c -o $@

bench : t/bench
	time t/bench

t/localtime_test : t/localtime_test.c time64.o
	$(LINK) time64.o t/localtime_test.c -o $@

t/gmtime_test : t/gmtime_test.c time64.o
	$(LINK) time64.o t/gmtime_test.c -o $@

t/year_limit.t : t/tap.c t/year_limit.t.c time64.o
	$(LINK) time64.o t/year_limit.t.c -o $@

t/negative.t : t/tap.c t/negative.t.c time64.o
	$(LINK) time64.o t/negative.t.c -o $@

t/overflow.t : t/tap.c t/overflow.t.c time64.o
	$(LINK) time64.o t/overflow.t.c -o $@

t/timegm.t : t/tap.c t/timegm.t.c time64.o
	$(LINK) time64.o t/timegm.t.c -o $@

t/safe_year.t : t/tap.c t/safe_year.t.c time64.c
	$(LINK) t/safe_year.t.c -o $@

t/gmtime64.t : t/tap.c t/gmtime64.t.c time64.o
	$(LINK) time64.o t/gmtime64.t.c -o $@

t/mktime64.t : t/tap.c t/mktime64.t.c time64.o
	$(LINK) time64.o t/mktime64.t.c -o $@

t/asctime64.t : t/tap.c t/asctime64.t.c time64.o
	$(LINK) time64.o t/asctime64.t.c -o $@

t/ctime64.t : t/tap.c t/ctime64.t.c time64.o
	$(LINK) time64.o t/ctime64.t.c -o $@

t/seconds_between_years.t : t/tap.c t/seconds_between_years.t.c time64.c
	$(LINK) t/seconds_between_years.t.c -o $@

test : tap_tests localtime_tests

localtime_tests: t/localtime_test t/gmtime_test
	@which bzdiff > /dev/null || (echo 'You need bzdiff to run these tests'; exit 1)
	@which less   > /dev/null || (echo 'You need less to run these tests';   exit 1)
	@echo "On failure, these tests will produce a diff between the failed and expected results.  If they pass they'll be quiet."
	TZ=Canada/Eastern t/gmtime_test | bzip2 -9 > t/gmtime_test.out.bz2
	bzdiff -u t/gmtime_test.out.bz2 t/gmtime.out.bz2 | less -F
	TZ=Canada/Eastern t/localtime_test | bzip2 -9 > t/eastern_test.out.bz2
	bzdiff -u t/eastern_test.out.bz2 t/eastern.out.bz2 | less -F
	TZ=Australia/West t/localtime_test | bzip2 -9 > t/oz_test.out.bz2
	bzdiff -u t/oz_test.out.bz2 t/oztime.out.bz2 | less -F

tap_tests: t/year_limit.t t/negative.t t/overflow.t t/timegm.t t/safe_year.t t/gmtime64.t
	@which prove > /dev/null || (echo 'You need prove (from the Test::Harness perl module) to run these tests'; exit 1)
	@prove --exec '' t/*.t

clean:
	-rm 	t/year_limit.t 		\
		t/negative.t		\
		t/timegm.t		\
		t/overflow.t		\
		t/safe_year.t		\
	   	t/localtime_test	\
		t/gmtime_test		\
		t/*_test.out.bz2	\
		t/bench			\
		*.o

