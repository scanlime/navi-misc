""" LibCIA.Debug

Remote interfaces for debugging and dynamic reloading while
CIA is running.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2007 Micah Dowty <micah@navi.cx>
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
import RpcServer
from cStringIO import StringIO
import gc, sys, traceback, code, types


class DebugInterface(RpcServer.Interface):
    """An XML-RPC interface for remote debugging and dynamic reloading of CIA."""
    def __init__(self):
        RpcServer.Interface.__init__(self)
        self.putSubHandler('gc', GcInterface())
        self.protected_resetInterpreter()

    def newCommandNamespace(self):
        """Return a dictionary that acts as a fresh namespace for
           executing debug console commands in.
           """
        return dict(globals())

    def protected_rebuild(self, *packageNames):
        """Use twisted.python.rebuild to reload the given package or module
           and all loaded packages or modules within it.
           """
        for packageName in packageNames:
            log.msg("Starting a rebuild at the package %r" % packageName)
            # The non-empty fromlist tells __import__ we want the module referred
            # to by the given path, not just its top-level module.
            package = __import__(packageName, globals(), locals(), [''])
            rebuildPackage(package)

    def protected_resetInterpreter(self):
        """Reinitialize the debug interpreter, beginning a new session with a new namespace"""
        self.interpreter = RemoteInterpreter()

    def protected_eval(self, source):
        """Evaluate arbitrary code in the context of this module.
           This is meant to be used to provide an interface similar to the python
           interpreter. For this reason, incomplete code is detected and causes False
           to be returned. If the code was complete, it is run and a string indicating
           the result to display is returned.

           NOTE: This is an extremely powerful function, so a 'debug' or 'debug.eval'
                 key should be treated with equivalent respect to a 'universe' key.
                 It is also quite dangerous. Besides being able to execute arbitrary
                 code, any infinite loops or code that takes a long time to run
                 will cause CIA to hang.
           """
        log.msg("Executing code in debug.evalCommand: %r" % source)
        if self.interpreter.runsource(source):
            # Incomplete
            return False
        else:
            return self.interpreter.collectOutput()

    def protected_getBanner(self):
        """Return an appropriate banner string to use for interactive interpreters"""
        return ('Python %s on %s\n'
                'Type "help", "copyright", "credits" or "license" for more information.\n'
                '*** Running remotely in CIA: commands can not be interrupted ***' %
                (sys.version, sys.platform))


class NullDev:
    """A fake device, like /dev/null"""
    def write(self, data):
        pass

    def read(self):
        return ''

    def readline(self):
        return ''


class RemoteInterpreter(code.InteractiveInterpreter):
    """An interpreter similar to the Python console that takes acts on commands
       delivered over XML-RPC and captures what would be stdout into a string.
       """
    def __init__(self):
        # By default, give them a new namespace filled with
        # all the bare functions from this module.
        ns = {}
        for name, value in globals().iteritems():
            if type(value) == types.FunctionType:
                ns[name] = value

        code.InteractiveInterpreter.__init__(self, ns)
        self.capturedOutput = StringIO()

    def runsource(self, source):
        # Redirect stdout and stderr to our capturedOutput.
        # We could just override write() to get tracebacks
        # and such, but we also want commands that generate
        # output via 'print' to work.
        #
        # We override stdin because it's pretty useless, and
        # commands like help() that try to read from it will stall.
        #
        # This could be dangerous if we have other threads
        # trying to write to them as well, but that would only
        # happen (currently at least) if there's an error in
        # one of our database threads. Since this is only used
        # for debugging and such, that's an acceptable risk.
        savedStdout = sys.stdout
        savedStderr = sys.stderr
        savedStdin = sys.stdin
        sys.stdout = self.capturedOutput
        sys.stderr = self.capturedOutput
        sys.stdin = NullDev()
        try:
            result = code.InteractiveInterpreter.runsource(self, source)
        finally:
            sys.stdout = savedStdout
            sys.stderr = savedStderr
            sys.stdin = savedStdin
        return result

    def collectOutput(self):
        """Return a string with all output generated since the last call"""
        output = self.capturedOutput.getvalue()
        self.capturedOutput = StringIO()
        return output


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


_leakTracker = None

def debugLeaks(reset=False, quiet=False, interestedTypes=None):
    """Show representations of all objects tracked by the garbage collector that
       were not present at the last invocation.
       """
    global _leakTracker
    gc.collect()

    if _leakTracker is None or reset:
        _leakTracker = {}
        quiet = True

    objlist = gc.get_objects()
    newCount = 0
    instCount = 0
    for object in objlist:
        typename = getTypeName(object)
        if interestedTypes is not None:
            if not typename in interestedTypes:
                continue

        instCount += 1
        if _leakTracker.get(id(object)) == typename:
            continue
        newCount += 1
        _leakTracker[id(object)] = typename

        if not quiet:
            print repr(object)

    print "\n-- %d new instances, %d total instances, %d total objects" % (
        newCount, instCount, len(objlist))


class GcInterface(RpcServer.Interface):
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
