package Time::y2038;

use strict;
use warnings;

use base qw(Exporter);
use XSLoader;

our $VERSION = 20081018;
our @EXPORT = qw(localtime gmtime);

XSLoader::load __PACKAGE__, $VERSION;

1;
