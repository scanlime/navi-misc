#!/usr/bin/env python
import sys, os
sys.path[0] = os.path.join(sys.path[0], '..')

from LibCIA import Database
from twisted.internet import reactor, defer, protocol, error
import time, math


class Selector:
    """A selector provides SQL and Python syntax for picking some
       subset of stats targets, and describes how those stats targets
       should be rendered.
       This is an abstract base class.
       """
    def getSQL(self, varName):
        """Returns an SQL expression that is true if the stats target
           path in 'varName' is part of this selector's subset.
           """
        pass

    def __contains__(self, path):
        """Returns True if the given path is part of this selector's subset."""
        pass

    def getAttributes(self, path):
        """Given a path contained by this selector's subset, return a
           dictionary of node attributes to pass to Graphviz.
           """
        pass


class PrefixSelector(Selector):
    """A selector that returns all nodes beginning with a particular prefix,
       drawing them without the prefix but with a distinctive style.
       """
    def __init__(self, prefix, **style):
        self.prefix = prefix
        self.style = style

    def getSQL(self, varName):
        return "%s LIKE '%s%%'" % (varName, self.prefix)

    def __contains__(self, path):
        return path.startswith(self.prefix)

    def getAttributes(self, path):
        d = dict(self.style)
        d['label'] = path[len(self.prefix):]
        return d


class RelationGrapher:
    """Creates graphs showing relationships between stats targets,
       using the Graphviz package. One or more Selectors are used
       to find interesting nodes and describe how they should be drawn.
       """
    def __init__(self, *selectors):
        self.selectors = selectors

    def getAllSelectorsSQL(self, varName):
        """Return an SQL expression that is true if any selector is
           interested in the stats target in the given variable name.
           """
        return "(%s)" % " OR ".join(["(%s)" % sel.getSQL(varName) for sel in self.selectors])

    def getQuery(self):
        """Returns an SQL query that returns all interesting edges in the
           relation graph, according to our list of selectors.
           """
        return ("SELECT target_a_path, target_b_path, strength, freshness "
                "FROM stats_relations WHERE %s AND %s" % (
            self.getAllSelectorsSQL("target_a_path"),
            self.getAllSelectorsSQL("target_b_path")))

    def generateDot(self, f):
        """Generate the 'dot' code for our graph, writing it to the given
           file-like object. Returns a Deferred signaling completion.
           """
        result = defer.Deferred()
        Database.pool.runQuery(self.getQuery()).addCallback(
            self._generateDot, f, result).addErrback(result.errback)
        return result

    def quote(self, t):
        """Quote the given text for inclusion in a dot file"""
        if type(t) in (int, float, bool):
            return str(t)
        else:
            t = str(t)
            for badChar in '\\":<>|':
                t = t.replace(badChar, '\\' + badChar)
            return '"' + t + '"'

    def dictToAttrs(self, d):
        """Convert a dictionary to a dot attribute string"""
        if d:
            return "[%s]" % ",".join(['%s=%s' % (key, self.quote(value))
                                      for key, value in d.iteritems()])
        else:
            return ""

    def _generateDot(self, rows, f, result):
        """Finish generateDot after receiving the SQL query results"""
        graphAttrs = {
            'packmode': 'graph',
            'center': True,
            'Damping': 0.999,
            }

        f.write("graph G {\n")
        f.write(''.join(['\t%s=%s;\n' % (key, self.quote(value))
                         for key, value in graphAttrs.iteritems()]))

        # Make a unique list of all nodes in this graph
        nodes = {}
        for row in rows:
            nodes[row[0]] = None
            nodes[row[1]] = None

        # Keep track of node parents, so later we can use them for special cases
        # and for determining how many children a particular node has.
        parents = {}
        for node in nodes:
            segments = node.split('/')
            # Make sure all ancestors of this node exist in the parents map
            for i in xrange(len(segments)):
                parent = '/'.join(segments[:i])
                if parent:
                    parents.setdefault(parent, [])
            # Add the node to its immediate parent
            if len(segments) > 1:
                parents[parent].append(node)

        # Find a selector for each node, and write out their attributes
        for node in nodes.keys():
            attributes = {}
            for selector in self.selectors:
                if node in selector:
                    nodes[node] = selector
                    attributes = selector.getAttributes(node)
                    break
            f.write('\t%s %s;\n' % (self.quote(node), self.dictToAttrs(attributes)))

        # Find the maximum strength and minimum freshness in our dataset
        now = time.time()
        maxStrength = 0
        minFreshness = now
        for row in rows:
            a, b, strength, freshness = row
            if strength > maxStrength:
                maxStrength = strength
            if freshness < minFreshness:
                minFreshness = freshness

        # Write edges
        for row in rows:
            a, b, strength, freshness = row

            # Exclude edges between nodes of dissimilar selectors when a parent node
            # is involved. The rationale for this is that this edge doesn't give any
            # real information, since the non-parent node will also have an edge
            # connecting to a child of the parent node. For example, this would
            # prevent author/bob from being linked to both project/kde/libwidgets
            # and project/kde. The link between author/bob and project/kde would
            # just clutter up the graph.
            if (parents.has_key(a) or parents.has_key(b)) and nodes[a] != nodes[b]:
                continue

            # Scale the strength and freshness logarithmically to be values between 0 and 1
            unitStrength = math.log(strength) / math.log(maxStrength)
            unitFreshness = 1 - (math.log(now - freshness) / math.log(now - minFreshness))

            # Determine the number of children this edge involves, for length scaling purposes
            children = len(parents.get(a,())) + len(parents.get(b,()))

            attributes = {
                # The length is semantically unimportant, but try to give the graph a good
                # layout. Increase the length proportionately with the number of children
                # our nodes have.
                'len': 5.0 + math.log(children + 1),
                'weight': 5.0,

                # Scale the line width according to the edge's strength
                'style': 'setlinewidth(%d)' % int(unitStrength * 10 + 1),
                }

            f.write('\t%s -- %s %s;\n' % (self.quote(a),
                                          self.quote(b),
                                          self.dictToAttrs(attributes)))

        f.write("}\n")
        print "Wrote dot source, %d nodes and %d edges" % (len(nodes), len(rows))
        result.callback(None)

    def render(self, f, format, bin="neato"):
        """Render a graph in the given format to the file-like object 'f'.
           Returns a deferred that indicates completion or error conditions.
           """
        result = defer.Deferred()
        p = StreamProcessProtocol(self.generateDot, f, result)
        reactor.spawnProcess(p, bin, [bin, "-T%s" % format], env=None)
        return result


