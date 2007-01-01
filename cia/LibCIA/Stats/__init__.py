""" LibCIA.Stats

The stats subsystem of CIA provides a way to associate messages
with 'stats targets', storing and retrieving the resulting data.

A target is a place messages can be delivered, identified by
a unix-style path name. Note that this path has no connection
to the real filesystem, it's just a convenient notation.

When a message is delivered to a stats target, the message itself
is stored in a FIFO, counters keeping track of that target's
activity are incremented, and associations between stats targets
are strengthened.

Additionally, metadata can be assigned to these stats targets
to help present them in a less abstract way.

This package is the backend for the stats system. It takes care
of actually storing and retrieving the data, and it provides
python and XML-RPC interfaces to this functionality. Messages
are directed into the stats subsystem using a stats:// URI handler.
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

# Convenience imports
import Handler
import Interface
import Target

### The End ###
