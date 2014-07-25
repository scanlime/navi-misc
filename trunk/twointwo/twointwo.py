#!/usr/bin/python

from twisted.protocols import irc, telnet
from twisted.internet import reactor, protocol
from twisted.internet.protocol import ClientCreator

import string, time

class Receiver(irc.DccFileReceive):
    def connectionMade(self):
        self.transport.log = self.log
        irc.DccFileReceive.connectionMade(self)

    def log(self, message):
        print message

class ReceiverFactory(protocol.ClientFactory):
    protocol = Receiver
    def __init__(self, filename, size, resume = 0):
        self.filename = filename
        self.resume = resume
        self.filesize = size

    def buildProtocol(self, addr):
        p = self.protocol(self.filename, self.filesize)
        p.set_overwrite(True)
        return p

    def clientConnectionLost(self, connector, reason):
        print 'dcc connection lost:',reason

    def clientConnectionFailed(self, connector, reason):
        print 'dcc connection failed:',reason

class TwoInTwo(irc.IRCClient):
    def __init__(self):
        self.nickname = 'jimmycarter'
        self.recievers = []
        self.lastmsg = 0
        self.queue = []

    def connectionMade(self):
        irc.IRCClient.connectionMade(self)
        print '[connected at %s]' % time.asctime(time.localtime(time.time()))
        self.factory.telnet.ircbot = self

    def signedOn(self):
        self.join(self.factory.channel)

    def connectionLost(self, reason):
        print '[disconnected at %s]' % time.asctime(time.localtime(time.time()))

    def joined(self, channel):
        print '[joined %s]' % channel

    def privmsg(self, user, channel, msg):
        # its hackish, but use this as a trigger for sending
        if time.time() > (self.lastmsg + 70) and len(self.queue) > 1:
            message = self.queue[0]
            self.queue = self.queue[1:]
            self.msg('#mp3_classical', message)
	    print 'sending message \'%s\'' % message
            self.lastmsg = time.time()

    def dccDoSend(self, user, address, port, fileName, size, data):
        f = ReceiverFactory(fileName, size)
        reactor.connectTCP(address, port, f)

class TwoInTwoFactory(protocol.ClientFactory):
    protocol = TwoInTwo
    telnet = None
    def __init__(self, channel):
        self.channel = channel

    def clientConnectionLost(self, connector, reason):
        connector.connect()

    def clientConnectionFailed(self, connector, reason):
        print 'connection failed:', reason

class TelnetClient(telnet.Telnet):
    def connectionMade(self):
        telnet.Telnet.connectionMade(self)
        self.lineBuffer = []

    def loggedIn(self):
        self.transport.write('>> ')

    def checkUserAndPass(self, username, password):
        return ((self.factory.username == username) and (password == self.factory.password))

    def write(self, data):
        self.transport.write(data)

    def telnet_Command(self, cmd):
        print 'recieved command "%s"' % cmd
        if cmd == 'exit' or cmd == 'quit':
            self.transport.loseConnection()
            return 'Command'
        self.factory.ircbot.queue.append(cmd)
        self.transport.write('>> ')
        return 'Command'

class TelnetFactory(protocol.Factory):
    username = 'admin'
    password = 'admin'
    protocol = TelnetClient

if __name__ == '__main__':
    import sys
    f = TwoInTwoFactory('#mp3_classical')
    reactor.connectTCP('us.undernet.org', 6667, f)

    g = TelnetFactory()
    f.telnet = g
    reactor.listenTCP(1024, g)

    reactor.run()
