""" LibCIA.RpcClient

URI handlers that deliver messages over various RPC methods to other servers
"""
#
# CIA open source notification system
# Copyright (C) 2003-2005 Micah Dowty <micah@navi.cx>
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
import Ruleset
import re

# This is a hack to keep "Starting factory..." and "Stopping factory..."
# messages from being logged every time we make an XML-RPC connection.
# This is less ugly than the alternative of having our own xmlrpc.Proxy
# implementation. If only xmlrpc.Proxy made it easy to set the factory class...
xmlrpc.QueryFactory.noisy = False


class XmlrpcURIHandler(Ruleset.RegexURIHandler):
    """Handles xmlrpc:// URIs, specifying an XML-RPC call to make,
       with an argument list optionally including literals, the
       message content, and the ruleset's resulting content.
       If the specified server name includes no path, '/RPC2' will be assumed.
       Some examples...

       To deliver a message to another CIA server running on some.host.net:

          xmlrpc://some.host.net/RPC2?hub.deliver(message)

       To call the function 'recordData' with 'muffin' and 42 as the first
       two arguments, and the formatter's result as the second argument, on
       an XML-RPC server located at http://example.com:8080/services/rpc:

          xmlrpc://example.com:8080/services/rpc?recordData('muffin',42,content)

       Only string literals, numeric literals, and variables are currently supported.

       Note that all connections are performed over http- this doesn't
       support making XML-RPC connections over https, and there would
       really be no point.
       """
    scheme = 'xmlrpc'

    regex = r"""
       ^xmlrpc://                                      # URI scheme
       (?P<server>[a-zA-Z]([a-zA-Z0-9.-]*[a-zA-Z0-9])? # Hostname
       (:[0-9]+)?                                      # Port
       (/[^ /\?]+)*)                                   # Path
       \?(?P<function>[^ \(\)]+)                       # Function name
       \((?P<args>.*)\)$                               # Args
       """

    argLexer = re.compile(r"""
        # Whitespace before each parameter
        \s*
        (
            # Literal types
            (?P<float>        \-?((\d+\.\d*|\.\d+)([eE][-+]?\d+)?|\d+[eE][-+]?\d+))|
            (?P<int>          \-?[1-9]\d*)|
            (?P<string>       ( \"([^\\\"]|\\.)*\" | \'([^\\\']|\\.)*\' ))|

            # Variables
            (?P<var>          [a-zA-Z0-9_]+)
        )
        # Whitespace and an optional comma after each parameter
        \s*,?\s*
        """, re.VERBOSE)

    def parseArgs(self, args, **vars):
        """Parse an argument list, as a string, with the given variables.
           Returns a list of parsed arguments, or raises an InvalidURIException.
           """
        argList = []
        for token in self.argLexer.finditer(args):
            for tokenType, tokenValue in token.groupdict().items():
                if tokenValue is not None:
                    argList.append(getattr(self, 'argtoken_'+tokenType)(tokenValue, vars))
        return argList

    def argtoken_float(self, value, vars):
        return float(value)

    def argtoken_var(self, value, vars):
        try:
            return vars[value]
        except KeyError:
            raise Ruleset.InvalidURIException("Unknown variable name %r" % value)

    def argtoken_int(self, value, vars):
        return int(value)

    def argtoken_string(self, value, vars):
        # CHEESY HACK!
        return value[1:-1]

    def assigned(self, uri, newRuleset):
        """Just validate the URI and argument list"""
        self.parseArgs(self.parseURI(uri)['args'], content=None, message=None)

    def message(self, uri, message, content):
        groups = self.parseURI(uri)

        # Add the implicit /RPC2 and http://, creating a server proxy object
        server = groups['server']
        if server.find('/') < 0:
            server = server + '/RPC2'
        server = 'http://' + server
        proxy = xmlrpc.Proxy(server)

        # Parse arguments, allowing use of our 'message' and 'content' variables.
        args = self.parseArgs(groups['args'],
                              message = str(message),
                              content = content)

        # Make the call, ignoring the resulting Deferred. If it succeeds,
        # we don't really care. If it fails, the error will get logged.
        proxy.callRemote(groups['function'], *args)

### The End ###
