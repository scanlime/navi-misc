#!/usr/bin/env python
''' trough
    Simple command-line interface to the trough core.
    It really is.  Believe me.  Yep. Golly.  I wonder why there's a docstring
    here.
    '''

# find a command line interface
try:    # python 2.3 case
    import optparse
except:    # revert to python 2.2 case
    import optik as optparse
from Trough import version

description = '''Select a random group of files from a cached directory tree.  Prints a randomly selected list of paths to stdout when run without arguments.'''


# Set up and run command line parser
#
parser = optparse.OptionParser (version=version, description=description)
parser.add_option('-a','--add',dest='add',action="store_true",
        help="add path(s) in argument(s) to the stored directory trees")
parser.add_option('-c','--config',dest='confdir',type="string",
        help="use alternate config directory (default is ~/.trough)")
parser.add_option('-d','--delete',dest='delete',action="store_true",
        help="remove path(s) in argument(s) to the stored directory trees")
parser.add_option('-i','--info',dest='info',action="store_true",
        help="retrieve stored tree information")
parser.add_option('-l','--list',dest='list',action="store_true",
        help="list roots stored tree root(s)")
parser.add_option('-r','--refresh',dest='refresh',action="store_true",
        help="refresh stored trees in database")
#parser.add_option('-p','--playlist',dest="playlist",type="string",
#        help="path to playlist file")
#parser.add_option('-v','--verbose',dest="verbose",action="store_true",
#        help="give verbose output")
(options,arguments) = parser.parse_args()

# Deal with command line options & arguments
#
import Trough.Config
import Trough.GroupList
import sys
import string

if options.confdir:
    conf = Trough.Config.Config(options.confdir)
else:
    conf = Trough.Config.Config()
conf.readTrees()

if options.list:
    for x in conf.getTrees():
	print x.getPath()
    sys.exit(0)

if options.refresh:
    for x in conf.getTrees():
	print 'refreshing '+x.getPath()+''
	x.read()
    sys.exit(0)

if options.add:
    for arg in arguments:
	print 'adding '+arg
    	conf.addTree(arg)
    sys.exit(0)

if options.delete:
    for arg in arguments:
	print 'adding '+arg
	conf.removeTree(arg)
    sys.exit(0)

conf.readProperties()
conf.applyProperties()
grouplist = Trough.GroupList.GroupList()

for x in conf.getTrees():
    grouplist.add(x)

if options.info:
    for arg in arguments:
	g = Trough.GroupList.GroupList()
	try:
	    g.add(conf.findTreeContaining(arg).find(arg))
	except:
	    print '** warning: '+arg+' not in tree, ignoring'
	    continue
	print string.rjust(arg+':',50), g.getCount()
    print string.rjust('(Total distinct file groups, in all trees:',50), grouplist.getCount(),')'
    sys.exit(0)

if grouplist.getCount() == 0:
    print '** warning: database empty, try `trough -a <path>` to create a tree from path'
    sys.exit(0)

for x in grouplist.random():
    print x.getPath()
