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

"""
Handy things for functional style programming.
"""

from __future__ import nested_scopes

import math as _math

def some(f, lst):
  """some(f, lst) -> true if F applied to some element of LST is true"""
  
  for x in lst:
    if f(x):
      return 1
  return 0

def every(f, lst):
  """every(f, lst) -> true if F applied to every element of LST is true"""
  
  for x in lst:
    if not f(x):
      return 0
  return 1

def find(p, lst, start=0):
  """find(p, lst, start)

  Returns the first index i (>= START) where P(LST[i]) is true or
  -1 if there is none.
  """
  
  for i in xrange(start, len(lst)):
    if p(lst[i]):
      return i

  return -1

def _remove_unhashable_duplicates(lst, key):
  result = []
  if key == None:
    for v in lst:
      if v not in result:
        result.append(v)
  else:
    seen_keys = []
    for v in lst:
      this_key = key(v)
      if this_key not in seen_keys:
        result.append(v)
        seen_keys.append(this_key)
  return result
  
def remove_duplicates(lst, key=None):
  """
  Returns a list equivalent to SEQ with repeated elements removed.
  (The first occurence of each value is retained.  Note that this is the
   opposite of the Common Lisp default behavior.)

  If KEY is provided, then repeats are detected by comparing KEY(ELEMENT)
  for each element.
  """
  
  d = {}
  result = []
  if key == None:
    for v in lst:
      try:
        if not d.has_key(v):
          result.append(v)
          d[v] = 1
      except TypeError:
        return _remove_unhashable_duplicates(lst, key)
  else:
    for v in lst:
      thiskey = key(v)
      try:
        if not d.has_key(thiskey):
          result.append(v)
          d[thiskey] = 1
      except TypeError:
        return _remove_unhashable_duplicates(lst, key)
  return result

def transpose(seq_of_seqs):
  """
  Returns the matrix transpose of SEQ_OF_SEQS as a list of tuples.

  SEQ_OF_SEQS must be rectangular for this to make sense.
  """
  
  return zip(*seq_of_seqs)

def intersection(a, b):
  """
  intersection(a, b) -> list of items in both A and B

  The order of the result items is unspecified.
  
  If all items are hashable, then this algorithm is
  O(size(a) + size(b)); otherwise, it is O(size(a) * size(b))
  """
  
  try:
    # Try using a dictionary.
    d = {}
    for x in b:
      d[x] = 1
    c = [x for x in a if d.has_key(x)]
  except TypeError:                     # really want HashError
    c = [x for x in a if x in b]
  return c

def partition_list(f, lst):
  """Given function F and list F, return tuple (matched, nonmatched),
  where matched is a list of all elements E for which F(E) is true, and
  nonmatched the remainder.
  """
  
  matched = []
  nonmatched = []
  for e in lst:
    if f(e):
      matched.append(e)
    else:
      nonmatched.append(e)
  return matched, nonmatched

def reverse(lst):
  """reverse(lst) -> reversed copy of LST"""
  
  lst = lst[:]
  lst.reverse()
  return lst
  
def sort(p, lst):
  """sort(p, lst) -> sorted copy of LST"""
  
  lst = lst[:]
  lst.sort(p)
  return lst

def maximum(cmp, lst):
  """maximum(cmp, lst)

  Returns the maximal element in non-empty list LST with elements
  compared via CMP() which should return values with the same semantics
  as Python's cmp().  If there are several maximal elements, the last
  one is returned.
  """
  
  if not lst:
    raise ValueError, 'empty list'

  maxval = lst[0]

  for i in xrange(1, len(lst)):
    v = lst[i]
    if cmp(maxval, v) <= 0:
      maxval = v

  return maxval

def minimum(cmp, lst):
  """minimum(cmp, lst)

  Returns the minimal element in non-empty list LST with elements
  compared via CMP() which should return values with the same semantics
  as Python's cmp().  If there are several minimal elements, the last
  one is returned.
  """

  if not lst:
    raise ValueError, 'empty list'

  minval = lst[0]

  for i in xrange(1, len(lst)):
    v = lst[i]
    if cmp(minval, v) > 0:
      minval = v

  return minval

def sum(lst):
  """sum(lst) -> sum of numbers in LST"""

  sum = 0
  for v in lst:
    sum += v
  return sum
  
def first_difference(lst):
  """first_difference(lst) -> the first differences of the values in LST"""
  
  d = []
  last = None
  for v in lst:
    if last != None:
      d.append(v - last)
    last = v
  return d

def mean(lst):
  """mean(lst) -> the arithmetic mean of the values in LST"""

  return sum(lst) / float(len(lst))

def variance(lst):
  """variance(lst) -> variance of values in LST"""
  
  mu = mean(lst)
  sum = 0.0
  for v in lst:
    sum += (v - mu) ** 2
  return sum / float(len(lst))

def stddev(lst):
  """stddev(lst) -> standard deviation of values in LST"""
  
  return _math.sqrt(variance(lst))

def lebesgue_norm(p, lst):
  """l_norm(p, lst) -> Lebesgue norm with parameter P for number list LST"""
  
  return ((sum(map(lambda x: float(abs(x)) ** p,
                   lst))
           / float(len(lst)))
          ** (1.0 / p))

def list2dict(lst):
  """list2dict(lst) -> dict mapping from LST's indices to its elements

  In Python 2.2, use the dict() constructor.
  """
  
  d = {}
  for i in xrange(len(lst)):
    d[i] = lst[i]
  return d

def mapdict(f, d):
  """mapdict(f, d)

  Return a new dict just like D, but with each value V replaced with F(V).
  """
  
  d1 = {}
  for k, v in d.items():
    d1[k] = f(v)
  return d1

def cyclic_pairs(lst):
  """cyclic_pairs(lst)

  Returns the cyclic pairs of LST as a list of 2-tuples.
  """

  n = len(lst)
  assert(n >= 2)
  cps = []
  for i in xrange(n - 1):
    cps.append((lst[i], lst[i + 1]))
  cps.append((lst[n - 1], lst[0]))
  return cps

def number_of_leading(p, lst):
  """number_of_leading(p, lst)

  Returns the number of leading elements X of LST for which P(X) is true.
  """
  
  i = 0
  for v in lst:
    if not p(v):
      break
    i += 1
  return i

def number_of_trailing(p, lst):
  """number_of_trailing(p, lst)

  Returns the number of trailing elements X of LST for which P(X) is true.
  """

  n = len(lst)
  for i in xrange(n - 1, -1, -1):
    if not p(lst[i]):
      return (n - 1) - i
  return len(lst)

def flatten1(seq):
  """
  Return a list with the contents of SEQ with sub-lists and tuples "exploded".
  This is only done one-level deep.
  """

  lst = []
  for x in seq:
    if type(x) is list or type(x) is tuple:
      for val in x:
        lst.append(val)
    else:
      lst.append(x)
  return lst

def flatten(seq):
  """
  Returns a list of the contents of seq with sublists and tuples "exploded".
  The resulting list does not contain any sequences, and all inner sequences
  are exploded.  For example:

  >>> flatten([7,(6,[5,4],3),2,1])
  [7,6,5,4,3,2,1]
  """
  lst = []
  for el in seq:
    if type(el) == list or type(el) is tuple:
      lst.extend(flatten(el))
    else:
      lst.append(el)
  return lst
