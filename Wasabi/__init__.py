""" Wasabi

This is a Python package providing modules used to implement Wasabi's
user interface software.
"""
#
# Wasabi Project
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

name    = "Wasabi"
version = "cvs-dev"

# Check the python version here before we proceed further
requiredPythonVersion = (2,2,1)
import sys, string
if sys.version_info < requiredPythonVersion:
    raise Exception("%s requires at least Python %s, found %s instead." % (
        name,
        string.join(map(str, requiredPythonVersion), "."),
        string.join(map(str, sys.version_info), ".")))

### The End ###
