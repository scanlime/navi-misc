#!/usr/bin/env python

import sys, os, time, math

def dejong(b, a, xy):
    x = math.sin(a * xy[1]) - math.cos(4 * xy[0])
    y = math.sin(-4 * xy[0]) - math.cos(-4 * xy[1])
    return (x, y)

def unique(values):
    intv = []
    floatv = []
    for value in values:
        intv.append(int(value[0] * 1000000))
    uniquev = {}
    for value in intv:
        uniquev[value] = 0
    values2 = uniquev.keys()
    for value in values2:
        floatv.append(float(value) / 1000000.0)
    return floatv

Al = -4
Au = 4
As = 0.005
b = 0.3
x0 = 0
y0 = 0
m = 5000
l = 4500

graph = open('de_jong_bifurc.ex', 'w')

A = Al

while(A < Au):
    iterates = []
    iterates.append((x0, y0))
    for i in range(m):
        iterates.append(dejong(b, A, iterates[i]))
    values = iterates[l:]
    uniquev = unique(values)
    for value in uniquev:
        graph.write('%f %f\n' % (A, value))
    graph.write('\n')
    A += As

graph.close()

gnuplot = os.popen2('gnuplot -persist')[0]

gnuplot.write('unset key\n')
gnuplot.write('set xlabel "x"\n')
gnuplot.write('set ylabel "dejong(x)"\n')
gnuplot.write('plot "de_jong_bifurc.ex" with dots')

gnuplot.close()
