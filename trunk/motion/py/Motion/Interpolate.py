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
        # n is the number of pairs of points.
        n = len (points) - 1

        # Create a list of all the intervals. intervals is a list of the form:
        #    [((t0,y0), (t1,y1)), ((t1,y1), (t2,y2)),...]
        intervals = []
        for i in range (n):
            t = 6 * i
            intervals.append (((t, points[i]), (t + 6, points[i + 1])))

        # Create the matrices
        s = n * 4
        A = Numeric.zeros ((s, s))
        b = Numeric.zeros ((s, 1))

        # FIXME - Filling the matrices could probably be optimized more...

        row = 0
        # For each interval, constrain the cubic approximation to equal the
        # endpoints at the endpoints.
        for i in range (n):
            col = 4 * i
            t, y = intervals[i][0]
            A[row, col:col+4] = [1, t, t**2, t**3]
            b[row] = y
            row += 1

            t, y = intervals[i][1]
            A[row, col:col+4] = [1, t, t**2, t**3]
            b[row] = y
            row += 1

        # Except for the end points, constrain the first and second derivatives of
        # each point to be equal across intervals.
        for i in range (n):
            col = 4 * i
            t, y = intervals[i][1]
            # First derivatives are equal.
            A[row, col:col+8] = [0, 1, 2*t, 3*t**2, 0, -1, -2*t, -3*t**2]
            row += 1
            # Second derivatives are equal.
            A[row, col:col+8] = [0, 0, 2, 6*t, 0, 0, -2, -6*t]
            row += 1

        # Constrain the second derivatives of the endpoints to be 0.
        t, y = intervals[0][0]
        A[row, :4] = [0, 0, 2, 6*t]
        row += 1
        t, y = intervals[-1][1]
        A[row, -4:] = [0, 0, 2, 6*t]

        # Solve for the coefficients.
        Ainv = inverse (A)
        z    = Numeric.matrixmultiply (Ainv, b)


# vim: ts=4:sw=4:et
