#!/usr/bin/env python
"""
A utility to set the value of a metadata key for the given stats path.
This can be used to affect the presentation of a particular path in the
stats browser. For example:

./stats_metadata -k stats.key project/pigomatic title 'Pig-O-Matic (tm)'
"""

from twisted.python import usage
import Client
import os, xmlrpclib

class Options(Client.Options):
    optFlags = [
        ['from-file', 'f', 'The value specified is actually a file to read the value from'],
        ['remove', 'r', 'Remove the specified metadata key']
        ]

    optParameters = [
        ['type', 't', 'text/plain', "The MIME type to use when setting a key value"]
        ]

    def getSynopsis(self):
        return Client.Options.getSynopsis(self) + ' path key [value]'

    def parseArgs(self, *args):
        if len(args) == 3:
            self['path'], self['dataKey'], self['dataValue'] = args
        elif len(args) == 2:
            self['path'], self['dataKey'] = args
            self['dataValue'] = None
        else:
            self.opt_help()

class MetadataTool(Client.App):
    optionsClass = Options

    def main(self):
        metadata = self.server.stats.metadata

        value = self.config['dataValue']
        if self.config['from-file'] and value:
            value = xmlrpclib.Binary(open(value, 'rb').read())

        if self.config['remove']:
            metadata.remove(self.key, self.config['path'], self.config['dataKey'])

        if self.config['dataValue'] is not None:
            metadata.set(self.key, self.config['path'], self.config['dataKey'], value, self.config['type'])

if __name__ == '__main__':
    MetadataTool().main()

### The End ###
