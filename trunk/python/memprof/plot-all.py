#!/usr/bin/env python

import Gnuplot
import os, time

os.chdir("types")
types = [t for t in os.listdir(".") if not t.startswith(".")]

g = Gnuplot.Gnuplot()
for t in types:
    g('set terminal png')
    g('set output "%s.png"' % t)
    g('plot "%s"' % t)
