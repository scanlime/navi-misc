"""Dance.MotionGraph

Build a graph from a corpus of motions
"""

from Graph.Data import Graph, Edge, AdjacencyList, VertexMap, EdgeList
from Graph.Algorithms import DotPrint
from Graph import algorithms_c

import Numeric
import MLab

import math

class ProbabilityEdge (Edge):
    __slots__ = ['u', 'v', 'dot_label', 'count', 'weight']

    def __init__ (self, u, v):
        Edge.__init__ (self, u, v)
        self.count = 0
        self.weight = None

    def visit (self):
        self.count += 1

    def normalize (self, total):
        self.weight = float (self.count) / total
        self.dot_label = '%.2f' % self.weight

    def __getstate__ (self):
        return self.u, self.v, self.dot_label, self.count, self.weight

    def __setstate__ (self, state):
        self.u, self.v, self.dot_label, self.count, self.weight = state

class MotionGraph (Graph):
    def __init__ (self):
        Graph.__init__ (self, [DotPrint])

class MotionGraphNode:
    __slots__ = ['mins', 'maxs']

    def __init__ (self, mins, maxs):
        self.mins = mins
        self.maxs = maxs
        self.center = []

        extents = []
        for i in (range (len (mins)) + 1):
            extents.append ('[%.2f, %.2f]' % (mins[i], maxs[i]))
            self.center.append (maxs[i] - mins[i])
        self.dot_label = '\\n'.join (extents)

    def __repr__ (self):
        return '<MGNode: %s>' % self.dot_label

    def inside (self, point):
        if len (point) != len (self.mins):
            raise AttributeError ("dimension doesn't match! %d vs %d" % (len (point), len (self.mins)))
        for i in range (len (point)):
            if point[i] < self.mins[i] or point[i] > self.maxs[i]:
                return False
        return True

    def center (self):
        return self.center

def search_graphs (graphs, starts, ends, depth):
    paths = {}
    for bone in graphs.keys():
        print '    searching',bone
        representation = graphs[bone].representations[AdjacencyList]
        paths[bone] = algorithms_c.depthLimitedSearch (representation, starts[bone], ends[bone], depth)

    retpaths = None
    for i in range (len (paths['root'])):
        coverage = 0
        # If all the paths at this depth are real, we're done
        match = True
        for bone in graphs.keys ():
            if paths[bone][i] is None:
                match = False
            else:
                coverage = coverage + 1

        if match:
            print '    depth = %2d, match!' % i
            retpaths = {}
            for bone,path in paths.items ():
                retpaths[bone] = path[i]
            break
        else:
            print '    depth = %2d, coverage = %2d/%2d, no match' % (i, coverage, len (graphs.keys ()))
    return retpaths

def cp_range (dof, angle):
    """Return a list of tuples which form a cartesian product of the range
       [0, 360] in steps of angle.  This is used to build our sea of nodes
       for building the graph.
       """
    if dof == 1:
        return [(x,) for x in (range (0, 360, angle))]
    if dof == 2:
        nodes = []
        for x in range (0, 360, angle):
            for y in range (0, 360, angle):
                nodes.append ((x, y))
        return nodes
    if dof == 3:
        nodes = []
        for x in range (0, 360, angle):
            for y in range (0, 360, angle):
                for z in range (0, 360, angle):
                    nodes.append ((x, y, z))
        return nodes

def fixnegative (x):
    while x < 0:
        x = x + 360
    return x

def fix360 (x):
    if x == 360:
        return 0
    return x

def build_graphs (key, datas):
    """Build a graph using the data arrays from any number of files."""
    # If this is the root, we only want the last 3 dof, for now
    # FIXME - we really should track root position, but that's more
    # complicated.  We also don't *know* that the last 3 DOF are going
    # to be orientation, that's just convention.
    if key == 'root':
        datas = [d[:,3:6] for d in datas]
    # It's silly to have angle values outside of [0, 360]
    datas = [Numeric.remainder (d, 360.0) for d in datas]

    # This assumes the same DOF for each bone in each file!
    dof = datas[0].shape[1]

    # degrees covered (angle-wise) within a single node.  Note that for some
    # bones, the number of nodes we have will be (360 / interval)^3, so be
    # sparing when decreasing this!
    interval = 5

    graph          = MotionGraph   ()
    adjacency_list = AdjacencyList (graph)
    vertex_map     = VertexMap     (graph)
    edge_list      = EdgeList      (graph)

    # Create list of nodes
    nodes = {}
    for angle in cp_range (dof, interval):
        node = MotionGraphNode ([n for n in angle], [n + interval for n in angle])
        nodes[angle] = node

    # Add edges for data from each file
    for d in datas:
        build_graph (d, graph, nodes, edge_list, interval)

    # Normalize probabilities for all edges coming out of a vertex. The sum
    # of probabilities along edges coming out of any given vertex will be 1.
    for vertex in vertex_map:
        total = 0
        edges = vertex_map.query (vertex)
        for edge in edges:
            if edge.u is vertex:
                total += edge.count
        for edge in edges:
            if edge.u is vertex:
                edge.normalize (total)

    return graph

def build_graph (d, graph, nodes, edge_list, interval):
    frames = d.shape[0]

    # find edges
    data1 = d[0,:]
    bases = tuple (int (n / interval) * interval for n in tuple (map (fixnegative, data1)))
    bases = tuple (map (fix360, bases))
    node1 = nodes[bases]

    for frame in range (1, frames):
        node2 = None
        data2 = d[frame,:]
        bases = tuple (int (n / interval) * interval for n in tuple (map (fixnegative, data2)))
        bases = tuple (map (fix360, bases))
        node2 = nodes[bases]

        # check to see if it's a self-loop
        if node1 is node2:
            try:
                edge = edge_list.query (node1, node2)
                edge.visit ()
            except KeyError:
                try:
                    edge = edge_list.query (node2, node1)
                    edge.visit ()
                except KeyError:
                    edge = ProbabilityEdge (node1, node2)
                    edge.visit ()
                    graph.addList ([edge])
        else:
            try:
                edge = edge_list.query (node1, node2)
                edge.visit ()
            except KeyError:
                edge = ProbabilityEdge (node1, node2)
                edge.visit ()
                graph.addList ([edge])
        data1 = data2
        node1 = node2
