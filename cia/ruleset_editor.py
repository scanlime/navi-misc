#!/usr/bin/env python
""" ruleset_editor.py

A PyGTK and Glade UI for editing CIA's rulesets interactively.
A ruleset defines the filters and formatters used to select and
represent messages for a particular URI.
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

import gtk, gobject, sys, os, gtk.glade, xmlrpclib


class GladeUI(object):
    """A user interface of some sort based on components
       defined in a Glade XML file. Created with either a
       gtk.glade.XML instance or the name of a file to load it from.
       On startup, it automatically connects any signal handlers
       starting with 'on_'
       """
    def __init__(self, xml):
        if isinstance(xml, gtk.glade.XML):
            self.xml = xml
        else:
            self.xml = gtk.glade.XML(xml)

        # Automatically wire up all member functions starting with "on_" as signal handlers
        for name in self.__class__.__dict__.iterkeys():
            if name.startswith("on_"):
                self.xml.signal_connect(name, getattr(self, name))


class CIAClient(object):
    """A simple client interface to the CIA server, providing
       a slightly higher level interface to it than XML-RPC.
       Includes functions for querying and installing rulesets,
       with a simple cache.
       """
    def __init__(self, serverURL):
        self.server = xmlrpclib.ServerProxy(serverURL)
        self.clearCache()
        self.queryRulesets()

    def message(self, bodyContent):
        """Given the text to put inside the <body> tag, send a
           message to the CIA server and return the result.
           This includes a <generator> tag with information about us.
           """
        generatorName = "PyGTK/Glade ruleset editor"
        generator = "<generator><name>%s</name></generator>" % generatorName
        message = "<message>%s<body>%s</body></message>" % (generator, bodyContent)
        return self.server.deliverMessage(message)

    def queryUriList(self):
        """Return a list of all URIs with rulesets assigned"""
        return self.message("<queryUriList/>")

    def queryRulesets(self, uri=None):
        """Return a list of rulesets, optionally constrained to the given URI"""
        if self.rulesetCache.has_key(uri):
            return self.rulesetCache[uri]
        else:
            if uri:
                result = self.message("<queryRulesets uri='%s'/>" % uri)
            else:
                result = self.message("<queryRulesets/>")
            self.rulesetCache[uri] = result
            return result

    def setRuleset(self, uri, ruleset):
        """Set a new ruleset for the given URI, also updating our cache"""
        self.message(ruleset)
        self.rulesetCache[uri] = [ruleset]

    def clearCache(self):
        """Clear our ruleset cache"""
        self.rulesetCache = {}


class NewRulesetDialog(GladeUI):
    """Implements the dialog box used to create new rulesets.
       It is created with a callback function that is called with
       the text of a new ruleset when one is created.
       """
    def __init__(self, xml, callback):
        GladeUI.__init__(self, xml)
        self.callback = callback
        self.window = self.xml.get_widget("NewRulesetDialog")

    def show(self):
        self.window.show()

    def hide(self):
        self.window.hide()

    def on_GenericRuleset_toggled(self, button):
        self.xml.get_widget("GenericRulesetOptions").set_sensitive(button.get_active())

    def on_IRCRuleset_toggled(self, button):
        self.xml.get_widget("IRCRulesetOptions").set_sensitive(button.get_active())

    def on_SingleProject_toggled(self, button):
        self.xml.get_widget("SingleProjectOptions").set_sensitive(button.get_active())

    def updateIrcURI(self):
        """Update the URI field from the current channel and server values"""
        channel = self.xml.get_widget("ChannelEntry").get_text()
        server = self.xml.get_widget("ServerEntry").get_text()

        # Strip off the '#' from the channel for our URI
        if channel and channel[0] == '#':
            channel = channel[1:]

        self.xml.get_widget("URIEntry").set_text("irc://%s/%s" % (server, channel))

    def on_ServerEntry_changed(self, entry):
        self.updateIrcURI()

    def on_ChannelEntry_changed(self, entry):
        self.updateIrcURI()

    def on_NewRulesetCancel_clicked(self, button):
        self.hide()

    def on_NewRulesetOk_clicked(self, button):
        self.callback(self.buildRuleset())
        self.hide()

    def buildRuleset(self):
        """Actually create a ruleset from our dialog's current parameters"""
        # All rulesets have a URI
        uri = self.xml.get_widget("URIEntry").get_text()

        # Call the proper function to build the guts of the ruleset:
        if self.xml.get_widget('GenericRuleset').get_active():
            rules = self.buildGenericRules()
        elif self.xml.get_widget('IRCRuleset').get_active():
            if self.xml.get_widget('SingleProject').get_active():
                rules = self.buildSingleProjectRules(self.xml.get_widget("ProjectEntry").get_text())
            elif self.xml.get_widget('AllProjects').get_active():
                rules = self.buildAllProjectsRules()
            elif self.xml.get_widget('BlankRuleset').get_active():
                rules = self.buildGenericRules()

        # Wrap the actual rules inside a ruleset with our URI
        return "<ruleset uri=%r>\n%s</ruleset>" % (uri, rules)

    def buildGenericRules(self):
        """Return a generic do-nothing ruleset"""
        return "\t<return/>\n"

    def buildAllProjectsRules(self):
        """Return a ruleset for showing all projects in IRC"""
        return "\t<formatter medium='irc'/>\n\t<formatter name='IRCProjectName'/>\n"

    def buildSingleProjectRules(self, project):
        """Return a ruleset for showing one project on IRC"""
        return "\t<match path='/message/source/project'>%s</match>\n\t<formatter medium='irc'/>\n" % project


