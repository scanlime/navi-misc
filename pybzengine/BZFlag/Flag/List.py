""" BZFlag.Flag.List

Collection of BZFlag.Flag.FlagBase subclasses that define the
name, type, description, and behavior of all flags in the game.
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

from BZFlag.Flag import FlagBase

########################################### Team flags

class TeamFlag(FlagBase):
    description = "Team flag:  If it's yours, prevent other teams from taking it.  If it's not take it to your base to capture it!"
    def __str__(self):
        return self.description

class RedTeamFlag(TeamFlag):
    abbreviation = "R*"
    type         = ['good']

class GreenTeamFlag(TeamFlag):
    abbreviation = "G*"
    type         = ['good']

class BlueTeamFlag(TeamFlag):
    abbreviation = "B*"
    type         = ['good']

class PurpleTeamFlag(TeamFlag):
    abbreviation = "P*"
    type         = ['good']

########################################### Good flags

class VelocityFlag(FlagBase):
    abbreviation = "V"
    type         = ['unstable', 'good']
    description  = "Tank moves faster.  Outrun bad guys."

class AngularVelocityFlag(FlagBase):
    abbreviation = "A"
    type         = ['unstable', 'good']
    description  = "Tank turns faster.  Dodge quicker."

class OscillationOverthrusterFlag(FlagBase):
    abbreviation = "OO"
    type         = ['unstable', 'good']
    description  = "Can drive through buildings.  Can't backup or shoot while inside."

class RapidFireFlag(FlagBase):
    abbreviation = "F"
    type         = ['unstable', 'specialShot', 'good']
    description  = "Shoots more often.  Shells go faster but not as far."

class MachineGunFlag(FlagBase):
    abbreviation = "MG"
    type         = ['unstable', 'specialShot', 'good']
    description  = "Very fast reload and very short range."

class GuidedMissleFlag(FlagBase):
    abbreviation = "GM"
    type         = ['unstable', 'specialShot', 'good']
    description  = "Shots track a target.  Lock on with right button.  Can lock on or retarget after firing."

class LaserFlag(FlagBase):
    abbreviation = "L"
    type         = ['unstable', 'specialShot', 'good']
    description  = "Shoots a laser.  Infinite speed and range but long reload time."

class RicochetFlag(FlagBase):
    abbreviation = "R"
    type         = ['unstable', 'specialShot', 'good']
    description  = "Shots bounce off walls.  Don't shoot yourself!"

class SuperBulletFlag(FlagBase):
    abbreviation = "SB"
    type         = ['unstable', 'specialShot', 'good']
    description  = "Shoots through buildings.  Can kill Phantom Zone."

class InvisibleBulletFlag(FlagBase):
    abbreviation = "IB"
    type         = ['unstable', 'good']
    description  = "Your shots don't appear on other radars.  Can still see them out window."

class StealthFlag(FlagBase):
    abbreviation = "ST"
    type         = ['unstable', 'good']
    description  = "Tank is invisible on radar.  Shots are still visible.  Sneak up behind enemies!"

class TinyFlag(FlagBase):
    abbreviation = "T"
    type         = ['unstable', 'good']
    description  = "Tank is small and can get through small openings.  Very hard to hit."

class NarrowFlag(FlagBase):
    abbreviation = "N"
    type         = ['unstable', 'good']
    description  = "Tank is super thin.  Very hard to hit from front but is normal size from side.  Can get through small openings."

class ShieldFlag(FlagBase):
    abbreviation = "SH"
    type         = ['unstable', 'good']
    description  = "Getting hit only drops flag.  Flag flies an extra-long time."

class SteamrollerFlag(FlagBase):
    abbreviation = "SR"
    type         = ['unstable', 'good']
    description  = "Destroys tanks you touch but you have to get really close."

class ShockWaveFlag(FlagBase):
    abbreviation = "SW"
    type         = ['unstable', 'good']
    description  = "Firing destroys all tanks nearby.  Don't kill teammates!  Can kill tanks on/in buildings."

class PhantomZoneFlag(FlagBase):
    abbreviation = "PZ"
    type         = ['unstable', 'good']
    description  = "Teleporting toggles Zoned effect.  Zoned tank can drive through buildings.  Zoned tank can't shoot or be shot (except by superbullet and shock wave)."

class GenocideFlag(FlagBase):
    abbreviation = "G"
    type         = ['unstable', 'good']
    description  = "Killing one tank kills that tank's whole team."

class JumpingFlag(FlagBase):
    abbreviation = "JP"
    type         = ['unstable', 'good']
    description  = "Tank can jump.  Use Tab key.  Can't steer in the air."

class IdentifyFlag(FlagBase):
    abbreviation = "ID"
    type         = ['unstable', 'good']
    description  = "Identifies type of nearest flag."

class CloakingFlag(FlagBase):
    abbreviation = "CL"
    type         = ['unstable', 'good']
    description  = "Makes your tank invisible out-the-window.  Still visible on radar."

class UselessFlag(FlagBase):
    abbreviation = "US"
    type         = ['unstable', 'good']
    description  = "You have found the useless flag. Use it wisely."

class MasqueradeFlag(FlagBase):
    abbreviation = "MQ"
    type         = ['unstable', 'good']
    description  = "In opponent's hud, you appear as a teammate."

class SeerFlag(FlagBase):
    abbreviation = "SE"
    type         = ['unstable', 'good']
    description  = "See stealthed, cloaked and masquerading tanks as normal."

class ThiefFlag(FlagBase):
    abbreviation = "TH"
    type         = ['unstable', 'good']
    description  = "Steal flags.  Small and fast but can't kill."

class BurrowFlag(FlagBase):
    abbreviation = "BU"
    type         = ['unstable', 'good']
    description  = "Tank burrows underground, impervious to normal shots, but can be steamrolled by anyone!"

########################################### Bad flags

class ColorblindnessFlag(FlagBase):
    abbreviation = "CB"
    type         = ['unstable', 'bad']
    description  = "Can't tell team colors.  Don't shoot teammates!"

class ObesityFlag(FlagBase):
    abbreviation = "O"
    type         = ['unstable', 'bad']
    description  = "Tank becomes very large.  Can't fit through teleporters."

class LeftTurnOnlyFlag(FlagBase):
    abbreviation = "LT"
    type         = ['unstable', 'bad']
    description  = "Can't turn right."

class RightTurnOnlyFlag(FlagBase):
    abbreviation = "RT"
    type         = ['unstable', 'bad']
    description  = "Can't turn left."

class MomentumFlag(FlagBase):
    abbreviation = "M"
    type         = ['unstable', 'bad']
    description  = "Tank has inertia.  Acceleration is limited."

class BlindnessFlag(FlagBase):
    abbreviation = "B"
    type         = ['unstable', 'bad']
    description  = "Can't see out window.  Radar still works."

class JammingFlag(FlagBase):
    abbreviation = "JM"
    type         = ['unstable', 'bad']
    description  = "Radar doesn't work.  Can still see."

class WideAngleFlag(FlagBase):
    abbreviation = "WA"
    type         = ['unstable', 'bad']
    description  = "Fish-eye lens distorts view."

### The End ###
