""" LibCIA.Debug

Remote interfaces for debugging and dynamic reloading while
CIA is running.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

from twisted.python import rebuild, log
import RPC
import gc
import sys, traceback


class DebugInterface(RPC.Interface):
    """An XML-RPC interface for remote debugging and dynamic reloading of CIA."""
    def __init__(self):
        RPC.Interface.__init__(self)
        self.putSubHandler('gc', GcInterface())

    def protected_rebuild(self, packageName):
        """Use twisted.python.rebuild to reload the given package or module
           and all loaded packages or modules within it.
           """
        log.msg("Starting a rebuild at the package %r" % packageName)
        # The non-empty fromlist tells __import__ we want the module referred
        # to by the given path, not just its top-level module.
        package = __import__(packageName, globals(), locals(), [''])
        rebuildPackage(package)

    def protected_eval(self, code):
        """Evaluate arbitrary code in the context of this module.
           Returns the repr() of the result.
           NOTE: This is an extremely powerful function, so a 'debug' or 'debug.eval'
                 key should be treated with equivalent respect to a 'universe' key.
           """
        log.msg("Executing code in debug.eval: %r" % code)
        return repr(eval(code))


def typeInstances(t):
    """Use gc mojo to return a list of all instances of the given type
       (as returned by getTypeName(). This is the implementation of
       debug.gc.typeInstances, and is provided as a module-level function
       so it can be used in debug.eval
       """
    return [object for object in gc.get_objects() if getTypeName(object) == t]


def getSingleton(t):
    """Returns the singleton with the given type name, raising an exception
       if exactly one object of that type doesn't exist.
       Meant to be used from inside debug.eval().
       """
    insts = typeInstances(t)
    if len(insts) != 1:
        raise Exception("Found %d instances of %r, expected it to be a singleton" % (len(insts), t))
    return insts[0]


def catchFault(message="Exception occurred"):
    """Put this in the 'except' section of a try block to convert the exception
       that just occurred to an XMLRPC Fault and log it.
       Not related to Messages, but this is as good a place as any for it right now...
       """
    e = sys.exc_info()[1]
    if isinstance(e, xmlrpc.Fault):
        raise
    else:
        log.msg(message + "\n" + "".join(traceback.format_exception(*sys.exc_info())))
        raise xmlrpc.Fault(e.__class__.__name__, str(e))


def rebuildPackage(package):
    """Recursively rebuild all loaded modules in the given package"""
    rebuild.rebuild(package)
    # If this is really a package instead of a module, look for children
    try:
        f = package.__file__
    except AttributeError:
        return
    if package.__file__.find("__init__") >= 0:
        for item in package.__dict__.itervalues():
            # Is it a module?
            if type(item) == type(package):
                rebuildPackage(item)


def getTypeName(obj):
    """Try as hard and as generically as we can to get a useful type/class name"""
    try:
        t = obj.__class__
    except:
        t = type(obj)
    try:
        t = t.__name__
    except:
        pass
    t = str(t)
    return t


class GcInterface(RPC.Interface):
    """Memory debugging and profiling via python's garbage collector interface"""
    def protected_garbageInfo(self):
        """Return a string representation of the items in gc.garbage"""
        return map(repr, gc.garbage)

    def protected_objectsInfo(self):
        """Return a string representation of the items in gc.get_objects().
           This can take a while and be very big!
           """
        return map(repr, gc.get_objects())

    def protected_typeInstances(self, t):
        """Return all objects of any one type, using the same type names as typeProfile"""
        return typeInstances(t)

    def protected_collect(self):
        """Force the garbage collector to run"""
        log.msg("Forcing garbage collection")
        gc.collect()

    def protected_typeProfile(self):
        """Return a chart showing the most frequently occurring types in memory"""
        # Create a mapping from type name to frequency,
        # and a mapping from type name to example instances
        objects = gc.get_objects()
        typeFreq = {}
        typeInstances = {}
        for object in objects:
            t = getTypeName(object)
            # Increment the frequency
            typeFreq[t] = typeFreq.setdefault(t, 0) + 1

            # Add to our list of example instances if it isn't already too big
            inst = typeInstances.setdefault(t, [])
            if len(inst) < 100:
                inst.append(repr(object))

        # Sort by frequency
        keys = typeFreq.keys()
        keys.sort(lambda a,b: cmp(typeFreq[a],typeFreq[b]))

        # And return a nice table
        lines = []
        for key in keys:
            contents = ", ".join(typeInstances[key]).replace("\n", "\\n")
            if len(contents) > 100:
                contents = contents[:100] + "..."
            lines.append("%45s :  %-10d %s" % (key, typeFreq[key], contents))
        return "\n".join(lines)

### The End ###
