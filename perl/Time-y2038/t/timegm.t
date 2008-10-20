#!/usr/bin/perl -w

use strict;
use warnings;

use Test::More 'no_plan';
use Test::Exception;

BEGIN {
    use_ok 'Time::y2038';
}

for my $time (-2**52, -1, 0, 1, 2**52) {
    is timegm(gmtime($time)),       $time, sprintf "timegm(%.0f)", $time;
    is timelocal(localtime($time)), $time, sprintf "timelocal(%.0f)", $time;
}

#line 16
throws_ok {
    timegm();
} qr[^Usage: timegm\(\$sec, \$min, \$hour, \$mday, \$month, \$year\) at \Q$0\E line 17\.$];

#line 24
throws_ok {
    timelocal();
} qr[^Usage: timelocal\(\$sec, \$min, \$hour, \$mday, \$month, \$year\) at \Q$0\E line 25\.$];
