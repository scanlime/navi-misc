#!/usr/bin/env python

from twisted.protocols import irc, basic
from twisted.internet import reactor, protocol
from twisted.python import util
import re


class RingBuffer:
    """A fixed-size iterable FIFO buffer"""
    def __init__(self, size):
        self.buffer = [None] * size
        self.head = 0
        self.tail = 0
        self.size = size
        self.numItems = 0

    def push(self, item):
        """Push a new item into the buffer, pushing old
           items out if it's full.
           """
        if self.numItems == self.size:
            self.tail = (self.tail + 1) % self.size
        else:
            self.numItems += 1
        self.buffer[self.head] = item
        self.head = (self.head + 1) % self.size

    def pop(self, item):
        """Remove and return the oldest item, returning None if empty"""
        if self.numItems:
            i = self.buffer[self.tail]
            self.tail = (self.tail + 1) % self.size
            return i

    def __iter__(self):
        """Iterate over all buffered items, from oldest to newest"""
        i = self.tail
        n = self.numItems
        while n > 0:
            yield self.buffer[i]
            i = (i + 1) % self.size
            n -= 1


class BaseChannel:
    """Base class for a communication channel. This could represent
       an IRC channel, private messages with one user, or server messages.
       Each channel has a writer and zero or more readers.
       """
    def __init__(self):
        self._readers = []

    def write(self, item):
        for reader in self._readers:
            reader(item)

    def addReader(self, reader):
        assert reader not in self._readers
        self._readers.append(reader)

    def removeReader(self, reader):
        self._readers.remove(reader)


class BufferedChannel(BaseChannel):
    """A channel that logs into a fixed-length in-memory ring buffer.
       The ring buffer is replayed to any new readers that connect.
       """
    def __init__(self, bufferSize):
        BaseChannel.__init__(self)
        self._buffer = None
        self.resizeBuffer(bufferSize)

    def resizeBuffer(self, size):
        oldBuffer = self._buffer
        self._buffer = RingBuffer(size)
        for item in oldBuffer:
            self._buffer.push(item)

    def addReader(self, reader):
        for item in self._buffer:
            reader(item)
        BaseChannel.addReader(self, reader)


class IRCMessage:
    """An abstraction for a one-line message received or sendable
       over IRC. We use this rather than Twisted's IRC client and
       server so that we can decode messages but then resend them
       without any modification. It also simplifies buffering and
       logging to have an object oriented abstraction for messages.
       """
    def __init__(self, command=None, params=None, prefix=None, line=None):
        self.line = line
        self.prefix = prefix
        self.command = command
        self.params = params

        if line is not None and command is None:
            self._unpackLine()
        elif line is None and command is not None:
            self._packLine()
        else:
            raise irc.IRCBadMessage("Messages must be created with either a line or a command")

    def _packLine(self):
        if not self.command:
            raise irc.IRCBadMessage("Empty command")
        if " " in self.command:
            raise irc.IRCBadMessage("Command contains whitespace")

        command = irc.symbolic_to_numeric.get(self.command, self.command)
        line = " ".join([command] + self.params)
        if self.prefix:
            self.line = ":%s %s" % (self.prefix, line)
        else:
            self.line = line

    def _unpackLine(self):
        self.prefix, command, self.params = irc.parsemsg(self.line)
        self.command = irc.numeric_to_symbolic.get(command, command.upper())


class IRCMessageReceiver(basic.LineReceiver):
    """A protocol class for sending and receiving IRC messages"""
    delimiter = '\n'

    def lineReceived(self, line):
        self.messageReceived(IRCMessage(line=line))

    def sendMessage(self, message):
        self.transport.write(message.line + "\r\n")

    def messageReceived(self, message):
        """By default, messages are dispatched to irc_* handlers"""
        f = getattr(self, "irc_%s" % message.command, None)
        if not f:
            f = self.irc_unknown
        f(message)

    def irc_unknown(self, message):
        raise NotImplementedError()


