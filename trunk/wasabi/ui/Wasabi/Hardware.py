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
        except:
            import sys
            print "*** Can't connect to lircd: %s" % sys.exc_info()[1]

### The End ###
