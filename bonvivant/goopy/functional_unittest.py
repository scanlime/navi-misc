#!/usr/bin/env python

# Copyright (c) 2005, Google Inc.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import sys
import unittest

from functional import *
  

class functionalUnitTest(unittest.TestCase):
  def testSomeEvery(self):
    assert some(lambda x: x == 5, range(10))
    assert not every(lambda x: x == 5, range(10))
    assert not some(lambda x: x == 5, [6] * 10)
    assert every(lambda x: x == 6, [6] * 10)

    n = 0
    for a, b in cyclic_pairs(range(5)):
      assert a == ((b - 1) % 5)
      n += 1
    assert n == 5

    a = range(10)
    b = range(5, 15)
    c = range(20, 30)
    assert not intersection(a, c)
    def same_set(a, b):
      return dict(zip(a, a)) == dict(zip(b, b))
    assert same_set(intersection(a, b), range(5, 10))

  def testParitionList(self):
    matched, unmatched = partition_list(lambda x: x % 2, range(5))
    self.assertEquals(matched, [1, 3])
    self.assertEquals(unmatched, [0, 2, 4])
    
  def testRemoveDuplicates(self):
    self.assertEquals(remove_duplicates(range(0, 10)
                                        + range(5, 15)
                                        + range(2, 12)),
                      range(0, 15))
    
  def testTranspose(self):
    self.assertEquals(transpose([range(i, i + 20)
                                 for i in range(10)]),
                      [tuple(range(j, j + 10))
                       for j in range(20)])

  def testFlatten(self):
    self.assertEquals(flatten1(zip(range(0, 10, 2), range(1, 11, 2))),
                      range(0, 10))
    self.assertEquals(flatten1(dict([(x, x) for x in range(3)]).items()),
                      [0, 0, 1, 1, 2, 2])

    self.assertEquals(flatten([7,(6,[5,4],3),2,1]), [7,6,5,4,3,2,1])
    self.assertEquals(flatten((4,5,3,2,1)), [4,5,3,2,1])
    self.assertEquals(flatten(zip(zip(range(0,10,2)), range(1,11,2))),
                      [0,1,2,3,4,5,6,7,8,9])
    
if __name__ == '__main__':
  unittest.main()
