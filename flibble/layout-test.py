#!/usr/bin/env python

import RandomArray, Numeric


class Edge:
    """Base class for graph edges. If edges need to include
       more information, this can be subclassed.
       """
    def __init__(self, nodeA, nodeB):
        self.a = nodeA
        self.b = nodeB


class Graph:
    """Represents a graph, where nodes and edges are represented by flexible Python
       objects and by an efficient Numeric array.
       The Graph must be constructed with a sequence of Edge instances. Nodes can
       be any hashable python object, and they will be inferred from the set of edges.
       """
    def __init__(self, edges):
        self.edgeList = edges

        # A unique list of nodes, and a mapping from each node to its index in the list
        self.nodeMap = {}
        self.nodeList = []

        # Create a Numeric array with indices into the nodeList for each edge
        self.edgeArray = Numeric.zeros((len(self.edgeList), 2), Numeric.Int)

        # Populate egeArray, nodeMap, and nodeList
        i = 0
        for edge in self.edgeList:
            # Add each node to the map and list
            for node in edge.a, edge.b:
                if node not in self.nodeMap:
                    id = len(self.nodeList)
                    self.nodeMap[node] = id
                    self.nodeList.append(node)

            # Add this edge to the array
            self.edgeArray[i] = self.nodeMap[edge.a], self.nodeMap[edge.b]
            i += 1

    def __repr__(self):
        return "<%s with %d nodes and %d edges>" % (self.__class__.__name__,
                                                    len(self.nodeList), len(self.edgeList))


if __name__ == '__main__':
    # Load the CIA dataset, formatted as a sequence of (path_a, path_b, strength, freshness) tuples
    import cPickle
    rows = cPickle.load(open('cia-dataset.p'))

    # Load the edges into a Graph object
    graph = Graph([Edge(a, b) for a, b, s, f in rows])
    print graph

    # Randomize the initial positions
    positions = RandomArray.uniform(0, 10, (len(graph.nodeList), 2))

    while True:
        energy = 0
        for a, b in graph.edgeArray:
            d = positions[a] - positions[b]
            energy += abs(Numeric.dot(d,d))
            d *= 0.01
            positions[a] -= d
            positions[b] += d
        print energy

