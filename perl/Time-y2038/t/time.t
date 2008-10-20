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

    is( localtime(2**52),      'Fri Dec  5 19:48:16 142715360' );
    is( localtime(-2**52),     'Mon Jan 25 12:11:44 -142711421' );
    is( localtime(1224479316), 'Sun Oct 19 22:08:36 2008' );
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
