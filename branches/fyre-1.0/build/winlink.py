#!/usr/bin/env python
#
# Command-line utility to create a .lnk file the hard way.
# Believe me, this was the last resort...
# This understands only a tiny subset of the .lnk format,
# but it's enough to convince Windows that we're serious.
# It will launch the application correctly, and automagically
# replace the .lnk file with a more well-formed version later.
#
# usage: winlink.py foo.lnk some/path/to/foo.exe [args]
#
# This is based on "The Windows Shortcut File Format as
# reverse-engineered by Jesse Hager".
#

import struct, sys

class ShellLink:
    magic = "L\x00\x00\x00"
    guid = "\x01\x14\x02\x00\x00\x00\x00\x00\xC0\x00\x00\x00\x00\x00\x00\x46"

    def __init__(self, targetPath,
                 arguments        = None,
                 workingDirectory = None,
                 relativePath     = None,
                 iconPath         = None):
        if relativePath is None:
            relativePath = targetPath

        self.targetPath = targetPath
        self.workingDirectory = workingDirectory
        self.arguments = arguments
        self.relativePath = relativePath
        self.iconPath = iconPath

    def save(self, filename):
        """Serialize this link to 'filename'"""
        f = open(filename, "wb");
        self.writeHeader(f)
        self.writePaths(f)
        self.writeStrings(f)
        f.write("\x00"*4)

    def writeHeader(self, f):
        flags = 1<<1   # Points to a file or directory
        if self.workingDirectory is not None:
            flags |= 1<<4
        if self.relativePath is not None:
            flags |= 1<<3
        if self.arguments is not None:
            flags |= 1<<5
        if self.iconPath is not None:
            flags |= 1<<6

        # Normal file
        file_attrs = 1<<7

        # Fake these...
        time1 = time2 = time3 = 0
        file_size = 0
        icon_number = 0

        # Normal window, no hotkey
        show_window = 1
        hot_key = 0

        f.write(self.magic + self.guid)
        f.write(struct.pack("<LLQQQLLLLLL",
                            flags,
                            file_attrs,
                            time1,
                            time2,
                            time3,
                            file_size,
                            icon_number,
                            show_window,
                            hot_key,
                            0, 0))

    def writePaths(self, f):
        # This is really a hack job at writing a
        # relatively complex structure. This omits
        # all volume information and such crap that
        # nobody should need in a glorified symlink

        f.write(struct.pack("<LLLLLLL",
                            0x1C + len(self.targetPath) + 1,   # Length of this whole section
                            0x1C,                              # Length of this structure
                            0,                                 # Volume flags (fake)
                            0,                                 # Local volume info offset (fake)
                            0,                                 # Local base path (fake)
                            0,                                 # Network base path (fake)
                            0x1C,                              # Remaining path
                            ) + self.targetPath + '\x00')

    def _writeString(self, f, s):
        """Write a string with a 16-bit length count prepended. Ignore if None."""
        if s is not None:
            f.write(struct.pack("<H", len(s)) + s)

    def writeStrings(self, f):
        self._writeString(f, self.relativePath)
        self._writeString(f, self.workingDirectory)
        self._writeString(f, self.arguments)
        self._writeString(f, self.iconPath)


if __name__ == "__main__":
    ShellLink(sys.argv[2].replace("/", "\\"),
              " ".join(sys.argv[3:])).save(sys.argv[1])

### The End ###
