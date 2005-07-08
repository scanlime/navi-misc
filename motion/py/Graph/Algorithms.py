""" Graph.Algorithms

Various algorithms which work themselves upon the appropriate
GraphRepresentations to make them fast(ish)

DFS and topoSort implementations originally Copyright (C)
    the Python Mission Control Kit team
"""

import Data

# Constants for graph coloring
WHITE, GRAY, BLACK = range(3)

class CyclicGraphException (Exception):
    """This exception indicates that a cyclic graph was found where
       an acyclic graph was required. It contains a list of the back
       edges found.
       """
    def __init__ (self, backEdges):
        self.backEdges = backEdges
        Exception.__init__ (self, "A cycle was found when expecting an acyclic graph.  Back edges are: %r" % self.backEdges)

class DFS:
    """An implementation of the Depth First Search algorithm, as
       described on page 540 of 'Introduction to Algorithms' by
       Cormen, Leiserson, Rivest, and Stein.

       The graph is represented as an AdjacencyList instance.
       The results of the search are all available in dictionaries
       keyed by vertex. Other algorithms that can be easily performed
       using the DFS results are also included.

       By default, vertices are visited in dictionary key order.
       This isn't guaranteed to give a stable result. If necessary,
       the vertex order can be overridden.
       """

    def __init__ (self, graph, vertexOrder=None):
        try:
            self.adjacency = graph.representations[Data.AdjacencyList]
        except KeyError:
            raise Exception ('Graph does not contain AdjacencyList representation')

        self.time = 0

        # Normal DFS results, keyed by vertex
        self.color = {}
        self.predecessor = {}
        self.discoveryTime = {}
        self.finishingTime = {}

        # Edge classification
        self.edgeColor = {}
        self.edgesByColor = {WHITE: [],
                             GRAY:  [],
                             BLACK: []}

        if vertexOrder is None:
            vertexOrder = self.adjacency.iterU ()
        for vertex in vertexOrder:
            if self.color.get (vertex, WHITE) == WHITE:
                self.visit (vertex)

    def visit (self, u):
        self.color[u] = GRAY
        self.time += 1
        self.discoveryTime[u] = self.time

        for edge in self.adjacency.query (u):
            vColor = self.color.get (edge.v, WHITE)

            # This is a modification to DFS that supports edge classification:
            # the edge is colored according to the color of 'v' the first
            # time we see that edge.
            if edge not in self.edgeColor:
                self.edgeColor[edge] = vColor
                self.edgesByColor[vColor].append (edge)

            if vColor == WHITE:
                self.predecessor[edge.v] = u
                self.visit (edge.v)

        self.color[u] = BLACK
        self.time += 1
        self.finishingTime[u] = self.time

    def isCyclic (self):
        """After a graph is processed, this tests whether it was found to be cyclic"""
        # A graph is cyclic if any GRAY edges were encountered
        return len (self.edgesByColor[GRAY]) != 0

    def failIfCyclic (self):
        """Raise a CyclicGraphException if the graph was found to be cyclic"""
        backEdges = self.edgesByColor[GRAY]
        if backEdges:
            raise CyclicGraphException (backEdges)

    def topoSort (self, vertices, reversed=False):
        """Topologically sort a given vertex list, returning a new list.
           Topographic sorts are explained on p549 of "Introduction to Algorithms":

           A topological sort of a dag G = (V, E) is a linear ordering
           of all is vertices such that if G contains an edge (u, v),
           then u appears before v in the ordering.

           This function, for any subset of the graph's vertices, ends up
           with a list satisfying this ordering constraint.

           This raises an exception if it's used on a cyclic graph.
           """
        self.failIfCyclic ()

        # Sort on the vertices' finishing times, using a Schwartzian
        # transform.
        #
        # Note that vertices not appearing in the constraints graph are
        # assigned arbitrarily to the highest value. As a tiebreaker, we
        # add the vertex's original list index- we can't necessarily do
        # numeric comparisons on the vertices themselves.

        unknowns = []
        l = []
        highest = 0
        for index, vertex in enumerate(vertices):
            f = self.finishingTime.get(vertex, 0)
            if not reversed:
                f = -f
            if f == 0:
                unknowns.append((f, index, vertex))
            else:
                l.append((f, index, vertex))
                if f > highest:
                    highest = f

        # Add in the unknowns at the end, with the highest f values
        for f, index, vertex in unknowns:
            l.append( (highest+1, index, vertex) )

        l.sort()
        return [i[2] for i in l]

class DotPrint:
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
       ...     Edge (nodes[3], nodes[0], ''),
       ...     ]
       >>> x = graph.addList (edges)
       >>> x = DotPrint (graph)
       Digraph {
       1 [label="1"];
       2 [label="2"];
       3 [label="3"];
       4 [label="4"];
       5 [label="5"];
       1 -> 3 [label=""];
       2 -> 5 [label=""];
       3 -> 4 [label=""];
       4 -> 1 [label=""];
       }

       """

    def __init__ (self, graph, file=None):
        try:
            self.vertexMap = graph.representations[Data.VertexMap]
        except KeyError:
            raise Exception ('Graph does not contain VertexMap representation')

        self.printline (file, 'Digraph {')

        self_loops = set([])

        # print vertices
        for vertex in self.vertexMap:
            # we use the hash of the vertex as the node id, since
            # only the label will be
            if hasattr (vertex, 'dot_label') and vertex.dot_label is not None:
                self.printline (file, '%s [label="%s"];' % (hash (vertex), vertex.dot_label))
            else:
                self.printline (file, '%s [label="%s"];' % (hash (vertex), vertex))

        # print edges
        for vertex in self.vertexMap:
            edges = self.vertexMap.query (vertex)
            for edge in edges:
                if edge.u is vertex:
                    if hasattr (edge, 'dot_label') and edge.dot_label is not None:
                        self.printline (file, '%s -> %s [label="%s"];' % (hash (edge.u), hash (edge.v), edge.dot_label))
                    else:
                        self.printline (file, '%s -> %s [label="%s"];' % (hash (edge.u), hash (edge.v), edge))

        self.printline (file, '}')

    def printline (self, file, string):
        if file is None:
            print string
        else:
            file.write (string + '\n')
