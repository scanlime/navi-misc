#!/usr/bin/env python
"""
A utility to set the value of a metadata key for the given stats path.
This can be used to affect the presentation of a particular path in the
stats browser. For example:

./stats_metadata -k stats.key project/pigomatic title 'Pig-O-Matic (tm)'
"""

import sys, os; sys.path[0] = os.path.join(sys.path[0], '..')
from twisted.python import usage
from LibCIA import Client
import os, xmlrpclib

class Options(Client.Options):
    optFlags = [
        ['from-file', 'f', 'The value specified is actually a file to read the value from']
        ]

    optParameters = [
        ['type', 't', None, "Sets the key's MIME type"]
        ]

    def getSynopsis(self):
        return Client.Options.getSynopsis(self) + ' path key value'

    def parseArgs(self, *args):
        if len(args) == 3:
            self['path'], self['dataKey'], self['dataValue'] = args
        else:
            self.opt_help()

class MetadataTool(Client.App):
    optionsClass = Options

    def main(self):
        value = self.config['dataValue']
        if self.config['from-file']:
            value = xmlrpclib.Binary(open(value, 'rb').read())

        d = [ (self.config['dataKey'], value) ]
        if self.config['type'] is not None:
            d.append(((self.config['dataKey'], 'type'), self.config['type']))
        self.server.stats.updateMetadata(self.config['path'], d, self.key)

if __name__ == '__main__':
    MetadataTool().main()

### The End ###
