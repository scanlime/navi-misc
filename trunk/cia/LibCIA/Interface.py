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


def catchFaults(callable, *args, **kwargs):
    """Call the provided function with the provided arguments.
       If an exception is raised, it is logged and converted to
       an XML-RPC Fault, and that is returned. If not, the value
       of the callable is returned. A None result is converted to False.
       """
    try:
        result = callable(*args, **kwargs)
    except:
        e = sys.exc_info()[1]
        log.msg("Exception occurred while calling %r with the args %r %r\n%s" %
                (callable, args, kwargs, "".join(traceback.format_exception(*sys.exc_info()))))
        return xmlrpc.Fault(e.__class__.__name__, str(e))
    if result is not None:
        return result
    return False


class SysInterface(xmlrpc.XMLRPC):
    """An interface over XML-RPC to functionality that doesn't belong in any one other module"""
    def xmlrpc_rebuild(self):
        """Use twisted.python.rebuild to reload all applicable modules"""
        # Rebuild our package to make sure we have the latest module list
        import LibCIA
        rebuild(LibCIA)

        # Now rebuild all loaded modules inside the LibCIA package
        for item in LibCIA.__dict__.itervalues():
            if type(item) == type(LibCIA):
                rebuild(item)
        return True

### The End ###
