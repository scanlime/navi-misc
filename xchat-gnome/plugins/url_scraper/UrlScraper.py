''' UrlScraper.py

xchat-gnome script for pulling URLs out of chat rooms
and displaying them in a separate window.
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

import xchat, string, pygtk, gtk, gobject, re

__module_name__ = 'URL Scraper'
__module_version__ = '0.1-pre'
__module_description__ = 'Collect URLs said and display them in a separate window'

def close( window, event, user_data ):
    window.hide()

window = gtk.Window( gtk.WINDOW_TOPLEVEL )
store = gtk.ListStore( gobject.TYPE_STRING, gobject.TYPE_STRING )
view = gtk.TreeView( store )
view.append_column( gtk.TreeViewColumn( 'Nick', gtk.CellRendererText(), text=0 ) )
view.append_column( gtk.TreeViewColumn( 'URL', gtk.CellRendererText(), text=1 ) )
window.add( view )
window.connect( 'destroy', close)
window.show_all()

def grabbedURL( nick, match ):
    print '%s said url: %s' % ( nick, match.group() )
    store.set( store.append(), 0, nick, 1, match.group() )

def grabbedEmail( nick, match ):
    print '%s said an email address: %s' % (nick, match.group())

regexes = {
    re.compile('(ht|f)tps?://[^\s\>\]\)]+'):                 grabbedURL,
    re.compile('[\w\.\-\+]+@([0-9a-zA-Z\-]+\.)+[a-zA-Z]+'):  grabbedEmail,
    }

def grabURL( word, word_eol, user_data ):
    ''' Check a message for a URL. '''
    nick = word[0]
    message = word[1]
    for regex, handler in regexes.iteritems():
        for match in regex.finditer(message):
            handler(nick, match)
    return xchat.EAT_NONE

def unload( user_data ):
    ''' Destroy the window and stuff before we go away. '''
    print 'URL Scraper unloaded.'

xchat.hook_print( 'Channel Message', grabURL )
xchat.hook_unload( unload )

print 'URL Scraper loaded.'
### The End ###

