#
# ledboard.py - Python module to communicate with the LED Board version 1,
#               via an LED server. The server hostname may be specified with the
#               LEDSERVER environment var, but it defaults to localhost.
#
# Copyright (C) 2002 Micah Dowty
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

import os, socket

port = 3827
default_server = 'localhost'
num_leds = 72
full_bright = 15

colors = ('white', 'red', 'blue', 'yellow', 'green')
groups = ('dot', 'pacman', 'grid', 'bars', 'corners')

led_db = [
    {'id': 0,	'color': 'white',	'group': 'dot' },
    {'id': 1,	'color': 'red',		'group': 'pacman',	'x': 0 },
    {'id': 2,	'color': 'blue',	'group': 'pacman',	'x': 0 },
    {'id': 3,	'color': 'yellow',	'group': 'pacman',	'x': 0 },
    {'id': 4,	'color': 'blue',	'group': 'pacman',	'x': 1 },
    {'id': 5,	'color': 'yellow',	'group': 'pacman',	'x': 1 },
    {'id': 7,	'color': 'red',		'group': 'pacman',	'x': 1 },
    {'id': 6,	'color': 'blue',	'group': 'pacman',	'x': 2 },

    {'id': 8,	'color': 'red',		'group': 'grid',	'x': 0, 'y': 7 },
    {'id': 9,	'color': 'red',		'group': 'grid',	'x': 0, 'y': 6 },
    {'id': 10,	'color': 'red',		'group': 'grid',	'x': 0, 'y': 5 },
    {'id': 11,	'color': 'red',		'group': 'grid',	'x': 0, 'y': 4 },
    {'id': 12,	'color': 'red',		'group': 'grid',	'x': 0, 'y': 3 },
    {'id': 13,	'color': 'red',		'group': 'grid',	'x': 0, 'y': 2 },
    {'id': 14,	'color': 'red',		'group': 'grid',	'x': 0, 'y': 1 },
    {'id': 15,	'color': 'red',		'group': 'grid',	'x': 0, 'y': 0 },
    {'id': 16,	'color': 'red',		'group': 'grid',	'x': 1, 'y': 7 },
    {'id': 17,	'color': 'red',		'group': 'grid',	'x': 1, 'y': 6 },
    {'id': 18,	'color': 'red',		'group': 'grid',	'x': 1, 'y': 5 },
    {'id': 19,	'color': 'red',		'group': 'grid',	'x': 1, 'y': 4 },
    {'id': 20,	'color': 'red',		'group': 'grid',	'x': 1, 'y': 3 },
    {'id': 21,	'color': 'red',		'group': 'grid',	'x': 1, 'y': 2 },
    {'id': 22,	'color': 'red',		'group': 'grid',	'x': 1, 'y': 1 },
    {'id': 23,	'color': 'red',		'group': 'grid',	'x': 1, 'y': 0 },
    {'id': 24,	'color': 'red',		'group': 'grid',	'x': 2, 'y': 7 },
    {'id': 25,	'color': 'red',		'group': 'grid',	'x': 2, 'y': 6 },
    {'id': 26,	'color': 'red',		'group': 'grid',	'x': 2, 'y': 5 },
    {'id': 27,	'color': 'red',		'group': 'grid',	'x': 2, 'y': 4 },
    {'id': 28,	'color': 'red',		'group': 'grid',	'x': 2, 'y': 3 },
    {'id': 29,	'color': 'red',		'group': 'grid',	'x': 2, 'y': 2 },
    {'id': 30,	'color': 'red',		'group': 'grid',	'x': 2, 'y': 1 },
    {'id': 31,	'color': 'red',		'group': 'grid',	'x': 2, 'y': 0 },
    {'id': 32,	'color': 'red',		'group': 'grid',	'x': 3, 'y': 7 },
    {'id': 33,	'color': 'red',		'group': 'grid',	'x': 3, 'y': 6 },
    {'id': 34,	'color': 'red',		'group': 'grid',	'x': 3, 'y': 5 },
    {'id': 35,	'color': 'red',		'group': 'grid',	'x': 3, 'y': 4 },
    {'id': 36,	'color': 'red',		'group': 'grid',	'x': 3, 'y': 3 },
    {'id': 37,	'color': 'red',		'group': 'grid',	'x': 3, 'y': 2 },
    {'id': 38,	'color': 'red',		'group': 'grid',	'x': 3, 'y': 1 },
    {'id': 39,	'color': 'red',		'group': 'grid',	'x': 3, 'y': 0 },
    {'id': 40,	'color': 'red',		'group': 'grid',	'x': 4, 'y': 7 },
    {'id': 41,	'color': 'red',		'group': 'grid',	'x': 4, 'y': 6 },
    {'id': 42,	'color': 'red',		'group': 'grid',	'x': 4, 'y': 5 },
    {'id': 43,	'color': 'red',		'group': 'grid',	'x': 4, 'y': 4 },
    {'id': 44,	'color': 'red',		'group': 'grid',	'x': 4, 'y': 3 },
    {'id': 45,	'color': 'red',		'group': 'grid',	'x': 4, 'y': 2 },
    {'id': 46,	'color': 'red',		'group': 'grid',	'x': 4, 'y': 1 },
    {'id': 47,	'color': 'red',		'group': 'grid',	'x': 4, 'y': 0 },

    {'id': 55,	'color': 'green',	'group': 'bars',	'x': 0 },
    {'id': 54,	'color': 'green',	'group': 'bars',	'x': 1 },
    {'id': 53,	'color': 'green',	'group': 'bars',	'x': 2 },
    {'id': 52,	'color': 'green',	'group': 'bars',	'x': 3 },
    {'id': 51,	'color': 'green',	'group': 'bars',	'x': 4 },
    {'id': 50,	'color': 'green',	'group': 'bars',	'x': 5 },
    {'id': 49,	'color': 'green',	'group': 'bars',	'x': 6 },
    {'id': 48,	'color': 'green',	'group': 'bars',	'x': 7 },

    {'id': 63,	'color': 'yellow',	'group': 'bars',	'x': 0 },
    {'id': 62,	'color': 'yellow',	'group': 'bars',	'x': 1 },
    {'id': 61,	'color': 'yellow',	'group': 'bars',	'x': 2 },
    {'id': 60,	'color': 'yellow',	'group': 'bars',	'x': 3 },
    {'id': 59,	'color': 'yellow',	'group': 'bars',	'x': 4 },
    {'id': 58,	'color': 'yellow',	'group': 'bars',	'x': 5 },
    {'id': 57,	'color': 'yellow',	'group': 'bars',	'x': 6 },
    {'id': 56,	'color': 'yellow',	'group': 'bars',	'x': 7 },
    
    {'id': 64,	'color': 'blue',	'group': 'corners',	'x': 0, 'y': 1 },
    {'id': 65,	'color': 'white',	'group': 'corners',	'x': 0, 'y': 1 },
    {'id': 66,	'color': 'blue',	'group': 'corners',	'x': 1, 'y': 1 },
    {'id': 67,	'color': 'white',	'group': 'corners',	'x': 1, 'y': 1 },
    {'id': 68,	'color': 'blue',	'group': 'corners',	'x': 1, 'y': 0 },
    {'id': 69,	'color': 'white',	'group': 'corners',	'x': 1, 'y': 0 },
    {'id': 70,	'color': 'blue',	'group': 'corners',	'x': 0, 'y': 0 },
    {'id': 71,	'color': 'white',	'group': 'corners',	'x': 0, 'y': 0 },
    ]


