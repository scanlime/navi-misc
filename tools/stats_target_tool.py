#!/usr/bin/env python
"""
A utility to manipulate stats targets.
"""

from twisted.python import usage
import Client
import os, xmlrpclib

class Options(Client.Options):
    optFlags = [
        ['clear', 'c', 'Permanently clear all data from the specified target'],
        ]

    def getSynopsis(self):
        return Client.Options.getSynopsis(self) + ' path'

    def parseArgs(self, *args):
        if len(args) == 1:
            self['path'] = args[0]
        else:
            self.opt_help()

class TargetTool(Client.App):
    optionsClass = Options

    def main(self):
        stats = self.server.stats

        if self.config['clear'] is not None:
            stats.clearTarget(self.key, self.config['path'])

if __name__ == '__main__':
    TargetTool().main()

### The End ###
