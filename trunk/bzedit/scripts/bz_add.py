#!BPY
#
# bz_add.py - A simple script that makes new BZFlag objects
#             accessable via the "Add" menu.
#

""" Registration info for Blender menus:
Name: 'BZFlag Object...'
Blender: 234
Group: 'Add'
Submenu: 'Box' box
Submenu: 'Pyramid' pyramid
Submenu: 'World' world
Tip: 'Add a new BZFlag object'
"""

import Blender

try:
    import bzflag
except ImportError:
    Blender.Draw.PupMenu("Error importing 'bzflag' module%t|"
                         "You may have to start the Blender game engine|"
                         "momentarily then exit it to set the Python|"
                         "path properly.")
    bzflag = None

if bzflag:
    # Create a new BZObject and convert that to a new Blender object
    bzo = bzflag.getTypeRegistry().dict[__script__.arg]()
    object = bzo.toBlender()

    # Put the new object at the cursor
    object.setLocation(*Blender.Window.GetCursorPos())

### The End ###
