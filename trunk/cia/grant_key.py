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

from twisted.python import usage
from LibCIA import Client

class Options(Client.Options):
    optParameters = [
        ['output', 'o', 'granted.key', 'A file to write the granted key to'],
        ]

    def parseArgs(self, *args):
        if len(args) > 1:
            self['capability'] = args
        elif len(args) == 1:
            self['capability'] = args[0]
        else:
            raise usage.UsageError("A capability name must be specified")

class KeyGranter(Client.App):
    optionsClass = Options

    def main(self):
        key = self.server.security.grant(self.config['capability'], self.key)
        open(self.config['output'], 'w').write(key)

if __name__ == '__main__':
    KeyGranter().main()

### The End ###
