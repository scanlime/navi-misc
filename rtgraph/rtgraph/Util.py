from __future__ import generators, division
import time, math

def limitFloatPrecision(v, d):
    """Return a string representation of v with at most d digits of precision.
       Operates recursively on tuples/lists, and uses repr() for other data types.
       """
    if type(v) == float:
        return ("%%.0%df" % d) % v
    elif type(v) == tuple:
        return "(%s)" % ", ".join([limitFloatPrecision(i, d) for i in v])
    elif type(v) == list:
        return "[%s]" % ", ".join([limitFloatPrecision(i, d) for i in v])
    else:
        return repr(v)

class TimeVariable:
    """A callable class that returns the number of seconds since it was created.
       This is helpful for use with FunctionChannel's variables feature.
       """
    def __init__(self):
        self.tInitial = time.time()

    def __call__(self):
        return time.time() - self.tInitial

def orthogonal(v):
    """Returns a 2-vector orthogonal to the given 2-vector"""
    return (v[1], -v[0])

def normalize(v):
    """Normalize the given 2-vector"""
    l = math.sqrt(v[0]**2 + v[1]**2)
    return (v[0]/l, v[1]/l)
