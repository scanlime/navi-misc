#!/usr/bin/env python
#
# Generates dependency graphs from Python packages.
# Fairly useless in its current state, but possibly entertaining.
# This uses the mf module from http://www.mcmillan-inc.com/mf.html,
# and generates Graphviz .dot files on stdout.
#
# For example, to generate a graph of all modules reachable from
# the 'cia_server' module but residing in the 'LibCIA' package,
# you might do something like:
#
#   PYTHONPATH=~/navi-misc/cia python makedot.py cia_server LibCIA | dot -Tsvg > cia.svg
#

import mf, sys

root = sys.argv[1]
if len(sys.argv) > 2:
    requiredPrefix = sys.argv[2]
else:
    requiredPrefix = ''

a = mf.ImportTracker()
a.analyze_r(root)

print 'digraph {'
print '\tranksep = 10;'
for moduleName, module in a.modules.iteritems():
    if (moduleName.startswith(requiredPrefix) or moduleName == root) and module:
        for i in module.imports:
            if i[0].startswith(requiredPrefix):
                print '\t"%s" -> "%s";' % (moduleName, i[0])
print '}'
