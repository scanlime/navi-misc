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

try:
    import bzflag
except ImportError:
    Blender.Draw.PupMenu("Error importing 'bzflag' module%t|"
                         "You may have to start the Blender game engine|"
                         "momentarily then exit it to set the Python|"
                         "path properly.")
    bzflag = None

bzflag.getTypeRegistry().dict[__script__.arg]().toBlender()

### The End ###
