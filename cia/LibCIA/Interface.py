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
       """
    def __init__(self, caps):
        self.caps = caps

    def xmlrpc_rebuild(self, key=None):
        """Use twisted.python.rebuild to reload all applicable modules"""
        self.caps.faultIfMissing(key, 'universe', 'sys', 'sys.rebuild')
        import LibCIA
        rebuildPackage(LibCIA)
        return True

### The End ###
