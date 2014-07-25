#
# tests.py - Unit tests for the Motion module.
#
# Copyright (C) 2005-2007 W. Evan Sheehan
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

from unittest import makeSuite, TestCase, TestSuite
from LinearAlgebra import inverse
import Interpolate, Numeric

def suite():
    tests = (makeSuite(TestSplines))
    return TestSuite(tests)


class TestSplines(TestCase):
    """Test the spline functions in Interpolate.py"""
    def setUp(self):
        self.data = Numeric.array([[1,3],[2,3],[4,2],[7,5]])
        self.A = Numeric.array([[1,1,1,1,0,0,0,0,0,0,0,0],
                               [1,2,4,8,0,0,0,0,0,0,0,0],
                               [0,0,0,0,1,2,4,8,0,0,0,0],
                               [0,0,0,0,1,3,9,27,0,0,0,0],
                               [0,0,0,0,0,0,0,0,1,3,9,27],
                               [0,0,0,0,0,0,0,0,1,4,16,64],
                               [0,1,4,12,0,-1,-4,-12,0,0,0,0],
                               [0,0,2,12,0,0,0,-2,-12,0,0,0],
                               [0,0,0,0,0,1,6,27,0,-1,-6,-27],
                               [0,0,0,0,0,0,2,18,0,0,-2,-18],
                               [0,0,2,6,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0,2,24]])
        self.b = [Numeric.reshape(Numeric.array([1,2,2,4,4,7,0,0,0,0,0,0]), (12,1)),
                  Numeric.reshape(Numeric.array([3,3,3,2,2,5,0,0,0,0,0,0]), (12,1))]

    def testMatrices(self):
        """Test the creation of matrices"""
        A, b = Interpolate._getMatrix(self.data, 2)
        self.assertEqual(b, self.b)
        self.assertEqual(A, self.A)

    def testInterpolate(self):
        """Test the interpolation"""
        self.fail("Not implemented")


if __name__ == "__main__":
    from unittest import TextTestRunner
    TextTestRunner(verbosity=2).run(suite())


# vim: ts=4:sw=4:et
