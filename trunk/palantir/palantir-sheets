#!/usr/bin/env python

''' sheetViewer

A small program for viewing and creating character sheets.  Does not build new
layouts for character sheets, can only use existing layouts and create new data
files using those layouts.
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

import sys, gtk, gtk.glade
from random import randint
from GtkPalantir import SheetElements
from Palantir.Sheet import Sheet
from Palantir.Character import Character

def init(filename=None):
  ''' Initialize the window and stuff using glade. '''
  # Create the window from the XML file.
  global tree
  tree = gtk.glade.XML('data/sheetviewer.glade')
  tree.OpenSheetDialog = tree.get_widget('OpenSheetDialog')
  tree.SaveSheetDialog = tree.get_widget('SaveSheetDialog')
  tree.ViewPort = tree.get_widget('SheetViewPort')

  # Create the window with a sheet already loaded.
  if filename:
    data = Character()
    data.readCharacter(filename)
    tree.sheet = Sheet(data, DieRoller(), SheetElements)
    tree.sheet.filename = filename
    tree.ViewPort.add(tree.sheet.root)
    tree.sheet.root.edit.set_active(gtk.TRUE)
    tree.sheet.root.show()

  for func in globals().iterkeys():
    if func.startswith('on_'):
      tree.signal_connect(func, globals()[func])

##### Signal handlers #####
def on_window_destroy(widget=None, data=None):
  gtk.main_quit()

def on_quit_activate(widget=None, data=None):
  gtk.main_quit()

def on_New_activate(widget, data=None):
  OpenFileDialog(NewSheet, 'layout/')

def on_Open_activate(widget, data=None):
  OpenFileDialog(OpenSheet, 'data/')

def on_Save_activate(widget, data=None):
  if hasattr(tree, 'sheet') and hasattr(tree.sheet, 'filename'):
    tree.sheet.root.applyChanges()
    tree.sheet.character.writeOut()
  elif hasattr(tree, 'sheet'):
    OpenFileDialog(SaveSheet, 'data/')
  else:
    print 'Error: No sheet to save.'

def on_Save_As_activate(widget, data=None):
  OpenFileDialog(SaveSheet, 'data/')

def OpenFileDialog(okCallback, defaultDir):
  ''' Open up the file selector dialog and connect the ok button the callback
      'okCallback'.
      '''
  tree.dialog = gtk.glade.XML('data/sheetselection.glade')
  tree.dialog.signal_autoconnect({'on_ok_button_clicked' : okCallback,
                                  'on_cancel_button_clicked' : CloseDialog})
  if defaultDir:
    tree.dialog.get_widget('SheetSelection').set_filename(defaultDir)

def CloseDialog(widget=None, data=None):
  tree.dialog.get_widget('SheetSelection').destroy()

def NewSheet(widget, data=None):
  ''' Create a new sheet from the layout file selected. '''
  data = Character()
  data.makeNewDoc()
  data.setNodeAttr('/character',
                    {'layout':tree.dialog.get_widget('SheetSelection').get_filename()})
  if hasattr(tree, 'sheet'):
    tree.ViewPort.remove(tree.sheet.root)
  tree.sheet = Sheet(data, DieRoller(), SheetElements)
  tree.ViewPort.add(tree.sheet.root)
  tree.sheet.root.edit.set_active(gtk.TRUE)
  tree.sheet.root.show()
  CloseDialog()

def OpenSheet(widget, data=None):
  ''' Open a character sheet in the window. '''
  data = Character(tree.dialog.get_widget('SheetSelection').get_filename())
  if hasattr(tree, 'sheet'):
    tree.ViewPort.remove(tree.sheet.root)
  action = DieRoller()
  tree.sheet = Sheet(data, action, SheetElements)
  tree.sheet.filename = tree.dialog.get_widget('SheetSelection').get_filename()
  tree.ViewPort.add(tree.sheet.root)
  tree.sheet.root.edit.set_active(gtk.TRUE)
  tree.sheet.root.show()
  CloseDialog()

def SaveSheet(widget, data=None):
  ''' Save the current character data to a file. '''
  tree.sheet.character.filename = tree.dialog.get_widget('SheetSelection').get_filename()
  tree.sheet.root.applyChanges()
  tree.sheet.character.writeOut()
  CloseDialog()

class DieRoller:
  ''' Debugging action object for the sheet viewer.  It just prints out all the relevant
      information about the die rolls to stdout.
      '''
  def roll(self, times, sides, mods):
    ''' Roll the dice, print results to stdout. '''
    total = 0
    rolls = []

    # Make the rolls, keep track of each individual roll and the running total.
    for time in times:
      for roll in range(time):
	rolls.append(randint(1, sides))
	total += rolls[len(rolls)-1]

    # Add the modifiers.
    for mod in mods:
      total += mod

    print times, sides, mods, rolls, total

if __name__ == '__main__':
  if len(sys.argv) > 1:
    init(sys.argv[1])
  else:
    init()
  gtk.main()
