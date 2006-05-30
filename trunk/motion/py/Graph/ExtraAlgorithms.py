""" Graph.ExtraAlgorithms

Various algorithms which cannot be represented as an Algorithm sub-class
for whatever reason.

Classes:
    - ParallelBFS           A BFS in multiple graphs simultaneously
    - ParallelBFSSearch
    - DLS                   A depth limited search of a graph
    - Heuristic             A heuristic best first search
"""

import Data, gc

class ParallelBFS:
    """A class which runs a breadth-first search in parallel across multiple graphs.
    
    This is for used to find paths of the same length within all the graphs.

    Members:
        - starts        Dictionary of starting points
        - ends          Dictionary of ending points
        - graphs        Dictionary of graphs

    Methods:
        - addGraph      Add a graph to the search
        - search        Execute the search
    """
    def __init__ (self):
        """Initialize all members to empty dictionaries."""
        self.starts = {}
        self.ends   = {}
        self.graphs = {}

    def addGraph (self, name, graph, startNode, endNode):
        """Add a graph with start and end nodes to the search.

        Arguments:
            - name          The name of the graph
            - graph         The graph
            - startNode     The node from which to start the search
            - endNode       The goal of the search
        """
        self.starts[graph] = startNode
        self.ends[graph]   = endNode
        self.graphs[graph] = name

    def search (self):
        """Execute the search and return a dictionary of all the paths."""
        bfsObjects = {}
        for key in self.starts.keys ():
            start = self.starts[key]
            end   = self.ends[key]
            name  = self.graphs[key]
            bfsObjects[key] = ParallelBFSSearch (key, name, start, end)

        done = False

        step = 0

        while not done:
            match = True
            for search in bfsObjects.values ():
                if not search.match:
                    match = False
            if match:
                break

            print '    step',step
            for search in bfsObjects.values ():
                search.step ()
            step = step + 1

            # Run a garbage collection pass
            gc.collect ()

        # We're done.
        paths = {}
        for key in self.starts.keys ():
            paths[key] = bfsObjects[key].getBestPath ()
        return paths

class ParallelBFSSearch:
    """A class which runs a BFS on a graph, but without any specific stopping
       point.
       """

    def __init__ (self, graph, name, start, end):
        """Initialize the search.

        Raise an exception if the graph doesn't have an AdjacencyList.

        Arguments:
            - graph     The graph to search
            - name      The name of the graph
            - start     The start of the search
            - end       The end of the search
        """
        self.graph = graph
        self.start = start
        self.end   = end
        self.name  = name
        self.paths = [[start, ], ]
        # If the start is also the end, we're already matching
        self.match = start is end
        try:
            self.adjacency = graph.representations[Data.AdjacencyList]
        except KeyError:
            raise Exception ('Graph does not contain AdjacencyList representation')

    def step (self):
        """Add a new path for every successor in every path.

        For each path, iterate over all the edges coming out of the last node
        and add a new path to the list of paths ending with each node at the
        other end of the edges.
        """
        newpaths = []
        for path in self.paths:
            #print '        current path is',path
            node = path[-1]
            for edge in self.adjacency.query (node):
                if edge.u is node:
                    newpaths.append (list (path + [edge.v]))
                    #print '        appending path',newpaths[-1]
        self.paths = newpaths
        #print '        %6d paths in %s sub-graph' % (len (self.paths), self.name)

        # Update match status
        self.match = False
        for path in self.paths:
            if path[-1] is self.end:
                self.match = True
                break

    def getMatchedPaths (self):
        """Return an iterator over all the paths that end in the goal node."""
        for path in self.paths:
            if path[-1] is self.end:
                yield path

    def computeProbability (self, path):
        """Returns the probability of a given path.

        Arguments:
            - path      The path whose probability is desired
        """
        probability = 1.0
        for i in range (len (path) - 2):
            u = path[i]
            v = path[i + 1]
            # FIXME - there's got to be a better way
            for edge in self.adjacency.query (u):
                if edge.v is v:
                    probability = probability * edge.weight
        return probability

    def getBestPath (self):
        """Return the path from start to end with the highest probability."""
        bestProbability = 0.0
        bestPath = None

        for path in self.getMatchedPaths ():
            pathProbability = self.computeProbability (path)
            if pathProbability > bestProbability:
                bestProbability = pathProbability
                bestPath        = path

        return bestPath


