#!/usr/bin/env perl
#
# Extract HTTP logs from CIA's twistd.log in Combined Log Format
#

while (<>) {
    /^\d{4}\/\d\d\/\d\d \d\d:\d\d ... \[(\-|HTTPChannel[^ \]]+)\] (\d+\.\d+\.\d+\.\d+ .*)/;
    print "$2\n";
}

