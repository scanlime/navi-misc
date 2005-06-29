""" Graph.Algorithms

Various algorithms which work themselves upon the appropriate
GraphRepresentations to make them fast(ish)
"""

class DotPrint:
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
       ...     Edge (nodes[3], nodes[0], ''),
       ...     ]
       >>> x = graph.addList (edges)
       >>> x = DotPrint (map)
       Digraph {
       1 [label="1"];
       2 [label="2"];
       3 [label="3"];
       4 [label="4"];
       5 [label="5"];
       1 -> 3 [label=""];
       2 -> 5 [label=""];
       3 -> 4 [label=""];
       4 -> 1 [label=""];
       }

       """

    def __init__ (self, vertexMap, file=None):
        self.vertexMap = vertexMap

        self.printline (file, 'Digraph {')

        # print vertices
        for vertex in vertexMap:
            # we use the hash of the vertex as the node id, since
            # only the label will be
            if hasattr (vertex, 'dot_label') and edge.dot_label is not None:
                # FIXME - escape things
                self.printline (file, '%s [label="%s"];' % (hash (vertex), vertex.dot_label))
            else:
                self.printline (file, '%s [label="%s"];' % (hash (vertex), vertex))

        # print edges
        for vertex in vertexMap:
            edges = vertexMap.query (vertex)
            for edge in edges:
                if edge.u is vertex:
                    if hasattr (edge, 'dot_label') and edge.dot_label is not None:
                        # FIXME - escape things
                        self.printline (file, '%s -> %s [label="%s"];' % (hash (edge.u), hash (edge.v), edge.dot_label))
                    else:
                        self.printline (file, '%s -> %s [label="%s"];' % (hash (edge.u), hash (edge.v), edge))

        self.printline (file, '}')

    def printline (self, file, string):
        if file is None:
            print string
        else:
            file.write (string + '\n')
