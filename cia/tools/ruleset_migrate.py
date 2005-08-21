#!/usr/bin/env python
"""
Perform a global search-and-replace on ruleset URIs, to migrate
them between IRC servers or networks.

./ruleset_migrate "from" "to"
"""

from twisted.python import usage
import Client
import os, xmlrpclib

class Options(Client.Options):
    def getSynopsis(self):
        return Client.Options.getSynopsis(self) + ' from to'

    def parseArgs(self, *args):
        if len(args) == 2:
            self['from'], self['to'] = args
        else:
            self.opt_help()

class RulesetMigrator(Client.App):
    optionsClass = Options

    def main(self):
        ruleset = self.server.ruleset

	print "Getting ruleset list..."
	uris = ruleset.getUriList()

	for uri in uris:
            if uri.find(self.config['from']) >= 0:
                newUri = uri.replace(self.config['from'], self.config['to'])
                print '"%s" -> "%s"' % (uri, newUri)

                # This is hacky...
                r = ruleset.getRuleset(uri)
                r = r.replace(uri, newUri)

                # Erase the old ruleset first
                ruleset.store(self.key, "<ruleset uri=%r/>" % uri)

                # Store the modified ruleset
                ruleset.store(self.key, r)

if __name__ == '__main__':
    RulesetMigrator().main()

### The End ###
