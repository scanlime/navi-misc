#!/usr/bin/env python
import BZFlag.Flag

flags = BZFlag.Flag.getDict().values()
lines = [str(flag()) for flag in flags]

# Print lines, sorted and with duplicates removed
# (all team flags show the same description)
lines.sort()
previousLine = None
for line in lines:
    if line != previousLine:
        print line
    previousLine = line


