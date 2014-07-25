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


# vim:ts=4:sw=4:et
