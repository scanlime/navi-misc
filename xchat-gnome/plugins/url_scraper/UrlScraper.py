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

import xchat, string, pygtk, gtk

__module_name__ = 'URL Scraper'
__module_version__ = '0.1-pre'
__module_description__ = 'Collect URLs said and display them in a separate window'

window = gtk.Window( gtk.WINDOW_TOPLEVEL )
window.connect( 'destroy', lambda w: gtk.main.quit() )
window.show()

def grabURL( word, word_eol, user_data ):
    ''' Check a message for a URL. '''
    nick = word[0]
    words = word[1].split()
    for x in words:
        if string.find( x, 'http' ) != -1:
            url = x
            print '%s said %s' % (nick, url)
            break
    return xchat.EAT_NONE

def unload( user_data ):
    ''' Destroy the window and stuff before we go away. '''
    print 'URL Scraper unloaded.'

xchat.hook_print( 'Channel Message', grabURL )
xchat.hook_unload( unload )

print 'URL Scraper loaded.'
### The End ###

