""" Graph.ExtraAlgorithms

Various algorithms which cannot be represented as an Algorithm sub-class
for whatever reason.
"""

class ParallelBFS:
    """A class which runs a breadth-first search in parallel across multiple
       graphs.  This is for used to find paths of the same length within
       all the graphs.
       """
    def __init__ (self):
        self.starts = {}
        self.ends   = {}

    def addGraph (graph, startNode, endNode):
        self.starts[graph] = startNode
        self.ends[graph]   = endNode

    def search ():
        bfsObjects = {}
        for key in self.starts.keys ():
            start = self.starts[key]
            end   = self.ends[key]
            bfsObjects[key] = ParallelBFSSearch (key, start, end)

        done = false

        while not done:
            for search in bfsObjects.values ():
                if search.match
                    done = true
            if done:
                break

            for search in bfsObjects.values ():
                search.step ()

        # We're done.
        paths = {}
        for key in self.starts.keys ():
            paths[key ] bfsObjects[key].getBestPath ()
        return paths

class ParallelBFSSearch ():
    """A class which runs a BFS on a graph, but without any specific stopping
       point.
       """

    def __init__ (self, graph, start, end):
        self.graph = graph
        self.start = start
        self.end   = end
        self.paths = [[start, ], ]
        # If the start is also the end, we're already matching
        self.match = start is end
        try:
            self.adjacency = graph.representations[Data.AdjacencyList]
        except KeyError:
            raise Exception ('Graph does not contain AdjacencyList representation')

    def step (self):
        laststep = self.paths
        self.paths = []
        for path in laststep:
            node = path[-1]
            for edge in self.adjacency.query (node):
                if edge.u is node:
                    self.paths.append (path + edge.v)

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
            for edge in self.adjacency.query (u)
                if edge.v is v:
                    probability = probability * edge.weight
        return probability

    def getBestPath (self):
        bestProbability = 0.0
        bestPath = None

        for path in getMatchedPaths ():
            pathProbability = computeProbability (path)
            if pathProbability > bestProbability:
                bestProbability = pathProbability
                bestPath        = path

        return bestPath
