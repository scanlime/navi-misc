"""Base

Base classes for the motion test suites.
"""

# Copyright (C) 2005 W. Evan Sheehan
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

import sys, re, unittest

class Frontend:
    def main (self):
        """Run the tests specified on the command line."""
        self.loadTests ()
        for suite in self.tests:
            unittest.TextTestRunner (verbosity=3).run (suite)

    def loadTests (self):
        self.tests = []
        loader = unittest.defaultTestLoader

        for testname in sys.argv[1:]:
            print "Loading: test_" + testname
            self.tests.append (loader.loadTestsFromModule ("test_"+testname))

# vim: ts=4:sw=4:et
