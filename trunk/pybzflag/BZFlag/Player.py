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


class Player:
    """Container for information about a player, contains
       zero or more of the classes below.
       """
    def __init__(self, identity=None, motion=None):
        self.identity = identity
        self.motion = motion


class Identity:
    """A simple container for basic player information"""
    def __init__(self, callSign, team='rogue', emailAddress="PyBZFlag", type='tank'):
        self.type = type
        self.callSign = callSign
        self.team = team
        self.emailAddress = emailAddress

### The End ###
        
    
