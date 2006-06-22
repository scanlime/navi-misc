"""Tools for printing graphs in a dot(1) format.

This module provides base classes for nodes and edges that need to be printed
to files readable by dot(1). The `dotString` function in each class returns a
string representation of the node or edge formatted for dot(1). Additionally,
the `__str__` function in each class is defined to use this `dotString`
function as well. The `DotPrint` class is an `Algorithm` that prints an entire
graph formatted for dot(1).
"""

# Copyright (C) 2006 W. Evan Sheehan
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

from Algorithms import Algorithm
import string

class Printable:
    """Base class for all printable graph objects.

    Provides basic initialization for attributes, defines the interface, and
    sets the `__str__` function to return the value of `dotString`.
    """

    __slots__ = ["dotAttrs"]

    def __init__ (self, **kwargs):
        """Initialize the ``dotAttrs`` member variable to the list of keyword
        arguments.

        Arguments:
            - ``kwargs``    An arbitrary list of name=value pairs of attributes
              for an element in dot. See the dot man page for information on
              available attributes.
        """
        self.dotAttrs = kwargs

    def dotString (self):
        """Return a string representing this graph element in the dot format.
        
        All printable classes need to override this function.
        """
        raise Exception ("dotString not overridden in %s" % (self.__class__.__name__))

    def __str__ (self):
        """By default, the str() representation of these classes is their
        dot-formatted string.
        """
        return self.dotString ()


class Node (Printable):
    """Printable node base class."""

    def dotString (self):
        """Return a string representing a node in the dot file format."""
        id = str (hash (self))
        attrs = ['%s="%s"' % (key, value) for key, value in \
                self.dotAttrs.iteritems ()]

        return "%s [%s];" % (id, string.join (attrs, ','))


class Edge (Printable):
    """Printable edge base class."""

    __slots__ = ["u", "v", "dotAttrs"]

    def __init__ (self, u, v, **kwargs):
        """Initialize the edge with 2 nodes and a list of attributes.

        Arguments:
            - ``u``         The node from which the edge originates
            - ``v``         The node to which the edge goes
            - ``kwargs``    The attribute list
        """
        Printable.__init__ (self, kwargs)
        self.u = u
        self.v = v

    def dotString (self):
        """Return a string representation of the edge formatted for dot."""
        id = "%s -> %s" % (hash (self.u), hash (self.v))
        attrs = ['%s="%s"' % (key, value) for key, value in \
                self.dotAttrs.iteritems ()]
        return "%s [%s];" % (id, string.join (attrs, ','))


class DotPrint (Algorithm):
    """Simple graph walker which prints the graph as a dot(1) file.

       By default, vertexes are visited in dictionary key order, but
       that doesn't matter, since dot doesn't care what order it sees
       things in.

       >>> from Data import Graph, VertexMap, Edge
       >>> nodes = [1,2,3,4,5]
       >>> graph = Graph ()
       >>> map = VertexMap (graph)
       >>> edges = [
       ...     Edge (nodes[0], nodes[2], ''),
       ...     Edge (nodes[1], nodes[4], ''),
       ...     Edge (nodes[2], nodes[3], ''),
       ...     Edge (nodes[3], nodes[0], 'hi'),
       ...     ]
       >>> x = graph.addList (edges)
       >>> x = DotPrint (graph)
       >>> print x.run ()
       Digraph {
       1 [label="1"];
       2 [label="2"];
       3 [label="3"];
       4 [label="4"];
       5 [label="5"];
       1 -> 3 [label=""];
       2 -> 5 [label=""];
       3 -> 4 [label=""];
       4 -> 1 [label="hi"];
       }
       <BLANKLINE>
       """

    desired_representation = Data.VertexMap

    def __init__ (self, graph, file=None):
        Algorithm.__init__ (self, graph)
        self.file = file
        self.run ()

    def invalidate (self):
        Algorithm.invalidate (self)
        self.results = ''
        try:
            self.vertexMap = self.graph.representations[Data.VertexMap]
        except KeyError:
            raise Exception ('Graph does not contain VertexMap representation')

    def run (self):
        if self.valid:
            return self.results

        self.results = 'Digraph {\n'

        self_loops = set([])

        # print vertices
        for vertex in self.vertexMap:
            self.results += '%s\n' % vertex.dotString ()

        # print edges
        for vertex in self.vertexMap:
            edges = self.vertexMap.query (vertex)
            for edge in edges:
                if edge.u is vertex:
                    self.results += '%s\n' % edge.dotString ()

        self.results += '}'

        if self.file is not None:
            self.file.write (self.results)

        self.valid = True
        return self.results

    def identity (self):
        return hash (self.graph)


# vim: ts=4:sw=4:et