def generateMotd(message, prefix=None):
    """Convert a message of the day string into a sequence of IRC messages"""
    lines = [line.strip() for line in message.strip().split("\n")]
    while len(lines) < 2:
        lines.append("")
    yield IRCMessage(irc.RPL_MOTDSTART, [":- %s" % lines[0]], prefix=prefix)
    for line in lines[1:-1]:
        yield IRCMessage(irc.RPL_MOTD, [":- %s" % line], prefix=prefix)
    yield IRCMessage(irc.RPL_ENDOFMOTD, [":- %s" % lines[-1]], prefix=prefix)


class Connection(IRCMessageReceiver):
    """An established connection to a remote IRC server. These can be shared by
       multiple proxy clients, and are usually persistent.
       """
    def __init__(self, name, server, port, nick):
        self.name = name
        self.server = server
        self.port = port
        self.nickname = nick
        self.status = "Connecting"
        self.emptyChannels()

    def emptyChannels(self):
        """Empty our list of active channels. Called when we aren't connected
           or we otherwise know this client isn't in any channels.
           """
        self.channels = util.InsensitiveDict()

    def __repr__(self):
        return "<Connection %r to %s:%d as %r>" % (
            self.name, self.server, self.port, self.nickname)

    def signedOn(self):
        self.factory.proxy.internalNotice("%r signed on" % self)
        self.status = "Signed on"

    def register(self, nickname, hostname=None, servername=None):
        """Twisted's default register() is silly in that it doesn't let us
           specify a new username. We currently use the client's username.
           """
        if self.password is not None:
            self.sendLine("PASS %s" % self.password)
        self.setNick(nickname)
        self.sendLine("USER %s %s %s :%s" % tuple(self.factory.proxy.loginInfo))

    def joined(self, channel):
        """We've successfully joined a channel"""
        self.channels[channel] = []

    def left(self, channel):
        """We've successfully parted a channel"""
        try:
            del self.channels[channel]
        except KeyError:
            pass

    def lineReceived(self, line):
        self.factory.proxy.sendLine(line)
        return irc.IRCClient.lineReceived(self, line)


class ConnectionFactory(protocol.ClientFactory):
    def __init__(self, proxy, name, server, port, nick):
        self.proxy = proxy
        self.name = name
        self.server = server
        self.port = port
        self.nick = nick
        reactor.connectTCP(server, port, self)

    def buildProtocol(self, addr):
        c = Connection(self.name, self.server, self.port, self.nick)
        c.factory = self
        self.proxy.factory.connections[self.name] = c
        self.proxy.internalNotice("%r is connecting..." % c)
        return c

    def clientConnectionLost(self, connector, reason):
        self.proxy.internalNotice("Connection lost: %r (%s)" % (self, reason))
        try:
            del self.proxy.factory.connections[self.name]
        except KeyError:
            pass

    def clientConnectionFailed(self, connector, reason):
        self.proxy.internalNotice("Connection failed: %r (%s)" % (self, reason))
        try:
            del self.proxy.factory.connections[self.name]
        except KeyError:
            pass


