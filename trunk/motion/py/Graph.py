#
# Graph.py - it's graphy stuff, yo.
#            some of this stuff is inspired by Micah Dowty's code
#            for spacegrant's "PyMCK" system, so be loving him.
#
# Copyright (C) 2005 David Trowbridge
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

import Observable

class Edge (object):
    """This object represents a graph edge.  It is immutable and
       usable as a dictionary key.
       """
    __slots__ = ['u', 'v']

    def __init__ (self, u, v):
        self.u = u
        self.v = v

    def __repr__ (self):
        return '<%s from %r to %r>' % (self.__class__.__name__, self.u, self.v)

    def __hash__ (self):
        return hash ((self.u, self.v))

    def __cmp__ (self, other):
        return cmp ((self.u, self.v), (other.u, other.v))

    def copy (self):
        return self.__class__ (self.u, self.v)

class Graph (object):
    """A generic graph. This object does not specify how the graph is stored
       or what kind of queries can be made against it- that is done by instantiating
       GraphRepresentation objects for this Graph. This lets one set of data be
       represented in multiple ways, depending on what kinds of questions we expect
       to ask.
       """
    edgeClass = Edge

    def __init__ (self):
        Observable.attachEvents (self, 'add', 'remove')

    def addList (self, edges):
        """Add multiple edges at once, from any iterable source.
           Returns the list of edges actually added.
           """
        if type (edges) is list:
            # An optimization for the case in which we already have a list
            for edge in edges:
                self.add (edge)
            return edges

        else:
            # If we have some other data type, like a generator, we need
            # to create a new list as we add the edges.
            l = []
            for edge in edges:
                self.add (edge)
                l.append (edge)
            return l

    def removeList (self, edges):
        """Remove multiple edges at once"""
        for edge in edges:
            self.remove (edge)

    def addTree (self, tree):
        """Add multiple edges at once, arranged as an adjacency list.
           'tree' is a dictionary mapping 'u' vertices to lists of 'v'
           vertices. Returns a list of edges actually added.
           """
        l = []
        for u, vList in tree.iteritems ():
            for v in vList:
                l.append (self.edgeClass (u, v))
        return self.addList (l)

    def cacheAlgorithm (self, attrib, factory, *args, **kwargs):
        """Update an attribute 'attrib' on this object that will contain
           the results of running 'factory' with the supplied arguments.
           Any time this graph is mutated, the cached result will be
           invalidated.
           """
        if not getattr (self, attrib, None):
            # Cache the algorithm result
            setattr (self, attrib, factory (*args, **kwargs))

            # Create an invalidator, and save a reference to it so it isn't GC'ed
            def cacheInvalidator (_, self=self, attrib=attrib):
                setattr (self, attrib, None)
            setattr (self, "_%s_invalidator" % attrib, cacheInvalidator)

            # Set up observers to invalidate it
            for mutator in (self.add, self.remove):
                mutator.observe (cacheInvalidator)

class GraphRepresentation (object):
    """This is an abstract base class for data structures used to represent
       a graph. A Graph object by itself has no representation at all-
       GraphRepresentation instances are attached to a graph in order to
       actually provide data structures that useful queries can be made
       against.

       Note that all GraphRepresentations you plan to use should be added
       to a Graph before populating it! GraphRepresentations will not be
       able to store edges added before they were attached.
       """
    def __init__ (self, graph):
        self.graph = graph;
        graph.add.observe (self.onAdd)
        graph.remove.observe (self.onRemove)

    def onAdd (self, edge):
        """Observer that is notified when an edge is added to the graph"""
        pass

    def onRemove (self, edge):
        """Observer that is notified when an edge is removed from the graph"""
        pass
