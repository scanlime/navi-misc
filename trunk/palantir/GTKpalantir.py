''' GTKpalantir.py

This module creates a UI for a Palantir client using GTK, it uses palantirIRC
for the IRC stuff.

  Copyright (C) 2004 W. Evan Sheehan
'''

# Install the gtk2reactor since we're using gtk2.
from twisted.internet import gtk2reactor
gtk2reactor.install()

import re, gtk, gtk.glade, palantirIRC
from random import randint
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
    self.factory = palantirIRC.PalantirClientFactory('nuku-nuku', ui=self)
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
    self.ConnectionDialog()
    #reactor.connectTCP('irc.freenode.net', 6667, self.factory)
    #reactor.run()

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
  def ConnectionDialog(self):
    ''' Brings up a small dialog for creating a connection, prompts for a server and a
        channel name.
	'''
    dialog = gtk.Dialog('Connection', self.tree.get_widget('Main'),
	                gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT)

    serverLabel = gtk.Label('Server:')
    serverLabel.show()
    serverArea = gtk.Entry()
    serverArea.show()
    serverBox = gtk.HBox(spacing=7)
    serverBox.pack_start(serverLabel)
    serverBox.pack_start(serverArea, fill=gtk.FALSE, padding=5)
    serverBox.show()

    channelLabel = gtk.Label('Channel:')
    channelLabel.show()
    channelArea = gtk.Entry()
    channelArea.show()
    channelBox = gtk.HBox()
    channelBox.pack_start(channelLabel)
    channelBox.pack_start(channelArea, fill=gtk.FALSE, padding=5)
    channelBox.show()

    ok = gtk.Button(stock=gtk.STOCK_OK)
    ok.connect('clicked', lambda w: self.Connect(serverArea.get_text(),
                                                 channelArea.get_text(),
						 dialog))
    ok.show()

    cancel = gtk.Button(stock=gtk.STOCK_CANCEL)
    cancel.connect('clicked', lambda w: dialog.destroy())
    cancel.show()

    dialog.action_area.pack_start(cancel)
    dialog.action_area.pack_start(ok)

    dialog.vbox.pack_start(serverBox, padding=5)
    dialog.vbox.pack_start(channelBox, padding=5)

    dialog.show()

  def Connect(self, server, channel=None, dialog=None):
    if channel:
      self.factory.AddChannel(channel)
    self.factory.SetServer(server)
    if dialog:
      dialog.destroy()
    reactor.connectTCP(server, 6667, self.factory)
    reactor.run()

  def OpenSheet(self, widget, data=None):
    # Store the character data.
    self.data = Character(self.tree.dialog.get_widget('SheetSelection').get_filename())

    # If we've already loaded a sheet it needs to be removed.
    if hasattr(self, 'sheet'):
      self.tree.get_widget('CharacterViewPort').remove(self.sheet.root)
    # Create a new sheet.
    self.sheet = GTKsheet(self.data, DieRoller(self))
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


class DieRoller:
  ''' This is the class that handles the dice rolling for the client.  When a character
      sheet is loaded into the window it will get an instance of this class to use for
      button presses.
      '''
  def __init__(self, ui):
    ''' The roller needs to have a reference to the UI so that it can display the results
        of the dice rolls.
	'''
    self.ui = ui

  # All action objects for the character sheet are required to have this function.
  def roll(self, times, sides, mods):
    ''' Roll the dice. '''
    total = 0
    totalTimes = 0
    rolls = []

    # Roll the dice and store each individual roll as well as a running total
    for time in times:
      totalTimes += time

    for roll in range(totalTimes):
	rolls.append(randint(1, sides))
	total += rolls[len(rolls) - 1]

    # Add the modifiers to the total.
    for mod in mods:
      total += mod

    # For now we're just printing this to stdout.
    self.ui.factory.Send(self.ui.factory.channels[0],
	'Rolled: ' + str(totalTimes) + 'd' + str(sides) + ' => ' + str(total))

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
