''' UrlScraper.py

xchat-gnome script for pulling URLs out of channels
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

import xchat, string, re, gconf
import pygtk
import gtk
import gobject

__module_name__ = 'URL Scraper'
__module_version__ = '0.1-pre'
__module_description__ = 'Collect URLs said and display them in a separate window'


class URLScraper:

    def __init__( self ):
        self.urls = 0
        self.new_urls = 0

        # Set up gconf stuff.
        self.client = gconf.client_get_default()
        self.get_prefs( self.client )

        # Create the window.
        self.window = gtk.Window( gtk.WINDOW_TOPLEVEL )
        self.window.set_default_size( 300, 350 )
        self.window.set_title( 'URL Scraper' )

        scrolled = gtk.ScrolledWindow()
        scrolled.set_policy( gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC )
        self.store = gtk.ListStore( gobject.TYPE_STRING,
                                    gobject.TYPE_STRING,
                                    gobject.TYPE_STRING )

        view = gtk.TreeView( self.store )
        nick_col =  gtk.TreeViewColumn( 'Nick', gtk.CellRendererText(), text=0 )
        nick_col.set_sort_column_id( 0 )
        chan_col = gtk.TreeViewColumn( 'Channel', gtk.CellRendererText(), text=1)
        chan_col.set_sort_column_id( 1 )
        url_col =  gtk.TreeViewColumn( 'URL', gtk.CellRendererText(), text=2 )

        view.append_column( nick_col )
        view.append_column( chan_col )
        view.append_column( url_col )

        scrolled.add_with_viewport( view )
        self.window.add( scrolled )
        self.window.connect( 'destroy', self.close )
        # FIXME: this causes xchat-gnome to crash.
        #self.window.connect( 'focus-in-event', self.windowFocused )
        self.window.show_all()

    def get_prefs( self, client ):
        # If our gconf keys don't exist...
        if not client.dir_exists( '/apps/xchat/url_scraper' ):
            client.add_dir( '/apps/xchat/url_scraper', gconf.CLIENT_PRELOAD_NONE )
            self.history = 10
            self.scrape_email = False
            self.new_url_in_title = False
            client.set_int( '/apps/xchat/url_scraper/history', self.history )
            client.set_bool( '/apps/xchat/url_scraper/scrape_email', self.scrape_email )
            client.set_bool( '/apps/xchat/url_scraper/new_url_in_title',
                             self.new_url_in_title )
        # If our gconf keys exist...
        else:
            self.history = client.get_int( '/apps/xchat/url_scraper/history' )
            self.scrape_email = client.get_bool( '/apps/xchat/url_scraper/scrape_email' )
            self.new_url_in_title = \
                    client.get_bool( '/apps/xchat/url_scraper/new_url_in_title' )


    def windowFocused( self, widget, direction, user_data=None ):
        if self.new_url_in_title:
            self.window.set_title( 'URL Scraper' )
            self.new_urls = 0

        return gtk.FALSE


    def grabbedURL( self, nick, match ):
        ''' Handler for a URL. Append the URL to the store. '''
        if self.urls >= self.history:
            iter = store.get_iter_first()
            store.remove( iter )
        else:
            self.urls += 1

        # FIXME: Commented out because removing the change to the title causes
        #        a crash.
        #if not self.window.has_focus and self.new_url_in_title:
        #    self.new_urls += 1
        #    self.window.set_title( '[' + str(self.new_urls) + '] URL Scraper' )

        self.store.set( self.store.append(), 0, nick, 1,
                xchat.get_info('channel'), 2, match.group())

    def grabbedEmail( self, nick, match ):
        ''' Handler for email addresses. Since the behaviour is the same
            as the URL handler we might as well just pass this on to it.
            The only difference is you can disable this one, so we check
            first.
            '''
        if self.scrape_email:
            self.grabbedURL( nick, match )

    regexes = {
        re.compile('(ht|f)tps?://[^\s\>\]\)]+'):                 grabbedURL,
        re.compile('[\w\.\-\+]+@([0-9a-zA-Z\-]+\.)+[a-zA-Z]+'):  grabbedEmail,
        }

    def grabURL( self, word, word_eol, user_data ):
        ''' Check a message for a URL. '''
        nick = word[0]
        message = word[1]
        for regex, handler in URLScraper.regexes.iteritems():
            for match in regex.finditer(message):
                handler( self, nick, match )
        return xchat.EAT_NONE

    def unload( self, user_data ):
        ''' Destroy the window and stuff before we go away. '''
        self.window.destroy()
        print 'URL Scraper unloaded.'

    def close( self, window, event=None, user_data=None ):
        ''' Unload the module when you close the window. '''
        # FIXME: In the future this might just hide the window if we can add a
        #        item to the main window that would allow us to show the window,
        #        in which case we wouldn't unload the plugin here.
        xchat.command( 'py unload '+__module_name__ )

scraper = URLScraper()
xchat.hook_print( 'Channel Message', scraper.grabURL )
xchat.hook_unload( scraper.unload )

print 'URL Scraper loaded.'
### The End ###
