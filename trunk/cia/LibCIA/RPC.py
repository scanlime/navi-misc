""" LibCIA.RPC

Base classes implementing CIA's XML-RPC interface. This extend's twisted's
usual XML-RPC support with our own security and exception handling code.
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

from twisted.web import xmlrpc
from twisted.internet import defer
from twisted.python import log


class Interface(xmlrpc.XMLRPC):
    """A web resource representing a set of XML-RPC functions, optionally
       with other Interfaces attached as subhandlers.
       """
    def _getFunction(self, fqname):
        """Override the default _getFunction with our own that:
           - fixes a bug in nested subhandlers which was present in Twisted until after version 1.1.1
           - enforces our security policy

           Functions named xmlrpc_* are found and returned as-is, without any capability testing.

           Functions named protected_* are called only if the first argument is verified to be
           a valid capability key. By default, the list of capabilities accepted for a function is
           generated as 'universe', the fully qualified XML-RPC name of this function, and the
           fully qualified name of each interface containing this function.

           This behaviour can be overriden per-function by defining caps_* for this function
           as either a callable (called with the same arguments as the function) or a static
           sequence of capabilities.
           """
        # The default _getFunction expands subhandlers recursively. We don't do that, since
        # our capability generation code needs to know the fully qualified names involved.
        if fqname.find(self.separator) == -1:
            path = (fqname,)
        else:
            path = fqname.split(self.separator)

        # Find the Interface instance that owns this function
        interface = self
        for subHandler in path[:-1]:
            interface = interface.getSubHandler(subHandler)
            if interface is None:
                raise xmlrpc.NoSuchFunction

        # Do we have a normal non-protected function?
        f = getattr(interface, "xmlrpc_%s" % path[-1], None)
        if f and callable(f):
            return f

        # Do we have a protected function?
        f = getattr(interface, "protected_%s" % path[-1], None)
        if f and callable(f):
            return self.protect(interface, path, f)
        raise xmlrpc.NoSuchFunction

    def protect(self, interface, path, f):
        """Given a protected RPC function, return a wrapper that
           checks capabilities before executing the original function.
           """
        def rpcWrapper(*args):
            result = defer.Deferred()

            # First argument is the key
            try:
                key = args[0]
                args = args[1:]
            except IndexError:
                raise TypeError("This is a protected function, the first argument must be a capability key")

            # Look up the capabilities list
            caps = getattr(interface, "caps_%s" % path[-1], None)
            if not caps:
                caps = self.makeDefaultCaps(path)
            if callable(caps):
                caps = caps(path, *args)

            # A callback invoked when our key is successfully validated
            def keyValidated(x=None):
                defer.maybeDeferred(f, *args).chainDeferred(result)

            # Check the capabilities asynchronously (requires a database query)
            import Security
            d = Security.caps.require(key, *caps)
            d.addCallback(keyValidated)
            d.addErrback(result.errback)
            return result
        return rpcWrapper

    def _cbRender(self, result, request):
        """Wrap the default _cbRender, converting None (which can't be serialized) into True"""
        if result is None:
            result = True
        xmlrpc.XMLRPC._cbRender(self, result, request)

    def _ebRender(self, failure):
        """Override the default errback for rendering such that non-Faults
           are wrapped usefully rather than being converted into an unhelpful 'error'.
           """
        if isinstance(failure.value, xmlrpc.Fault):
            return failure.value
        log.err(failure)
        return xmlrpc.Fault(str(failure.type), str(failure.value))

    def makeDefaultCaps(self, path):
        """Create a default list of acceptable capability IDs for the given path"""
        caps = ['universe']
        base = []
        for segment in path:
            base.append(str(segment))
            caps.append(".".join(base))
        return caps

### The End ###
