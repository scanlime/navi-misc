#!/usr/bin/env python

from esquilax import _bsax
import random

max_open_files = 1000
block_size = 16384
max_blocks = 5000

def get_filename_by_id(id):
    return "testfile-%d" % id

_bsax.filecache_init(max_open_files, get_filename_by_id)
_bsax.blockcache_init(block_size, max_blocks)

files = range(100)
blocks = range(0, 1024*1024, block_size)

for iter in xrange(1000000):
    i = random.choice(files)
    o = random.choice(blocks)

    s1 = _bsax.blockcache_lookup(i, o)
            
#    f = open("testfile-%d" % i)
#    f.seek(o)
#    s2 = f.read(block_size)
#    assert s1 == s2
