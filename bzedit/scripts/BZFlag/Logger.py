""" BZFlag.Logger

This class collects error and warning messages, presening them
to the user in batches.
"""
#
# Copyright (C) 2005 David Trowbridge
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
import Blender

class Logger:
    def __init__(self):
        self.clear()

    def clear(self):
        self.log = []
        self.numErrors = 0
        self.numMessages = 0
        self.numWarnings = 0

    def msg(self, s):
        """Log a nonfatal message"""
        self.log.append(s)
        self.numMessages += 1

    def err(self, s):
        """Log an error"""
        self.log.append("ERROR: %s" % s)
        self.numErrors += 1

    def warn(self, s):
        """Log a nonfatal warning"""
        self.log.append("Warning: %s" % s)
        self.numWarnings += 1

    def report(self, title="Errors reported"):
        """Report errors to the user"""
        Blender.Draw.PupMenu("%s:%%t|%s" % (title, "|".join(self.log)))
        self.clear()
