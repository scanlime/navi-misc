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
    ''' The realname for every client will be the same so that it's easy to tell who's
        using Palantir and who isn't, that way you won't necessarily flood a channel with
        CTCP messages.
	'''
    self.realname = 'Palantir'

  ### IRC Callbacks ###
  def connectionMade(self):
    # Set the nickname.
    self.nickname = self.factory.nickname
    irc.IRCClient.connectionMade(self)

  def connectionLost(self, reason):
    print 'Connection Lost:', reason

  def signedOn(self):
    ''' Join the channels in factory.channels upon connecting. '''
    for channel in self.factory.channels:
      self.join(channel)

  def privmsg(self, user, channel, msg):
    ''' Called when a private message is recieved. '''
    if self.factory.ui:
      self.factory.ui.messageReceive(user, channel, msg)
    else:
      print '<', user, '>', msg, '\n'

  def action(self, user, channel, msg):
    ''' Called when someone emotes something. '''
    if self.factory.ui:
      self.factory.ui.meReceive(user, channel, msg)
    else:
      print '*', user, ' ', msg, '\n'

  #def irc_NICK(self, prefix, params):
    #if self.factory.ui:
      #self.factory.ui.nickReceive(params[0])

class PalantirClientFactory(protocol.ClientFactory):
  ''' Factory to create the IRC client.  The factory will be used as an interface
      between the client and the ui, so the factory saves a reference to the ui and
      the ui should save a reference to the factory.  The client that is created
      saves a reference to the factory by default.
      '''
  protocol = PalantirClient

  def __init__(self, nick, server='irc.freenode.net', channels='#palantir', ui=None):
    ''' 'channels' should be a string of channels, separated by commas that the client
        will join upon connecting.  'ui' should be a reference to the ui that the client
	will use.  The rest should be apparent.
	'''
    self.nickname = nick
    self.server = server
    self.channels = [channel.strip() for channel in channels.split(',')]
    self.ui = ui

  def clientConnectionLost(self, connector, reason):
    ''' Reconnect if we lose a connection. '''
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

  def Send(self, channel, msg):
    ''' Interface between client and UI, sends a message to a channel. '''
    self.client.say(channel, msg)

  ### IRC Commands ###
  def me(self, action):
    ''' Emote something. '''
    self.client.me(self.channels, action)

  def join(self, channel):
    ''' Join a channel '''
    self.client.leave(self.channels, 'Leaving...')
    self.channels.append(channel)
    self.client.join(channel)

  def nick(self, nick):
    ''' Change your nick. '''
    self.nickname = nick
    if hasattr(self, 'client'):
      self.client.setNick(self.nickname)


# Just for a little testing.
if __name__ == '__main__':
  import sys
  f = PalantirClientFactory(sys.argv[1])
  reactor.connectTCP('irc.freenode.net', 6667, f)
  reactor.run()
