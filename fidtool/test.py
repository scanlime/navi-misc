#!/usr/bin/env python
import _fidtool
import os, time, random

#import psyco
#psyco.full()

def build_fid(dataset, fd):
    dataset.sort()
    _fidtool.append_samples(fd, dataset)

def reference_query(stamps, seek_to):
    i = 0
    while i < len(stamps) and stamps[i] < seek_to:
        i += 1
    return (stamps[i], i)

def test_fid(dataset, fd):
    query = [int(random.uniform(dataset[0] - 100, dataset[-1] + 200) + 0.5)
             for i in xrange(1000)]
    query.sort()

    while 1:
        r1 = _fidtool.query_samples(fd, query)
        r2 = [reference_query(dataset, q) for q in query]

        if r1 == r2:
            print "Success"
        elif len(r1) != len(r2):
            print "Failure, len(r1)=%d, len(r2)=%d" % (len(r1), len(r2))
            break
        else:
            for i, (j, k) in enumerate(zip(r1, r2)):
                if j != k:
                    print "%s: %s != %s" % (query[i], j, k)
                else:
                    print "%s: %s" % (query[i], j)
            break

        break

def dump_fid(fd):
    sample = 0
    i = -1

    while 1:
        next_sample, next_i = _fidtool.query_samples(fd, [sample])[0]
        if next_i <= i:
            break

        for j in xrange(i, next_i):
            print next_sample

        sample = next_sample + 1
        i = next_i

class DebuggyFile:
    def __init__(self, f):
        self.f = f

    def write(self, data):
        print "write: %r" % data
        self.f.write(data)

    def close(self):
        self.f.close()

def graph_test(fd):
    f = DebuggyFile(open("foo.png", "wb"))
    _fidtool.graph_png(f, (256, 12800))
    f.close()

if __name__ == "__main__":
    dataset = map(int, open("cia-commits.dataset"))
    #dataset = [x * 5 for x in range(6500)]
    
    fd = os.open("foo", os.O_CREAT | os.O_TRUNC | os.O_RDWR, 0666)
    build_fid(dataset, fd)
    #test_fid(dataset, fd)
    graph_test(fd)
    os.close(fd)