class StreamProcessProtocol(protocol.ProcessProtocol):
    """Upon connection to the child process, stdinCallback is called with
       a file-like object representing the process's stdin. When the
       stdinCallback returns (possibly via a Deferred) the subprocess'
       stdin is closed.

       The process' stdout is directed to the provided file-like object
       stdoutStream. When the process finishes, a None is sent to
       resultDeferred. Errors are errback()'ed to resultDeferred.
       """
    def __init__(self, stdinCallback, stdoutStream, resultDeferred):
        self.stdinCallback = stdinCallback
        self.stdoutStream = stdoutStream
        self.resultDeferred = resultDeferred

    def connectionMade(self):
        defer.maybeDeferred(self.stdinCallback, self.transport).addCallback(
            self._finishedWriting).addErrback(self.resultDeferred.errback)

    def _finishedWriting(self, result):
        self.transport.closeStdin()

    def outReceived(self, data):
        self.stdoutStream.write(data)

    def processEnded(self, reason):
        if isinstance(reason.value, error.ProcessDone):
            self.resultDeferred.callback(None)
        else:
            self.resultDeferred.errback(reason)


if __name__ == '__main__':
    def done(result):
        print "All done"
        reactor.stop()
    def oops(result):
        print result
        reactor.stop()

    RelationGrapher(
        PrefixSelector('project/', color='#FF0000', shape='box'),
        PrefixSelector('author/', color='#0000FF'),
        ).render(open("output.svg", 'w'), "svg").addCallback(done).addErrback(oops)
    reactor.run()
