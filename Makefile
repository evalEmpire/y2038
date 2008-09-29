.PHONY : test

OPTIMIZE = -g
INCLUDE  = -I.
CC       = gcc
WARNINGS = -Wall -ansi -pedantic -Wno-long-long -Wextra -Wdeclaration-after-statement -Wendif-labels
CFLAGS   = $(WARNINGS) $(OPTIMIZE) $(INCLUDE)
COMPILE  = $(CC) $(CFLAGS)
LINK     = $(COMPILE)

all : time64.o

time64.o : time64.h time64.c Makefile

t/bench : t/bench.c time64.o
	$(LINK) time64.o t/bench.c -o $@

bench : t/bench
	time t/bench

t/localtime_test : t/localtime_test.c time64.o
	$(LINK) time64.o t/localtime_test.c -o $@

t/gmtime_test : t/gmtime_test.c time64.o
	$(LINK) time64.o t/gmtime_test.c -o $@

t/year_limit_test.t : t/tap.c t/year_limit_test.c time64.o
	$(LINK) time64.o t/year_limit_test.c -o $@

t/negative_test.t : t/tap.c t/negative_test.c time64.o
	$(LINK) time64.o t/negative_test.c -o $@

t/overflow.t : t/tap.c t/overflow.c time64.o
	$(LINK) time64.o t/overflow.c -o $@

t/timegm.t : t/tap.c t/timegm.c time64.o
	$(LINK) time64.o t/timegm.c -o $@

t/safe_year.t : t/tap.c t/safe_year.c time64.c
	$(LINK) t/safe_year.c -o $@

test : tap_tests localtime_tests

localtime_tests: t/localtime_test t/gmtime_test
	@which bzdiff > /dev/null || (echo "You need bzdiff to run these tests"; exit 1)
	@which less   > /dev/null || (echo "You need less to run these tests";   exit 1)
	TZ=Canada/Eastern t/gmtime_test | bzip -9 > t/gmtime_test.out.bz2
	bzdiff -u t/gmtime_test.out.bz2 t/gmtime.out.bz2 | less -F
	TZ=Canada/Eastern t/localtime_test | bzip -9 > t/eastern_test.out.bz2
	bzdiff -u t/eastern_test.out.bz2 t/eastern.out.bz2 | less -F
	TZ=Australia/West t/localtime_test | bzip -9 > t/oz_test.out.bz2
	bzdiff -u t/oz_test.out.bz2 t/oztime.out.bz2 | less -F

tap_tests: t/year_limit_test.t t/negative_test.t t/overflow.t t/timegm.t t/safe_year.t
	@perl -MTest::Harness -wle 'if( $$Test::Harness::VERSION < 3.00 ) {	\
		print "You need the Test::Harness 3 Perl module to run these tests";	\
	        exit 1 \
	}'
	@prove --exec '' t/*.t

clean:
	-rm 	t/year_limit_test.t 	\
		t/negative_test.t	\
		t/timegm.t		\
		t/overflow.t		\
		t/safe_year.t		\
	   	t/localtime_test	\
		t/gmtime_test		\
		t/*_test.out.bz2	\
		t/bench			\
		*.o

