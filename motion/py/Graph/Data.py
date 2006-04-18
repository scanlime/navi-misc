""" Graph.Data
Graph data structures, for use in stuff.  Much of this code is "inspired"
by Micah Dowty's code for space-grant's PyMCK system, so you should love him.

Originally Copyright (C) 2004-2005 the Python Mission Control Kit team
"""

import Observable

class Edge (object):
    """This object represents a graph edge.  It is immutable and
       usable as a dictionary key.
       """
    __slots__ = ['u', 'v', 'dot_label']

    def __init__ (self, u, v, dot_label=None):
        self.u = u
        self.v = v
        self.dot_label = dot_label

    def __repr__ (self):
        return '<%s from %r to %r>' % (self.__class__.__name__, self.u, self.v)

    def __hash__ (self):
        return hash ((self.u, self.v))

    def __cmp__ (self, other):
        return cmp ((self.u, self.v), (other.u, other.v))

    def copy (self):
        return self.__class__ (self.u, self.v)

    def __getstate__ (self):
        return self.u, self.v, self.dot_label

    def __setstate__ (self, state):
        self.u, self.v, self.dot_label = state

class Graph (object):
    """A generic graph. This object does not specify how the graph is stored
       or what kind of queries can be made against it- that is done by instantiating
       GraphRepresentation objects for this Graph. This lets one set of data be
       represented in multiple ways, depending on what kinds of questions we expect
       to ask.

       >>> g = Graph ()
       >>> g.addTree ({
       ...     0 : [1],
       ...     1 : [2, 3, 4],
       ...     2 : [3, 0],
       ...     4 : [3],
       ...     })
       [<Edge from 0 to 1>, <Edge from 1 to 2>, <Edge from 1 to 3>, <Edge from 1 to 4>, <Edge from 2 to 3>, <Edge from 2 to 0>, <Edge from 4 to 3>]
       """
    edgeClass = Edge

    __slots__ = ['representations']

    def __init__ (self, algorithms=[]):
        self.addEvents ()
        self.representations = {}

        for algorithm in algorithms:
            try:
                self.representations.append (algorithm.desired_representation ())
            except:
                pass

    def addEvents (self):
        Observable.attachEvents (self, 'add', 'remove')

    def has_representation (self, representation):
        return self.representations.has_key (representation)

    def addList (self, edges):
        """Add multiple edges at once, from any iterable source.
           Returns the list of edges actually added.
           """
        if type (edges) is list:
            # An optimization for the case in which we already have a list
            for edge in edges:
                self.add (edge)
            return edges

        else:
            # If we have some other data type, like a generator, we need
            # to create a new list as we add the edges.
            l = []
            for edge in edges:
                self.add (edge)
                l.append (edge)
            return l

    def removeList (self, edges):
        """Remove multiple edges at once"""
        for edge in edges:
            self.remove (edge)

    def addTree (self, tree):
        """Add multiple edges at once, arranged as an adjacency list.
           'tree' is a dictionary mapping 'u' vertices to lists of 'v'
           vertices. Returns a list of edges actually added.
           """
        l = []
        for u, vList in tree.iteritems ():
            for v in vList:
                l.append (self.edgeClass (u, v))
        return self.addList (l)

    def cacheAlgorithm (self, attrib, factory, *args, **kwargs):
        """Update an attribute 'attrib' on this object that will contain
           the results of running 'factory' with the supplied arguments.
           Any time this graph is mutated, the cached result will be
           invalidated.
           """
        if not getattr (self, attrib, None):
            # Cache the algorithm result
            setattr (self, attrib, factory (*args, **kwargs))

            # Create an invalidator, and save a reference to it so it isn't GC'ed
            def cacheInvalidator (_, self=self, attrib=attrib):
                setattr (self, attrib, None)
            setattr (self, "_%s_invalidator" % attrib, cacheInvalidator)

            # Set up observers to invalidate it
            for mutator in (self.add, self.remove):
                mutator.observe (cacheInvalidator)

    def __getstate__ (self):
        return self.representations

    def __setstate__ (self, state):
        self.representations = state

