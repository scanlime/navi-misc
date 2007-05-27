#
# Tool to generate font dictionaries for rwand.Font
#
# -- Micah Dowty <micah@navi.cx>
#

import ImageFont
import sys, string

f = ImageFont.truetype("04B_03__.TTF", 8)
printable = string.letters + string.punctuation + string.digits

for char in printable:
    m = f.getmask(char)

    width, height = m.size
    assert height == 8

    bytes = []
    for x in xrange(width):
        byte = 0
        for y in range(height):
            if m.getpixel((x,y)):
                byte = byte | (1 << y)

        byte = chr(byte)
        if (byte in printable) and (byte not in "'\"\\"):
            bytes.append(byte)
        else:
            bytes.append(r"\x%02x" % ord(byte))

    assert bytes[-1] == r'\x00'
    if char in r"'\":
        char = '\\' + char

    print "    '%s': '%s'," % (char, ''.join(bytes[:-1]))
