#!/usr/bin/env python
"""
Grant a key for the given capability. The capability
name is specified on the command line, and the returned
key is written to the specified file.

If one argument is given, that is treated as a string
giving the capability's name. If multiple arguments
are given, they are treated as a tuple of strings.

For example, to grant a new key for modifying the ruleset:

   ./grant_key.py -k data/universe.key -o ruleset.key ruleset

Or to grant a key for modifying only the ruleset for
the URI 'irc://irc.freenode.net/tacobeam':

   ./grant_key.py -k data/universe.key -o tacobeam.key ruleset.uri irc://irc.freenode.net/tacobeam
"""

import sys, os; sys.path[0] = os.path.join(sys.path[0], '..')
from twisted.python import usage
from LibCIA import Client
import os

class Options(Client.Options):
    optParameters = [
        ['output', 'o', 'granted.key', 'A file to write the granted key to'],
        ]

    def getSynopsis(self):
        return Client.Options.getSynopsis(self) + ' capability'

    def parseArgs(self, *args):
        if len(args) > 1:
            self['capability'] = args
        elif len(args) == 1:
            self['capability'] = args[0]
        else:
            self.opt_help()

class KeyGranter(Client.App):
    optionsClass = Options

    def main(self):
        # Ask the server to grant us this capability
        key = self.server.security.grant(self.key, self.config['capability'])

        # Write the key to disk, setting the file permissions on it such to make it more private
        fileName = self.config['output']
        f = open(fileName, 'w')
        os.chmod(fileName, 0600)
        f.write(key)

if __name__ == '__main__':
    KeyGranter().main()

### The End ###
