#!/usr/bin/env perl

$limit = $ARGV[1];

while (<STDIN>) {
    print;
    while (1) {
        open STATUS, "/proc/io_status";
        <STATUS> =~ /io_tokens=([0-9]+)/;
        if ($1 < $limit) {
            sleep(0.5);
        }
        else {
            break;
        }
    }
}
