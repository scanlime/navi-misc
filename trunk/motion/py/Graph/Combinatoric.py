"""Data structures for building graphs by combining multiple graphs.

A module for manipulating combinatoric graphs. It's easier on memory to keep
the individual graphs separate and pretend that there is a giant graph composed
of all possible combinations of the nodes in the individual graphs. These
classes allow you to manipulate this pretend graph as though it actually
existed in memory.
"""

# Copyright (C) 2006 W. Evan Sheehan
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

import Data, Dot, string

__all__ = ["Node", "AdjacenceyList", "VertexMap", "EdgeList"]

class Node (Dot.Node):
    """Represent a node built from nodes in multiple graphs.

    These objects can be treated similarly to dictionaries: [] can be used to
    read data from individual graphs, but they cannot be written to.
    """

    def __init__ (self, data, **kwargs):
        """Initialize the node.

        Raise a TypeError if ``data`` is not a dictionary.
        """
        Dot.Node.__init__ (self, **kwargs)

        if type (data) is Node:
            return data

        if not isinstance (data, dict):
            raise TypeError ("Combinatoric.Node requires dictionary data")

        self.data = data.copy ()
        self.dotAttrs['label'] = string.join (map (repr, self.data.values ()), ',')

    def iterkeys (self):
        return self.data.iterkeys ()

    def __getitem__ (self, key):
        return self.data[key]

    def __repr__ (self):
        return "%s(%r)" % (self.__class__, self.data)

    def __hash__ (self):
        return hash (repr (self.data))

    def __eq__ (self, other):
        return (self.data == other.data)

    def __ne__ (self, other):
        return (self.data != other.data)


class CombinatoricRepresentation (Data.GraphRepresentation):
    """Base class for all combinatoric graph representations.

    This class handles some of the error checking required by all the
    combinatoric representations.
    """

    def __init__ (self, graph):
        Data.GraphRepresentation.__init__ (self, graph)
        self.data = []

    def onAdd (self, data):
        try:
            if len (data) != 2:
                raise ValueError ("Combinatoric graph representations expect \
                        list or tuple of the form (name, graph), got: %s" %
                        (data))
        except TypeError:
            raise TypeError ("Combinatoric graph representations expect a \
                    list or tuple, got: %s" % (data))


class AdjacencyList (CombinatoricRepresentation):
    """Combinatoric adjacency list."""

    def __iter__ (self):
        """Iterate over all the edges in the graph."""
        def combine (graphs):
            """Recursive function for combining the edges in the adjacency
               lists of all graphs.
               """
            name, adj = graphs[0]
            for edge in adj:
                if len(graphs) == 1:
                    u = {name: edge.u}
                    v = {name: edge.v}
                    yield (u, v)
                    continue

                for u, v in combine(graphs[1:]):
                    u[name] = edge.u
                    v[name] = edge.v
                    yield (u, v)

        for u, v in combine (self.data):
            yield self.graph.edgeClass (Node (u), Node (v))

    def __contains__ (self, edge):
        """Returns true if edge exists in the adjacency list."""
        names = []
        for (name, adj) in self.data:
            # Try to create an edge for this just this graph. If we get a
            # KeyError here, 'edge' doesn't have data for one of our graphs so
            # return False.
            try:
                e = self.graph.edgeClass (edge.u[name], edge.v[name])
            except KeyError:
                return False

            # If this graph doesn't contain 'e', we don't contain 'edge'
            if e not in adj:
                return False

            names.append (name)

        # If 'edge' has data for a graph we don't have, we don't contain 'edge'
        if names != edge.u.keys ():
            return False
                   
        # If nothing has failed, assume we contain 'edge'
        return True

    def onAdd (self, data):
        CombinatoricRepresentation.onAdd (self, data)
       
        name, graph = data
        adj = graph.representations[Data.AdjacencyList]

        if (name, adj) in self.data:
            raise ValueError ("Duplicate graph %s" % data)
        self.data.append ((name, adj))

    def onRemove (self, data):
        name, graph = data
        adj = graph.representations[Data.AdjacencyList]
        self.data.remove ((name, adj))

    def iterU (self):
        """Iterate over all the 'u' vertices in the edges of the adjacency
           list.
           """
        def combine (graphs):
            """Recursive function to combine the 'u' vertices into dictionaries
               mapping the graph name to the vertex.
               """
            name, adj = graphs[0]
            for u in adj.iterU ():
                if len(graphs) == 1:
                    yield {name: u}
                    continue
                
                for dictU in combine(graphs[1:]):
                    dictU[name] = u
                    yield dictU

        for u in combine (self.data):
            yield Node (u)

    def query (self, u):
        """Iterate over all edges going out from vertex 'u'."""
        def combine (graphs):
            """Recursively combine all the edges from the graphs going out from
               'u'.
               """
            name, adj = graphs[0]
            for edge in adj.query(u[name]):
                if len(graphs) == 1:
                    yield {name: edge.v}
                    continue

                for node in combine (graphs[1:]):
                    node[name] = edge.v
                    yield node

        for v in combine (self.data):
            yield self.graph.edgeClass (u, Node (v))


