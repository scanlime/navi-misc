""" BZFlag.Flag

Catalog of all supported flag types, and related utilities.
"""
#
# Python BZFlag Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from BZFlag import Event, Errors, Util
from BZFlag.Protocol import FromServer
import re
from StringIO import StringIO


class FlagMotion:
    def __init__(self, position=[0,0,0]):
        self.position = position
        self.launch = [0,0,0]
        self.landing = [0,0,0]
        self.flightTime = 0
        self.flightEndTime = 0
        self.initialVelocity = 0

    def integrate(self, dt):
        pass


class FlagBase:
    """Abstract base type for a flag. Subclasses should provide
       the flag info missing here, and they may implement event handlers
       for the events specified here.
       """
    name = None
    abbreviation = None
    type = []
    description = None

    def __init__(self, number=None):
        self.number = number
        self.status = None
        self.owner = None
        self.motion = FlagMotion()
        Event.attach(self, 'onGrab', 'onDrop', 'onUpdate')

    def getName(self):
        """Infer a flag's name from the class name"""
        name = self.__class__.__name__
        name = re.sub(r"Flag$", r"", name)
        name = re.sub(r"([A-Z])", r" \1", name)
        return name.strip()

    def getAlignment(self):
        """Returns a short abbreviation of the flag's alignment toward good or evil"""
        if 'good' in self.type:
            return "+"
        elif 'bad' in self.type:
            return "-"
        return ""

    def getMnemonic(self):
        """Return a version of the flag's name with the abbreviation characters capitalized"""
        name = self.getName()

        if len(name.split(" ")) == len(self.abbreviation):
            # This is probably an acronym, leave the capitalization alone
            return name

        # It's not an acronym, search for the first occurrance of the
        # abbreviation's letters in the name and capitalize them.
        m = name.lower()
        for letter in self.abbreviation:
            i = m.find(letter.lower())
            if i >= 0:
                m = m[:i] + letter + m[i+1:]
        return m

    def __str__(self):
        # If we have an abbreviation that doesn't start with a letter, add some whitespace
        # between it and the alignment so the whole thing doesn't just look like line noise.
        abbreviation = self.abbreviation
        if not re.match("[A-Za-z]", abbreviation[0]):
            abbreviation = " " + abbreviation

        return "%s (%s%s):  %s" % (self.getMnemonic(), self.getAlignment(),
                                   abbreviation, self.description)

    def updateFromMessage(self, msg):
        """Update this flag from any message with an 'update' attribute
           holding a FlagUpdate structure.
           """
        self.status = msg.update.status
        self.owner = msg.update.owner
        self.type = msg.update.type
        self.motion.position = msg.update.position
        self.motion.launch = msg.update.launch
        self.motion.landing = msg.update.landing
        self.motion.flightTime = msg.update.flightTime
        self.motion.flightEndTime = msg.update.flightEndTime
        self.motion.initialVelocity = msg.update.initialVelocity

        self.onUpdate()
        if msg.__class__ == FromServer.MsgDropFlag:
            self.onDrop(msg.playerId)
        elif msg.__class__ == FromServer.MsgGrabFlag:
            self.onGrab(msg.playerId)


def getDict():
    """Return a dictionary mapping flag abbreviations to flag classes"""
    import BZFlag.Flag.List
    return Util.getSubclassDict(BZFlag.Flag.List, FlagBase, 'abbreviation')

def joinAbbreviations(flags):
    """Given a list of flag classes or instances, return a list of 2-byte
       abbreviations ready to use in flag negotiation.
       """
    def padAbbreviation(str):
        if len(str) < 2:
            return str + chr(0)
        return str
    return "".join(map(padAbbreviation, flags))

def splitAbbreviations(str):
    """Convert a list of packed abbreviations to a list of abbreviation strings"""
    data = StringIO(str)
    lst = []
    for i in xrange(len(str)/2):
        flag = data.read(2)
        if flag[1] == chr(0):
            flag = flag[0]
        lst.append(flag)
    return lst

### The End ###
