
.DELETE_ON_ERROR:

.PHONY : test bench clean tap_tests localtime_tests

OPTIMIZE = -O3 -g
WARNINGS = -W -Wall -Wextra -ansi -pedantic -Wno-long-long -Wdeclaration-after-statement \
  -Wendif-labels -Wconversion -Wcast-qual -Wwrite-strings -Wmissing-prototypes \
  -Wc++-compat
#clang: -Wno-incompatible-pointer-types-discards-qualifiers
#gcc:   -Wno-discarded-qualifiers
INCLUDE  = -I.
CFLAGS   = $(WARNINGS) $(OPTIMIZE) $(INCLUDE)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  CFLAGS += -DHAS_TM_TM_GMTOFF -DHAS_TM_TM_ZONE
endif
ifeq ($(UNAME_S),Linux)
  # Under Linux/glibc you will need flag _BSD_SOURCE for names tm_gmtoff and tm_zone
  # (instead of __tm_gmtoff and __tm_zone) in struct tm,
  # and _POSIX_SOURCE (there are alternatives) for tzset().
  CFLAGS += -D_BSD_SOURCE -D_POSIX_SOURCE
endif
ifneq ($(DEBUG),)
  CFLAGS  += -DTIME_64_DEBUG
  OPTIMIZE = -g
endif

TIME64_OBJECTS = time64.o
CHECK_MAX_BIN  = bin/check_max

all : $(CHECK_MAX_BIN)

$(CHECK_MAX_BIN) : $(TIME64_OBJECTS)

time64.o time64.i: time64_config.h time64_limits.h time64.h Makefile

%.i: %.c
	$(CC) -c $(CFLAGS) -o $@ -E -c $<

bench : t/bench t/bench_system
	time t/bench_system
	time t/bench

t/bench t/bench_system : $(TIME64_OBJECTS)
t/localtime_test : $(TIME64_OBJECTS)
t/gmtime_test : $(TIME64_OBJECTS)

BLACKBOX_TESTS = 	t/year_limit.t	\
			t/negative.t 	\
			t/overflow.t 	\
			t/timegm.t 	\
			t/gmtime64.t	\
			t/mktime64.t	\
			t/asctime64.t	\
			t/ctime64.t

GLASSBOX_TESTS = 	t/safe_year.t	\
			t/seconds_between_years.t

$(BLACKBOX_TESTS) : t/tap.c $(TIME64_OBJECTS)
$(GLASSBOX_TESTS) : t/tap.c

test : tap_tests localtime_tests
test_debug: CFLAGS += $(DEBUG)
test_debug: test

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

tap_tests: $(BLACKBOX_TESTS) $(GLASSBOX_TESTS)
	@which prove > /dev/null || (echo 'You need prove (from the Test::Harness perl module) to run these tests'; exit 1)
	@prove --exec '' t/*.t

clean:
	-rm -f 	t/*.t 			\
	   	t/localtime_test	\
		t/gmtime_test		\
		t/*_test.out.bz2	\
		t/bench			\
		t/bench_system		\
		$(CHECK_MAX_BIN)	\
		*.o
	-rm -rf	t/*.dSYM/		\
		bin/*.dSYM/		\
		*.dSYM/

