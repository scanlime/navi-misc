""" BZFlag.Writer

Writes lines, blocks or objects to a BZFlag world file using callbacks.
"""
#
# Copyright (C) 2005 David Trowbridge
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
class Writer:
    def __init__(self, filename):
        self.stream = open(filename, 'w')
        self.writeHeading()

    def writeHeading(self):
        self.stream.write('# Created with Blender (www.blender3d.org)\n')

    def writeLine(self, content):
        if content is None:
            self.stream.write('\n')
        elif type(content) is tuple:
            self.stream.write(' '.join([str(i) for i in content]) + '\n')
        else:
            raise TypeError('Unsupported type in BZFlag.Writer.writeLine()')

    def writeBlock(self, type, callback):
        self.writeLine(None)
        self.writeLine((type,))
        callback(self.writeLine)
        self.writeLine(('end',))

    def writeObject(self, object):
        self.writeBlock(object.type, object.serialize)
