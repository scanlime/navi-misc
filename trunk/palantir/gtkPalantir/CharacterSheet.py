''' CharacterSheet.py

Object for dealing with the character sheets loaded into the MainWindow.

  Copyright (C) 2004 W. Evan Sheehan
'''

class CharacterSheet:
  def __init__(self, viewport):
    self.sheet = None
    self.view = viewport

  def openSheet(self):
    self.tree.dialog = gtk.glade.XML('data/sheetselection.glade')
    self.tree.dialog.get_widget('SheetSelection').set_filename('CharacterSheet/data/')
    self.tree.dialog.signal_autoconnect({ 'on_ok_button_clicked':self.installSheet,
      'on_cancel_button_clicked':lambda w: self.tree.dialog.get_widget('SheetSelection').destroy()})
  
  def installSheet(self, widget, data=None):
    ''' Open up a character sheet in the client. '''
    # Store the character data.
    self.data = Character(self.tree.dialog.get_widget('SheetSelection').get_filename())

    # If we've already loaded a sheet it needs to be removed.
    if self.sheet != None:
      self.characterSheetWindow.remove(self.sheet.root)

    # Create a new sheet.
    self.sheet = Sheet(self.data, self.dieRoller, CharacterSheet.GtkSheetElements)

    # Store the filename the sheet was read from... (why did I do this?)
    self.filename = self.tree.dialog.get_widget('SheetSelection').get_filename()
    # Add the sheet to the CharacterViewPort and show it.
    self.view.add(self.sheet.root)
    self.sheet.root.show()

    # Kill the file selector.
    self.tree.dialog.get_widget('SheetSelection').destroy()
    self.view.show()
