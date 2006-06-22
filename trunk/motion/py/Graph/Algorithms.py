""" Graph.Algorithms

Various algorithms which work themselves upon the appropriate
GraphRepresentations to make them fast(ish)

*DFS and topoSort implementations originally Copyright (C)*
    *the Python Mission Control Kit team*
"""

from Dot import DotPrint
import Data, Combinatoric

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


class Heuristic (Algorithm):
    """A heuristic best first search."""

    def __init__(self, graph, costf, source, goal):
        self.costf = costf
        self.source = source
        self.goal = goal
        Algorithm.__init__ (self, graph)
        self.run ()

    def invalidate (self):
        Algorithm.invalidate (self)
        self.path = None
        self.costs = {}
        self.predecessors = {repr(self.source):None}

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
            next = self.predecessors[repr(next)]

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
        
        if self.graph.representations.has_key (Combinatoric.BayesAdjacency):
            adj = self.graph.representations[Combinatoric.BayesAdjacency]
        elif self.graph.representations.has_key (Combinatoric.AdjacencyList):
            adj = self.graph.representations[Combinatoric.AdjacencyList]
        else:
            adj = self.graph.representations[Data.AdjacencyList]

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
            for s in adj.query (node):
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
        return hash ((self.graph, repr(self.source), repr(self.goal), self.costf))

    def identity (self):
        """Identify runs of this algorithm by its hash value."""
        return hash (self)


class HeuristicPrint (Heuristic):
    """A debugging search that prints each step of the search to a dot file."""

    def __init__ (self, graph, costf, source, goal):
        source.dotAttrs.update ([('style', 'filled'),
                ('fillcolor', 'green')])
        goal.dotAttrs.update ([('style', 'filled'),
                ('fillcolor', 'red')])
        Heuristic.__init__ (self, graph, costf, source, goal)

    def run (self):
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
        path = None
        step = 0
        adj = self.graph.representations[Data.AdjacencyList]
        eList = self.graph.representations[Data.EdgeList]

        while len(agenda) > 0:
            # Get the next node and test for the goal
            node = agenda.pop()
            visited.append(node)

            path = self.pathToNode (node)
            for n in path:
                if n != self.source and n != self.goal:
                    n.dotAttrs.update ([('style', 'filled'),
                            ('fillcolor', 'blue')])

            for i in range (len (path) - 1):
                edge = eList.query (path[i], path[i+1])
                edge.dotAttrs['color'] = 'blue'

            print "printing step %i" % step
            f = file ('graphs/%s.dot' % step, 'w')
            DotPrint (self.graph, f)
            f.close ()

            for n in path:
                if n != self.source and n != self.goal:
                    del n.dotAttrs['style']
                    del n.dotAttrs['fillcolor']

            for i in range (len (path) - 1):
                edge = eList.query (path[i], path[i+1])
                del edge.dotAttrs['color']

            if node == self.goal:
                # Reconstruct the path to the goal
                self.path = path
                break

            # Add the successors of this node to the agenda and record the node
            # that generated these successors.
            numSucc = numAdded = 0
            for edge in adj.query (node):
                numSucc += 1
                if edge.v not in visited:
                    numAdded += 1
                    self.predecessors[repr(edge.v)] = node
                    agenda.append(edge.v)

            # Some debuggative statements
            print "%d likely successors" % (numSucc)
            print "    %d added to agenda" % (numAdded)

            # Resort the queue
            agenda.sort(cmp=self.compare)
            step += 1

        return self.path


# vim: ts=4:sw=4:et
