#!/usr/bin/env python
"""
A client providing easy access to common debugging features
"""

import Client
import code, sys, readline


class Options(Client.Options):
    optFlags = [
        ['type-profile', 't', 'Show a chart profiling which data types have the most instances'],
        ['console', 'c', 'Start an interactive debugging console'],
	]

    optParameters = [
        ['rebuild', 'r', None, 'Rebuild one specific package or module'],
        ['eval', 'e', None, 'Evaluate arbitrary code in the current remote interpreter namespace'],
	]


class RemoteConsole(code.InteractiveConsole):
    """An InteractiveConsole that works over XML-RPC :)
       Commands are sent to debug.eval(), which returns False to
       indicate an incomplete command, or a string with captured output
       if the command can be run.
       """
    def __init__(self, app):
        self.app = app
        code.InteractiveConsole.__init__(self)

    def runsource(self, source, filename=None):
        result = self.app.server.debug.eval(self.app.key, source)
        if result is False:
            return True
        else:
            sys.stdout.write(result)
            return False

    def interact(self, banner=None):
        if banner is None:
            banner = ("Remote python console connected to %s\n"
                      "Remember that commands can not be interrupted. Infinite loops are bad!" %
                      self.app.config['server'])
        code.InteractiveConsole.interact(self, banner)


class DebugTool(Client.App):
    optionsClass = Options

    def main(self):
        if self.config['rebuild']:
            self.server.debug.rebuild(self.key, self.config['rebuild'])

        if self.config['type-profile']:
            print self.server.debug.gc.typeProfile(self.key)

        if self.config['eval']:
            result = self.server.debug.eval(self.key, self.config['eval'])
            if result == False:
                print "Incomplete command"
            else:
                print result

        if self.config['console']:
            RemoteConsole(self).interact()

if __name__ == '__main__':
    DebugTool().main()

### The End ###
