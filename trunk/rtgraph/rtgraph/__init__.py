""" rtgraph

A python package providing several widgets for graphing data
in real-time, using PyGTK.
"""
#
# rtgtaph real-time graphing package for PyGTK
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

__version__ = "pre-0.70"

# Check the python version here before we proceed further
requiredPythonVersion = (2,2,1)
def checkVersion():
    import sys, string
    if sys.version_info < requiredPythonVersion:
        raise Exception("%s requires at least Python %s, found %s instead." % (
            name,
            string.join(map(str, requiredPythonVersion), "."),
            string.join(map(str, sys.version_info), ".")))
del checkVersion

# Convenience imports
from HScrollLineGraph import *
from HScrollGraph import *
from Channel import *
from Graph import *
from PolarVectorGraph import *
from IsometricVectorGraph import *
from UI import *
import Tweak

### The End ###
