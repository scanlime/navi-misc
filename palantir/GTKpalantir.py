import gtk, gtk.glade

def ShowMain():
  global tree
  tree = gtk.glade.XML('palantirMain.glade')
  for func in globals().iterkeys():
    if func.startswith('on_'):
      tree.signal_connect(func, globals()[func])

def on_character_sheet_activate(widget, data=None):
  global tree
  if tree.get_widget('character_sheet').get_active():
    tree.get_widget('CharacterSheetPane').show()
  else:
    tree.get_widget('CharacterSheetPane').hide()
