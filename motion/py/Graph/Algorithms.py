""" Graph.Algorithms

Various algorithms which work themselves upon the appropriate
GraphRepresentations to make them fast(ish)

*DFS and topoSort implementations originally Copyright (C)*
    *the Python Mission Control Kit team*
"""

import Data

# Constants for graph coloring
WHITE, GRAY, BLACK = range(3)

class Algorithm:
    """A generic algorithm class.  This provides the basic infrastructure
       for maintaining cached results, so if algorithms depend on one
       another, they won't be re-run if they don't have to.
       """
    def __init__ (self, graph):
        self.graph = graph
        self.invalidate ()

        try:
            for mutator in (graph.add, graph.remove):
                mutator.observe (self.invalidate)
        except:
            graph.addEvents ()
            for mutator in (graph.add, graph.remove):
                mutator.observe (self.invalidate)

    def invalidate (self):
        self.valid = False

    def run (self):
        pass

    def identity ():
        """Subclasses implement this to provide some unique identifier.
           This allows us to request algorithm results and check the
           cache for a matching algorithm run, since different algorithms
           require different amounts of specificity.
           """
        pass

class CyclicGraphException (Exception):
    """This exception indicates that a cyclic graph was found where
       an acyclic graph was required. It contains a list of the back
       edges found.
       """
    def __init__ (self, backEdges):
        self.backEdges = backEdges
        Exception.__init__ (self, "A cycle was found when expecting an acyclic graph.  Back edges are: %r" % self.backEdges)

class DFS (Algorithm):
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

    desired_representation = Data.AdjacencyList

    def __init__ (self, graph, vertexOrder=None):
        Algorithm.__init__ (self, graph)
        self.vertexOrder = vertexOrder

    def invalidate (self):
        Algorithm.invalidate (self)

        # Normal DFS results, keyed by vertex
        self.color = {}
        self.predecessor = {}
        self.discoveryTime = {}
        self.finishingTime = {}
        self.time = 0

        # Edge classification
        self.edgeColor = {}
        self.edgesByColor = {WHITE: [],
                             GRAY:  [],
                             BLACK: []}

        try:
            self.adjacency = graph.representations[Data.AdjacencyList]
        except KeyError:
            raise Exception ('Graph does not contain AdjacencyList representation')

    def run (self):
        if self.valid:
            return self

        if vertexOrder is None:
            vertexOrder = self.adjacency.iterU ()
        for vertex in vertexOrder:
            if self.color.get (vertex, WHITE) == WHITE:
                self.visit (vertex)

        self.valid = True
        return self

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

def Relax (edge, weightf, distance, precedessors):
    """Relaxation of path constraint, used in numerous pathfinding
       algorithms.  Tests whether we can improve the shortest path to v.
       """
    w = weightf (edge)

    if distance[edge.v] > (distance[edge.u] + w):
        distance[edge.v] = distance[edge.u] + w
        predecessors[edge.v] = edge.u

class Dijkstra (Algorithm):
    """An implementation of Dijkstra's single-source shortest
       path algorithm, described on page 521 of 'Introduction to
       Algorithms'.
       """

    def __init__(self, graph, source, weightf):
        Algorithm.__init__ (self, graph)
        self.source = source
        self.weightf = weightf

        self.run ()

    def invalidate (self):
        Algorithm.invalidate (self)
        self.estimates = {}
        self.predecessors = {}

        try:
            self.vertexMap = self.graph.representations[Data.VertexMap]
        except KeyError:
            raise Exception ('Graph does not contain VertexMap representation')

        for vertex in self.vertexMap:
            # Technically this should be Inf, but I'm not sure how to notate
            # that in python.  2**30 should be sufficiently bigger than any
            # path we might find (famous last words...)
            self.estimates[vertex] = 2**30
            self.predecessors[vertex] = None
        self.estimates[self.source] = 0

    def run (self):
        if self.valid:
            return self.results

        queue = PriorityQueue ()
        queue.enqueue_list (self.vertexMap)
        found = []
        while not queue.Empty ():
            u = queue.extract ()
            found.append (u)
            # Iterate over all edges which have their origin at u
            for edge in self.vertexMap.query (u):
                if edge[0] is u:
                    Relax (edge, self.weightf, self.estimates, self.predecessors)

        self.valid = True
        return self.results

    def identity (self):
        return hash (self.graph, self.source)

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
            self.results += '%s\n' % vertex

        # print edges
        for vertex in self.vertexMap:
            edges = self.vertexMap.query (vertex)
            for edge in edges:
                if edge.u is vertex:
                    self.results += '%s\n' % edge

        self.results += '}'

        if self.file is not None:
            self.file.write (self.results)

        self.valid = True
        return self.results

    def identity (self):
        return hash (self.graph)


class Heuristic (Algorithm):
    """A heuristic best first search."""

    def __init__(self, graph, costf, successors, source, goal):
        Algorithm.__init__ (self, graph)
        self.costf = costf
        self.successors = successors
        self.source = source
        self.goal = goal

    def invalidate (self):
        Algorithm.invalidate (self)
        self.path = None
        self.costs = {}
        self.predecessors = {repr(source):None}

    def compare(self, a, b):
        keyA = repr(a)
        keyB = repr(b)

        if not self.costs.has_key(keyA):
            a_path = self.pathToNode(a)
            self.costs[keyA] = self.costf(a_path, self.goal)

        if not self.costs.has_key(keyB):
            b_path = self.pathToNode(b)
            self.costs[keyB] = self.costf(b_path, self.goal)

        ac = self.costs[keyA]
        bc = self.costs[keyB]

        return (bc - ac)

    def pathToNode(self, node):
        path = [node]
        next = self.predecessors[repr(node)]

        while next:
            path.insert(0, next)
            next = predecessors[repr(next)]

        return path

    def run(self):
        """Execute a heuristic search of a graph.
        
        Returns:
            A list of nodes that is the path from the source to the goal or
            ``None`` if there is no path.
        """
        if self.path:
            return self.path

        # Initialize all the local variables
        agenda = [self.source]
        visited = []

        while len(agenda) > 0:
            # Get the next node and test for the goal
            node = agenda.pop()
            visited.append(node)

            if node == self.goal:
                # Reconstruct the path to the goal
                self.path = self.pathToNode(node)
                break

            # Add the successors of this node to the agenda and record the node
            # that generated these successors.
            numSucc = numAdded = 0
            for s in self.successors(self.graph, node):
                numSucc += 1
                if s not in visited:
                    numAdded += 1
                    self.predecessors[repr(s)] = node
                    agenda.append(s)

            # Some debuggative statements
            print "%d likely successors" % (numSucc)
            print "    %d added to agenda" % (numAdded)

            # Resort the queue
            agenda.sort(cmp=self.compare)

        return self.path

    def __hash__ (self):
        """The hash value for this algorithm is the hash of the graph, source,
        goal, and cost function.
        """
        return hash ((self.graph, self.source, self.goal, self.costf))

    def identity (self):
        """Identify runs of this algorithm by its hash value."""
        return hash (self)


# vim: ts=4:sw=4:et
