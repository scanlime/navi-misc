""" LibCIA.Interface

Remote interfaces for CIA's functionality that aren't specific to
any one other module.
"""
#
# CIA open source notification system
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from twisted.web import xmlrpc
from twisted.python.rebuild import rebuild
import sys, traceback
from twisted.python import log
import gc


def catchFault(message="Exception occurred"):
    """Put this in the 'except' section of a try block to convert the exception
       that just occurred to an XMLRPC Fault and log it.
       """
    e = sys.exc_info()[1]
    if isinstance(e, xmlrpc.Fault):
        raise
    else:
        log.msg(message + "\n" + "".join(traceback.format_exception(*sys.exc_info())))
        raise xmlrpc.Fault(e.__class__.__name__, str(e))


def rebuildPackage(package):
    """Recursively rebuild all loaded modules in the given package"""
    rebuild(package)
    for item in package.__dict__.itervalues():
        # Is it a module?
        if type(item) == type(package):
            rebuild(item)
            # Is it also a package?
            if item.__file__.find("__init__") >= 0:
                rebuildPackage(item)


class SysInterface(xmlrpc.XMLRPC):
    """An interface over XML-RPC to functionality that doesn't belong in any one other module

       caps : The CapabilityDB used to authorize access to these functions

       FIXME: rename this to DebugInterface
       """
    def __init__(self, caps):
        self.caps = caps

    def xmlrpc_rebuild(self, key):
        """Use twisted.python.rebuild to reload all applicable modules"""
        self.caps.faultIfMissing(key, 'universe', 'sys', 'sys.rebuild')
        import LibCIA
        rebuildPackage(LibCIA)
        return True

    def xmlrpc_gcGarbageInfo(self, key):
        """Return a string representation of the items in gc.garbage"""
        self.caps.faultIfMissing(key, 'universe', 'sys', 'sys.gc', 'sys.gcGarbageInfo')
        return map(repr, gc.garbage)

    def xmlrpc_gcObjectsInfo(self, key):
        """Return a string representation of the items in gc.get_objects().
           This can take a while and be very big!
           """
        self.caps.faultIfMissing(key, 'universe', 'sys', 'sys.gc', 'sys.gcObjectsInfo')
        return map(repr, gc.get_objects())

    def getTypeName(self, obj):
        # Try as hard and as generically as we can to get a useful type/class name
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

    def xmlrpc_getTypeObjects(self, t, key):
        """Return all objects of any one type, using the same type names as gcObjectProfile"""
        self.caps.faultIfMissing(key, 'universe', 'sys', 'sys.gc', 'sys.gcTypeObjects')
        results = []
        for object in gc.get_objects():
            if self.getTypeName(object) == t:
                results.append(repr(object))
        return results

    def xmlrpc_gcCollect(self, key):
        self.caps.faultIfMissing(key, 'universe', 'sys', 'sys.gc', 'sys.gcCollect')
        gc.collect()
        return True

    def xmlrpc_gcObjectProfile(self, key):
        """Print a chart showing the most frequently occurring types in memory"""
        self.caps.faultIfMissing(key, 'universe', 'sys', 'sys.gc', 'sys.gcObjectProfile')

        # Create a mapping from type name to frequency,
        # and a mapping from type name to example instances
        typeFreq = {}
        typeInstances = {}
        for object in gc.get_objects():
            t = self.getTypeName(object)
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
        return "\n".join([
            "%30s :  %-10d %s..." % (key, typeFreq[key], ", ".join(typeInstances[key])[:80])
            for key in keys])

### The End ###
