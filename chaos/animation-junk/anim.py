#!/usr/bin/env python
import os
from Numeric import arange

frame = 0
for b in arange(-10, 10, 0.01):
    print b
    os.system('./de-jong-explorer -i dancing-ghost.png -a %f -t 1000 -o frame%05d.png' % (b, frame))
    frame += 1

