package Time::y2038;

use strict;
use warnings;

use base qw(Exporter);
use XSLoader;

our $VERSION = 20081020;
our @EXPORT = qw(localtime gmtime timegm timelocal);

XSLoader::load __PACKAGE__, $VERSION;

1;
__END__

=head1 NAME

Time::y2038 - Versions of Perl's time functions which work beyond 2038

=head1 SYNOPSIS

    use Time::y2038;

    print scalar gmtime 2**52;  # Sat Dec  6 03:48:16 142715360

=head1 DESCRIPTION

On many computers, Perl's time functions will not work past the year
2038.  This is a design fault in the underlying C libraries Perl uses.
Time::y2038 provides replacements for those functions which will work
accurately +/1 142 million years.

This only imports the functions into your namespace.  To replace it
everywhere, see L<Time::y2038::Everywhere>.

Replaces the following functions:

=head3 gmtime()

See L<perlfunc/gmtime> for details.

=head3 localtime()

See L<perlfunc/localtime> for details.

=head3 timegm()

    my $time = timegm($sec, $min, $hour, $month_day, $month, $year);

The inverse of C<gmtime()>, takes a date and returns the coorsponding
$time (number of seconds since Midnight, January 1st, 1970 GMT).  All
values are the same as C<gmtime()> so $month is 0..11 (January is 0)
and the $year is years since 1900 (2008 is 108).

    # June 4, 1906 03:02:01 GMT
    my $time = timegm(1, 2, 3, 4, 5, 6);

timegm() can take two additional arguments which are always ignored.
This lets you feed the results from gmtime() back into timegm()
without having to strip the arguments off.

The following is always true:

    timegm(gmtime($time)) == $time;

=head3 timelocal()

    my $time = timelocal($sec, $min, $hour, $mday, $month, $year);

Like C<timegm()>, but interprets the date in the current time zone.


=head1 LIMITATIONS

The safe range of times is +/ 2**52 (about 142 million years).

Although the underlying time library can handle times from -2**63 to
2**63-1 (about +/- 292 billion years) Perl uses floating point numbers
internally and so accuracy degrates after 2**52.


=head1 BUGS & FEEDBACK

See F<http://rt.cpan.org> to report and view bugs.

If you like the module, please drop the author an email.


=head1 AUTHOR

Michael G Schwern <schwern@pobox.com>


=head1 LICENSE & COPYRIGHT

Copyright 2008 Michael G Schwern

This program is free software; you can redistribute it and/or
modify it under the same terms as Perl itself.

See F<http://www.perl.com/perl/misc/Artistic.html>


=head1 SEE ALSO

L<Time::y2038::Everywhere> overrides localtime() and gmtime() across the whole program.

The y2038 project at F<http://y2038.googlecode.com/>

=cut
