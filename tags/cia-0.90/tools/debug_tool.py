#!/usr/bin/env python
"""
A client providing easy access to common debugging features
"""

import Client
import code, sys, readline, socket


class Options(Client.Options):
    optFlags = [
        ['type-profile', 't', 'Show a chart profiling which data types have the most instances'],
        ['console', 'c', 'Start an interactive debugging console'],
        ['preserve-namespace', 'p', "Don't reset the remote interpreter namespace"],
        ]

    optParameters = [
        ['rebuild', 'r', None, 'Rebuild one specific package or module'],
        ['eval', 'e', None, 'Evaluate one statement of arbitrary code, printing the output'],
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
        try:
            result = self.app.server.debug.eval(self.app.key, source)
        except socket.error:
            # We won't let a puny socket error stop us!
            # Any errors in establishing an initial connection
            # will still show up when we run getBanner(), but this
            # keeps an otherwise perfectly good debug session
            # from being ended if you press 'enter' while restarting
            # the server.
            print "Communications Error: %s" % sys.exc_info()[1]
            return False

        if result is False:
            # The input is incomplete
            return True
        else:
            # It was complete, show the result adding a trailing
            # newline if there isn't one already.
            sys.stdout.write(result)
            if result and result[-1] != '\n':
                sys.stdout.write('\n')
            return False

    def interact(self, banner=None):
        if banner is None:
            banner = self.app.server.debug.getBanner(self.app.key)
        code.InteractiveConsole.interact(self, banner)


class DebugTool(Client.App):
    optionsClass = Options

    def main(self):
        if self.config['rebuild']:
            self.server.debug.rebuild(self.key, self.config['rebuild'])

        if self.config['type-profile']:
            print self.server.debug.gc.typeProfile(self.key)

        if self.config['eval'] or self.config['console']:
            if not self.config['preserve-namespace']:
                self.server.debug.resetInterpreter(self.key)

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
