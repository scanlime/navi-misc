#!/usr/bin/env python

import pygtk
pygtk.require('2.0')

import gtk, libgmail, os, sys

ICON_PATH=sys.path[0]+"/icon.png"
ICON2_PATH=sys.path[0]+"/icon2.png"
ICON_DISCONNECTED_PATH=sys.path[0]+"/icon-disconnected.png"
POPUP_IMAGE = sys.path[0]+"/gmail-notify-icon.png"

class GmailNotify(object):
    maintimer = None
    mailcheck=0
    unreadmsgcount=0

    def __init__(self):
        f = file(os.path.expanduser("~/.gmailnotify"), "r")
        for line in f:
            attr, val = map(str.strip, line.split(":"))
            if attr == "poll interval":
                self.interval = int(val)
            else:
                setattr(self, attr, val)

        # Lame ass error checking
        assert hasattr(self, "username")
        assert hasattr(self, "password")
        assert hasattr(self, "interval")

        # Put gmail-notify icon to all application windows
        icon = gtk.gdk.pixbuf_new_from_file(ICON_DISCONNECTED_PATH)
        gtk.window_set_default_icon_list((icon))

        # Create the tray icon
        self.tray_icon = GmailTrayIcon.GmailTrayIcon()
        self.tray_icon.set_icon(ICON_PATH)

        pixmap = gtk.Image()
        pixmap.set_from_file(POPUP_IMAGE)
        self.notifyIcon = pixmap.get_pixbuf()
        self.notify = pynotify.Notification("No new mail", "")
        self.notify.set_icon_from_pixbuf(self.notifyIcon)

        # Create the connection
        self.account = libgmail.GmailAccount(self.username, self.password)

        # Check mail for first time
        self.check()

        # Schedule mail check
        self.timer = gtk.timeout_add(self.interval, self.check)

        gtk.main()

    def check(self):
        msgs = self.account.getUnreadMessages()

        if len(msgs) == 0:
            # Hide the tray icon when there are no messages
            self.tray_icon.hide()
        else:
            for msg in msgs:
                if msg not in self.messages:
                    self.messages.append(msg)
            latest = self.messages[-1]
            self.notify.update(latest.subject)

        return gtk.TRUE

if __name__ == "__main__":
    gmailnotifier = GmailNotify()

# vim: ts=4:sw=4:et
