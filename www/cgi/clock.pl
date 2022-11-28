#!/usr/local/perl

use POSIX qw(strftime);

my $date = strftime "%c", localtime;
print ("$date\n");
