#!/usr/bin/env python
import sys, re, time

limit = int(sys.argv[1])

def getTokens():
    return int(re.search("io_tokens=([0-9]+)",
        open("/proc/io_status").read()).group(1))

while 1:
    line = sys.stdin.readline()
    if not line:
        break
    while getTokens() < limit:
        time.sleep(0.1)
    sys.stdout.write(line)
