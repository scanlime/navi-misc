#!/usr/bin/env python

import force_directed


class Edge:
    """Base class for graph edges. If edges need to include
       more information, this can be subclassed.
       """
    def __init__(self, vertexA, vertexB):
        self.a = vertexA
        self.b = vertexB


class Graph:
    """A higher level graph abstraction in which edges are instances
       of the Edge class and vertices can be any hashable python object.
       This creates an edge list suitable for passing to a low-level graph.
       """
    def __init__(self, edges):
        # A unique list of vertices, and a mapping from each vertex to its index in the list
        self.vertexMap = {}
        self.vertexList = []

        # A list of edges, as 2-tuples of vertex indices
        self.edgeTuples = []

        # Populate edgeList, vertexMap, and vertexList
        i = 0
        for edge in edges:
            # Add each vertex to the map and list
            for vertex in edge.a, edge.b:
                if vertex not in self.vertexMap:
                    id = len(self.vertexList)
                    self.vertexMap[vertex] = id
                    self.vertexList.append(vertex)

            # Add this edge to the array
            self.edgeTuples.append((self.vertexMap[edge.a], self.vertexMap[edge.b]))


if __name__ == '__main__':
    # Load the CIA dataset, formatted as a sequence of (path_a, path_b, strength, freshness) tuples
    import cPickle
    rows = cPickle.load(open('cia-dataset.p'))

    # Load the edges into a high-level graph object
    hlGraph = Graph([Edge(a, b) for a, b, s, f in rows])

    # Make a force_directed graph from that
    fdGraph = force_directed.Graph(hlGraph.edgeTuples, len(hlGraph.vertexList))
    print fdGraph

    fdGraph.randomize(0, 0, 100, 100)


    vectors = fdGraph.getVectors()

#     # Solve for a local minimum in the energy of our spring system
#     springLen = 5
#     try:
#         while 1:
#             energy = 0
#             for a, b in graph.edgeArray:
#                 ab = positions[a] - positions[b]
#                 magnitude = Numeric.dot(ab, ab)
#                 unitv = ab / magnitude
#                 forceMag = (magnitude - springLen) * 0.2
#                 energy += abs(forceMag)
#                 force = unitv * forceMag
#                 positions[a] -= force
#                 positions[b] += force
#             print energy
#     except KeyboardInterrupt:
#         pass

    # Begin the SVG
    svg = open('graph.svg', 'w')
    svg.write('<?xml version="1.0" encoding="UTF-8"?>\n'
              '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"\n'
              '  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">\n'
              '<svg\n'
              '  xmlns="http://www.w3.org/2000/svg" version="1"\n'
              '  x="0" y="0" width="100" height="100">\n')

    # Write all edges
    for a, b in hlGraph.edgeTuples:
        va = vectors[a]
        vb = vectors[b]
        svg.write('<line x1="%f" y1="%f" x2="%f" y2="%f" stroke="black" stroke-width="0.1"/>\n' %
                  (va[0], va[1], vb[0], vb[1]))

    # Write all nodes
    for x, y in vectors:
        svg.write('<circle cx="%f" cy="%f" r="0.2" stroke="black" fill="yellow" stroke-width="0.05"/>\n' %
                  (x, y))

    svg.write('</svg>\n')
    print "Graph written"
