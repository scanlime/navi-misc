#!/usr/bin/env python
#
# create_dot_graphs.py - loads a pickled set of graphs and creates .dot
#                        files for each bone
#
# Copyright (C) 2005-2006 David Trowbridge
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

from Graph.Algorithms import DotPrint
import sys, pickle

def go (filename):
    graphs = pickle.load (open (filename))

    for key, graph in graphs.iteritems():
        if graph is not None:
            print 'drawing',key
            f = file ('graphs/%s.dot' % key, 'w')
            DotPrint (graph, f)
            f.close ()

if len (sys.argv) != 2:
    print 'Usage: %s [graph pickle]' % sys.argv[0]
else:
    go (sys.argv[1])
