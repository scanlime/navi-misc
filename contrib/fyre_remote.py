#!/usr/bin/env python
#
# This is an example Python module for controlling Fyre
# in remote control mode. Run it for a quick demonstration,
# or import it in your own scripts.
#

import socket

RESPONSE_READY        = 220
RESPONSE_OK           = 250
RESPONSE_PROGRESS     = 251
RESPONSE_FALSE        = 252
RESPONSE_BINARY       = 380
RESPONSE_UNRECOGNIZED = 500
RESPONSE_BAD_VALUE    = 501
RESPONSE_UNSUPPORTED  = 502


class FyreException(Exception):
    def __init__(self, code, message, command=None):
        msg = "%d %s" % (code, message)
        if command:
            msg = "%s (in response to %r" % (msg, command)
        Exception.__init__(self, msg)


class FyreServer:
    """This class represents a socket connection to
       a remote Fyre server (fyre -r)
       """
    defaultPort = 7931

    def __init__(self, host):
        if host.find(":") >= 0:
            host, port = host.split(":")
            port = int(port)
        else:
            port = self.defaultPort

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((host, port))
        self.file = self.socket.makefile()
        self.asyncQueue = []

        # Read the server's greeting
        code, message = self._readResponse()
        if code != RESPONSE_READY:
            raise FyreException(code, message)

    def flush(self):
        """Send all pending asynchronous commands. If any
           of them returned an error, this raises an
           exception indicating which command resulted
           in which response code.
           """
        self.file.flush()
        for command in self.asyncQueue:
            code, message = self._readResponse()
            if code < 200 or code >= 300:
                raise FyreException(code, message, command)
        self.asyncQueue = []

    def _readResponse(self):
        """Read the server's next response, returning
           a (response_code, message) tuple.
           """
        line = self.file.readline().strip()
        code, message = line.split(" ", 1)
        return int(code), message

    def command(self, *tokens):
        """Send a command in which the return value doesn't
           matter except for detecting errors. This command
           will be buffered, and any errors won't be detected
           until the next flush().
           """
        cmd = " ".join(map(str, tokens))
        self.file.write(cmd + "\n")
        self.asyncQueue.append(cmd)

    def query(self, *tokens):
        """Immediately send a command in which we care about
           the response value. This forces a flush() first,
           so errors from other commands may be first discovered
           here. Returns a (response_code, message) tuple.
           """
        self.flush()
        cmd = " ".join(map(str, tokens))
        self.file.write(cmd + "\n")
        self.file.flush()
        return self._readResponse()

    def setParams(self, **kwargs):
        """Set any number of parameters, given as keyword arguments.
           Like command(), these will not take effect until the next
           flush().
           """
        for item in kwargs.iteritems():
            self.command("set_param", "%s=%s" % item)


if __name__ == "__main__":
    # A simple demo...
    # This connects to the Fyre server, sets up a bunch of image
    # parameters, (copied straight out of a saved .png file) then
    # varies some of the parameters to create a simple animation.
    # This could be extended into a more complex animation, maybe
    # even one based on sensors or a simulation.

    import math, sys

    if len(sys.argv) > 1:
        host = sys.argv[1]
    else:
        host = "localhost"

    fyre = FyreServer(host)
    fyre.command("set_render_time", 0.02)
    fyre.setParams(size = "400x300",
                   exposure = 0.030000,
                   zoom = 0.8,
                   gamma = 1.010000,
                   a = 4.394958,
                   b = 1.028872,
                   c = 1.698752,
                   d = 3.954149,
                   xoffset = 0.308333,
                   yoffset = 0.058333,
                   emphasize_transient = 1,
                   transient_iterations = 3,
                   initial_conditions = "circular_uniform",
                   initial_xscale = 0.824000,
                   initial_yscale = 0.018000)
    fyre.command("set_gui_style", "simple")

    t = 0
    while 1:
        fyre.setParams(initial_xoffset = t * 1.5,
                       initial_yoffset = math.sin(t) * 0.1)
        fyre.command("calc_step")
        fyre.flush()
        t += 0.1

### The End ###

