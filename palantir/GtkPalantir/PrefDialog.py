''' PrefDialog.py

Creates and controls the preference dialog.
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

import gtk, gtk.glade, gobject
from Common.Prefs import Prefs
from gtk.gdk import color_parse

class PrefDialog:
  def __init__(self, tree, prefs=None):
    self.tree = tree
    dialog = tree.get_widget('Preferences')
    tree.get_widget('pref notebook').set_show_tabs(gtk.FALSE)

    self.SetUpNav()

    if prefs==None:
      self.prefs = Prefs()
    else:
      self.prefs = prefs

    # Create an object for the general prefs.
    self.general = GenPrefs(self.tree)
    self.general.Set(self.prefs, self.tree)

    # Create an object for the color prefs.
    self.colors = ColorPrefs(self.tree)
    self.colors.set(self.prefs, self.tree)

    # Autoconnect the signals that can be autoconnected.
    for func in self.__class__.__dict__.iterkeys():
      if func.startswith('on_'):
        self.tree.signal_connect(func, getattr(self, func))

    # Hook up the buttons. The 'Apply' button is hooked up in the main window.
    tree.get_widget('pref cancel').connect('clicked',lambda w: dialog.hide())
    tree.get_widget('pref ok').connect('clicked', self.SavePrefs)
    tree.get_widget('pref ok').connect('clicked', lambda w: dialog.hide())

    # Hook up the navigation area.
    tree.get_widget('prefs navigation').get_selection().connect('changed', 
                                                self.nav_selection_changed)

  def SetUpNav(self):
    ''' SetUpNav creates the list of pages in the navigation list from the pages
        in the notebook.
        '''
    navigation = self.tree.get_widget('prefs navigation')
    notebook = self.tree.get_widget('pref notebook')

    store = gtk.ListStore(gobject.TYPE_STRING, gobject.TYPE_INT)
    cell = gtk.CellRendererText()
    cell.visible = gtk.TRUE
    navigation.set_model(model=store)
    navigation.append_column(gtk.TreeViewColumn('Pages', cell, text=0))

    for num in range(notebook.get_n_pages()):
      page = notebook.get_nth_page(num)
      pageName = notebook.get_tab_label_text(page)
      store.set(store.append(), 0, pageName, 1, num)

  def SavePrefs(self, widget, data=None):
    self.general.Save(self.tree, self.prefs)
    self.prefs.Save()

  def nav_selection_changed(self, treeselection, data=None):
    model, iter = treeselection.get_selected()
    notebook = self.tree.get_widget('pref notebook')
    notebook.set_current_page(model.get(iter, 1)[0])

  def on_text_color_button_color_set(self, button, data=None):
    color = button.get_color()
    red = str(hex(color.red))[2:]
    blue = str(hex(color.blue))[2:]
    green = str(hex(color.green))[2:]
    self.prefs.text_color = '#' + red + green + blue

  def on_background_color_button_color_set(self, button, data=None):
    color = button.get_color()
    red = str(hex(color.red))[2:]
    blue = str(hex(color.blue))[2:]
    green = str(hex(color.green))[2:]
    self.prefs.background_color = '#' + red + green + blue

class GenPrefs:
  def __init__(self, tree):
    self.page = tree.get_widget('general')

    # Make all the text entries the same width.
    sizegroup = gtk.SizeGroup(gtk.SIZE_GROUP_HORIZONTAL)
    sizegroup.add_widget(tree.get_widget('nickname'))
    sizegroup.add_widget(tree.get_widget('realname'))
    sizegroup.add_widget(tree.get_widget('quitmsg'))
    sizegroup.add_widget(tree.get_widget('partmsg'))
    sizegroup.add_widget(tree.get_widget('awaymsg'))

    sizegroup.add_widget(tree.get_widget('highlight'))

    # Little formatting for the highlighting area.
    sizegroup = gtk.SizeGroup(gtk.SIZE_GROUP_VERTICAL)
    sizegroup.add_widget(tree.get_widget('add_remove'))
    sizegroup.add_widget(tree.get_widget('highlight'))

  def Save(self, tree, prefs):
    ''' Save the values on the general preferences page into 'prefs'. prefs must
        be a Prefs object from Common.Prefs
        '''
    prefs.nickname = tree.get_widget('nickname').get_text()
    prefs.realname = tree.get_widget('realname').get_text()
    prefs.quitmsg = tree.get_widget('quitmsg').get_text()
    prefs.partmsg = tree.get_widget('partmsg').get_text()
    prefs.awaymsg = tree.get_widget('awaymsg').get_text()

  def Set(self, prefs, tree):
    ''' Set the fields in the dialog to reflect the user's prefs. 'prefs' must be
        an instance of Prefs from Common.Prefs
        '''
    tree.get_widget('nickname').set_text(getattr(prefs, 'nickname', ''))
    tree.get_widget('realname').set_text(getattr(prefs, 'realname', 'Unknown'))
    tree.get_widget('quitmsg').set_text(getattr(prefs, 'quitmsg', 'good-bye'))
    tree.get_widget('partmsg').set_text(getattr(prefs, 'partmsg', 'Leaving...'))
    tree.get_widget('awaymsg').set_text(getattr(prefs, 'awaymsg', 'Away...'))

class ColorPrefs:
  def __init__(self, tree):
    self.page = tree.get_widget('colors')

    sizegroup = gtk.SizeGroup(gtk.SIZE_GROUP_HORIZONTAL)
    sizegroup.add_widget(tree.get_widget('color label 1'))
    sizegroup.add_widget(tree.get_widget('color label 2'))
    sizegroup.add_widget(tree.get_widget('color label 3'))
    sizegroup.add_widget(tree.get_widget('color label 4'))

  def set(self, prefs, tree):
    tree.get_widget('text color button').set_color(color_parse(prefs.text_color))
    tree.get_widget('background color button').set_color(color_parse(prefs.background_color))
