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

class PrefDialog:
  def __init__(self, tree):
    self.dialog = tree.get_widget('Preferences')
    self.general = GenPrefs(tree)
    
class GenPrefs:
  def __init__(self, tree):
    self.page = tree.get_widget('general')

    # Make all the text entries the same height.
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
