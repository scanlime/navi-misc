''' GTKpalantir.py

This module creates a UI for a Palantir client using GTK, it uses palantirIRC
for the IRC stuff.

  Copyright (C) 2004 W. Evan Sheehan
'''

import gtk, gtk.glade, palantirIRC
from CharacterSheet.Character import Character
from CharacterSheet.GTKsheet import GTKsheet

def Init():
  ''' Show the main window. '''
  global tree
  tree = gtk.glade.XML('palantirMain.glade')
  for func in globals().iterkeys():
    if func.startswith('on_'):
      tree.signal_connect(func, globals()[func])

  global client
  client = palantirIRC.PalantirClient()
  client.factory = palantirIRC.PalantirClientFactory('Silme')

def on_new_connection_activate(widget, data=None):
  client.makeConnection(palantirIRC.Connect(client.factory))
  palantirIRC.Start()

def on_character_sheet_activate(widget, data=None):
  if tree.get_widget('character_sheet').get_active():
    tree.get_widget('CharacterSheetPane').show()
  else:
    tree.get_widget('CharacterSheetPane').hide()

def on_open_sheet_activate(widget, data=None):
  tree.dialog = gtk.glade.XML('CharacterSheet/sheetselection.glade')
  tree.dialog.get_widget('SheetSelection').set_filename('CharacterSheet/data/')
  tree.dialog.signal_autoconnect({ 'on_ok_button_clicked':OpenSheet,
   'on_cancel_button_clicked':lambda w: tree.dialog.get_widget('SheetSelection').destroy()})

def OpenSheet(widget, data=None):
  data = Character(tree.dialog.get_widget('SheetSelection').get_filename())
  if hasattr(tree, 'sheet'):
    tree.get_widget('CharacterViewPort').remove(tree.sheet.root)
  tree.sheet = GTKsheet(data)
  tree.sheet.filename = tree.dialog.get_widget('SheetSelection').get_filename()
  tree.get_widget('CharacterViewPort').add(tree.sheet.root)
  tree.sheet.root.show()
  tree.dialog.get_widget('SheetSelection').destroy()

def on_SendButton_clicked(widget, data=None):
  client.Send(tree.get_widget('SendField').get_text())

def on_Main_destroy(widget, data=None):
  gtk.main_quit()

def on_quit_activate(widget, data=None):
  palantirIRC.Disconnect()
  gtk.main_quit()
