""" LibCIA.Client

Utilities to aid in writing clients for CIA's XML-RPC interface.
Includes a command line parser that can handle setting a server URI
and setting a key file.

These utilities are based on the twisted.python.usage command line parser
and xmlrpclib. If you need anything different, this module isn't for you.
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

from twisted.python import usage
import xmlrpclib


class Options(usage.Options):
    optParameters = [
        ['server', 's', 'http://localhost:3910', 'The URI of the CIA server to connect to'],
        ['key', 'k', None, 'Load a capabilities key from this file'],
        ]

    def opt_h(self):
        # By default only --help is recognized, not -h
        self.opt_help()


class App(object):
    """An object that can be subclassed to create a quick client app"""
    optionsClass = Options

    def __init__(self):
        self.config = self.optionsClass()
        self.config.parseOptions()

        self.server = xmlrpclib.ServerProxy(self.config['server'], allow_none=True)
        if self.config['key']:
            self.key = open(self.config['key']).read()
        else:
            self.key = None

### The End ###
