#!/usr/bin/env python
"""
Work-in-progress memory profiler/visualizer using the gc
module and graphviz.

Copyright (C) 2004 Micah Dowty <micah@navi.cx>
"""

import gc


class MemProfiler:
    """This class can create a graph relating all python
       objects and the references between them, and process
       this graph in various ways.
       """
    def collect(self):
        """Collect information about all currently allocated python objects"""
        # Make sure any previous references are dead
        self.objects = None
        gc.collect()

        # Make a map from ID to all objects represented
        # by this graph. This lets us access all objects without
        # creating too many extra references to them. Since
        # we create the self.objects dict after get_objects,
        # it won't be included in self.references below.
        objList = gc.get_objects()
        self.objects = {}
        for object in objList:
            self.objects[id(object)] = object

        # Collect a dict mapping each object ID to the IDs it refers to
        self.references = {}
        for object in self.objects.itervalues():
            refs = []
            for ref in gc.get_referents(object):
                if id(ref) in self.objects:
                    refs.append(id(ref))
            self.references[id(object)] = refs

        # Guesstimate the size of each object by looking at the differences
        # in IDs, which in CPython at least are memory addresses.
        sorted = self.objects.keys()
        sorted.sort()
        self.sizeEstimates = {}
        for i in xrange(len(sorted)-1):
            s = sorted[i+1] - sorted[i]
            # If the difference is more than 5MB, assume there's
            # a discontinuity in the address space and ignore it.
            if s < 5 * 1024 * 1024:
                self.sizeEstimates[sorted[i]] = s

    def estimateTotalRam(self):
        """Return the estimated total memory usage of the python object heap"""
        return sum(self.sizeEstimates.itervalues())

    def getRefs(self, obj):
        """Generate a list of objects that 'obj' refers to, using
           the data captured by collect()
           """
        for ref in prof.references[id(obj)]:
            yield prof.objects[ref]

    def getEdges(self):
        """Iterate over all edges in the python object digraph, returning
           (referrer, referent) tuples.
           """
        for referrer, referentList in self.references.iteritems():
            for referent in referentList:
                yield (prof.objects[referrer], prof.objects[referent])

    def _markReachable(self, obj, memo):
        """Recursively mark all object IDs reachable from the given object
           ID, in the given memo dictionary.
           """
        if obj not in memo:
            memo[obj] = True
            for referent in self.references[obj]:
                self._markReachable(referent, memo)

    def getReachable(self, obj):
        """Return a list of all object reachable from the given object"""
        memo = {}
        self._markReachable(id(obj), memo)
        for i in memo.iterkeys():
            yield self.objects[i]


class MemGrapher:
    """This class creates Graphviz-compatible graphs of python objects
       and their relationships, using data collected from a MemProfiler.
       """
    def __init__(self, profiler, graphAttrs={}):
        self.graphAttrs = {
            'packmode': 'graph',
            'center': True,
            'Damping': 0.9,
            }
        self.graphAttrs.update(graphAttrs)
        self.profiler = profiler

    def quote(self, t):
        """Quote the given text for inclusion in a dot file"""
        if type(t) in (int, float, bool):
            return str(t)
        else:
            t = str(t)
            for badChar in '\\":<>|':
                t = t.replace(badChar, '\\' + badChar)
            t = t.replace("\n", "\\n")
            return '"' + t + '"'

    def getObjectAttrs(self, obj):
        """Get graph attributes describing a python object"""
        d = {'color': '#0000FF'}

        if type(obj) in (dict, list, tuple):
            d['color'] = '#FF0000'
            d['label'] = type(obj).__name__
        else:
            d['label'] = repr(obj)

        if id(obj) in self.profiler.sizeEstimates:
            d['label'] += "\n%d bytes" % (self.profiler.sizeEstimates[id(obj)])

        return d

    def dictToAttrs(self, d):
        """Convert a dictionary to a dot attribute string"""
        if d:
            return "[%s]" % ",".join(['%s=%s' % (key, self.quote(value))
                                      for key, value in d.iteritems()])
        else:
            return ""

    def writeGraph(self, stream, root):
        """Write a graphviz-compatible graph of all python objects reachable
           from the given object.
           """
        stream.write("graph G {\n")
        stream.write(''.join(['\t%s=%s;\n' % (key, self.quote(value))
                              for key, value in self.graphAttrs.iteritems()]))

        objects = self.profiler.getReachable(root)
        ids = []
        for object in objects:
            ids.append(id(object))
            attrs = self.getObjectAttrs(object)
            stream.write("\t%s %s;\n" % (self.quote(id(object)),
                                         self.dictToAttrs(attrs)))

        for objA, objB in self.profiler.getEdges():
            if id(objA) in ids and id(objB) in ids:
                attrs = {'weight': 5.0,
                         'len': 5.0}
                stream.write("\t%s -- %s %s;\n" % (self.quote(id(objA)),
                                                   self.quote(id(objB)),
                                                   self.dictToAttrs(attrs)))
        stream.write("}\n")


if __name__ == "__main__":

    prof = MemProfiler()

    from Ft.Xml import Domlette
    doc = Domlette.NonvalidatingReader.parseString("<doc><foo><a/><b/></foo></doc>", "foo://bar")
    root = [[1, 2, 3], "apple", doc]

    prof.collect()

    print "Total RAM: %d" % prof.estimateTotalRam()
    MemGrapher(prof).writeGraph(open("graph.dot", "w"), root)

### The End ###
