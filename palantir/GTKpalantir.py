import gtk, gtk.glade
from CharacterSheet.Character import Character
from CharacterSheet.GTKsheet import GTKsheet

def Init():
  ''' Show the main window. '''
  global tree
  tree = gtk.glade.XML('palantirMain.glade')
  for func in globals().iterkeys():
    if func.startswith('on_'):
      tree.signal_connect(func, globals()[func])

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

def on_Main_destroy(widget, data=None):
  gtk.main_quit()

def on_quit_activate(widget, data=None):
  gtk.main_quit()