class GraphRepresentation (object):
    """This is an abstract base class for data structures used to represent
       a graph. A Graph object by itself has no representation at all-
       GraphRepresentation instances are attached to a graph in order to
       actually provide data structures that useful queries can be made
       against.

       Note that all GraphRepresentations you plan to use should be added
       to a Graph before populating it! GraphRepresentations will not be
       able to store edges added before they were attached.
       """
    def __init__ (self, graph):
        if graph.has_representation (self.__class__):
            return graph.representations[self.__class__]
        self.graph = graph;
        graph.add.observe (self.onAdd)
        graph.remove.observe (self.onRemove)
        graph.representations[self.__class__] = self

    def onAdd (self, edge):
        """Observer that is notified when an edge is added to the graph"""
        pass

    def onRemove (self, edge):
        """Observer that is notified when an edge is removed from the graph"""
        pass

class AdjacencyList (GraphRepresentation):
    """A graph representation in which every vertex 'u' is mapped
       to a hash of all 'v' for which an edge (u, v) exists.
       """

    def __init__ (self, graph):
        self.graph = graph
        GraphRepresentation.__init__ (self, graph)
        self.data = {}

    def __iter__ (self):
        """Iterate over all edges in this graph. As an adjacency list,
           vertices can't be efficiently iterated over.
           """
        for u, vList in self.data.iteritems ():
            for v in vList:
                yield self.graph.edgeClass (u, v)

    def __contains__ (self, edge):
        """Determine whether this already contains a given edge"""
        return (edge.u in self.data) and (edge.v in self.data[edge.u])

    def iterU (self):
        """Iterate over all 'u' vertices efficiently"""
        return self.data.iterkeys ()

    def query (self, u):
        """Iterate over all edges containing u"""
        vList = self.data.get (u)
        if vList:
            for v in vList:
                yield self.graph.edgeClass (u, v)

    def onAdd (self, edge):
        if edge in self:
            raise KeyError ("Duplicate edge %r" % edge)
        self.data.setdefault (edge.u, {})[edge.v] = 1

    def onRemove (self, edge):
        del self.data[edge.u][edge.v]
        if not self.data[edge.u]:
            del self.data[edge.u]

class VertexMap (GraphRepresentation):
    """A graph representation that maps each vertex to a hash of all
       existing edges that use that vertex. This is an efficient way to
       iterate over all vertices in the graph, or to receive events
       when a vertex is first seen or entirely removed.
       """

    def __init__ (self, graph):
        GraphRepresentation.__init__ (self, graph)
        self.data = {}

        # 'added' is called with a vertex the first time
        # it's seen, 'removed' is called with a vertex after
        # the last edge using it is removed.
        Observable.attachEvents (self, 'added', 'removed')

    def __iter__ (self):
        """Iterate over all vertices in this graph"""
        return self.data.iterkeys ()

    def query (self, v):
        """Iterate over all edges containing v"""
        try:
            return self.data[v]
        except KeyError:
            return []

    def onAdd (self, edge):
        # if we have a self-loop, we only want to add this edge once, since
        # we'll get a dupe if we just add it the normal way
        vertices = None
        if edge.u is edge.v:
            vertices = [edge.u]
        else:
            vertices = [edge.u, edge.v]

        for vertex in vertices:
            if vertex not in self.data:
                self.data[vertex] = [edge]
                self.added (vertex)
            else:
                self.data[vertex].append (edge)

    def onRemove (self, edge):
        # similar check for self-loops
        vertices = None
        if edge.u is edge.v:
            vertices = [edge.u]
        else:
            vertices = [edge.u, edge.v]

        for vertex in vertices:
            self.data[vertex].remove (edge)
            if not self.data[vertex]:
                del self.data[vertex]
                self.removed (vertex)

class EdgeList (GraphRepresentation):
    """A very simple graph representation that keeps a map between
       (u,v) pairs and edge objects.
       """

    def __init__ (self, graph):
        GraphRepresentation.__init__ (self, graph)
        self.data = {}

    def __iter__ (self):
        """Iterate over all edges in this graph"""
        return self.data.itervalues ()

    def query (self, u, v):
        return self.data[hash ((u, v))]

    def onAdd (self, edge):
        self.data[hash((edge.u, edge.v))] = edge

    def onRemove (self, edge):
        pass


class CNode (object):
    """A class for representing a combination of nodes from multiple graphs.
       This primarily exists to provide a hashable object for the dictionary of
       back pointers used by ExtraAlgorithms.Heuristic.
       """
    def __init__(self, data):
        self.data = data

    def iteritems(self):
        return self.data.iteritems()

    def iterkeys(self):
        return self.data.iterkeys()

    def get(self, k, x=None):
        return self.data.get(k, x)

# vim: ts=4:sw=4:et
