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
        self.unsetCurrentChannel()

        # Connect signals on the RulesetEditor buffer
        self.xml.get_widget('RulesetEditor').get_buffer().connect(
            "modified_changed", self.on_RulesetBuffer_modified_changed)

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
        self.loadChannelListModel()
        self.currentChannel = None

        column = gtk.TreeViewColumn("Channel", gtk.CellRendererText(), text=0)
        column.set_sort_column_id(0)
        view.append_column(column)

        column = gtk.TreeViewColumn("Server", gtk.CellRendererText(), text=1)
        column.set_sort_column_id(1)
        view.append_column(column)

    def loadChannelListModel(self):
        """Generate a new model corresponding to the current ruleset dict
           and assign it to our channel list view.
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
        self.xml.get_widget('ChannelList').set_model(model)

    def on_ChannelList_cursor_changed(self, tree):
        """A new row may have been selected in the channel list"""
        model, i = tree.get_selection().get_selected()
        channel = model.get_value(i, 0)
        server = model.get_value(i, 1)
        if (channel, server) != self.currentChannel:
            self.setCurrentChannel(channel, server)

    def on_RulesetBuffer_modified_changed(self, buffer):
        if buffer.get_modified():
            self.xml.get_widget('RevertButton').set_sensitive(gtk.TRUE)
            self.xml.get_widget('ApplyButton').set_sensitive(gtk.TRUE)
        else:
            self.xml.get_widget('RevertButton').set_sensitive(gtk.FALSE)
            self.xml.get_widget('ApplyButton').set_sensitive(gtk.FALSE)

    def setCurrentChannel(self, channel, server):
        """Set the channel whose ruleset we're now editing"""
        self.xml.get_widget('DeleteButton').set_sensitive(gtk.TRUE)
        self.xml.get_widget('RulesetEditor').set_sensitive(gtk.TRUE)

        self.currentChannel = channel, server

        # Show the new channel/server name
        self.xml.get_widget('RulesetName').set_markup("<big><b>%s</b> on %s</big>" % (channel, server))

        self.loadRuleset()

    def unsetCurrentChannel(self):
        """Called during initialization or when the current channel is deleted.
           Sets all the toolbar buttons insensitive, removes the contents
           of the ruleset editor, and clears the current ruleset name.
           """
        self.xml.get_widget('DeleteButton').set_sensitive(gtk.FALSE)
        self.xml.get_widget('RevertButton').set_sensitive(gtk.FALSE)
        self.xml.get_widget('ApplyButton').set_sensitive(gtk.FALSE)
        self.xml.get_widget('RulesetEditor').set_sensitive(gtk.FALSE)
        self.xml.get_widget('RulesetEditor').get_buffer().set_text('')
        self.xml.get_widget('RulesetName').set_markup('')
        self.currentChannel = None

    def loadRuleset(self):
        """Load the ruleset from the current channel into our editor widget"""
        ruleset = self.rulesets[self.currentChannel]
        buffer = self.xml.get_widget('RulesetEditor').get_buffer()
        buffer.set_text(ruleset.toxml())
        buffer.set_modified(gtk.FALSE)

    def on_RefreshButton_clicked(self, button):
        """Reload all IRC rulesets from the server and regenerate the channel list model"""
        self.queryRulesets()
        self.loadChannelListModel()

    def on_ApplyButton_clicked(self, button):
        """Send our ruleset to the server, and assuming it's valid set
           the modified flag to False
           """
        self.applyCurrentRuleset()
        buffer = self.xml.get_widget('RulesetEditor').get_buffer()
        buffer.set_modified(gtk.FALSE)

    def on_RevertButton_clicked(self, button):
        """Reload the original ruleset, discarding changes"""
        self.loadRuleset()

    def applyCurrentRuleset(self):
        """Send the ruleset currently being edited back to the CIA server"""
        channel, server = self.currentChannel
        buffer = self.xml.get_widget('RulesetEditor').get_buffer()
        ruleset = buffer.get_text(*buffer.get_bounds())
        self.server.deliverMessage(
            "<message><body><ircRuleset channel=%r server=%r>%s</ircRuleset></body></message>" %
            (channel, server, ruleset))

    def on_NewButton_clicked(self, button):
        """Show our dialog box for creating new rulesets for a given channel"""
        self.xml.get_widget("NewRulesetDialog").show()

    def on_SingleProject_toggled(self, button):
        self.xml.get_widget("ProjectNameTable").set_sensitive(button.get_active())

    def on_RulesetDialogCancel_clicked(self, button):
        self.xml.get_widget("NewRulesetDialog").hide()

    def on_DeleteButton_clicked(self, button):
        # Delete the ruleset in our local channel list and on the server
        del self.rulesets[self.currentChannel]
        self.loadChannelListModel()
        self.server.deliverMessage(
            "<message><body><ircRuleset channel=%r server=%r/></body></message>" %
            self.currentChannel)
        self.unsetCurrentChannel()

    def on_RulesetDialogOk_clicked(self, button):
        # Create the new ruleset according to our dialog's settings
        if self.xml.get_widget("AllProjects").get_active():
            # All projects
            ruleset = '<ruleset>\n\t<formatter medium="irc"/>\n\t<formatter name="IRCProjectName"/>\n</ruleset>'
        elif self.xml.get_widget("SingleProject").get_active():
            # One project
            projectName = self.xml.get_widget("SingleProjectName").get_text()
            ruleset = ('<ruleset>\n\t<match path="/message/source/project">' +
                       projectName + '</match>\n\t<formatter medium="irc"/>\n</ruleset>')
        else:
            # Empty
            ruleset = "<ruleset/>"

        # What channel and server is this for?
        channel = self.xml.get_widget("ChannelEntry").get_text()
        server = self.xml.get_widget("ServerEntry").get_text()

        # Canonicalize the channel and server name a bit so we don't get
        # duplicates on our local list.
        if channel[0] != '#':
            channel = '#' + channel
        if server.find(":") < 0:
            server = server + ":6667"

        # Apply the ruleset in our local channel list and on the server
        self.rulesets[channel, server] = minidom.parseString(ruleset).getElementsByTagName('ruleset')[0]
        self.loadChannelListModel()
        self.setCurrentChannel(channel, server)
        self.applyCurrentRuleset()

        # Kerpoof, hide the dialog
        self.xml.get_widget("NewRulesetDialog").hide()


if __name__ == "__main__":
    # Locate our glade file by looking in the 'data' directory wherever our source file is located.
    gladeFile = os.path.join(os.path.dirname(sys.argv[0]), "data", "ruleset_editor.glade")
    ui = RulesetEditor(gladeFile, "http://yoshi:3910")
    ui.window.connect("destroy", gtk.mainquit)
    gtk.main()

### The End ###
