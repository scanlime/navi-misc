#!/usr/bin/env python

import random
import sys
from types import *

def Select(groups):
    if groups.getCount() == 0:
        sys.stderr.write("warning: no files to choose from\n")
        return []
    s = random.randint(0,groups.getCount()-1)
    return groups.getGroup(s).getWhere()

if __name__ == "__main__":
    import sys

    p = Pieces()
    p.addPaths (sys.argv[1:])

    s = Select(p)

    s = Select(p)
    if type(s) is StringType:
        s = [s]
    for x in s:
        print x
