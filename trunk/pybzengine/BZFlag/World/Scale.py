""" BZFlag.World.Scale

Define constants that give the absolute and relative scale of objects
in the BZFlag universe. These correspond to global.h.
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

import math

Gravity        = -9.81                 # meters/sec/sec
WorldSize      = 800.0                 # meters
BaseSize       = 60.0                  # meters

TankLength     = 6.0                   # meters
TankWidth      = 2.8                   # meters
TankHeight     = 2.05		       # meters
TankRadius     = 0.72 * TankLength     # meters
MuzzleHeight   = 1.57                  # meters
MuzzleFront    = TankRadius + 0.1      # meters

ShotRadius     = 0.5		       # meters
ShotLength     = 0.5		       # meters
ShotTailLength = 4.0		       # meters

WallHeight     = 3.0 * TankHeight      # meters

TeleUnit       = 0.4 * TankWidth       # meters
TeleWidth      = 1.0 * TeleUnit        # meters
TeleBreadth    = 4.0 * TeleUnit        # meters
TeleHeight     = 9.0 * TeleUnit        # meters

TankSpeed      = 25.0                  # meters/sec
TankAngVel     = math.pi / 4.0         # radians/sec
ShotSpeed      = 100.0                 # meters/sec
ShotRange      = 350.0                 # meters
ReloadTime     = ShotRange / ShotSpeed # seconds

ExplodeTime    = 5.0                   # seconds
TeleportTime   = 1.0	               # seconds
FlagAltitude   = 11.0	               # meters
FlagRadius     = 2.5		       # meters

### The End ###
