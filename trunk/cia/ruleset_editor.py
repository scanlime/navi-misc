#!/usr/bin/env python
""" ruleset_editor.py

A PyGTK and Glade UI for editing CIA's IRC rulesets interactively
"""
#
# CIA open source notification system
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import gtk, gtk.glade, gobject
import xmlrpclib, os, sys
from xml.dom import minidom


class RulesetEditor:
    """A glade-based UI for editing CIA's IRC rulesets.
       connects to the CIA server over XML-RPC.

       The resulting top-level window is available via the 'window'
       attribute.
       """
    def __init__(self, gladeFile, ciaServer):
        self.xml = gtk.glade.XML(gladeFile)
        self.server = xmlrpclib.ServerProxy(ciaServer)

        # Automatically wire up all member functions starting with "on_" as signal handlers
        for name in self.__class__.__dict__.iterkeys():
            if name.startswith("on_"):
                self.xml.signal_connect(name, getattr(self, name))

        # Get an initial list of rulesets
        self.queryRulesets()

        self.window = self.xml.get_widget('RulesetEditorWindow')
        self.initChannelList(self.xml.get_widget('ChannelList'))

    def queryRulesets(self):
        """Retrieve all rulesets from the server, updating our local
           mapping from server and channel to ruleset.
           """
        self.rulesets = {}
        results = self.server.deliverMessage("<message><body><queryIrcRulesets/></body></message>")
        for ircRuleset in results:
            # Just think, all of LibCIA could be this gross if we used
            # minidom or a real dom instead of twisted's domish :)
            xml = minidom.parseString(ircRuleset)
            root = xml.getElementsByTagName('ircRuleset')[0]
            channel = root.getAttribute('channel')
            server = root.getAttribute('server')
            ruleset = root.getElementsByTagName('ruleset')[0]
            self.rulesets[(channel, server)] = ruleset

    def initChannelList(self, view):
        """Create a channel list in the given GtkTreeView widget"""
        self.loadChannelListModel(view)
        self.currentChannel = None

        column = gtk.TreeViewColumn("Channel", gtk.CellRendererText(), text=0)
        column.set_sort_column_id(0)
        view.append_column(column)

        column = gtk.TreeViewColumn("Server", gtk.CellRendererText(), text=1)
        column.set_sort_column_id(1)
        view.append_column(column)

    def loadChannelListModel(self, view):
        """Generate a new model corresponding to the current ruleset dict
           and assign it to the provided view.
           """
        model = gtk.ListStore(gobject.TYPE_STRING,   # (0) channel
                              gobject.TYPE_STRING,   # (1) server
                              )
        for (channel, server), ruleset in self.rulesets.iteritems():
            i = model.append()
            model.set(i,
                0, channel,
                1, server,
		)
        view.set_model(model)

    def on_ChannelList_cursor_changed(self, tree):
        """A new row may have been selected in the channel list"""
        model, i = tree.get_selection().get_selected()
        channel = model.get_value(i, 0)
        server = model.get_value(i, 1)
        if (channel, server) != self.currentChannel:
            self.setCurrentChannel(channel, server)

    def setCurrentChannel(self, channel, server):
        """Set the channel whose ruleset we're now editing"""
        self.currentChannel = channel, server
        ruleset = self.rulesets[channel, server]

        # Show the new channel/server name
        self.xml.get_widget('RulesetName').set_markup("<big><b>%s</b> on %s</big>" % (channel, server))

        # Load the ruleset into our editor widget
        self.xml.get_widget('RulesetEditor').get_buffer().set_text(ruleset.toxml())

    def on_ApplyButton_clicked(self, button):
        self.applyCurrentRuleset()

    def applyCurrentRuleset(self):
        """Send the ruleset currently being edited back to the CIA server"""
        channel, server = self.currentChannel
        buffer = self.xml.get_widget('RulesetEditor').get_buffer()
        ruleset = buffer.get_text(*buffer.get_bounds())
        self.server.deliverMessage("<message><body><ircRuleset channel=%r server=%r>%s</ircRuleset></body></message>" %
                                   (channel, server, ruleset))


if __name__ == "__main__":
    # Locate our glade file by looking in the 'data' directory wherever our source file is located.
    gladeFile = os.path.join(os.path.dirname(sys.argv[0]), "data", "ruleset_editor.glade")
    ui = RulesetEditor(gladeFile, "http://yoshi:3910")
    ui.window.connect("destroy", gtk.mainquit)
    gtk.main()

### The End ###
