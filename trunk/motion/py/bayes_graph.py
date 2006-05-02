#!/usr/bin/env python

import sys
import pickle

if len(sys.argv) != 3:
    print 'Usage: %s: [bayes pickle] [bone]' % sys.argv[0]
    sys.exit(1)


bayes = pickle.load(open(sys.argv[1]))

xs = []
ys = []

data = bayes[sys.argv[2]]
for x, y in data.keys():
    if x not in xs:
        xs.append(x)
    if y not in ys:
        ys.append(y)

xn = {}
yn = {}

for i in range(len(xs)):
    xn[xs[i]] = i
for i in range(len(ys)):
    yn[ys[i]] = i

for key, value in data.iteritems():
    x, y = key
    print xn[x], yn[y], value
