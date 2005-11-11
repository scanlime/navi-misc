#!/usr/bin/env python
import _fidtool
import os, time, random
import psyco
psyco.full()

def build_fid(dataset, fd):
    dataset.sort()
    print "Dataset:"
    for i in dataset:
        print "\t%s" % i
    _fidtool.append_samples(fd, dataset)

def reference_query(stamps, seek_to):
    i = 0
    while i < len(stamps) and stamps[i] < seek_to:
        i += 1
    return i

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
                    print "%d: %d != %d" % (query[i], j, k)
                else:
                    print "%d: %d" % (query[i], j)
            break

        random.shuffle(query)


if __name__ == "__main__":
    dataset = map(int, open("cia-commits.dataset"))[2000:2020]
    dataset = [x * 5 for x in range(200)]

    fd = os.open("foo", os.O_CREAT | os.O_TRUNC | os.O_RDWR, 0666)
    build_fid(dataset, fd)
    test_fid(dataset, fd)
    os.close(fd)
