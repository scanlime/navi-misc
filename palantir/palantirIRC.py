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

  def joined(self, channel):
    ''' When we join a channel get a list of who is in the channel. '''
    self.sendLine('WHO ' + channel)
    self.factory.ui.messageReceive(None, channel, 'Joined ' + channel + '\n')
    self.factory.ui.messageReceive(None, channel, 'Topic for ' + channel + ' is: ')
    self.ctcpMakeQuery(channel, [('DMQUERY',None)])

  def left(self, channel):
    ''' Called when we've left a channel, print a message that we've gone. '''
    self.factory.ui.messageReceive(None, channel, 'Left ' + channel)

  def irc_RPL_WHOREPLY(self, prefix, params):
    ''' When we get a reply from a WHO query send the nick and channel to the ui. '''
    self.factory.ui.AddUserToList(params[5], params[1])

  def noticed(self, user, channel, message):
    self.factory.ui.messageReceive(user, channel, message)

  def privmsg(self, user, channel, msg):
    ''' Called when a private message is recieved. '''
    self.factory.ui.messageReceive(user, channel, msg)

  def action(self, user, channel, msg):
    ''' Called when someone emotes something. '''
    self.factory.ui.meReceive(user, channel, msg)

  def topicUpdated(self, user, channel, newTopic):
    self.factory.ui.topicReceive(user, channel, newTopic)
    self.factory.ui.messageReceive(user, channel, newTopic)

  def ctcpUnknownQuery(self, user, channel, tag, data):
    getattr(self.factory.ui, tag, 'unknownTCP')(user, channel, data)

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
    connector.connect()

  def buildProtocol(self, addr):
    ''' Overridden to save a reference to the instance of the actual IRC client
        so that it can be accessed via the factory that created it.
	'''
    self.client = protocol.ClientFactory.buildProtocol(self, addr)
    return self.client

  def Send(self, channel, msg):
    ''' Interface between client and UI, sends a message to a channel. '''
    self.client.say(channel, msg)

  def SendCTCP(self, channel, messages):
    ''' Send a CTCP query. '''
    self.client.ctcpMakeQuery(channel, messages)

  def SetServer(self, servername):
    self.server = servername

  def AddChannel(self, channelname):
    self.channels.append(channelname)

  ### IRC Commands ###
  def me(self, channel, action):
    ''' Emote something. '''
    self.client.me(channel, action)

  def join(self, channel, msg=None):
    ''' Join a channel '''
    self.channels.append(channel)
    self.client.join(channel)

  def close(self, channel, reason):
    self.channels.remove(channel)
    self.client.leave(channel, reason)

  def quit(self, message='Leaving...'):
    ''' Quit IRC. '''
    self.client.quit(message)

  def nick(self, channel, nick):
    ''' Change your nick. '''
    self.nickname = nick
    if hasattr(self, 'client'):
      self.client.setNick(self.nickname)

  def topic(self, channel, topic):
    self.client.topic(channel, topic)


# Just for a little testing.
if __name__ == '__main__':
  import sys
  f = PalantirClientFactory(sys.argv[1])
  reactor.connectTCP('irc.freenode.net', 6667, f)
  reactor.run()
