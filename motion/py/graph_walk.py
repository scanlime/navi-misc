#!/usr/bin/env python

from optparse import OptionParser
from Dance import Sequence, MotionGraph

def clicheWalk (graph, len):
    """Find a path in graph of length len by following the edges with the
       highest probabilities.
       """
    pass


def randomWalk (graph, len):
    """Find a path in graph of langth len by following a random edge."""
    pass


parser = OptionParser ("usage: %prog <graph pickle> <output amc>")
parser.add_option ("--cliche", dest="cliche", default=false,
        action="store_true", help="Choose edges based on probability")
parser.add_option ("-l", "--len", dest="len", default=1000,
        help="Set length of paths")

opts, args = parser.parse_args ()

if len (args) != 2: parser.error ("graph and output file required")

graphs = pickle.load (open (args[1]))

# vim:ts=4:sw=4:et
