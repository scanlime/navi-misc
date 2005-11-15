""" Graph.ExtraAlgorithms

Various algorithms which cannot be represented as an Algorithm sub-class
for whatever reason.
"""

import Data, gc

class ParallelBFS:
    """A class which runs a breadth-first search in parallel across multiple
       graphs.  This is for used to find paths of the same length within
       all the graphs.
       """
    def __init__ (self):
        self.starts = {}
        self.ends   = {}
        self.graphs = {}

    def addGraph (self, name, graph, startNode, endNode):
        self.starts[graph] = startNode
        self.ends[graph]   = endNode
        self.graphs[graph] = name

    def search (self):
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
        for path in self.paths:
            if path[-1] is self.end:
                yield path

    def computeProbability (self, path):
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
        bestProbability = 0.0
        bestPath = None

        for path in self.getMatchedPaths ():
            pathProbability = self.computeProbability (path)
            if pathProbability > bestProbability:
                bestProbability = pathProbability
                bestPath        = path

        return bestPath


class DLS:
    """A depth limited search of a graph."""

    def __init__ (self, graph, start, end):
        self.graph = graph
        self.start = start
        self.end   = end
        self.paths = []

        try:
            self.adjacency = graph.representations[Data.AdjacencyList]
        except KeyError:
            raise Exception ("Graph does not contain AdjacencyList representation")

    def search (self, depth):
        """Search the graph to a maximum depth of depth."""
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
        probability = 1.0
        for i in range (len (path) - 2):
            u = path[i]
            v = path[i + 1]
            for edge in self.adjacency.query (u):
                if edge.v is v:
                    probability = probability * edge.weight
        return probability

    def getBestPath (self, paths):
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

# vim:ts=4:sw=4:et
