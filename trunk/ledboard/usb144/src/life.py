
import random

def randPattern(proportion=0.33, len=16*9):
    numDots = int(len * proportion)
    p = ['f'] * numDots + ['0'] * (len - numDots)
    random.shuffle(p)
    return p

def emptyPattern(len=16*9):
    return ['0'] * len

def life(pattern, pitch=16):
    out = []
    l = len(pattern)
    for i, cell in enumerate(pattern):
        neighbours = (i-pitch-1, i-pitch, i-pitch+1,
                      i-1, i+1,
                      i+pitch-1, i+pitch, i+pitch+1)
        count = 0
        for n in neighbours:
            if n < 0:
                n += l
            if n >= l:
                n -= l
            if pattern[n] != '0':
                count += 1
        if cell != '0':            
            out.append("00ff00000"[count])
        else:
            out.append("000f00000"[count])
    return out

def draw(pattern):
    print ''.join(pattern)

def isEmpty(pattern):
    for p in pattern:
        if p != '0':
            return True
    return False

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

speed = 5
slowSpeed = 15

while True:
    # Beginning of the universe...

    a = emptyPattern()
    b = randPattern()
    history = {}
    fadeBetween(a, b, slowSpeed)
    a, b = b, life(b)

    #  Let it evolve until it reaches a state we've seen before

    while tuple(b) not in history:
        history[tuple(b)] = True
        fadeBetween(a, b, speed)
        a, b = b, life(b)

    # Show the end state, then fade out.

    if not isEmpty(b):

        for i in range(10):
            fadeBetween(a, b, speed)
            a, b = b, life(b)

        fadeBetween(a, b, slowSpeed)
        a, b = b, emptyPattern()
        fadeBetween(a, b, 15)
