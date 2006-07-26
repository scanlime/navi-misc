#!/usr/bin/env python

from Graph.Data import AdjacencyList
from optparse import OptionParser
import pickle

options = OptionParser (usage='%prog [options] <pickle> <bone>')
options.add_option ('-o', dest='file',
		help='Save the Bayes table to a file')
opts, args = options.parse_args ()

if len(args) < 2:
    options.error ('Missing arguments')

graph = pickle.load(open(args[0]))
bayes = graph.representations[AdjacencyList].bayes

net = bayes[args[1]]

output = (args[1] + ' Bayes Table').center (50) + '\n\n'
output += '  %s %s %s\n' % ('Parent'.center (18), 'Child'.center (18),
        'Prob.'.center (10))
output += '  ================== ================== ==========\n'

for spot, prob in net.iteritems ():
    output += '  %-18s %-18s %10f\n' % (repr (spot[0]), repr (spot[1]), prob)

print output

if opts.file:
    f = file (opts.file, 'w')
    f.write (output)
    f.close ()

# vim: ts=4:sw=4:et
