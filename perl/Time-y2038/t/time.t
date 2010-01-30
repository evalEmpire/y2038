#!/usr/bin/perl -w

use strict;
use warnings;

use Test::More 'no_plan';
use Test::Warn;

BEGIN {
    use_ok 'Time::y2038';
}

# Try to set the time zone so we can reliably test localtime().
local $ENV{TZ} = 'US/Pacific';
my $Test_Localtime = localtime(0) eq 'Wed Dec 31 16:00:00 1969';


# Test that we match the core's results inside the safe range.
{
    is_deeply( [gmtime(0)],      [CORE::gmtime(0)],    'gmtime(0)' );
    is_deeply( [localtime(0)],   [CORE::localtime(0)], 'localtimetime(0)' );

    is_deeply( [gmtime(2**30)],    [CORE::gmtime(2**30)],    'gmtime(2**30)' );
    is_deeply( [localtime(2**30)], [CORE::localtime(2**30)], 'localtimetime(2**30)' );

    is gmtime(0),        CORE::gmtime(0),        'scalar gmtime(0)';
    is localtime(0),     CORE::localtime(0),     'scalar localtime(0)';
    is gmtime(2**30),    CORE::gmtime(2**30),    'scalar gmtime(2**30)';
    is localtime(2**30), CORE::localtime(2**30), 'scalar localtime(2**30)';
}


{
    is_deeply( [gmtime(2**52)],  [16, 48, 3, 6, 11, 142713460, 6, 340, 0], 'gmtime(2**52)' );
    is_deeply( [gmtime(-2**52)], [44, 11, 20, 25, 0, -142713321, 1, 24, 0], 'gmtime(-2**52)' );

    is( gmtime(2**52),  'Sat Dec  6 03:48:16 142715360' );
    is( gmtime(-2**52), 'Mon Jan 25 20:11:44 -142711421' );
}


SKIP: {
    skip "localtime() tests specific to US/Pacific time zone", 6 unless $Test_Localtime;

    is_deeply( [localtime(2**52)],  [16, 48, 19, 5, 11, 142713460, 5, 339, 0], 'localtime(2**52)' );
    is_deeply( [localtime(-2**52)], [44, 11, 12, 25, 0, -142713321, 1, 24, 0], 'localtime(-2**52)' );
    is_deeply( [localtime(1224479316)], [36, 8, 22, 19, 9, 108, 0, 292, 1], 'localtime() w/dst' );

    # This is inverted because hash keys get stringified and the
    # big numbers may lose accuracy.
    my %times = (
        'Fri Dec  5 19:48:16 142715360'         => 2**52,
        'Mon Jun 19 06:54:08 71358665'          => 2**51,
        'Tue Sep 25 11:57:04 35680317'          => 2**50,
        'Mon Oct 25 20:46:08 3058'              => 2**35,
        'Fri Mar  7 12:13:52 881'               => -2**35,
        'Thu Apr  7 22:02:56 -35676378'         => -2**50,
        'Sat Jul 14 03:05:52 -71354726'         => -2**51,
        'Mon Jan 25 12:11:44 -142711421'        => -2**52,
        'Sun Oct 19 22:08:36 2008'              => 1224479316,
    );
    for my $want (keys %times) {
        my $time = $times{$want};
        is localtime($time), $want, sprintf "localtime(%.0f)", $time;
    }
}


# Some sanity tests for the far, far future and far, far past
{
    my %time2year = (
        -2**62  => -146138510344,
        -2**52  => -142711421,
        -2**48  => -8917617,
        -2**46  => -2227927,
         2**46  => 2231866,
         2**48  => 8921556,
         2**52  => 142715360,
         2**62  => 146138514283
    );

    for my $time (sort keys %time2year) {
        my $want = $time2year{$time};

        my $have = (gmtime($time))[5] + 1900;
        is $have, $want, "year check, gmtime($time)";

        $have = (localtime($time))[5] + 1900;
        is $have, $want, "year check, localtime($time)";
    }
}


for my $name (qw(gmtime localtime)) {
    my $func = do {
        no strict 'refs';
        \&{$name};
    };

    # Test in void context
#line 18
    warning_like {
        1;
        $func->(0);
        1;
    } qr/^\QUseless use of $name() in void context at $0 line 20.\E$/,
      "void context warning";


    # Check the prototype
    is( prototype($name), prototype("CORE::$name"), "prototype($name)" );


    # Test with no args.
    # Ignore the minutes and seconds in case they get run at a minute/second boundry.
    is_deeply( [($func->())[2..8]],    [($func->(time))[2..8]], "$name()" );


    # Test too big or small a time.
    my $huge_time = sprintf "%.0f", 2**65;
#line 58
    warning_like {
        is $func->($huge_time), undef;
    } qr/^\Q$name($huge_time) can not be represented at $0 line 59\E/;

#line 63
    warning_like {
        is $func->(-$huge_time), undef;
    } qr/^\Q$name(-$huge_time) can not be represented at $0 line 64\E/;
}