class VertexMap (CombinatoricRepresentation):
    """Maps each vertex to a hash of all the edges connected to that vertex."""

    def __init__ (self, graph):
        self.graph = graph
        graph.add.observe (self.onAdd)
        graph.remove.observe (self.onRemove)
        graph.representations[Data.VertexMap] = self
        self.data = []

    def __iter__ (self):
        """Iterate over all the vertices in the graph."""
        def combine (graphs):
            """Recursively combine the individual vertices into the
               combinatoric nodes.
               """
            name, map = graphs[0]
            for vertex in map:
                if len(graphs) == 1:
                    yield {name: vertex}
                else: 
                    for v in combine (graphs[1:]):
                        v[name] = vertex
                        yield v

        for n in combine (self.data):
            yield Node (n)

    def onAdd (self, data):
        CombinatoricRepresentation.onAdd (self, data)

        name, graph = data
        map = graph.representations[Data.VertexMap]

        if (name, map) in self.data:
            raise ValueError ("Duplicate graph %s" % (data))

        self.data.append ((name, map))

    def onRemove (self, data):
        name, graph = data
        map = graph.representations[Data.VertexMap]
        self.data.remove ((name, map))

    def query (self, node):
        """Iterate over the edges containing the vertex 'u'."""
        def combineU (graphs):
            """Yield all edges coming into this node except self-loops."""
            name, map = graphs[0]
            for edge in map.query (node[name]):
                if edge.v == node[name] and edge.u != edge.v:
                    if len (graphs) == 1:
                        yield {name: edge.u}
                        continue

                    for u in combineU (graphs[1:]):
                        u[name] = edge.u
                        yield u

        def combineV (graphs):
            """Yield all edges leaving this node including self-loops."""
            name, map = graphs[0]
            for edge in map.query (node[name]):
                if edge.u == node[name]:
                    if len (graphs) == 1:
                        yield {name: edge.v}
                        continue

                    for v in combineV (graphs[1:]):
                        v[name] = edge.v
                        yield v

        for u in combineU (self.data):
            yield self.graph.edgeClass (Node (u), node)

        for v in combineV (self.data):
            yield self.graph.edgeClass (node, Node (v))


class EdgeList (CombinatoricRepresentation):
    """A simple graph representation that maps (u,v) pairs to edge objects."""

    def __iter__ (self):
        """Iterate over the edges of the graph."""
        def combine (graphs):
            name, edges = graphs[0]

            for edge in edges:
                if len (graphs) == 1:
                    u = {name: edge.u}
                    v = {name: edge.v}
                    yield (u, v)
                    continue

                for u, v in combine (graphs[1:]):
                    u[name] = edge.u
                    v[name] = edge.v
                    yield (u, v)

        for u, v in combine (self.data):
            yield self.graph.edgeClass (Node (u), Node (v))

    def onAdd (self, data):
        CombinatoricRepresentation.onAdd (self, data)

        name, graph = data
        edges = graph.representations[Data.EdgeList]

        if (name, edges) in self.data:
            raise ValueError ("Duplicate graph %s", data)
        self.data.append ((name, edges))

    def onRemove (self, data):
        name, graph = data
        edges = data.representations[Data.EdgeList]
        self.data.remove ((name, edges))

    def query (self, u, v):
        """Return the edge from 'u' to 'v'."""
        combU = {}
        combV = {}
        for name in u.iterkeys ():
            edges = dict (self.data)[name]
            edge = edges.query (u[name], v[name])
            combU[name] = edge.u
            combV[name] = edge.v

        return self.graph.edgeClass (Node (combU), Node (combV))


class BayesAdjacency (AdjacencyList):
    """An adjacency list filtered by a Bayes net.

    This representation uses a Bayes net to filter out unlikely combinations of
    nodes to reduce the number of successors for a combinatoric node.
    """

    def __init__ (self, graph, bayes, asf=None, eps=0.3**29):
        AdjacencyList.__init__ (self, graph)
        self.bayes = bayes
        self.epsilon = eps
        if asf:
            self.setOrder (asf)

    def setOrder (self, asf):
        """Set the build order for nodes from the hierarchy in an ASF file.

        Arguments:
            - ``asf``   An ASF object
        """
        self.order = ["root"]
        self.parents = {}
        self.bones = asf.bones

        for group in asf.hierarchy:
            if len (group) > 1:
                for child in group[1:]:
                    self.parents[child] = group[0]
                    if group[0] in self.order and child not in self.order:
                        self.order.append (child)

    def query (self, u):
        """Iterate over the edges going out from vertex ``u`` that are not
        filtered out by the Bayes net.
        """
        def filter (order, current={}, prob=1.0):
            """Generator function used for creating filtered successors of
            ``u``.
            """
            # If there are no bones left in ``order``, yield the current node
            if len (order) == 0:
                yield current.copy ()
                return

            bone = order[0]

            # If the bone has no DOF, skip it
            if not self.bones[bone].dof:
                for child in filter (order[1:], current, prob):
                    yield child

            # Get parent information
            if bone in self.parents:
                parent = self.parents[bone]
                pbone = current.get(parent)

            # Iterate over all the nodes connected to this one
            for node in self.adj[bone].query (u[bone]):
                current[bone] = node

                # If the bone has no parent or the parent has no mocap data
                # (i.e. the parent has no DOF), skip it
                if (bone not in self.parents or not pbone):
                    for child in filter (order[1:], current, prob):
                        yield child
                # If the bone has a parent and the parent has a position in
                # ``current``, apply the Bayes net to this bone to filter
                # unlikely positions
                else:
                    net = self.bayes[bone]
                    # Generate the index used by the Bayes net
                    spot = (tuple (pbone.mins), tuple (node.mins))

                    # If the Bayes net has an entry for this pair of positions,
                    # calculate the new probabilities. If they're high enough,
                    # recurse further down the body. If they're too low, ignore
                    # them.
                    if spot in net:
                        p = net[spot]
                        newProb = prob * p
                        # FIXME: What is this 0.1 and why is it hard-coded?
                        if newProb > self.eps and prob > 0.1:
                            for child in filter (order[1:], current, newProb):
                                yield child

                    # FIXME: what if ``spot`` isn't in the Bayes net?
       
        # Yield edges with a high enough probability to warrant checking.
        for v in filter (self.order):
            yield self.graph.edgeClass (Node (u), Node (v))


# vim: ts=4:sw=4:et
