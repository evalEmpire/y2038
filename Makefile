.PHONY : test

OPTIMIZE = -g
CC       = gcc
CCFLAGS  = -Wall
INCLUDE  = -I.
COMPILE  = $(CC) $(OPTIMIZE) $(INCLUDE) $(CCFLAGS)
LINK     = $(COMPILE)

all : t/localtime_test

localtime64.o : localtime64.h localtime64.c

t/localtime_test : t/localtime_test.c localtime64.o
	$(LINK) localtime64.o t/localtime_test.c -o $@

t/year_limit_test.t : tap.c t/year_limit_test.c localtime64.o
	$(LINK) localtime64.o t/year_limit_test.c -o $@

t/negative_test.t : tap.c t/negative_test.c localtime64.o
	$(LINK) localtime64.o t/negative_test.c -o $@

t/overflow.t : tap.c t/overflow.c localtime64.o
	$(LINK) localtime64.o t/overflow.c -o $@

t/my_timegm.t : tap.c t/my_timegm.c localtime64.o
	$(LINK) localtime64.o t/my_timegm.c -o $@

test : localtime_test tap_tests

localtime_test: t/localtime_test
	TZ=Canada/Eastern t/localtime_test | bzip -9 > t/eastern_test.out.bz2
	bzdiff -u t/eastern_test.out.bz2 t/localtime.out.bz2 | less -F
	TZ=Australia/West t/localtime_test | bzip -9 > t/oz_test.out.bz2
	bzdiff -u t/oz_test.out.bz2 t/oztime.out.bz2 | less -F

tap_tests: t/year_limit_test.t t/negative_test.t t/overflow.t t/my_timegm.t
	@prove --exec '' t/*.t

clean:
	-rm 	t/year_limit_test.t 	\
		t/negative_test.t	\
	   	t/localtime_test	\
		t/eastern_test.out.bz2	\
		t/oz_test.out.bz2	\
		localtime64.o

