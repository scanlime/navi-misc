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
    positions = RandomArray.uniform(0, 100, (len(graph.nodeList), 2))

    # Solve for a local minimum in the energy of our spring system
    springLen = 5
    try:
        while 1:
            energy = 0
            for a, b in graph.edgeArray:
                ab = positions[a] - positions[b]
                magnitude = Numeric.dot(ab, ab)
                unitv = ab / magnitude
                forceMag = (magnitude - springLen) * 0.2
                energy += abs(forceMag)
                force = unitv * forceMag
                positions[a] -= force
                positions[b] += force
            print energy
    except KeyboardInterrupt:
        pass

    # Begin the SVG
    svg = open('graph.svg', 'w')
    svg.write('<?xml version="1.0" encoding="UTF-8"?>\n'
              '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"\n'
              '  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">\n'
              '<svg\n'
              '  xmlns="http://www.w3.org/2000/svg" version="1"\n'
              '  x="0" y="0" width="100" height="100">\n')

    # Write all edges
    for a, b in graph.edgeArray:
        va = positions[a]
        vb = positions[b]
        svg.write('<line x1="%f" y1="%f" x2="%f" y2="%f" stroke="black" stroke-width="0.1"/>\n' %
                  (va[0], va[1], vb[0], vb[1]))

    # Write all nodes
    for x, y in positions:
        svg.write('<circle cx="%f" cy="%f" r="0.2" stroke="black" fill="yellow" stroke-width="0.05"/>\n' %
                  (x, y))

    svg.write('</svg>\n')
    print "Graph written"
