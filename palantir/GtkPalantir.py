''' GTKpalantir.py

This module creates a UI for a Palantir client using GTK, it uses palantirIRC
for the IRC stuff.

  Copyright (C) 2004 W. Evan Sheehan
'''

# Install the gtk2reactor since we're using gtk2.
from twisted.internet import gtk2reactor
gtk2reactor.portableInstall()

import string, re, gtk, gtk.glade, gobject, palantirIRC, CharacterSheet.GtkSheetElements
from GtkChatBuffer import GtkChatBuffer
from time import localtime
from dieRoller import DieRoller
from CharacterSheet.Character import Character
from CharacterSheet.Sheet import Sheet
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

    # Connect up the dice buttons separately because I don't know how to tell
    # glade to send data along with the callback and I don't think we need
    # separate callbacks for each of these buttons, just a bit of data indicating
    # the number of sides on the die (10 is default, so it doesn't get sent).
    self.tree.get_widget('d4').connect('clicked',
	lambda w,d: self.on_dice_button_clicked(w,d), 4)
    self.tree.get_widget('d6').connect('clicked',
	lambda w,d: self.on_dice_button_clicked(w,d), 6)
    self.tree.get_widget('d8').connect('clicked',
	lambda w,d: self.on_dice_button_clicked(w,d), 8)
    self.tree.get_widget('d10').connect('clicked',
	lambda w: self.on_dice_button_clicked(w))
    self.tree.get_widget('d12').connect('clicked',
	lambda w,d: self.on_dice_button_clicked(w,d), 12)
    self.tree.get_widget('d20').connect('clicked',
	lambda w,d: self.on_dice_button_clicked(w,d), 20)
    self.tree.get_widget('d100').connect('clicked',
	lambda w,d: self.on_dice_button_clicked(w,d), 100)
    self.tree.get_widget('wwd10').connect('clicked',
	lambda w: self.on_dice_button_clicked(w))

    # Set up our user list with a column for pixbufs and a column for text.
    list = self.tree.get_widget('UserList')
    list.set_model(model=gtk.ListStore(gtk.gdk.Pixbuf, gobject.TYPE_STRING))

    # Columns in the tree view.
    list.append_column(gtk.TreeViewColumn('Icons', gtk.CellRendererPixbuf(), pixbuf=0))
    list.append_column(gtk.TreeViewColumn('Users', gtk.CellRendererText(), text=1))

    # Set the column spacing, doesn't appear to do anything, ATM.
    list.get_column(0).set_spacing(5)
    list.get_column(1).set_spacing(5)

    # Create a chat buffer from our custom widget.
    self.chatWindow = GtkChatBuffer()
    self.tree.get_widget('ChatScroller').add_with_viewport(self.chatWindow)
    self.chatWindow.show()

    # Client factory.
    self.factory = palantirIRC.PalantirClientFactory('nuku-nuku', ui=self)

    # Create an object to handle die rolls.
    self.dieRoller = DieRoller(self)

  def GetFormattedTime(self):
    ''' Uses GetTime to retrieve the current time, but formats it in an xchat way. '''
    hour, min, sec = self.GetTime()
    time = '[' + hour + ':' + min + ':' + sec + '] '
    return time

  def GetNick(self, user):
    ''' Separates out the nick from the long string returned by IRC. '''
    return re.search('([^!]*).*', user).group(1)

  ### Must be implemented for palantirIRC to work.  These methods are called by the
  ### the client when it receives certain events and needs to display them in the UI.
  def messageReceive(self, user, channel, msg):
    ''' This handles the formatting of regular conversation.  Gets the nick of the
        person who said it, encloses it in <>, if timestamps are visible it grabs
	the formatted time and determines if we need to highlight the nick. It sends
	all this to the chat buffer UI.
	'''
    # If there's a user associated with the message, get the nick.
    if user:
      nick = self.GetNick(user)
      nickends = (' <','> ')
    else:
      nickends = ('','')
      nick = ''

    # If we're showing timestamps get the time.
    if self.tree.get_widget('time_stamps').get_active():
      time = self.GetFormattedTime()
    else:
      time = ''

    # If we're being addressed in this message.
    if msg.find(self.factory.nickname) is not -1:
      addressed = True
    else:
      addressed = False

    # Display the message in the chat window.
    self.chatWindow.DisplayText(time, nick, msg, nickends, addressed)

  def meReceive(self, user, channel, msg):
    ''' When someone does a '/me' display the action. '''
    nick = self.GetNick(user)
    # Get the time if we're displaying timestamps.
    if self.tree.get_widget('time_stamps').get_active():
      time = self.GetFormattedTime()
    else:
      time = ''

    # If our name is in the message we need to tell the chat window to highlight it.
    if msg.find(self.factory.nickname) is not -1:
      addressed = True
    else:
      addressed = False

    # Send the message to the chat window.
    self.chatWindow.DisplayText(time, nick, msg, (' * ',' '), addressed)

  def nickReceive(self, oldNick, channel, newNick):
    ''' When someone changes a nick display it. '''
    # If we're using timestamps get the time.
    if self.tree.get_widget('time_stamps').get_active():
      time = self.GetFormattedTime()
    else:
      time = ''

    self.chatWindow.DisplayText(time, '', oldNick + ' is now known as ' + newNick)

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

  # CTCP messages.
  def DM(self, user, channel, data):
    ''' The user who sent this message has DM status, so we display that icon next to their
        name in the user list.
	'''
    nick = self.GetNick(user)
    image = gtk.Image()
    image.set_from_file('pixmaps/dm.png')
    self.tree.get_widget('UserList').get_model().foreach(self.setUserIcon, (nick, image.get_pixbuf()))
    self.messageReceive(None, channel, '*** ' + nick + ' now has DM status.')

  def UNDM(self, user, channel, data):
    ''' The user who sent this message has removed DM status from themselves, so remove the
        icon from next to their name.
	'''
    nick = self.GetNick(user)
    self.tree.get_widget('UserList').get_model().foreach(self.setUserIcon, (nick, None))
    self.messageReceive(None, channel, '*** ' + nick + ' has removed DM status.')

  def DMQUERY(self, user, channel, data):
    ''' This is a request for our DM status.  If we have ourselves marked as DM we send a
        send a CTCP DM message to the person who made the query, otherwise do nothing.
	'''
    if self.tree.get_widget('dungeon_master').get_active():
      self.factory.SendCTCP(self.GetNick(user), [('DM',None)])

  def ROLL(self, user, channel, data):
    ''' Someone rolled some dice, so we'll format the results and display. '''
    nick = self.GetNick(user)
    data = re.search('(\[.*\]) ([0-9]*) ([0-9]*)', messages[0][1])
    text = nick + ' rolled a ' + str(len(data.group(1).split())) + 'd' + data.group(2) + ': ' + data.group(1) + ' => ' + data.group(3)
    self.messageReceive(None, channel, text)

  def unknownCTCP(self, user, channel, data):
    ''' Any non-specific ctcp message just gets displayed as text. '''
    nick = self.GetNick(user)
    if messages[0][1]:
      message = string.join(messages[0])
    else:
      message = messages[0][0]
    text = 'Received a CTCP ' + message + ' from ' + nick + ' (to ' + channel + ')'
    self.messageReceive(None, channel, text)

  ### Glade Callbacks ###
  # Menu Items.
  def on_new_connection_activate(self, widget, data=None):
    ''' Create a new connection. '''
    self.ConnectionDialog()

  def on_set_character_as_nick_activate(self, widget, data=None):
    ''' Looks in the name tag of the data for a character name and sets that
        as the user's nick.
	'''
    if hasattr(self, 'data'):
      nick = self.data.getData('/character/name')
      if nick is not '':
	self.factory.nick(self.factory.channels[0], str(nick))
	self.tree.get_widget('Nick').set_text(nick)

  def on_open_sheet_activate(self, widget, data=None):
    ''' Open a character sheet, it will displayed in the CharacterSheetPane if that
        is visible.
	'''
    self.tree.dialog = gtk.glade.XML('CharacterSheet/sheetselection.glade')
    self.tree.dialog.get_widget('SheetSelection').set_filename('CharacterSheet/data/')
    self.tree.dialog.signal_autoconnect({ 'on_ok_button_clicked':self.OpenSheet,
      'on_cancel_button_clicked':lambda w: self.tree.dialog.get_widget('SheetSelection').destroy()})

  def on_preferences_activate(self, widget, data=None):
    ''' Open the color selection dialog. '''
    self.tree.get_widget('colorselectiondialog').show()

  def on_dd_dice_activate(self, widget, data=None):
    ''' Set the dice system for Dungeons & Dragons. '''
    self.tree.get_widget('DiffBox').hide()
    self.tree.get_widget('wwd10').hide()
    self.tree.get_widget('DiceButtons').show()
    self.dieRoller.system = 'D&D'

  def on_white_wolf_dice_activate(self, widget, data=None):
    ''' Set the dice system for White Wolf games. '''
    self.tree.get_widget('DiffBox').show()
    self.tree.get_widget('DiceButtons').hide()
    self.tree.get_widget('wwd10').show()
    self.dieRoller.system = 'WW'

  def on_dungeon_master_activate(self, widget, data=None):
    ''' When the Dungeon Master menu item is toggled either display or hide the DM icon
        next to our nick.
	'''
    store = self.tree.get_widget('UserList').get_model()
    image = gtk.Image()
    if widget.get_active():
      image.set_from_file('pixmaps/dm.png')
      text = '*** You now have DM status.'
      ctcp = ('DM',None)
    else:
      text = '*** You removed DM status.'
      ctcp = ('UNDM',None)

    self.factory.SendCTCP(self.factory.channels[0], [ctcp])
    self.messageReceive(None, self.factory.channels[0], text)
    store.foreach(self.setUserIcon, (self.factory.nickname, image.get_pixbuf()))

  ### Color Selection Dialog ###
  def on_color_ok_button_clicked(self, widget, data=None):
    dialog = self.tree.get_widget('color_selection')
    self.chatWindow.background.modify_bg(gtk.STATE_NORMAL, dialog.get_current_color())
    self.tree.get_widget('colorselectiondialog').hide()

  def on_color_cancel_button_clicked(self, widget, data=None):
    self.tree.get_widget('colorselectiondicalog').hide()

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

  def on_dice_button_clicked(self, widget, data=10):
    ''' Whenever one of the dice buttons is clicked roll it. '''
    # Get number of times to roll and modifiers.
    time = self.tree.get_widget('times').get_text()
    mods = self.tree.get_widget('mods').get_text()
    # If we're using the White Wolf dice system we'll need to get the difficulty.
    if self.tree.get_widget('white_wolf_dice').get_active():
      diff = self.tree.get_widget('diff').get_text()
      if not diff:
	diff = 6
    else:
      diff = 0
    # Default values if the fields are blank.
    if not time:
      time = 1
    if not mods:
      mods = 0

    # Roll the dice.
    self.dieRoller.roll([int(time)], data, [int(mods)], int(diff))

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
    serverArea.connect('activate', lambda w: self.Connect(serverArea.get_text(),
                                                          channelArea.get_text(),
							  dialog))
    serverArea.show()
    serverBox = gtk.HBox(spacing=7)
    serverBox.pack_start(serverLabel)
    serverBox.pack_start(serverArea, fill=gtk.FALSE, padding=5)
    serverBox.show()

    channelLabel = gtk.Label('Channel:')
    channelLabel.show()
    channelArea = gtk.Entry()
    channelArea.connect('activate', lambda w: self.Connect(serverArea.get_text(),
                                                           channelArea.get_text(),
							   dialog))
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
      self.factory.channels[0] = channel
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
    self.sheet = Sheet(self.data, self.dieRoller, CharacterSheet.GtkSheetElements)
    # Store the filename the sheet was read from... (why did I do this?)
    self.sheet.filename = self.tree.dialog.get_widget('SheetSelection').get_filename()
    # Add the sheet to the CharacterViewPort and show it.
    self.tree.get_widget('CharacterViewPort').add(self.sheet.root)
    self.sheet.root.show()

    # Kill the file selector.
    self.tree.dialog.get_widget('SheetSelection').destroy()
    self.tree.get_widget('CharacterSheetPane').show()

  def AddUserToList(self, nick, channel):
    ''' Add nick the userlist. '''
    list = self.tree.get_widget('UserList')
    store = list.get_model()
    store.set(store.append(), 1, nick)

  def SendRoll(self, times, sides, rolls, total):
    ''' Implemented for the DieRoller used when loading character sheets.  Sends a CTCP
        to the channel with the roll information and displays the information on your
	screen as well.
	'''
    # Send a CTCP message to the channel.
    self.factory.SendCTCP(self.factory.channels[0], [('roll', [rolls, sides, total])])

    # Format the text to display in our window.
    if self.tree.get_widget('time_stamps').get_active():
      hour, min, sec = self.GetTime()
      time = '[' + hour + ':' + min + ':' + sec + ']'
    else:
      time = ''
    self.chatWindow.DisplayText(time, '', 'You rolled a ' + str(len(rolls)) + 'd' + str(sides) + ': ' + str(rolls) + ' => ' + str(total) + '\n')

  def GetTime(self):
    ''' Return the local hour, minute and seconds. '''
    time = localtime()

    hour = str(time[3])
    if len(hour) is 1: hour = '0'+hour

    min = str(time[4])
    if len(min) is 1: min = '0'+min

    sec = str(time[5])
    if len(sec) is 1: sec = '0'+sec

    return (hour, min, sec)

  def setUserIcon(self, listStore, path, iter, data):
    ''' Sets the icon next to the username in the userlist, return gtk.TRUE if and when we find the
        user specified in data, return gtk.FALSE otherwise.  data is a list or tuple of
        the username and a gdk.Pixbuf.
	'''
    # Unpack.
    name, pixbuf = data

    # Get the username we're on.
    user = listStore.get_value(iter, 1)

    if user == name:
      listStore.set(iter, 0, pixbuf)
      return gtk.TRUE
    else:
      return gtk.FALSE
