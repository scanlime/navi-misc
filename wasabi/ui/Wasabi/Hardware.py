""" Wasabi.Hardware

Detection and initialization for all of wasabi's hardware. This includes
the mi6k (both through the mi6k module and through lircd) and the uvswitch
(through its device node).
"""
#
# Wasabi Project
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

class Devices:
    """Container for hardware connected to wasabi. Initializes what hardware
       is available, and provides functionality to tie the hardware and
       software together at a high level.
       """
    def __init__(self, eventLoop):
        # Try to connect to lircd
        try:
            import IR
            IR.initDefaultClient(eventLoop)
        except IOError:
            import sys
            self.warn("Can't connect to lircd, %s" % sys.exc_info()[1])

        # Try to initialize the mi6k
        try:
            import mi6k
            self.mi6k = mi6k.Device()
        except:
            import sys
            self.warn("Can't connect to the mi6k, %s" % sys.exc_info()[1])
            self.mi6k = None

        # Try to initialize the uvswitch
        try:
            import VideoSwitch
            self.uvswitch = VideoSwitch.Device()
        except IOError:
            import sys
            self.warn("Can't connect to the uvswitch, %s" % sys.exc_info()[1])
            self.uvswitch= None

    def warn(self, msg):
        """Issue a warning related to hardware initialization"""
        print "*** Warning: %s" % msg

### The End ###
