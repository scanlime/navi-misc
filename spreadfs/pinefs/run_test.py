#!/usr/bin/env python

"""For use with idl/test.x"""

import xdrlib

import test

def test_one_ll (ll):
    l = []
    top = 500
    for i in range (0,top):
        tmp = test.ll_test1 (a=i, b=i + 1)
        l.append (tmp)

    p = xdrlib.Packer ()
    test.ll_test1.pack (p, l)
    val = p.get_buffer ()
    up = xdrlib.Unpacker (val)
    b = test.ll_test1.unpack (up)
    assert len (l) == len (b)
    for x,y in zip (l, b):
        assert x.a == y.a
        assert x.b == y.b

test_one_ll (test.ll_test1)
test_one_ll (test.ll_test2)
test_one_ll (test.ll_test3)


