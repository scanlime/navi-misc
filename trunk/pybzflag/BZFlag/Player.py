""" BZFlag.Player

Class representing a BZFlag player, divided up into subclasses for
areas like identity and motion that may be managed individually.
"""
# 
# Python BZFlag Protocol Package
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

from BZFlag import Util


class Identity:
    """A simple container for basic player information"""
    def __init__(self, callSign=None, team='rogue', emailAddress="PyBZFlag", type='tank', playerId=None):
        self.playerId = playerId
        self.type = type
        self.callSign = callSign
        self.team = team
        self.emailAddress = emailAddress

    def __str__(self):
        return "%s (%s)" % (self.callSign, self.emailAddress)


class Motion:
    """Contains information about a player's motion, integrates velocity over time"""
    def __init__(self, position=[0,0,0], azimuth=0, velocity=[0,0,0], angularVelocity=0):
        self.position = position
        self.azimuth = azimuth
        self.velocity = velocity
        self.angularVelocity = angularVelocity

    def integrate(self, dt):
        self.velocity += dt * self.angularVelocity
        self.position = [
            self.velocity[0] * dt,
            self.velocity[1] * dt,
            self.velocity[2] * dt,
            ]


class Score:
    """Information about a a player's current score"""
    def __init__(self):
        self.wins = 0
        self.losses = 0
        self.teamKills = 0

    def __str__(self):
        return "%d (%d-%d)[%d]" % (self.wins - self.losses, self.wins,
                                   self.losses, self.teamKills)


class Player:
    """Container for information about a player, divided into subclasses
       for the player's identity, scoring, and movement.
       """
    def __init__(self, identity=None):
        if identity:
            self.identity = identity
        else:
            self.identity = Identity()
        self.motion = Motion()
        self.score = Score()

    def __str__(self):
        return "%s  %s" % (self.score, self.identity)


def fromMessage(msg):
    """Create a new Player class with the information from
       a MsgAddPlayer message"""
    p = Player()
    p.identity.playerId = msg.id
    p.identity.type = msg.type
    p.identity.team = msg.team
    p.score.wins = msg.wins
    p.score.losses = msg.losses
    p.score.teamKills = msg.teamKills
    p.identity.callSign = msg.callSign
    p.identity.emailAddress = msg.emailAddress
    return p

### The End ###
        
    
