#!/usr/bin/env python
"""tests

Unit tests for the Dance module.
"""

# Copyright (C) 2006-2007 W. Evan Sheehan
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

import sys, math, Numeric
from unittest import makeSuite, TestCase, TestSuite
from ODE import RK4, Time
from Systems import Lorenz
from Sequence import Sequence, _Coordinate


def suite ():
    tests = (makeSuite (TestRK4),
            makeSuite (TestLorenz),
            makeSuite (TestSequence),
            makeSuite (TestCoordinate))
    return TestSuite (tests)


class TestRK4 (TestCase):
    """Test the Runge-Kutte 4 solver."""
    def setUp (self):
        self.solver = RK4 (Lorenz (1, 1, 1))

    def testStep (self):
        """Test an RK4 step"""
        self.fail ("Not implemented.")


class TestLorenz (TestCase):
    """Test the Lorenz equations."""
    def setUp (self):
        self.system = Lorenz (1, 1, 1)

    def testEquation (self):
        """Test the Lorenz system"""
        self.assertEqual (Numeric.array ([1, -9, 2]), self.system ([2, 3, 4],
            0, 0))

class TestCoordinate (TestCase):
    """Test the hashable _Coordinate class."""
    def setUp (self):
        self.coordinates = [\
                _Coordinate ([1,1,1]), \
                _Coordinate ([2,2,2]), \
                _Coordinate ([1,2,1])]

    def testLess (self):
        """Test _Coordinate less than operator"""
        self.assertTrue (self.coordinates[0] < self.coordinates[1])
        self.assertFalse (self.coordinates[1] < self.coordinates[0])

    def testEqual (self):
        """Test _Coordinate equals operator"""
        self.assertTrue (self.coordinates[0] == _Coordinate ([1,1,1]))
        self.assertFalse (self.coordinates[0] == self.coordinates[1])

    def testGreater (self):
        """Test _Coordinate greater than operator"""
        self.assertTrue (self.coordinates[1] > self.coordinates[0])
        self.assertFalse (self.coordinates[0] > self.coordinates[1])

    def testNotEqual (self):
        """Test _Coordinate not equal to operator"""
        self.assertTrue (self.coordinates[0] != self.coordinates[1])
        self.assertFalse (self.coordinates[0] != _Coordinate ([1,1,1]))

    def testHash (self):
        """Test _Coordinate hashing"""
        x = {self.coordinates[0]:0}
        self.assertEqual (x[self.coordinates[0]], 0)
        self.assertEqual (x[_Coordinate([1,1,1])], 0)


class TestSequence (TestCase):
    """Test the Sequence class that maps frames to a chaotic trajectory."""
    def setUp (self):
        data = self._createFakeData ()
        ic = Numeric.array ([2, 3, 4])
        self.seq = Sequence (Lorenz (1, 1, 1), ic, Time (0, 100, .01), data)

    def testDistance (self):
        """Test the Euclidean distance calculation"""
        x = Numeric.array ([1,1,1])
        y = Numeric.array ([2,2,2])

        self.assertEqual (self.seq._distance (x, y), math.sqrt (3))

    def testNearestNeigbor (self):
        """Test the calculation of the nearest neighbor"""
        self.fail ("Not implemented")

    def testShuffle (self):
        """Test the chaotic shuffle"""
        # FIXME This test is pretty meaningless.
        shuffled = self.seq.shuffle ([3,2,9], Time (0, 100, .01))
        original = self.seq.mapping.values ()

        for d in shuffled:
            self.assertTrue (d in original)

    def _createFakeData (self):
        """Return a dictionary that can be used as data for the Sequence
           object.
           """
        d = {}
        keys = [letter for letter in "ABCDEFG"]
        for key in keys:
            d[key] = Numeric.array ([[1,2,3],[4,5,6],[7,8,9]])

        return d


if __name__ == "__main__":
    from unittest import TextTestRunner
    TextTestRunner (verbosity=2).run (suite ())


# vim: ts=4:sw=4:et
