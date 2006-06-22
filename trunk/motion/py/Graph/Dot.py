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
    __slots__ = ["dotAttrs"]

    def __init__ (self, **kwargs):
        self.dotAttrs = kwargs

    def dotString (self):
        raise Exception ("dotString not overridden in %s" % (self.__class__.__name__))

    def __str__ (self):
        return self.dotString ()


class Node (Printable):
    def dotString (self):
        id = str (hash (self))
        attrs = ['%s="%s"' % (key, value) for key, value in \
                self.dotAttrs.iteritems ()]

        return "%s [%s];" % (id, string.join (attrs, ','))


class Edge (Printable):
    __slots__ = ["u", "v", "dotAttrs"]

    def __init__ (self, u, v, **kwargs):
        Printable.__init__ (self, kwargs)
        self.u = u
        self.v = v

    def dotString (self):
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

