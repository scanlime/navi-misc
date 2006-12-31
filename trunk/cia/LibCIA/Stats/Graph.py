""" LibCIA.Stats.Graph

Maintains an undirected graph of associations between stats targets.
These associations are reinforced when one message is delivered to
multiple targets.

Using this graph stored in the database, various methods of visualization
have been implemented here.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2006 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from LibCIA import Database, Cache
from twisted.python import log
from twisted.internet import reactor, defer, protocol, error
from cStringIO import StringIO
import time, math


class Relation:
    """Represents a relationship between two stats targets- an edge on the stats graph.
       These relationships can be queried or reinforced. You can ask a stats target for
       a list of relations containing it.
       """
    def __init__(self, a, b):
        # Our targets must be sorted by path, to make this edge unique
        if a.path > b.path:
            a, b = b, a
        self.a = a
        self.b = b

    def reinforce(self):
        """Increment this relation's strength and set its freshness to the current time"""
        return Database.pool.runInteraction(self._reinforce)

    def _reinforce(self, cursor):
        """Database interaction implementing reinforce()"""
        # First touch the edge to make sure it exists. We have to do this
        # inside two autoCreateTargetFor levels, to create both stats targets
        # if they don't yet exist.
        self.a._autoCreateTargetFor(cursor, self.b._autoCreateTargetFor,
                                    cursor, cursor.execute,
                                    "INSERT IGNORE INTO stats_relations "
                                    "(target_a_path, target_b_path) VALUES(%s, %s)" %
                                    (Database.quote(self.a.path, 'varchar'),
                                     Database.quote(self.b.path, 'varchar')))

        cursor.execute("UPDATE stats_relations "
                       "SET strength = strength + 1, freshness = %s "
                       "WHERE target_a_path = %s AND target_b_path = %s" %
                       (Database.quote(int(time.time()), 'bigint'),
                        Database.quote(self.a.path, 'varchar'),
                        Database.quote(self.b.path, 'varchar')))


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

    def __repr__(self):
        return "<PrefixSelector %r %r>" % (self.prefix, self.style)

    def getSQL(self, varName):
        return "%s LIKE '%s%%'" % (varName, self.prefix)

    def __contains__(self, path):
        return path.startswith(self.prefix)

    def getAttributes(self, path):
        d = dict(self.style)
        d['label'] = path[len(self.prefix):]
        return d


class RelationGrapher:
    """Creates graphs showing relationships between stats targets.
       One or more Selectors are used to find interesting nodes and
       describe how they should be drawn. The output is given as a
       'dot' file that can be passed to the 'neato' utility in the
       AT&T Graphviz package.
       """
    def __init__(self, *selectors):
        self.selectors = selectors

    def __repr__(self):
        return "<RelationGrapher %r>" % (self.selectors,)

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

    def render(self, f):
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
            'Damping': 0.9,
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
        result.callback(None)


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

    def errReceived(self, data):
        log.err(data)

    def processEnded(self, reason):
        if isinstance(reason.value, error.ProcessDone):
            self.resultDeferred.callback(None)
        else:
            self.resultDeferred.errback(reason)


class GraphLayout:
    """Given an object with a render(f) function that writes 'dot' source to
       a file-like object, this runs Graphviz's 'neato' utility to lay out the
       graph and produce a vector image.
       """
    def __init__(self, dataSource, format="svg", bin="neato"):
        self.dataSource = dataSource
        self.bin = bin
        self.args = [bin, "-T%s" % format]

    def __repr__(self):
        return "<GraphLayout for %r using %r>" % (self.dataSource, self.args)

    def render(self, f):
        """Lay out a graph using input from our data source, writing
           the completed graph to the given file-like object.
           """
        result = defer.Deferred()
        p = StreamProcessProtocol(self.dataSource.render, f, result)
        reactor.spawnProcess(p, self.bin, self.args, env=None)
        return result


class HeaderChoppingProtocol(StreamProcessProtocol):
    """This is a protocol that works around sodipodi's tencency to spew some
       debuggative cruft to stdout rather than to stderr. Ignores all writes
       we get before the first one >= 1 kB.
       """
    inhibit = True

    def outReceived(self, data):
        if self.inhibit:
            if len(data) < 1024:
                return
            self.inhibit = False
        self.stdoutStream.write(data)


class SvgRasterizer:
    """Uses sodipodi to rasterize SVG vector images into PNG bitmaps.
       dataSource is any object with a render(f) function that can
       write an SVG to a file-like object and might return a deferred.
       This provides a render() function that writes the resulting PNG
       image to a file-like object.
       """
    def __init__(self, dataSource,
                 width      = None,
                 height     = None,
                 dpi        = None,
                 size       = None,
                 background = None,
                 bin        = "sodipodi"):
        self.dataSource = dataSource
        self.bin = bin
        self.args = [bin, "-f", "-", "-e", "/dev/stdout"]
        if dpi:
            self.args.extend(["-d", str(dpi)])
        if background:
            self.args.extend(["-b", str(background)])
        if size:
            self.args.extend(["-w", str(size[0]), "-h", str(size[1])])
        if width:
            self.args.extend(["-w", str(width)])
        if height:
            self.args.extend(["-h", str(height)])

    def __repr__(self):
        return "<SvgRasterizer for %r using %r>" % (self.dataSource, self.args)

    def render(self, f):
        result = defer.Deferred()
        p = HeaderChoppingProtocol(self.dataSource.render, f, result)
        reactor.spawnProcess(p, self.bin, self.args, env=None)
        return result


class RenderCache(Cache.AbstractStringCache):
    """A cache that transparently wraps any object with a render(f) function
       that writes its results to a file-like object and has a repr() that
       uniquely represents its state.

       Cached results expire after 4 hours by default.
       """
    def __init__(self, obj, lifespan=60*60*4):
        self.obj = obj
        self.lifespan = lifespan

    def __repr__(self):
        return "<RenderCache of %r>" % self.obj

    def render(self, f):
        """A render() function that should appear to work just like the
           original object's render() function.
           """
        result = defer.Deferred()
        self.get(self.obj).addCallback(
            self._render, f, result).addErrback(result.errback)
        return result

    def _render(self, value, f, result):
        f.write(value)
        result.callback(None)

    def miss(self, obj):
        """The cache miss function, as required by AbstractStringCache.
           The cache is keyed on our renderable object, and this eventually
           returns a string.
           """
        f = StringIO()
        result = defer.Deferred()
        defer.maybeDeferred(obj.render, f).addCallback(
            self._finishedRendering, f, result).addErrback(result.errback)
        return result

    def _finishedRendering(self, d, f, result):
        result.callback(f.getvalue())

### The End ###
