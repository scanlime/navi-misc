#!/usr/bin/env python
#
# Little python proggie to extract a variable from a Makefile.am file
# usage: extract-var.py file varname
#

import sys, re

filename, varname = sys.argv[1:]

# Remove escaped newlines while reading in the whole file
text = open(filename).read().replace("\\\n","")

# Clean up whitespace
text = re.sub('[ \t]+', ' ', text)

for line in text.split("\n"):
    line = line.strip()
    if not line:
        continue
    if line.find("=") < 0:
        continue
    key, value = line.split("=", 1)

    if key.strip() == varname:
        print value