class URIList(GladeUI):
    """Implements a UI for listing, adding, deleting, and refreshing URIs.
       The provided callback is called when the current URI is changed.
       """
    def __init__(self, xml, client, callback):
        GladeUI.__init__(self, xml)
        self.client = client
        self.callback = callback
        self.view = self.xml.get_widget('URIList')
        self.initView()
        self.newDialog = NewRulesetDialog(xml, self.newRuleset)

    def initView(self):
        self.refresh()
        column = gtk.TreeViewColumn("URI", gtk.CellRendererText(), text=0)
        column.set_sort_column_id(0)
        self.view.append_column(column)

    def refresh(self):
        """Download a new list of URIs from the server and build a ListModel holding them"""
        self.client.clearCache()
        self.list = self.client.queryUriList()
        self.list.sort()
        model = gtk.ListStore(gobject.TYPE_STRING)
        for uri in self.list:
            i = model.append()
            model.set(i, 0, uri)
        self.view.set_model(model)
        self.setCurrentURI(None)

    def setCurrentURI(self, uri):
        """Called when our currently selected URI changes. This is used to update
           our button sensitivity and to trigger our owner's callback.
           """
        self.xml.get_widget('DeleteButton').set_sensitive(uri is not None)
        self.currentURI = uri
        self.callback(uri)

    def on_URIList_cursor_changed(self, tree):
        """A new row may have been selected in the URI list"""
        model, i = tree.get_selection().get_selected()
        uri = model.get_value(i, 0)
        if uri != self.currentURI:
            self.setCurrentURI(uri)

    def on_RefreshButton_clicked(self, button):
        self.refresh()

    def on_NewButton_clicked(self, button):
        self.newDialog.show()

    def newRuleset(self, ruleset):
        """Called by the 'new ruleset' dialog when a new ruleset is successfully created"""
        self.client.message(ruleset)
        self.refresh()


class RulesetEditor(GladeUI):
    """Implements the actual ruleset editor widget, the current ruleset
       title, and synchronizes the displayed ruleset with the server's
       ruleset.
       """
    def __init__(self, xml, client):
        GladeUI.__init__(self, xml)
        self.client = client
        self.editor = self.xml.get_widget('RulesetEditor')
        self.buffer = self.editor.get_buffer()

        # We have to connect signals on the buffer manually
        self.buffer.connect("modified_changed", self.on_RulesetBuffer_modified_changed)

    def setCurrentURI(self, uri):
        """Change the current URI displayed by our editor"""
        self.currentURI = uri

        # Set widget sensitivities
        self.xml.get_widget('RevertButton').set_sensitive(gtk.FALSE)
        self.xml.get_widget('ApplyButton').set_sensitive(gtk.FALSE)
        self.xml.get_widget('RulesetEditor').set_sensitive(uri is not None)

        # Show the current URI name
        if uri:
            self.xml.get_widget('RulesetName').set_markup('<big>%s</big>' % uri)
        else:
            self.xml.get_widget('RulesetName').set_text('')

        # Fetch the server's ruleset
        if uri:
            self.buffer.set_text(self.client.queryRulesets(uri)[0])
        else:
            self.buffer.set_text('')
        self.buffer.set_modified(gtk.FALSE)

    def on_RulesetBuffer_modified_changed(self, buffer):
        modified = buffer.get_modified()
        self.xml.get_widget('RevertButton').set_sensitive(modified)
        self.xml.get_widget('ApplyButton').set_sensitive(modified)

    def on_RevertButton_clicked(self, button):
        """Reload our current ruleset, discarding changes"""
        self.setCurrentURI(self.currentURI)

    def on_ApplyButton_clicked(self, button):
        """Send a modified ruleset to the server"""
        ruleset = self.buffer.get_text(*self.buffer.get_bounds())
        self.client.setRuleset(self.currentURI, ruleset)
        self.buffer.set_modified(gtk.FALSE)


class RulesetWindow(GladeUI):
    """A class which holds and initializes all the UI elements necessary
       for a complete CIA ruleset editor.
       """
    def __init__(self, xml, server):
        GladeUI.__init__(self, xml)

        self.client = CIAClient(server)
        self.editor = RulesetEditor(self.xml, self.client)
        self.uriList = URIList(self.xml, self.client, self.editor.setCurrentURI)

        # Add our server name to the window title
        self.window = self.xml.get_widget('RulesetWindow')
        self.window.set_title(self.window.get_title() + " - " + server)


def main():
    # Locate our glade file by looking in the same directory as our source
    gladeFile = os.path.join(os.path.dirname(sys.argv[0]), "ruleset_editor.glade")

    # Default to connecting to navi, but let the user override that on the command line
    server = "http://navi.picogui.org:3910"
    if len(sys.argv) > 1:
        server = sys.argv[1]

    ui = RulesetWindow(gladeFile, server)
    ui.window.connect("destroy", gtk.mainquit)
    gtk.main()

if __name__ == "__main__":
    main()

### The End ###
