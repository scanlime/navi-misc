#!/usr/bin/env python
#
# build_bayes_net.py - builds a set of probability relationships between bones
#                      in order to determine the likelihood of a particular
#                      pose.
#
# Copyright (C) 2006 David Trowbridge
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

from Graph import MotionGraph
from Motion import ASFReader
from optparse import OptionParser
import Numeric, pickle

options = OptionParser ('%prog [output file] [ASF FILE] [AMC FILE]...')
options.add_option ('-d', dest='degrees', type=int, default=5,
		help='Set the discretization size of graph nodes')

opts, args = options.parse_args()
if len(args) < 3:
    options.error ('missing arguments')
else:
    print 'loading %s' % args[1]
    asf = ASFReader()
    asf.parse(args[1])

    nets = MotionGraph.build_bayes (asf, args[2:], opts.degrees)

    print 'writing pickle'
    file = open(args[0], 'w')
    pickle.dump(nets, file)
    print 'flushing'
    file.flush()
    file.close()
    print 'done'

# vim: ts=4:sw=4:et
