package Local::Module::Build;

use strict;
use base qw(Module::Build);

sub note_time_limits {
    my $self = shift;

    my $source = "check_max.c";
    my $exe    = "check_max";
    unless( $self->up_to_date($source => $exe) ) {
        warn "Building a program to test the range of your system time functions...\n";
        my $cb = $self->cbuilder;
        my $obj = $cb->compile(source => "check_max.c", include_dirs => ['y2038']);
        $exe = $cb->link_executable(objects => $obj, exe_file => $exe);
    }

    $self->add_to_cleanup($exe);

    return if $self->up_to_date(["munge_config", $exe] => "y2038/time64_config.h");
    warn "  and running it...\n";

    my @maxes = `./$exe`;

    warn "  Done.\n";

    my %limits;
    for my $line (@maxes) {
        next if $line =~ /^#/;
        my($key, $val) = split /\s+/, $line;
        next unless $key and $val;
        $limits{$key} = $val;
    }

    # Windows lies about being able to handle just a little bit of
    # negative time.
    for my $key (qw(gmtime_min localtime_min)) {
        if( -10_000 < $limits{$key} && $limits{$key} < 0 ) {
            $limits{$key} = 0;
        }
    }

    for my $key (sort { $a cmp $b } keys %limits) {
        my $val = $limits{$key};
        warn sprintf "%15s:  %d\n", $key, $val;
        $self->notes($key, $limits{$key});
    }

    return;
}


sub ACTION_code {
    my $self = shift;

    $self->note_time_limits;

    return $self->SUPER::ACTION_code(@_);
}

1;

