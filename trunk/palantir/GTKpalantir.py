''' GTKpalantir.py

This module creates a UI for a Palantir client using GTK, it uses palantirIRC
for the IRC stuff.

  Copyright (C) 2004 W. Evan Sheehan
'''

# Install the gtk2reactor since we're using gtk2.
from twisted.internet import gtk2reactor
gtk2reactor.install()

import re, gtk, gtk.glade, palantirIRC
from CharacterSheet.Character import Character
from CharacterSheet.GTKsheet import GTKsheet
from twisted.internet import reactor

class PalantirWindow:
  ''' Creates the main window. '''
  def __init__(self):
    ''' Create the layout tree from the .glade file and connect everything. '''
    self.tree = gtk.glade.XML('palantirMain.glade')

    # Connect the functions to their appropriate widgets.  By convention all callbacks
    # for the window start with 'on_' and match the name of the callback defined in 
    # the .glade file.
    for func in self.__class__.__dict__.iterkeys():
      if func.startswith('on_'):
        self.tree.signal_connect(func, getattr(self, func))

    # Client factory.
    self.factory = palantirIRC.PalantirClientFactory('nuku-nuku',
	channels='#palantir', ui=self)
    self.tree.get_widget('Nick').set_text(self.factory.nickname)

  ### Must be implemented for palantirIRC to work.  These methods are called by the
  ### the client when 
  def messageReceive(self, user, channel, msg):
    ''' When the client receives a privmsg it calls this function to display the message
        in the UI, however the UI sees fit.
	'''
    # Format the nick if there is one, otherwise just display the text.
    if user:
      nick = re.search('([^!]*)!?[^@]*@?.*', user).group(1)
      text = '<' + nick + '>' + msg + '\n'
    else:
      text = msg + '\n'
    self.PrintText(text)

  def meReceive(self, user, channel, msg):
    ''' When someone does a '/me' display the action. '''
    # Format the nick.
    nick = re.search('([^!]*).*', user).group(1)
    text = '* ' + nick + ' ' + msg + '\n'
    self.PrintText(text)

  def nickReceive(self, oldNick, channel, newNick):
    ''' When someone changes a nick display it. '''
    text = oldNick + ' is now known as ' + newNick + '\n'
    self.PrintText(text)

  def topicReceive(self, user, channel, topic):
    ''' Recieved a topic change, so set the topic bar to the new topic. '''
    self.tree.get_widget('Topic').set_text(topic)

  def joinReceive(self, user, channel, newChannel):
    ''' When we join a new channel close the connection to the current channel and join
        the new one.
	'''
    self.closeReceive(user, channel)
    self.factory.join(newChannel)

  def closeReceive(self, user, channel, msg='Leaving...'):
    ''' When we leave a channel close the connection to the channel and clear the topic
        and userlist from that channel.
	'''
    self.factory.close(channel, msg)
    self.tree.get_widget('UserList').get_buffer().set_text('')
    self.tree.get_widget('Topic').set_text('')

  ### Glade Callbacks ###
  # Menu Items.
  def on_new_connection_activate(self, widget, data=None):
    ''' Create a new connection. '''
    reactor.connectTCP('irc.freenode.net', 6667, self.factory)
    reactor.run()

  def on_character_sheet_activate(self, widget, data=None):
    '''Show the part of the window that displays character sheets. '''
    if self.tree.get_widget('character_sheet').get_active():
      self.tree.get_widget('CharacterSheetPane').show()
    else:
      self.tree.get_widget('CharacterSheetPane').hide()

  def on_set_character_as_nick_activate(self, widget, data=None):
    ''' Looks in the name tag of the data for a character name and sets that
        as the user's nick.
	'''
    if hasattr(self, 'data'):
      nick = self.data.getData('/character/name')
      if nick is not '':
	self.factory.nick(str(nick))
	self.tree.get_widget('Nick').set_text(nick)

  def on_open_sheet_activate(self, widget, data=None):
    ''' Open a character sheet, it will displayed in the CharacterSheetPane if that
        is visible.
	'''
    self.tree.dialog = gtk.glade.XML('CharacterSheet/sheetselection.glade')
    self.tree.dialog.get_widget('SheetSelection').set_filename('CharacterSheet/data/')
    self.tree.dialog.signal_autoconnect({ 'on_ok_button_clicked':self.OpenSheet,
      'on_cancel_button_clicked':lambda w: self.tree.dialog.get_widget('SheetSelection').destroy()})

  # Not Menu Items.
  def on_SendButton_clicked(self, widget, data=None):
    self.on_SendField_activate(self.tree.get_widget('SendField'))

  def on_SendField_activate(self, widget, data=None):
    ''' Send a message. '''
    # Get the message.
    text = widget.get_text()

    # If the message is a command (i.e. starts with '/') try and execute that command.
    if text.startswith('/'):
      # Strip out the slash and the arguments to the command.
      command = text[1:text.find(' ')].lower()

      # Get the arguments to the command.
      arg = text[text.find(' '):].strip()

      # If the client has a method for the command execute it and assume the UI has a
      # matching receive method.
      if hasattr(self.factory, command):
        getattr(self, command + 'Receive')(self.factory.nickname,
	    self.factory.channels[0], arg)
	getattr(self.factory, command)(self.factory.channels[0], arg)

    # If the message isn't a command it's a regular message.
    else:
      self.factory.Send(self.factory.channels[0], text)
      self.messageReceive(self.factory.nickname, self.factory.channels[0], text)

    # Reset the text in the text field.
    widget.set_text('')

  def on_ChannelTabs_switch_page(self, widget, data, tab):
    ''' When the current tab is changed, update self.currentTab to reflect that. '''
    self.currentTab = self.tree.get_widget('ChannelTabs').get_nth_page(tab)

  def on_Topic_activate(self, widget, data=None):
    ''' Change the topic. '''
    self.factory.topic(self.factory.channels[0], widget.get_text())

  # Closing stuff.
  def on_Main_destroy(self, widget, data=None):
    self.on_quit_activate(widget)

  def on_quit_activate(self, widget, data=None):
    gtk.main_quit()
    reactor.stop()

  ### Misc. Necessary Functions ###
  def OpenSheet(self, widget, data=None):
    # Store the character data.
    self.data = Character(self.tree.dialog.get_widget('SheetSelection').get_filename())

    # If we've already loaded a sheet it needs to be removed.
    if hasattr(self, 'sheet'):
      self.tree.get_widget('CharacterViewPort').remove(self.sheet.root)
    # Create a new sheet.
    self.sheet = GTKsheet(self.data)
    # Store the filename the sheet was read from... (why did I do this?)
    self.sheet.filename = self.tree.dialog.get_widget('SheetSelection').get_filename()
    # Add the sheet to the CharacterViewPort and show it.
    self.tree.get_widget('CharacterViewPort').add(self.sheet.root)
    self.sheet.root.show()

    # Kill the file selector.
    self.tree.dialog.get_widget('SheetSelection').destroy()

  def AddUserToList(self, nick, channel):
    ''' Add nick the userlist. '''
    buffer = self.tree.get_widget('UserList').get_buffer()
    buffer.insert(buffer.get_end_iter(), nick + '\n')

  def PrintText(self, text):
    ''' Print the text in the chat buffer. '''
    buffer = self.tree.get_widget('ChatArea').get_buffer()
    buffer.insert(buffer.get_end_iter(), text)
    self.tree.get_widget('ChatArea').scroll_to_iter(buffer.get_end_iter(), 0.0)


### This was created for doing tabbed chatting, so that you could connect to multiple
### channels.  The client still supports multiple channels, but the UI does not.  I'm
### leaving this here in case, at some later date, it becomes necessary to reimplement
### the tabs.
class ChatWindowUI(gtk.ScrolledWindow):
  ''' Objects for creating text buffers for displaying text from the channels.
      This will hold a reference to the name of the channel the buffer is showing.
      '''
  def __init__(self, channel='none'):
    self.channel = channel
    gtk.ScrolledWindow.__init__(self)
    self.set_policy(2, 1)
    self.chatBox = gtk.TextView()
    self.chatBox.set_cursor_visible(gtk.FALSE)
    self.chatBox.set_editable(gtk.FALSE)
    self.chatBox.set_wrap_mode(2)
    self.chatBox.show()
    self.add_with_viewport(self.chatBox)


  def show(self):
    self.chatBox.show()
    gtk.ScrolledWindow.show(self)

  def display(self, message):
    ''' Display the message. '''
    buffer = self.chatBox.get_buffer()
    buffer.insert(buffer.get_end_iter(), message)
    # Supposed to scroll the window down to see the message, but it doesn't work, ATM.
    self.chatBox.scroll_to_iter(buffer.get_end_iter(), 0.0)
