#!/usr/bin/perl -w

use Module::Build;

my $build = Module::Build->new(
    module_name         => 'Time::y2038',
    license             => 'perl',

    include_dirs        => ['y2038'],
    c_source            => 'y2038',

    configure_requires  => {
        'Module::Build'         => '0.2808',
    },

    build_requires      => {
        'Test::More'            => 0.82,
        'ExtUtils::CBuilder'    => 0,
    },

    requires            => {
        perl                    => '5.6.1'
    },
);
$build->create_build_script;
