#!/usr/bin/env perl
#
# Extract HTTP logs from CIA's twistd.log in Combined Log Format
#

while (<>) {
    print "$2\n" if /^\d{4}\/\d\d\/\d\d \d\d:\d\d [A-Z]+ \[(\-|HTTPChannel[^ \]]+)\] (\d+\.\d+\.\d+\.\d+ .*)/;
}

