#!/usr/bin/env python
#
# event-recorder.py - A simple commandline utility to record and playback input
#                     filter events to a PicoGUI server. This should handle
#                     mouse, keyboard, joystick, or any future events supported
#                     by the PicoGUI server.
#
# Copyright (C) 2002-2004 Micah Dowty
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

import PicoGUI, pickle

class EventRecorder:
    def __init__(self, app=None):
        if not app:
            app = PicoGUI.InvisibleApp()
        self.app = app
        self.run = app.run

    def record(self):
        self.list = []
        self.filter = self.app.addInfilter()
        self.app.link(self._handler, self.filter)

    def load(self, file):
        f = open(file, "rb")
        self.list = pickle.load(f)

    def save(self, file):
        f = open(file, "wb")
        pickle.dump(self.list, f, True)

    def _handler(self, ev, widget):
        # Strip out some of the event's attributes that won't pickle well.
        # Widgets will refer back to an app and therefore a socket.
        # Both widget references and other picogui handles won't be good later
        # when we play this back.
        ev.widget = None
        ev.sender = 0
        ev.cursor_handle = 0
        ev._data[0] = 0         # Reset the ID of the originating input filter
        self.list.append(ev)

    def play(self):
        cursor = self.app.server.mkcursor()
        for e in self.list:
            e.cursor_handle = cursor
            self.app.server.infiltersend(e)
            self.app.server.update()
        self.app.server.free(cursor)

if __name__ == '__main__':
    import getopt, sys

    help = """
Simple recorder for PicoGUI input filter events

usage: %s [-r] [-f file.ev]

  -h  : This text
  -r  : Record to the specified file. The default is to play.
  -f  : Specify the event file to record to or play from.
        The default is 'recording.ev'
""" % (sys.argv[0])

    try:
        opts, args = getopt.getopt(sys.argv[1:], "rf:")
    except getopt.GetoptError:
        print help
        sys.exit(2)

    record = False
    file = 'recording.ev'

    for o, a in opts:
        if o == '-h':
            print help
            sys.exit(2)
        if o == '-r':
            record = True
        if o == '-f':
            file = a

    recorder = EventRecorder()

    if record:
        recorder.record()
        try:
            recorder.run()
        except PicoGUI.responses.Dead:
            pass
        recorder.save(file)
    else:
        recorder.load(file)
        recorder.play()

### The End ###
