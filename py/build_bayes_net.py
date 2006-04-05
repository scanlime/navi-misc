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
import Numeric
import sys, pickle

def fixnegative (x):
    while x < 0:
        x = x + 360
    return x

def fix360 (x):
    if x == 360:
        return 0
    return x

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
                if child in amcs[0].bones:
                    nets[child] = {}

    # this *ought* to match the interval we use in the grap building, but I'm
    # not sure it really matters
    interval = 5

    print 'building bayes nets'
    for parent, children in relationships.iteritems():
        for child in children:
            print 'building net for %s' % child
            if child not in amcs[0].bones:
                # If the child has no DOF, we don't need to build a net at
                # all, since we won't be interpolating that bone
                continue
            if parent not in amcs[0].bones:
                # If the parent has no DOF, the bayes net for this
                # relationship simplifies to a simple histogram of the
                # bone positions
                net = nets[child]
                for amc in amcs:
                    cbone = amc.bones[child]

                    # Chomp to within interval
                    c = tuple (int (n / interval) * interval for n in tuple (map (fixnegative, c)))
                    c = tuple (map (fix360, c))
                    if ((), c) in net:
                        net[((), c)] = net[((), c)] + 1
                    else:
                        net[((), c)] = 1
            else:
                net = nets[child]

                for amc in amcs:
                    pbone = Numeric.remainder (amc.bones[parent], 360.0)
                    cbone = Numeric.remainder (amc.bones[child], 360.0)

                    for frame in range(len(pbone)):
                        p = pbone[frame,:]
                        c = cbone[frame,:]

                        # Chomp to within interval
                        p = tuple (int (n / interval) * interval for n in tuple (map (fixnegative, p)))
                        c = tuple (int (n / interval) * interval for n in tuple (map (fixnegative, c)))
                        p = tuple (map (fix360, p))
                        c = tuple (map (fix360, c))

                        if (p, c) in net:
                            net[(p, c)] = net[(p, c)] + 1
                        else:
                            net[(p, c)] = 1

    newnets = {}
    for bone, net in nets.iteritems():
        newnet = {}
        parents = {}
        for pose, count in net.iteritems():
            ppose = pose[0]
            cpose = pose[1]
            if ppose not in parents:
                parents[ppose] = {}
            parents[ppose][cpose] = count
        newnets[bone] = parents

    del nets

    nets = {}
    for bone, parents in newnets.iteritems():
        for parent, children in parents.iteritems():
            total = 0
            for count in children.itervalues():
                total += count
            for child, count in children.iteritems():
                nets[(parent, child)] = float(count) / float(total)

    return nets

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
