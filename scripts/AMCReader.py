#
# AMCReader.py: parses Acclaim AMC (motion capture) files
#
# Copyright (C) 2005-2006 David Trowbridge
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
from pyparsing import *
import Log

log = Log.Logger()

class AMCFrame:
    def __init__(self):
        self.number = None
        self.bones = {}

class AMCReader:
    frames = []

    def parse (self, filename):
        f = open(filename, 'r')
        l = f.readlines()
        f.close()
        curFrame = None

        for i in range(len(l)):
            line = l[i].strip(' \n\r')
            if line[0] == '#' or line[0] == ':':
                # comments - we treat : as a comment even though it really isn't,
                # because it's just giving us redundant information from the ASF
                continue
            data = line.split(' ')
            if len(data) == 1:
                if curFrame is not None:
                    self.frames.append(curFrame)
                curFrame = AMCFrame()
                curFrame.number = int(data[0])
            else:
                curFrame.bones[data[0]] = [float(n) for n in data[1:]]
        self.frames.append(curFrame)
