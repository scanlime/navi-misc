''' palantirIRC.py

This modules controls the IRC aspect of the Palantir RPG client.

  Copyright (C) 2004 W. Evan Sheehan <evan@navi.cx>
'''

# Note: palantirIRC must be imported before gtk because of
#       problems with the reactor.

from __future__ import nested_scopes

# Get a reactor that allows for gtk, so gtk.main doesn't get held up
# by running the reactor
from twisted.internet import gtk2reactor
gtk2reactor.install()

from twisted.protocols import irc
from twisted.internet import protocol, reactor

def Connect(factory, server='irc.freenode.net'):
  global reactor
  return getattr(reactor.connectTCP(server, 6667, factory), 'transport')

def Disconnect():
  if hasattr(globals(), 'reactor'):
    reactor.stop()

def Start():
  reactor.run()
class PalantirClient(irc.IRCClient):
  ''' This is the class that handles all the nitty gritty of the IRC
      stuff.  Yay for twisted making things pretty easy on me.
      '''
  def __init__(self):
    self.realname = 'Palantir'

  def Send(self, msg):
    self.say('#palantir', msg)

  ### IRC Callbacks ###
  def connectionMade(self):
    print 'connected'
    self.nickname = self.factory.nick
    irc.IRCClient.connectionMade(self)

  def connectionLost(self, reason):
    print 'Connection Lost:', reason

  def signedOn(self):
    self.join(self.factory.channel)

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
