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

def buildTree(a, origin=(0,0), vertical=False):
    # Stopping conditions
    if a.shape[0] == 1 and a.shape[1] == 1:
        return origin
    if a.shape[0] < 1 or a.shape[0] < 1:
        return None

    if vertical:
        s = a.shape[0] / 2
        subimages = a[:s,:], a[s:,:]
        origin2 = (origin[0], origin[1]+s)
    else:
        s = a.shape[1] / 2
        subimages = a[:,:s], a[:,s:]
        origin2 = (origin[0]+s, origin[1])

    sums = [sum(sum(i)) for i in subimages]
    total = sums[0] + sums[1]
    if not total:
        # All image areas under this split have a probability of zero
        return None
    prob = sums[0] / float(total)

    return (prob,
            buildTree(subimages[0], origin, not vertical),
            buildTree(subimages[1], origin2, not vertical))

def walkTree(t):
    while 1:
        if len(t) == 2:
            return t
        else:
            if random() < t[0]:
                t = t[1]
            else:
                t = t[2]

print "Opening image..."
a = openGrayImage("test.png")
print "Building tree..."
t = buildTree(a)

print "Plotting points..."
out = zeros(a.shape, Int32)
for i in xrange(100000):
    v = walkTree(t)
    try:
        out[v[1], v[0]] += 1
    except IndexError:
        pass

print "Generating image"
peak = maximum.reduce( maximum.reduce(out) )
print "Peak density %d" % peak
out = out * (255.0 / peak)

showArray(out)