class DLS:
    """A depth limited search of a graph.
    
    Members:
        - graph                 The graph to search
        - start                 The node to start the search from
        - end                   The goal of the search
        - paths                 A list of paths to the goal, the index into the list
                                corresponds to the depth of the search

    Methods:
        - search                Execute the search
        - computeProbability    Compute the probability of a path
        - getBestPath           Get the highest probability path

    """

    def __init__ (self, graph, start, end):
        """Initialize the search.

        Raises an exception if the graph has no AdjacencyList representation.

        Arguments:
            - graph     The graph to search
            - start     The start of the search
            - end       The goal
        """
        self.graph = graph
        self.start = start
        self.end   = end
        self.paths = []

        try:
            self.adjacency = graph.representations[Data.AdjacencyList]
        except KeyError:
            raise Exception ("Graph does not contain AdjacencyList representation")

    def search (self, depth):
        """Search the graph to a maximum depth of depth.
        
        Arguments:
            - depth     The maximum depth to which the graph should be searched

        Returns:
            A list of paths to the goal, the index into the list corresponds to
            the length of the path.
        """
        newpaths = [[self.start, ], ]

        for i in range (depth):
            paths     = []
            goodpaths = []

            for path in newpaths:
                # Extend the current node one more level.
                node = path[-1]

                for edge in self.adjacency.query (node):
                    if edge.u is node:
                        # If this edge connects us to our goal, it is a good
                        # path and deserves a cookie.
                        if edge.v is self.end:
                            goodpaths.append (list(path + [edge.v]))
                        # If this edge doesn't get us to our goal, we remember
                        # the path and check it again next iteration one level deeper.
                        else:
                            paths.append (list (path + [edge.v]))

            # Any good paths get stored, otherwise None is stored for this depth
            if len (goodpaths) > 0:
                self.paths.append (goodpaths)
            else:
                self.paths.append (None)

            newpaths = paths

        # find the best path for each depth
        for i in range (depth):
            if self.paths[i]:
                self.paths[i] = self.getBestPath (self.paths[i])

        return self.paths

    def computeProbability (self, path):
        """Return the probability of a path.

        Arguments:
            - path      The path whose probability is desired
        """
        probability = 1.0
        for i in range (len (path) - 2):
            u = path[i]
            v = path[i + 1]
            for edge in self.adjacency.query (u):
                if edge.v is v:
                    probability = probability * edge.weight
        return probability

    def getBestPath (self, paths):
        """Return the path with the highest probability.

        Arguments:
            - paths     A list of paths from which the best path is selected
        """
        bestPath = None
        bestProbability = 0.0

        # HACK!
        return paths[0]

        for path in paths:
            probability = self.computeProbability (path)
            if probability > bestProbability:
                bestPath = path
                bestProbability = probability

        return bestPath


class Heuristic:
    """A heuristic best first search.
   
    Members:
        - path              The path from start to end
        - predecessors      A dictionary of back pointers
        - costs             Cached path costs
        - graph             The graph to search
        - source            The node to start the search from
        - goal              The goal of the search
        - costf             The cost function
        - successorf        The successor generating function

    Methods:
        - run               Execute the search
        - pathToNode        Return a path to given node
    """
    def __init__(self, graph, source, goal, costf, successorf):
        self.path = None
        self.predecessors = {source: None}
        self.costs = {}
        self.graph = graph
        self.source = source
        self.goal = goal
        self.costf = costf
        self.successorf = successorf

    def run(self):
        """Execute a heuristic search of a graph.
        
        Returns:
            A list of nodes that is the path from the source to the goal or
            None if there is no path.
        """
        # Use this function to sort the agenda
        def compare(a, b):
            if not self.costs.has_key(a):
                a_path = self.pathToNode(a)
                self.costs[a] = self.costf(a_path, self.goal)

            if not self.costs.has_key(b):
                b_path = self.pathToNode(b)
                self.costs[b] = self.costf(b_path, self.goal)

            ac = self.costs[a]
            bc = self.costs[b]

            return (bc - ac)

        agenda = [self.source]
        visited = []

        while len(agenda) > 0:
            # Get the next node and test for the goal
            node = agenda.pop()
            visited.append(node)
            if node == self.goal:
                print "GOAL!"
                # Reconstruct the path to the goal
                self.path = self.pathToNode(node)
                break
            print "not goal"

            # Add the successors of this node to the agenda and record the node
            # that generated these successors.
            for s in self.successorf(self.graph, node):
                print "a successor"
                if s not in visited:
                    print "    not visited"
                    self.predecessors[s] = node
                    agenda.append(s)

            print "sorting..."
            # Resort the queue
            agenda.sort(cmp=compare)
            print "    done."

        return self.path

    def pathToNode(self, node):
        """Return the path to a given node.

        Follows the back pointers in predecessors to find a path from the
        source to node.

        Arguments:
            - node      The node at the end of the desired path
        """
        path = [node]
        next = self.predecessors[node]
        while next:
            path.insert(0, next)
            next = self.predecessors[next]

        return path

# vim:ts=4:sw=4:et
