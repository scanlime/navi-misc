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

class PrefDialog:
  def __init__(self, tree):
    self.tree = tree
    dialog = tree.get_widget('Preferences')
    tree.get_widget('pref notebook').set_show_tabs(gtk.FALSE)

    self.SetUpNav()

    self.prefs = Prefs()

    # Create an object for the general prefs.
    self.general = GenPrefs(self.tree)
    self.general.Set(self.prefs)

    # Hook up the buttons.
    tree.get_widget('pref cancel').connect('clicked',lambda w: dialog.hide())
    tree.get_widget('pref apply').connect('clicked', self.SavePrefs)
    tree.get_widget('pref ok').connect('clicked', self.SavePrefs)
    tree.get_widget('pref ok').connect('clicked', lambda w: dialog.hide())

  def SetUpNav(self):
    ''' SetUpNav creates the list of pages in the navigation list from the pages
        in the notebook.
        '''
    navigation = self.tree.get_widget('prefs navigation')
    notebook = self.tree.get_widget('pref notebook')

    store = gtk.ListStore(gobject.TYPE_STRING)
    cell = gtk.CellRendererText()
    cell.visible = gtk.TRUE
    navigation.set_model(model=store)
    navigation.append_column(gtk.TreeViewColumn('Pages', cell, text=0))

    for num in range(notebook.get_n_pages()):
      page = notebook.get_nth_page(num)
      pageName = notebook.get_tab_label_text(page)
      store.set(store.append(), 0, pageName)

  def SavePrefs(self, widget, data=None):
    self.general.Save(self.prefs)
    self.prefs.Save()

  def GetNick(self):
    return getattr(self.prefs, 'nickname', '')

  def GetQuitMsg(self):
    return self.prefs.quitmsg

  def GetPartMsg(self):
    return self.prefs.partmsg

  def GetAwayMsg(self):
    return self.prefs.awaymsg

class GenPrefs:
  def __init__(self, tree):
    self.page = tree.get_widget('general')

    # FIXME: why are we storing all of this???
    self.nickname = tree.get_widget('nickname')
    self.realname = tree.get_widget('realname')
    self.quitmsg = tree.get_widget('quitmsg')
    self.partmsg = tree.get_widget('partmsg')
    self.awaymsg = tree.get_widget('awaymsg')

    # Make all the text entries the same width.
    sizegroup = gtk.SizeGroup(gtk.SIZE_GROUP_HORIZONTAL)
    sizegroup.add_widget(self.nickname)
    sizegroup.add_widget(self.realname)
    sizegroup.add_widget(self.quitmsg)
    sizegroup.add_widget(self.partmsg)
    sizegroup.add_widget(self.awaymsg)

    sizegroup.add_widget(tree.get_widget('highlight'))

    # Little formatting for the highlighting area.
    sizegroup = gtk.SizeGroup(gtk.SIZE_GROUP_VERTICAL)
    sizegroup.add_widget(tree.get_widget('add_remove'))
    sizegroup.add_widget(tree.get_widget('highlight'))

  def Save(self, prefs):
    ''' Save the values on the general preferences page into 'prefs'. prefs must
        be a Prefs object from Common.Prefs
        '''
    prefs.nick = self.nickname.get_text()
    prefs.realname = self.realname.get_text()
    prefs.quitmsg = self.quitmsg.get_text()
    prefs.partmsg = self.partmsg.get_text()
    prefs.awaymsg = self.awaymsg.get_text()

  def Set(self, prefs):
    ''' Set the fields in the dialog to reflect the user's prefs. 'prefs' must be
        an instance of Prefs from Common.Prefs
        '''
    self.nickname.set_text(getattr(prefs, 'nickname', ''))
    self.realname.set_text(getattr(prefs, 'realname', 'Unknown'))
    self.quitmsg.set_text(getattr(prefs, 'quitmsg', 'good-bye'))
    self.partmsg.set_text(getattr(prefs, 'partmsg', 'Leaving...'))
    self.awaymsg.set_text(getattr(prefs, 'awaymsg', 'Away...'))
