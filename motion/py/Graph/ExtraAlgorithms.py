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
        bfsObjects = []
        for key in self.starts.keys ():
            start = self.starts[key]
            end   = self.ends[key]
            bfsObjects.append (ParallelBFSSearch (key, start, end))

class ParallelBFSSearch ():
    """A class which runs a BFS on a graph, but without any specific stopping
       point.
       """

    def __init__ (self, graph, start, end):
        self.graph = graph
        self.start = start
        self.end   = end
