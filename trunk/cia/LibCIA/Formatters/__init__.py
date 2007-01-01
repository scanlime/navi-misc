""" LibCIA.Formatters

A collection of Formatter subclasses that can be searched and
instantiated via the 'factory' object here.

This is a package holding modules for each category of formatter.
The modules are aggregated together here and indexed by the factory.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2007 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import Commit, ColorText, Builder, Other, Patch

_factory = None

def getFactory():
    from LibCIA import Message
    global _factory
    if not _factory:
        _factory = Message.FormatterFactory(Commit, ColorText, Builder, Other, Patch)
    return _factory

### The End ###
