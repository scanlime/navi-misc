#!/usr/bin/env python

"""Unit tests for the Graph module."""

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


import sys, pickle, random
from unittest import makeSuite, TestCase, TestSuite
from Graph import algorithms_c
from Graph.Data import Graph, VertexMap, AdjacencyList
from Dance.MotionGraph import MotionGraph

def suite ():
    """Return a TestSuite consisting of all the tests in this module."""
    tests = (makeSuite (TestDijkstra),
             makeSuite (TestAStar),
             makeSuite (TestIterativeDeepening))
    return TestSuite (tests)


class GraphTest (TestCase):
    """Base class for graph tests. Initializes the graph used by all the test
       suites.
       """
    def setUp (self):
        def _subgraph (graph, node, depth):
            ret = {}
            vs = [edge.v for edge in graph.query (node)]
            if depth >= 0:
                for v in vs:
                    n = _subgraph (graph, v, depth - 1)
                    for key, value in n.iteritems():
                        ret[key] = value
            if len (vs):
                ret[node] = vs
            return ret

        graph = pickle.load (open (sys.argv[1]))
        root = graph['lradius']

        start = random.choice (root.representations[VertexMap].data.keys ())
        nodes = _subgraph (root.representations[AdjacencyList], start, 3)

        self.graph = MotionGraph ()

        self.v_map = VertexMap (self.graph)
        self.adj   = AdjacencyList (self.graph)

        self.graph.addTree (nodes)


class TestDijkstra (GraphTest):
    """Test Dijkstra's shortest path algorithm."""
    def setUp (self):
        GraphTest.setUp(self)

        self.path = []
        def find_path ():
            start = random.choice (self.v_map.data.keys ())
            self.path = [start]
            for i in range (3):
                adjacencies = list (self.adj.query (self.path[-1]))
                if len (adjacencies) == 0:
                    self.path = []
                    return
                choice = random.choice (adjacencies)
                self.path.append (choice.v)

        while len (self.path) == 0:
            find_path ()

    def testZeroLen (self):
        """Dijkstra: Start and goal are the same"""
        # First we need to find a self-loop
        loop = None
        for u in self.v_map:
            for edge in self.v_map.query (u):
                if edge.v is u:
                    loop = u
                    break
        path = algorithms_c.dijkstraSearch (self.adj, loop, loop)
        self.assertEqual (len (path), 1)
        self.assertEqual (path[0], loop)

    def testOneLen (self):
        """Dijkstra: Goal is a successor of start"""
        path = algorithms_c.dijkstraSearch (self.adj, self.path[0], self.path[1])
        self.assertEqual (len (path), 2)

    def testFullPath (self):
        """Dijkstra: Full path"""
        path = algorithms_c.dijkstraSearch (self.adj, self.path[0], self.path[-1])
        self.failIf (len (path) > len (self.path))


class TestAStar (TestCase):
    """Test the A* search."""
    def testZeroLen (self):
        """A*: Start and goal are the same"""
        self.fail ("unimplemented")

    def testOneLen (self):
        """A*: Goal is a successor of start"""
        self.fail ("unimplemented")

    def testFullPath (self):
        """A*: Full path"""
        self.fail ("unimplemented")


class TestIterativeDeepening (TestCase):
    """Test the iterative deepening depth first search."""
    def testZeroLen (self):
        """Iterative Deepening: Start and goal are the same"""
        self.fail ("unimplemented")

    def testOneLen (self):
        """Iterative Deepening: Goal is a successor of start"""
        self.fail ("unimplemented")

    def testFullPath (self):
        """Iterative Deepening: Full path"""
        self.fail ("unimplemented")


if __name__ == "__main__":
    from unittest import TextTestRunner
    TextTestRunner (verbosity=2).run (suite ())


# vim: ts=4:sw=4:et