class ProxyServer(IRCMessageReceiver):
    """One ProxyServer instance exists for every client connected. Multiple clients
       can share the same dictionary of connections.
       """

    serverName = "tacoproxy"
    motd = """
    Welcome to tacoproxy.
    Guacamole not included!
    Eat at your own risk.

    Try "/taco help" or "/quote taco help", depending on your client.
    """

    # User status
    loginInfo = None
    nick = None
    currentConnection = None

    def showHelpFor(self, regex):
        """Show help for any set of commands selected by a regex.
           Usage and a short description are extracted from the docstrings.
           """
        # Select and sort commands
        r = re.compile(regex)
        commands = [c for c in dir(self) if r.match(c)]
        commands.sort()

        # Extract fields from each docstring
        rows = []
        for command in commands:
            doc = getattr(self, command).__doc__.split("\n")
            rows.append([line.strip() for line in doc[:2]])

        # Calculate maximum widths
        widths = [0] * len(rows[0])
        for fields in rows:
            for i in xrange(len(widths)):
                w = len(fields[i])
                if w >= widths[i]:
                    widths[i] = w

        for fields in rows:
            fmt = ""
            for i in xrange(len(widths)):
                fmt += "%%-%ds" % (widths[i] + 2)
            yield fmt % tuple(fields)

    def notice(self, message, sender=None, recip=None):
        """Send a notice to the user. By default, this is directed from the proxy
           itself to our client's nick.
           """
        if not sender:
            sender = "tacoproxy!@%s" % self.serverName
        if not recip:
            recip = self.nick
        self.sendMessage(IRCMessage("NOTICE", [recip, ":%s" % message], prefix=sender))

    def taco_help(self):
        """/taco help
           Lists all commands
           """
        yield "Available commands:"
        for line in self.showHelpFor("taco_.*"):
            yield "  %s" % line

    def taco_new(self, name, serverAndPort, nick=None):
        """/taco new <name> <server>[:<port>] [<nick>]
           Create a new connection.
           """
        if name in self.factory.connections:
            yield "Connection %r already exists" % name
            return
        if not nick:
            nick = self.nick
        tokens = serverAndPort.split(":", 1)
        server = tokens[0]
        if len(tokens) > 1:
            try:
                port = int(tokens[1])
            except ValueError:
                yield "Invalid port"
                return
        else:
            port = 6667
        ConnectionFactory(self, name, server, port, nick)

    def taco_delete(self, name):
        """/taco delete <name>
           Delete a connection
           """
        try:
            c = self.factory.connections[name]
            del self.factory.connections[name]
        except KeyError:
            yield "Connection %r does not exist" % name
        c.disconnect()

    def taco_passwd(self, old, new):
        """/taco passwd <old> <new>
           Change passwords
           """
        yield "not implemented"
        
    def taco_list(self):
        """/taco list
           Show all connections and their status.
           """

        if self.factory.connections:
            yield "Active connections:"
            for name, connection in self.factory.connections.iteritems():
                yield "  %r (%s)" % (connection, connection.status)
        else:
            yield "No connections, use '/taco new' to create one."

    def taco_connect(self, name):
        """/taco connect <name>
           Switch to a new active connection
           """
        try:
            self.currentConnection = self.factory.connections[name]
            yield "Switching to %r" % name
        except KeyError:
            yield "Connection %r does not exist" % name

    def irc_TACO(self, message):
        """Dispatch a /taco command. Each command handler is a generator that
           yields lines to send back to the user as necessary.
           """
        # Don't allow commands when not logged in
        if not self.loginInfo:
            return

        # Look up the command handler
        if message.params:
            command = message.params[0]
            commandParams = message.params[1:]
        else:
            command = ""
            commandParams = []
        methodName = "taco_%s" % command.lower()
        f = getattr(self, methodName, None)
        if not f:
            f = self.taco_help

        # If there's a mismatch in the number of arguments, show usage
        try:
            lines = f(*commandParams)
        except TypeError:
            lines = ["usage: %s" % line for line in self.showHelpFor(methodName)]

        # Send back results
        for line in lines:
            self.notice(line)

    def irc_NICK(self, message):
        self.nick = message.params[0]

    def irc_USER(self, message):
        if self.loginInfo:
            self.sendMessage(IRCMessage(irc.ERR_ALREADYREGISTERED, [":You are already logged in"]))
        else:
            self.loginInfo = message.params
            for message in generateMotd(self.motd, prefix=self.serverName):
                self.sendMessage(message)

    def irc_PING(self, message):
        self.sendMessage(IRCMessage("PONG", [":%s" % message.params[-1]], prefix=self.serverName))

    def irc_unknown(self, message):
        if self.loginInfo:
            # If we're connected, just pass this on to the server
            self.currentConnection.sendMessage(message)
        else:
            self.sendMessage(IRCMessage(irc.ERR_UNKNOWNCOMMAND, ["%s :Unknown command %r %r" % (
                message.command, message.command, message.params)]))


if __name__ == "__main__":
    factory = protocol.Factory()
    factory.protocol = ProxyServer

    # All incoming clients share the same connections dict
    factory.connections = {}

    reactor.listenTCP(6667, factory)
    reactor.run()

### The End ###
