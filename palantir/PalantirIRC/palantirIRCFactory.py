''' palantirIRCFactory.py

PalantirClientFactory is used to create instances of the PalantirClient. The GUI
should store a ref to the factory. The factory can be used to access the instance
of PalantirClient.

  Copyrit (C) 2004 W. Evan Sheehan <evan@navi.cx>
'''

from palantirIRCClient import PalantirClient
from twisted.internet import protocol, reactor

class PalantirClientFactory(protocol.ClientFactory):
  ''' Factory to create the IRC client.  The factory will be used as an interface
      between the client and the ui, so the factory saves a reference to the ui and
      the ui should save a reference to the factory.  The client that is created
      saves a reference to the factory by default.
      '''
  protocol = PalantirClient

  def __init__(self, nick, server=None, channels=None, ui=None):
    ''' 'channels' should be a string of channels, separated by commas that the client
        will join upon connecting.  'ui' should be a reference to the ui that the client
	will use.  The rest should be apparent.
	'''
    self.nickname = nick
    self.server = server
    if channels:
      self.channels = [channel.strip() for channel in channels.split(',')]
    else:
      self.channels = [None]
    self.ui = ui
    self.did_quit = False

  def clientConnectionLost(self, connector, reason):
    ''' Reconnect if we lose a connection. '''
    if not self.did_quit:
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

  def SendCTCP(self, channel, messages):
    ''' Send a CTCP query. '''
    self.client.ctcpMakeQuery(channel, messages)

  def SetServer(self, servername):
    self.server = servername

  def AddChannel(self, channelname):
    self.channels.append(channelname)

  ### IRC Commands ###
  def join(self, channel, msg=None):
    ''' Join a channel '''
    if None in self.channels:
      self.channels[self.channels.index(None)] = channel
    else:
      self.channels.append(channel)
    self.client.join(channel)

  def close(self, channel, reason='Leaving...'):
    self.channels.remove(channel)
    self.client.leave(channel, reason)
    if len(self.channels) is 0:
      self.channels.append(None)

  def quit(self, message='Leaving...'):
    ''' Quit IRC. '''
    self.did_quit = True
    if self.client:
      self.client.quit(message)
    self.channels = [None]
    self.client = None