class Server:
    def __init__(self):
        self.hostname = os.environ.get('LEDSERVER', default_server)
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        except:
            pass
        self.socket.connect((self.hostname, port))
        self.values = [0] * num_leds
        self.clear()
        self.blit()
        self.vfd = CenturyVFD(self.socket.send)

    def clear(self):
        for i in range(num_leds):
            self.set(i,0)

    def set(self, led, brightness):
        """
        This is a very versatile function for setting LED brightnesses.
        The 'led' parameter can be a single LED number, an LED record
        from the database, or a list of either.
        The brightness can be a single value that's set across all LEDs
        in the list, a sequence of brightnesses to use, or a function that
        translates the led to a brightness level. The brightness can
        be an interger, or None to leave it unaffected.
        """
        if type(led) != type([]):
            led = [led]
        brightnessListPos = 0
        for i in led:
            if type(brightness) == type([]):
                b = brightness[brightnessListPos]
                brightnessListPos += 1
            else:
                b = brightness
            if callable(b):
                b = b(i)
            if type(i) == type({}):
                i = i['id']
            if b != None:
                self.values[i] = b
                self.socket.send("set %d %d\n" % (i, b))

    def get(self, led):
        if type(led) == type([]):
            results = []
            for i in led:
                if type(i) == type({}):
                    i = i['id']
                results.append(self.values[i])
            return results
        else:
            if type(led) == type({}):
                led = led['id']
            return self.values[led]

    def fade(self, led=led_db, amount=1):
        values = self.get(led)
        for i in range(len(values)):
            values[i] -= amount
            if values[i] < 0:
                values[i] = 0
        self.set(led,values)

    def blit(self):
        self.socket.send("blit\n")

    def select(self, criteria={}, db=led_db):
        if criteria == None:
            return []
        results = []
        for led in db:
            match = True
            for key in criteria.keys():
                if criteria[key] != led[key]:
                    match = False
            if match:
                results.append(led)
        return results


class GridPattern:
    "Utility class for storing patterns for the 5x8 grid"
    def __init__(self, pattern, palette={' ': 0, 'o': 15, '.': 4}):
        self.pattern = pattern
        self.palette = palette

    def scrollLeft(self, amount=1):
        for i in range(len(self.pattern)):
            self.pattern[i] = self.pattern[i][amount:] + self.pattern[i][:amount]

    def scrollRight(self, amount=1):
        for i in range(len(self.pattern)):
            self.pattern[i] = self.pattern[i][-amount:] + self.pattern[i][:-amount]

    def func(self, led):
        return self.palette[self.pattern[led['y']][led['x']]]

    def set(self, led):
        led.set(led.select({'group': 'grid'}), self.func)


class CenturyVFD:
    width = 20
    lines = 2

    def __init__(self, rawWrite):
        self.rawWrite = rawWrite

    def write(self, data):
        "Write a string, handling newlines"
        lines = data.split('\n')
        lastLine = lines[-1]
        for l in lines[:-1]:
            self.writeVFD(l)
            self.nl()
        self.writeVFD(lastLine)

    def writeVFD(self, data):
        self.rawWrite("write %s\n" % data)

    def cr(self):
        self.rawWrite("cr\n")

    def lf(self):
        self.rawWrite("lf\n")

    def nl(self):
        self.cr()
        self.lf()

    def clear(self):
        self.writeVFD(chr(0x15))

    def home(self):
        self.writeVFD(chr(0x16))

    def writeScreen(self, data):
        "Write a complete screen of data, without clearing the screen"
        lines = data.split('\n')
        while len(lines)<self.lines:
            lines.append("")
        self.home()
        for l in lines:
            if len(l) < self.width:
                l += " " * (self.width - len(l))
            elif len(l) > self.width:
                l = l[:self.width]
            self.writeVFD(l)
    
