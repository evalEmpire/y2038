#!/usr/bin/perl -w

use Config;
unless( defined $Config{quadtype} and length $Config{quadtype} ) {
    warn "Time::y2038 needs \$Config{quadtype} set to work.\n";
    die "OS Unsupported\n";
}



use ExtUtils::CBuilder;

my $cb = ExtUtils::CBuilder->new;
unless( $cb->have_compiler ) {
    warn "Time::y2038 requires a C compiler.\n";
    die "OS Unsupported.\n";
}

sub test_time_limits {
    my $build = shift;

    warn "Building a program to test the range of your system time functions...\n";

    my $obj = $cb->compile(source => "check_max.c", include_dirs => ['y2038']);
    my $exe = $cb->link_executable(objects => $obj);

    warn "  and running it...\n";

    my @maxes = `./$exe`;

    warn "  Done.\n";

    my %limits;
    for my $line (@maxes) {
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

    return %limits;
}



use Module::Build;

my $build = Module::Build->new(
    module_name         => 'Time::y2038',
    license             => 'perl',

    include_dirs        => ['y2038'],
    c_source            => 'y2038',

#    extra_compiler_flags => "-g -Wall -ansi -pedantic -Wno-long-long -Wextra -Wdeclaration-after-statement -Wendif-labels -Wconversion",

    PL_files => {
        'munge_config'  => 'y2038/time64_config.h'
    },

    configure_requires  => {
        'Module::Build'         => '0.2808',
    },

    build_requires      => {
        'Module::Build'         => '0.2808',
        'Test::Warn'            => '0.11',
        'Test::Exception'       => '0.27',
        'Test::More'            => 0.62,
        'ExtUtils::CBuilder'    => 0,
    },

    requires            => {
        perl                    => '5.6.1'
    },
);

my %limits = test_time_limits;
for my $key (keys %limits) {
    my $val = $limits{$key};
    warn sprintf "%15s:  %d\n", $key, $val;
    $build->notes($key, $limits{$key});
}

$build->create_build_script;
