#!/usr/bin/env python
import os, time, sys

class Font:

    glyphs = {
        '0': [" ### ",
              "#   #",
              "#   #",
              "#   #",
              "#   #",
              "#   #",
              "#   #",
              " ### "],

        '1': ["  #  ",
              " ##  ",
              "# #  ",
              "  #  ",
              "  #  ",
              "  #  ",
              "  #  ",
              "#####"],

        '2': [" ### ",
              "#   #",
              "    #",
              "   # ",
              "  #  ",
              " #   ",
              "#    ",
              "#####"],

        '3': ["#####",
              "    #",
              "   # ",
              "  ## ",
              "    #",
              "    #",
              "#   #",
              " ### "],

        '4': ["    #",
              "#   #",
              "#   #",
              "#   #",
              "#####",
              "    #",
              "    #",
              "    #"],

        '5': ["#####",
              "#    ",
              "#    ",
              "#### ",
              "    #",
              "    #",
              "#   #",
              " ### "],

        '6': [" ### ",
              "#    ",
              "#    ",
              "#### ",
              "#   #",
              "#   #",
              "#   #",
              " ### "],

        '7': ["#####",
              "    #",
              "   # ",
              "  #  ",
              "  #  ",
              "  #  ",
              "  #  ",
              "  #  "],

        '8': [" ### ",
              "#   #",
              "#   #",
              " ### ",
              "#   #",
              "#   #",
              "#   #",
              " ### "],

        '9': [" ### ",
              "#   #",
              "#   #",
              "#   #",
              " ####",
              "    #",
              "    #",
              " ### "],

        ':': ["   ",
              "   ",
              " # ",
              "   ",
              "   ",
              " # ",
              "   ",
              "   "],
        }

    def __init__(self):
        self.encodedGlyphs = {}
        for name, pattern in self.glyphs.iteritems():
            self.encodedGlyphs[name] = self.encodePattern(pattern)

    def encode(self, string, spacing=2):
        return "".join([self.encodedGlyphs[c] + chr(0)*spacing for c in string])

    def encodePattern(self, pattern):
        w = len(pattern[0])
        bytes = []
        for x in xrange(w):
            byte = 0
            for y in xrange(8):
                if pattern[y][x] != " ":
                    byte = byte | (1<<y)
            bytes.append(chr(byte))
        return "".join(bytes)


if len(sys.argv) > 1:
    dev = sys.argv[1]
else:
    dev = "/dev/rwand0"

f = os.open(dev, os.O_RDWR)
font = Font()
while 1:
    os.write(f, font.encode(time.strftime("%H:%M:%S")))
    time.sleep(0.5)
