''' CharacterSheet.py

Object for dealing with the character sheets loaded into the MainWindow.
'''

# Copyright (C) 2003 W. Evan Sheehan <evan@navi.cx>
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

from Common.Sheet import Sheet
from Common.Character import Character
import SheetElements, gtk, gtk.glade

class CharacterSheet:
  def __init__(self, viewport):
    self.sheet = None
    self.view = viewport

  def openSheet(self, widget=None, data=None):
    self.dialog = gtk.glade.XML('data/sheetselection.glade')
    self.dialog.get_widget('SheetSelection').set_filename('CharacterSheet/data/')
    self.dialog.signal_autoconnect({ 'on_ok_button_clicked':self.installSheet,
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
    self.dialog.get_widget('SheetSelection').destroy()
    self.view.show()
