#!/usr/bin/env python
"""
Load all <ruleset> tags found in a given XML file
"""

import Client
from xml.dom import minidom

class Options(Client.Options):
    def getSynopsis(self):
        return Client.Options.getSynopsis(self) + ' filename'

    def parseArgs(self, *args):
        if len(args) == 1:
            self['file'] = args[0]
        else:
            self.opt_help()

class RulesetLoader(Client.App):
    optionsClass = Options

    def main(self):
        # Start recursively searching the given file for rulesets
        self.search(minidom.parse(self.config['file']).documentElement)

    def search(self, node):
        # Is this a ruleset?
        if node.nodeType == node.ELEMENT_NODE and node.tagName == 'ruleset':
            self.foundRuleset(node)

        # Nope, search its children
        else:
            for child in node.childNodes:
                self.search(child)

    def foundRuleset(self, node):
        self.server.ruleset.store(self.key, node.toxml())

if __name__ == '__main__':
    RulesetLoader().main()

### The End ###
