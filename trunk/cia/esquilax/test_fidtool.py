#!/usr/bin/env python
import _fidtool
import os, time, random, datetime

#import psyco
#psyco.full()

def build_fid(dataset, fd):
    dataset.sort()
    _fidtool.append_samples(fd, dataset)

def reference_query(stamps, seek_to):
    i = 0
    while i < (len(stamps)-1) and stamps[i] < seek_to:
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

def dayGrid(t):
    """Generator for day-aligned grids, starting no later than the given time"""
    date = datetime.datetime.fromtimestamp(t).date()

    while 1:
        yield int(time.mktime(date.timetuple())), _fidtool.GRID_SOLID
        date += datetime.timedelta(days=1)

def graph_test(fd, fromTime, toTime):
    size = (512, 128)
    colors = (
        (1.0, 1.0, 1.0, 1.0),
        (0.0, 0.0, 0.0, 0.2),
        (0.0, 0.0, 1.0, 0.6),
        )
    x_scale = (fromTime, float(toTime - fromTime) / 20)
    y_scale = 5000
    x_grid = dayGrid(x_scale[0])
    y_grid = ( (10*i, _fidtool.GRID_SOLID) for i in xrange(1, y_scale // 10 +1) )

    scales = (
        (x_scale, x_grid),
        (y_scale, y_grid),
        )

    f = open("foo.png", "wb")
    _fidtool.graph_png(fd, f, size, colors, scales)
    f.close()
        

if __name__ == "__main__":
    dataset = map(int, open("timestamps.txt"))
    #dataset = [x * 5 for x in range(6500)]
    
    fd = os.open("foo", os.O_CREAT | os.O_RDWR, 0666)

    #print "Building FID"
    #build_fid(dataset, fd)

    #print "Testing FID"
    #test_fid(dataset, fd)
 
    print "Graphing"
    graph_test(fd, dataset[0], dataset[-1])
    os.close(fd)
