#!/usr/bin/perl -w
########################################################
# this script takes two arguments
# ./compine.pl <last_frame> <file>
# last_frame: the last frame of the first file compiled
# file: the file you are adding
# sends output to stdout
########################################################


if($#ARGV != 1)
{
    die "usage: combine.pl <last_frame> <file>\n";
}

my $lastframe = $ARGV[0];
my $file = $ARGV[1];

# create file handle
open(FILE,$file);

# burn the first junk in the file
for(my $i = 0; $i < 3; $i++) {
    my $junk = <FILE>;
}

while(<FILE>) {
    chomp $_;
    $lastframe++;
    print "$lastframe\n";
    for(my $i = 0; $i < 29; $i++) {
        my $line = <FILE>;
        chomp $line;
        print "$line\n";
    }
}


