''' palantirIRC.py

The PalantirClient class handles all of the IRC protocols.
'''

# Copyright (C) 2004 W. Evan Sheehan <evan@navi.cx>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

from twisted.protocols import irc

class Client(irc.IRCClient):
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

  def signedOn(self):
    ''' Join the channels in factory.channels upon connecting. '''
    for channel in self.factory.channels:
      if channel:
        self.join(channel)

  def joined(self, channel):
    ''' When we join a channel get a list of who is in the channel. '''
    if hasattr(self.factory.ui, 'joined'):
      self.factory.ui.joined(channel)
    self.ctcpMakeQuery(channel, [('DMQUERY',None)])

  def left(self, channel):
    ''' Called when we've left a channel, print a message that we've gone. '''
    if hasattr(self.factory.ui, 'left'):
      self.factory.ui.left(channel)

  def irc_RPL_WHOREPLY(self, prefix, params):
    ''' When we get a reply from a WHO query send the nick and channel to the ui. '''
    if hasattr(self.factory.ui, 'whoReply'):
      self.factory.ui.whoReply(params)

  def irc_RPL_TOPIC(self, prefix, params):
    ''' When we join a channel and the server is informing us of the topic. '''
    if hasattr(self.factory.ui, 'setTopicOnJoin'):
      self.factory.ui.setTopicOnJoin(params)

  def irc_RPL_NAMREPLY(self, prefix, params):
    if hasattr(self.factory.ui, 'nameReply'):
      self.factory.ui.nameReply(params)

  def irc_RPL_YOURHOST(self, prefix, params):
    self.host = prefix

  def noticed(self, user, channel, message):
    self.factory.ui.messageReceive(user, channel, message)

  def privmsg(self, user, channel, msg):
    ''' Called when a private message is recieved. '''
    self.factory.ui.messageReceive(user, channel, msg)

  def action(self, user, channel, msg):
    ''' Called when someone emotes something. '''
    self.factory.ui.meReceive(user, channel, msg)

  def topicUpdated(self, user, channel, newTopic):
    if hasattr(self.factory.ui, 'topicReceive'):
      self.factory.ui.topicReceive(user, channel, newTopic)

  def ctcpUnknownQuery(self, user, channel, tag, data):
    if hasattr(self.factory.ui, 'unknownCTCP'):
      self.factory.ui.unknownCTCP(user, channel, data)

  def userJoined(self, user, channel):
    if hasattr(self.factory.ui, 'userJoined'):
      self.factory.ui.userJoined(user, channel)

  def userLeft(self, user, channel):
    if hasattr(self.factory.ui, 'userLeft'):
      self.factory.ui.userLeft(user, channel)

  def pong(self, user, secs):
    if hasattr(self.factory.ui, 'pong'):
      self.factory.ui.pong(user, secs)
