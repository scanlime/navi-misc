#!/usr/bin/env python
#
# Convert an efs-*.log file from the tracker program to
# three SVM training files, one for each output axis.
# Usage:
#   ./split-efs-log.py <log file> <x file> <y file> <z file>
#
# --Micah
#

import sys

logFile, xFile, yFile, zFile = sys.argv[1:]

xf = open(xFile, "w")
yf = open(yFile, "w")
zf = open(zFile, "w")

for line in open(logFile):
    values = map(float, line.split())
    trainingData = " ".join(["%d:%f" % (i, values[i]) for i in xrange(8)])
    x = values[8]
    y = values[9]
    z = values[10]
    xf.write("%f %s\n" % (x, trainingData))
    yf.write("%f %s\n" % (y, trainingData))
    zf.write("%f %s\n" % (z, trainingData))
