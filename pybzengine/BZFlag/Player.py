""" BZFlag.Player

Class representing a BZFlag player, divided up into subclasses for
areas like identity and motion that may be managed individually.
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

from BZFlag import Event, Animated
from BZFlag.World import Scale


class Identity:
    """A simple container for basic player information"""
    def __init__(self, callSign=None, team='rogue', emailAddress="PyBZFlag", type='tank', **kw):
        self.type = type
        self.callSign = callSign
        self.team = team
        self.emailAddress = emailAddress
        self.__dict__.update(kw)

    def __str__(self):
        return "%s (%s)" % (self.callSign, self.emailAddress)


class Motion:
    """Contains information about a player's motion, integrates velocity over time"""
    def __init__(self, position=[0,0,0], azimuth=0, velocity=[0,0,0], angularVelocity=0):
        self.velocity = list(velocity)
        self.position = Animated.Vector(Animated.Velocity(self.velocity), position)
        self.azimuth = azimuth
        self.angularVelocity = angularVelocity

    def integrate(self, dt):
        self.azimuth += dt * self.angularVelocity
        self.position.integrate(dt)


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
    def __init__(self):
        self.id = None
        self.status = []
        self.identity = Identity()
        self.motion = Motion()
        self.score = Score()
        Event.attach(self, 'onChangeStatus', 'onMove')

    def __str__(self):
        return "%s : %s" % (self.score, self.identity)

    def updateFromMessage(self, msg):
        """Update the player from a MsgPlayerUpdate"""
        if msg.status != self.status:
            self.status = msg.status
            self.onChangeStatus(self)

        self.motion.position.set(msg.position)
        self.motion.velocity[:] = list(msg.velocity)
        self.motion.azimuth = msg.azimuth
        self.motion.angularVelocity = msg.angularVelocity
        self.onMove(self)

    def integrate(self, dt):
        if not 'paused' in self.status:
            if 'falling' in self.status:
                self.motion.velocity[2] += Scale.Gravity * dt
            self.motion.integrate(dt)
        self.onMove(self)


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
