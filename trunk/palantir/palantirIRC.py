''' palantirIRC.py

This modules controls the IRC aspect of the Palantir RPG client.

  Copyright (C) 2004 W. Evan Sheehan <evan@navi.cx>
'''

from twisted.protocols import irc
from twisted.internet import protocol, reactor

class PalantirClient(irc.IRCClient):
  ''' This is the class that handles all the nitty gritty of the IRC
      stuff.  Yay for twisted making things pretty easy on me.
      '''
  def Send(self, msg):
    ''' Send a message to the channel. '''
    self.say(self.factory.channel, msg)

  ### IRC Callbacks ###
  def connectionMade(self):
    print 'connected'
    self.nickname = self.factory.nick
    irc.IRCClient.connectionMade(self)

  def connectionLost(self, reason):
    print 'Connection Lost:', reason

  def signedOn(self):
    self.join(self.factory.channel)

  def joined(self, channel):
    self.say(channel,'I is here!')

  def privmsg(self, user, channel, msg):
    print user, ':', msg

  def action(self, user, channel, msg):
    print 'action'

  def irc_NICK(self, prefix, params):
    print 'Nick'

class PalantirClientFactory(protocol.ClientFactory):
  ''' Factory to create the IRC client. '''
  protocol = PalantirClient

  def __init__(self, nick, server='irc.freenode.net', channel='#palantir'):
    self.nick = nick
    self.server = server
    self.channel = channel

  def clientConnectionLost(self, connector, reason):
    connector.connect()

  def clientConnectionFailed(self, connector, reason):
    print 'Connection failed:', reason
    reactor.stop()


# Just for a little testing.
if __name__ == '__main__':
  import sys
  f = PalantirClientFactory(sys.argv[1])
  reactor.connectTCP('irc.freenode.net', 6667, f)
  reactor.run()
