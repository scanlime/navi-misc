#
# Interpolate.py: Classes for interpolating motion data.
#
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

from LinearAlgebra import inverse
import Numeric

class Spline:
    """Represents a spline."""
    def __init__ (self, data):
        self.data = data
        A, b = self.__createMatrices (data)

        # Solve for the coefficients.
        Ainv = inverse (A)
        z    = Numeric.matrixmultiply (Ainv, b)

        self.constants = {}
        for i in range (Numeric.size (z) / 4):
            self.constants[(i, i+1)] = z[i*4:(i+1)*4, 0]

    def __createMatrices (self, data):
        """Generate the two matrices for the system of linear equations used to
           create the spline.
           """
        n = len (data) - 1

        # Create the matrices
        s = n * 4
        A = Numeric.zeros ((s, s))
        b = Numeric.zeros ((s, 1))

        # FIXME - Filling the matrices could probably be optimized more...
        row = 0
        for i in range (n):
            col = 4 * i
            A[row, col:col+4] = [1, i, i**2, i**3]
            b[row] = data[i]
            row += 1

            if i <= n:
                A[row, col:col+4] = [1, i+1, (i+1)**2, (i+1)**3]
                b[row] = data[i + 1]
                row += 1

        # Except for the end points, constrain the first and second derivatives of
        # each point to be equal across intervals.
        for i in range (1, n):
            col = 4 * (i - 1)
            # First derivatives are equal.
            A[row, col:col+8] = [0, 1, 2*i, 3*i**2, 0, -1, -2*i, -3*i**2]
            row += 1
            # Second derivatives are equal.
            A[row, col:col+8] = [0, 0, 2, 6*i, 0, 0, -2, -6*i]
            row += 1

        # Constrain the second derivatives of the endpoints to be 0.
        t, y = (0, data[0])
        A[row, :4] = [0, 0, 2, 6*t]
        row += 1
        t, y = (n, data[-1])
        A[row, -4:] = [0, 0, 2, 6*t]

        return (A, b)

    def interpolate (self, points):
        """Return a list representing the interpolated data with `points'
           additional inserted inbetween the data.
           """
        step = 1. / (points + 1)
        smoothed = [self.data[0]]

        for i in range (1, len (self.data)):
            a, b, c, d = self.constants[(i-1, i)]
            for j in Numeric.arange (i - 1, i, step):
                smoothed.append (a + b*j + c*j**2 + d*j**3)
            smoothed.append (self.data[i])

        return smoothed


    class BoneSpline:
        """A class for interpolating a single bone from an AMC file."""
        def __init__ (self, bone):
            self.data = bone
            self.splines = []

            for i in range (Numeric.size (bone, 1)):
                self.splines.append (Spline (self.data[,i]))

        def interpolate (self, points):
            smoothed = []

            for spline in splines:
                smoothed.append (spline.interpolate (points))

            return Numeric.array (smoothed)

# vim: ts=4:sw=4:et
