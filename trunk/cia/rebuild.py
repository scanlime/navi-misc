#!/usr/bin/env python
"""
Send a command over XML-RPC to the CIA server triggering
a rebuild of all applicable python modules using
twisted.python.rebuild. This loads the latest code from
disk, and replaces references to the old code with references
to the new code.
"""

from LibCIA import Client

class Rebuilder(Client.App):
    def main(self):
        self.server.sys.rebuild(self.key)

if __name__ == '__main__':
    Rebuilder().main()

### The End ###
