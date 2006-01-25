#!/usr/bin/env python
"""
A simple tool for modifying users and capabilities.

To create a new user:

   ./security.py -c name -e email

To add capabilities:

   ./security.py -g uid capability

Some examples:

   ./security.py -c 'Johnny Pineapple' -e 'john@pineapple.co.uk'

   ./security.py -g 123 ruleset.uri irc://irc.freenode.net/tacobeam

   ./security.py -g 123 stats.path project/e-duck
"""

from twisted.python import usage
import Client
import os

class Options(Client.Options):
    optParameters = [
        ['create', 'c', None, 'Create a new user with the given name'],
        ['email', 'e', None, 'Set the email address of the new user'],
        ['grant', 'g', None, 'Grant a capability to the user with the given UID'],
        ]

    def parseArgs(self, *args):
        if len(args) > 1:
            self['capability'] = args
        elif len(args) == 1:
            self['capability'] = args[0]

class SecurityTool(Client.App):
    optionsClass = Options

    def main(self):
        if self.config['create']:
            uid, key = self.server.security.createUser(self.key, self.config['create'], self.config['email'])
            print "uid: %d, key: %s" % (uid, key)

        elif self.config['grant']:
            print self.server.security.grant(self.key, self.config['capability'], self.config['grant'])

if __name__ == '__main__':
    SecurityTool().main()

### The End ###
