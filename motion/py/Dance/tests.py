#!/usr/bin/env python
"""tests

Unit tests for the Dance module.
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

import unittest, sys, Numeric
from ODE import RK4, Time
from Systems import Lorenz
from Sequence import Sequence


def suite ():
    return unittest.TestSuite ([TestRK4, TestLorenz])


class TestRK4 (unittest.TestCase):
    def setUp (self):
        self.solver = RK4 (Lorenz (1, 1, 1))

    def testStep (self):
        """Test an RK4 step."""
        self.fail ("Not implemented.")


class TestLorenz (unittest.TestCase):
    def setUp (self):
        self.system = Lorenz (1, 1, 1)

    def testEquation (self):
        """Test the Lorenz system."""
        self.assertEqual (Numeric.array ([1, -9, 2]), self.system (2, 3, 4))


if __name__ == "__main__":
    suite().run(unittest.TestResult ())


# vim: ts=4:sw=4:et
