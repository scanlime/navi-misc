#!/usr/bin/env python
import _fidtool
import os, time, random

fd = os.open("foo", os.O_CREAT | os.O_TRUNC | os.O_RDWR, 0666)

r = range(10)
_fidtool.append_samples(fd, r)

while 1:
    r2 = _fidtool.query_samples(fd, r)
    if r2 == r:
        print "Success"
    else:
        print "Failure: %r -> %r" % (r, r2)
        break
    random.shuffle(r)

#dataset = map(int, open("cia-commits.dataset"))
#_fidtool.append_samples(fd, dataset)

#start = 1124400000
#hour_length = 60 * 60
#times = [start + hour_length * hour for hour in xrange(24*7*4)]

##indices = _fidtool.query_samples(fd, times)
#indices = [_fidtool.query_samples(fd, (t,))[0] for t in times]

#for i in xrange(len(times)-1):
#    print times[i], indices[i+1] - indices[i]

os.close(fd)
