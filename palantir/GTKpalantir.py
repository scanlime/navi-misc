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
    ''' Show the main window. '''
    self.tree = gtk.glade.XML('palantirMain.glade')
    for func in self.__class__.__dict__.iterkeys():
      if func.startswith('on_'):
        self.tree.signal_connect(func, getattr(self, func))
    # Chat buffer.
    self.chatBuffer = self.tree.get_widget('ChatWindow').get_buffer()
    # Client factory.
    self.factory = palantirIRC.PalantirClientFactory('darth_balls',
	channels=('#palantir'), ui=self)
    self.tree.get_widget('Nick').set_text(self.factory.nickname)
    self.tabs = {'none':self.tree.get_widget('ChatWindow')}

  ### Must be implemented for palantirIRC to work. ###
  def messageReceive(self, user, channel, msg):
    ''' When the client receives a privmsg it calls this function. '''
    if user:
      nick = re.search('([^!]*)!?[^@]*@?.*', user).group(1)
      text = '<' + nick + '>' + msg + '\n'
    else:
      text = msg + '\n'
    buffer = self.tree.get_widget('ChatWindow').get_buffer()
    buffer.insert(buffer.get_end_iter(), text)
    self.tree.get_widget('ChatWindow').scroll_to_iter(buffer.get_end_iter(), 0.0)

  def meReceive(self, user, channel, msg):
    ''' When someone does a '/me'. '''
    nick = re.search('([^!]*).*', user).group(1)
    text = '* ' + nick + ' ' + msg + '\n'
    buffer = self.tree.get_widget('ChatWindow').get_buffer()
    buffer.insert(buffer.get_end_iter(), text)
    self.tree.get_widget('ChatWindow').scroll_to_iter(buffer.get_end_iter(), 0.0)

  def nickReceive(self, oldNick, channel, newNick):
    ''' When someone changes a nick display it. '''
    text = oldNick + ' is now known as ' + newNick + '\n'
    buffer = self.tree.get_widget('ChatWindow').get_buffer()
    buffer.insert(buffer.get_end_iter(), text)
    self.tree.get_widget('ChatWindow').scroll_to_iter(buffer.get_end_iter(), 0.0)

  ### Glade Callbacks ###
  def on_new_connection_activate(self, widget, data=None):
    reactor.connectTCP('irc.freenode.net', 6667, self.factory)
    self.tabs = {self.factory.channels:self.tabs['none']}
    self.tree.get_widget('DefaultTab').set_text(self.factory.channels)
    reactor.run()

  def on_character_sheet_activate(self, widget, data=None):
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
      print nick
      if nick is not '':
	self.factory.nick(str(nick))
	self.tree.get_widget('Nick').set_text(nick)

  def on_open_sheet_activate(self, widget, data=None):
    self.tree.dialog = gtk.glade.XML('CharacterSheet/sheetselection.glade')
    self.tree.dialog.get_widget('SheetSelection').set_filename('CharacterSheet/data/')
    self.tree.dialog.signal_autoconnect({ 'on_ok_button_clicked':self.OpenSheet,
      'on_cancel_button_clicked':lambda w: self.tree.dialog.get_widget('SheetSelection').destroy()})

  def OpenSheet(self, widget, data=None):
    self.data = Character(self.tree.dialog.get_widget('SheetSelection').get_filename())
    if hasattr(self, 'sheet'):
      self.tree.get_widget('CharacterViewPort').remove(self.sheet.root)
    self.sheet = GTKsheet(self.data)
    self.sheet.filename = self.tree.dialog.get_widget('SheetSelection').get_filename()
    self.tree.get_widget('CharacterViewPort').add(self.sheet.root)
    self.sheet.root.show()
    self.tree.dialog.get_widget('SheetSelection').destroy()

  def on_SendButton_clicked(self, widget, data=None):
    self.on_SendField_activate(self.tree.get_widget('SendField'))

  def on_SendField_activate(self, widget, data=None):
    text = widget.get_text()
    if text.startswith('/'):
      command = text[1:text.find(' ')].lower()
      arg = text[text.find(' '):].strip()
      if hasattr(self, command + 'Receive'):
        getattr(self, command + 'Receive')(self.factory.nickname,
	    self.factory.channels, arg)
      if hasattr(self.factory, command):
	getattr(self.factory, command)(arg)
    else:
      self.factory.Send(text)
      self.messageReceive(self.factory.nickname, self.factory.channels, text)
    widget.set_text('')

  def on_Main_destroy(self, widget, data=None):
    gtk.main_quit()

  def on_quit_activate(self, widget, data=None):
    gtk.main_quit()
