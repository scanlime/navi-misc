''' palantirIRC.py

This modules controls the IRC aspect of the Palantir RPG client.

  Copyright (C) 2004 W. Evan Sheehan <evan@navi.cx>
'''

from twisted.protocols import irc
from twisted.internet import protocol, reactor


class PalantirClient(irc.IRCClient):
  ''' This is the class that handles all the nitty gritty of the IRC
      stuff.
      '''
  def __init__(self):
    self.realname = 'Palantir'

  def Send(self, msg):
    self.say('#palantir', msg)

  ### IRC Callbacks ###
  def connectionMade(self):
    print 'Connected'
    self.nickname = self.factory.nickname
    irc.IRCClient.connectionMade(self)

  def connectionLost(self, reason):
    print 'Connection Lost:', reason

  def signedOn(self):
    self.join(self.factory.channels)

  def privmsg(self, user, channel, msg):
    if self.factory.ui:
      self.factory.ui.messageReceive(user, channel, msg)
    else:
      print '<', user, '>', msg, '\n'

  def action(self, user, channel, msg):
    if self.factory.ui:
      self.factory.ui.meReceive(user, channel, msg)
    else:
      print '*', user, ' ', msg, '\n'

  #def irc_NICK(self, prefix, params):
    #if self.factory.ui:
      #self.factory.ui.nickReceive(params[0])

class PalantirClientFactory(protocol.ClientFactory):
  ''' Factory to create the IRC client. '''
  protocol = PalantirClient

  def __init__(self, nick, server='irc.freenode.net', channels='#palantir', ui=None):
    self.nickname = nick
    self.server = server
    self.channels = channels
    self.ui = ui

  def clientConnectionLost(self, connector, reason):
    connector.connect()

  def clientConnectionFailed(self, connector, reason):
    print 'Connection failed:', reason
    reactor.stop()

  def buildProtocol(self, addr):
    ''' Overridden to save a reference to the instance of the actual IRC client
        so that it can be accessed via the factory that created it.
	'''
    self.client = protocol.ClientFactory.buildProtocol(self, addr)
    return self.client

  def Send(self, msg):
    self.client.say(self.channels, msg)

  def me(self, action):
    self.client.me(self.channels, action)

  def join(self, channel):
    self.client.leave(self.channels, 'Leaving...')
    self.channels = channel
    self.client.join(self.channels)

  def nick(self, nick):
    self.nickname = nick
    if hasattr(self, 'client'):
      self.client.setNick(self.nickname)

# Just for a little testing.
if __name__ == '__main__':
  import sys
  f = PalantirClientFactory(sys.argv[1])
  reactor.connectTCP('irc.freenode.net', 6667, f)
  reactor.run()
