#!/usr/bin/env python
""" gmailapplet.py

This is a GNOME notification-area applet which shows when you
have unread conversations on GMail.  Requires pygtk, gnome-python
and gnome-python-extras.

I've bundled libgmail CVS here, since 0.0.8 isn't sufficient.
"""
#
# Copyright (C) 2005-2007 David Trowbridge
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
import sys
import time

import pygtk
pygtk.require ('2.0')
import gtk
import gtk.gdk
import gobject
import gnome
import egg.trayicon

import libgmail

### replace these with your own
username = ''
password = ''

def login_g ():
    # log into gmail
    global account
    account = libgmail.GmailAccount (username, password)
    try:
        account.login ()
        return True
    except libgmail.GmailLoginFailure:
        print 'login failed'
        return False

def initialize ():
    if not login_g ():
        sys.exit (1)

    # set up gtk stuffs
    icon = egg.trayicon.TrayIcon ('GMail')
    eventbox = gtk.EventBox()
    icon.add (eventbox)
    image = gtk.Image ()
    image.set_from_file ('mail.png')
    eventbox.add (image)
    tips = gtk.Tooltips ()

    # connect to 'clicked' event
    eventbox.set_above_child (True)

    def open_gmail (widget, data=None):
        gnome.url_show ('http://mail.google.com/mail')
    eventbox.add_events (gtk.gdk.BUTTON_RELEASE)
    eventbox.connect ('button-release-event', open_gmail)

    def check_mail (data=None):
        try:
            unread = account.getUnreadMsgCount ()
            if unread:
                icon.show_all ()
                tips.set_tip (icon, '%d unread messages' % unread)
                tips.enable ()
            else:
                icon.hide_all ()
                tips.disable ()
            return True
        except Exception, e:
            login_g ()

    # start out with an update
    check_mail ()

    # mail-check timeout - 15s is probably a little excessive,
    # but why not
    gobject.timeout_add (15000, check_mail)

if __name__ == '__main__':
    initialize ()
    gtk.main ()
