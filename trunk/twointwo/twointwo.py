#!/usr/bin/python

from twisted.protocols import irc
from twisted.internet import reactor, protocol
from twisted.internet.interfaces import IReactorTCP
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
        self.nickname = 'testingbot'
        self.recievers = []

    def connectionMade(self):
        irc.IRCClient.connectionMade(self)
        print '[connected at %s]' % time.asctime(time.localtime(time.time()))

    def signedOn(self):
        self.join(self.factory.channel)

    def connectionLost(self, reason):
        print '[disconnected at %s]' % time.asctime(time.localtime(time.time()))

    def joined(self, channel):
        print '[joined %s]' % channel

    def privmsg(self, user, channel, msg):
        user = string.split(user, '!', 1)[0]
        print '<%s> %s' % (user, msg)

    def dccDoResume(self, user, file, port, resumePos):
        print '%s is trying to resume %s at %s, on port %s' % (user, file, resumePos, port)

    def dccDoSend(self, user, address, port, fileName, size, data):
        f = ReceiverFactory(fileName, size)
        reactor.connectTCP(address, port, f)

class LogBotFactory(protocol.ClientFactory):
    protocol = TwoInTwo
    def __init__(self, channel):
        self.channel = channel

    def clientConnectionLost(self, connector, reason):
        connector.connect()

    def clientConnectionFailed(self, connector, reason):
        print 'connection failed:', reason

if __name__ == '__main__':
    import sys
    f = LogBotFactory('#testing123')
    reactor.connectTCP('irc.freenode.net', 6667, f)
    reactor.run()
