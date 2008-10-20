#!/usr/bin/perl -w

use strict;
use warnings;

use Test::More 'no_plan';

BEGIN {
    use_ok 'Time::y2038';
}

is_deeply( [gmtime(0)], [CORE::gmtime(0)], 'gmtime(0)' );
is_deeply( [gmtime(2**52)], [16, 48, 3, 6, 11, 142713460, 6, 340, 0], 'gmtime(2**52)' );
is_deeply( [gmtime(-2**52)], [44, 11, 20, 25, 0, -142713321, 1, 24, 0], 'gmtime(-2**52)' );
