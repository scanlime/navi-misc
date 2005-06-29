""" GraphAlgorithms

Various algorithms which work themselves upon the appropriate
GraphRepresentations to make them fast(ish)
"""

class DotPrint:
    """Simple graph walker which prints the graph as a dot(1) file.
       By default, vertexes are visited in dictionary key order, which
       may or may not be useful.
       """
    # FIXME - should allow the user to override the vertex order

    def __init__ (self, vertexMap, file=None):
        self.vertexMap = vertexMap

        self.print (file, 'Digraph {')

        # print vertices
        for vertex in vertexMap:
            # we use the hash of the vertex as the node id, since
            # only the label will be
            if (hasattr (vertex, 'dot_label')):
                # FIXME - escape things
                self.print (file, '%s [label="%s"];' % (hash (vertex), vertex.dot_label))
            else:
                self.print (file, '%s [label="%s"];' % (hash (vertex), vertex))

        # print edges
        for vertex in vertexMap:
            edges = vertexMap.query (vertex)
            for edge in edges:
                if edge.u is vertex:
                    if (hasattr (edge, 'dot_label')):
                        # FIXME - escape things
                        self.print (file, '%s -> %s [label="%s"];' % (hash (edge.u), hash (edge.v), edge.dot_label))
                    else
                        self.print (file, '%s -> %s [label="%s"];' % (hash (edge.u), hash (edge.v), edge))

        self.print (file, '}')

    def print (self, file, string):
        if file is None:
            print string
        else:
            file.write (string + '\n')
