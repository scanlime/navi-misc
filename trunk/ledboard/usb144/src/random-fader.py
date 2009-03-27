
import random

def randPattern(proportion=0.33, len=16*9):
    numDots = int(len * proportion)
    p = ['f'] * numDots + ['0'] * (len - numDots)
    random.shuffle(p)
    return p

def emptyPattern(len=16*9):
    return ['0'] * len

def draw(pattern):
    print ''.join(pattern)

def fadeBetween(a, b, steps=16):
    if steps <= 1:
        draw(b)
        return
    for i in xrange(1, steps):
        alpha = i / float(steps - 1)
        c = []
        for ac, bc in zip(a, b):
            c.append("%x" % (int(int(ac, 16) * (1 - alpha) +
                                 int(bc, 16) * alpha)))
        draw(c)


while True:
    p = randPattern()
    e = emptyPattern()
    fadeBetween(e, p)
    fadeBetween(p, p)
    fadeBetween(p, e)
