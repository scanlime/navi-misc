#!/usr/bin/env python

from Motion import AMC
from LinearAlgebra import inverse
import Numeric, sys

if len (sys.argv) != 3:
    print "usage:", sys.argv[0], "<input AMC file> <output AMC file>"
    sys.exit (1)

amc = AMC.from_file (sys.argv[1])
interpolated = AMC ()
interpolated.format = amc.format

for bone, data in amc.bones.iteritems():
    temp = []
    # Iterate across the rows of the transposed matrix. We have to transpose
    # because we want a list of the angles in one degree of freedom across all
    # of the frames, but the data is stored with the rows being the angles for
    # all degrees of freedom across only one frame.
    for row in Numeric.transpose (data): temp.append (interpolate (row))
    # Create a new matrix from the smoothed data and transpose it again so that
    # the rows and columsn are frames and degrees of freedom, respectively.
    interpolated.bones[bone] = Numeric.transpose (Numeric.array (temp))

interpolated.save (sys.argv[2])

# vim: ts=4:sw=4:et
