#!/usr/bin/env python
from LibCIA import Database
from twisted.internet import reactor, defer
import sys


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

    def _generateDot(self, rows, f, result):
        """Finish generateDot after receiving the SQL query results"""
        f.write("graph G {\n")

        # Make a unique list of all nodes in this graph
        nodes = {}
        for row in rows:
            nodes[row[0]] = 1
            nodes[row[1]] = 1

        # Write out attributes for each node
        for node in nodes:
            attributes = {}
            for selector in self.selectors:
                if node in selector:
                    attributes = selector.getAttributes(node)
                    break
            attrString = ",".join(['%s="%s"' % item for item in attributes.iteritems()])
            f.write('\t"%s" [%s];\n' % (node, attrString))

        # Write edges
        for row in rows:
            f.write('\t"%s" -- "%s";\n' % (row[0], row[1]))

        f.write("}\n")


if __name__ == '__main__':
    RelationGrapher(
        PrefixSelector('project/', color='#FF0000'),
        PrefixSelector('author/', color='#0000FF'),
        ).generateDot(sys.stdout)
    reactor.run()
