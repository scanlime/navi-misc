""" BZEngine.UI.Drawable.VRML

A drawable that can load a very small subset of the VRML model format.
This module does not make any attempt to resemble a full VRML browser
implementation. It simply understands small bits of the VRML syntax
and uses this to extract geometry information.

Why VRML?

The other drawables in this directory are here because their geometry
largely depends on per-instance parameters. Objects like player models
and interface widgets, however, need little modification at runtime
and would be much more difficult to construct. Hence, the need for a
model format.

The requirements for this model format didn't leave many options open:

 - Easy to export from Blender
 - Relatively easy to parse
 - Capable of representing multiple named meshes per file, with
   transformation information for each
 - Preferably an open standard

VRML seemed to be the only format meeting all the criteria. There is
already a comprehensive VRML97 browser as part of the OpenGLContext
package, but that would be extreme overkill for our simple needs here.

This has been tested with VRML files from Blender and Milkshape 3D,
as well as a few random VRML files found on the 'net.
"""
#
# Python BZEngine Package
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

from Parser import *
from Mesh import *
from Cache import *


defaultCache = Cache()
def load(name):
    from BZEngine import Util
    return defaultCache.load(Util.dataFile(name))

### The End
