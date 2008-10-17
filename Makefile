.PHONY : test

OPTIMIZE = -g
INCLUDE  = -I.
CC       = gcc
WARNINGS = -Wall -ansi -pedantic -Wno-long-long -Wextra -Wdeclaration-after-statement -Wendif-labels -Wconversion
CFLAGS   = $(WARNINGS) $(OPTIMIZE) $(INCLUDE)
COMPILE  = $(CC) $(CFLAGS)
LINK     = $(COMPILE)
OBJECTS  = time64.o strtime64.o

all : $(OBJECTS) Makefile

time64.h : time64_config.h

$(OBJECTS) : time64.h Makefile

time64.o : time64.c

strtime64.o : strtime64.c

t/bench : t/bench.c $(OBJECTS)
	$(LINK) $(OBJECTS) t/bench.c -o $@

bench : t/bench
	time t/bench

t/localtime_test : t/localtime_test.c $(OBJECTS)
	$(LINK) $(OBJECTS) t/localtime_test.c -o $@

t/gmtime_test : t/gmtime_test.c $(OBJECTS)
	$(LINK) $(OBJECTS) t/gmtime_test.c -o $@

t/year_limit.t : t/tap.c t/year_limit.t.c $(OBJECTS)
	$(LINK) $(OBJECTS) t/year_limit.t.c -o $@

t/negative.t : t/tap.c t/negative.t.c $(OBJECTS)
	$(LINK) $(OBJECTS) t/negative.t.c -o $@

t/overflow.t : t/tap.c t/overflow.t.c $(OBJECTS)
	$(LINK) $(OBJECTS) t/overflow.t.c -o $@

t/timegm.t : t/tap.c t/timegm.t.c $(OBJECTS)
	$(LINK) $(OBJECTS) t/timegm.t.c -o $@

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

t/strftime.t : t/tap.c t/strftime.t.c $(OBJECTS)
	$(LINK) $(OBJECTS) t/strftime.t.c -o $@

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

tap_tests: t/year_limit.t t/negative.t t/overflow.t t/timegm.t t/safe_year.t t/gmtime64.t t/asctime64.t t/ctime64.t t/strftime.t
	@which prove > /dev/null || (echo 'You need prove (from the Test::Harness perl module) to run these tests'; exit 1)
	@prove --exec '' t/*.t

clean:
	-rm 	t/*.t 			\
	   	t/localtime_test	\
		t/gmtime_test		\
		t/*_test.out.bz2	\
		t/bench			\
		$(OBJECTS)
