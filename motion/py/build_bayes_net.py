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

from Motion import AMC, ASFReader
import sys, pickle

def load(asf, files):
    amcs = []
    nets = {}

    for filename in files:
        print 'loading %s' % filename
        amcs.append(AMC.from_file(filename))

    relationships = {}
    for rel in asf.hierarchy:
        parent = rel[0]
        children = rel[1:]
        if len(parent):
            relationships[parent] = children
            for child in children:
                nets[child] = {}
    print relationships

    #for amc in amcs:
    for parent, children in relationships.iteritems():
        for child in children:
            print '%s: %d' % (child, len(asf.bones[child].dof or []))

if len(sys.argv) < 4:
    print 'Usage: %s [output file] [ASF FILE] [AMC FILE]...' % sys.argv[0]
else:
    print 'loading %s' % sys.argv[2]
    asf = ASFReader()
    asf.parse(sys.argv[2])

    nets = load(asf, sys.argv[3:])

    print 'writing pickle'
    file = open(sys.argv[1], 'w')
    pickle.dump(nets, file)
    print 'flushing'
    file.flush()
    file.close()
    print 'done'
