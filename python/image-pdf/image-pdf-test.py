#!/usr/bin/env python
#
# Test program for an algorithm to efficiently sample
# from a 2D probability distribution function defined by
# an arbitrary image file.
#
# Copyright (C) 2004 Micah Dowty <micah@navi.cx>
#

import Image, os
from random import random
from Numeric import *
import psyco

def openGrayImage(f):
    img = Image.open(f).convert("L")
    a = fromstring(img.tostring(), UnsignedInt8)
    a.shape = (img.size[1], img.size[0])
    return a.astype(Float)

def showArray(a):
    a = a.astype(UnsignedInt8)
    img = Image.fromstring("L", (a.shape[1],
                                 a.shape[0]), a.tostring())
    img.save("output.png")
    os.system("eog output.png")

def binSearch(array, key):
    """Return the index of the largest value in 'array' less than or
       equal to 'key'. The contents of 'array' must be sorted.
       """
    a = 0
    b = len(array)-1
    while 1:
        if array[b] <= key:
            return b
        if array[a] > key:
            return None
        center_low = (a + b) / 2
        center_high = center_low + 1
        if array[center_low] <= key and array[center_high] > key:
            return center_low
        elif array[center_low] > key:
            b = center_low
        elif array[center_high] <= key:
            a = center_high
        else:
            assert 0

def buildTree(a, origin=0):

    # Stopping conditions
    if len(a) == 1:
        # Got a single pixel
        return origin
    if len(a) == 0:
        # Empty region, this should never be reached when walking the tree
        return None

    sums = cumsum(a)
    total = sums[-1]

    if total < 1:
        return None

    # Split at approximately 0.5 probability
    splitLoc = binSearch(sums, total / 2 + 1)
    if not splitLoc:
        splitLoc = 1

    left = a[:splitLoc]
    right = a[splitLoc:]

    return (buildTree(left, origin),
            buildTree(right, origin + splitLoc))


def walkTree(t):
    while 1:
        if type(t) is tuple:
            if random() < 0.5:
                t = t[0]
            else:
                t = t[1]
        else:
            return t

psyco.bind(binSearch)
psyco.bind(buildTree)
psyco.bind(walkTree)

print "Opening image..."
a = openGrayImage("test.png")
originalShape = a.shape
a = reshape(a, (-1,))

print "Building tree..."
t = buildTree(a)

print "Plotting points..."
out = zeros(a.shape, Int32)
for i in xrange(100000):
    v = walkTree(t)
    try:
        out[v] += 1
    except IndexError:
        pass

print "Generating image"
peak = maximum.reduce(out)
print "Peak density %d" % peak
out = reshape(out * (255.0 / peak), originalShape)
showArray(out)
