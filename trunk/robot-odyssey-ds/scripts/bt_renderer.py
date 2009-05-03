#!/usr/bin/env python
#
# This is a special-purpose binary translation patch which starts with
# GAME.EXE, and creates a trimmed down binary which we run internally
# to perform rendering on the sub screen.
#
# We want to start with GAME.EXE since it's the only binary that knows
# about the fourth robot, but we trim out most of the code via
# patching.  Everything that modifies the world state can be removed,
# since we're just rendering the state copied from another binary.
#
# Micah Dowty <micah@navi.cx>
#

import sbt86
import bt_common

b = sbt86.DOSBinary('build/game.exe')

bt_common.patch(b)
bt_common.patchChips(b)

# Skip command line parsing
b.patch('0DAB:0005', 'jmp 0x005D')

# Skip reading world data
b.patch('0DAB:01A3', 'jmp 0x01CE')

# Skip reading circuit data
b.patch('0DAB:01F0', 'jmp 0x0219')

# Statically remove all main loops other than Level 1, skip video mode
# initialization, and skip chip loading. We still need to be sure to
# run the sprite table initialization though, which is buried in the
# same subroutine as chip loading.
b.patch('0DAB:2C0F', 'jmp 0x66b5')
b.patch('0DAB:38B9', 'jmp 0x3988')

# Skip the first part of the main loop, from the top down to just
# before update_all_object_motion. This nulls out all input processing.
# We want update_all_object_motion so that grabbed objects stay attached
# to the robots when we move them.
b.patch('0DAB:66DF', 'jmp 0x6708')

# Now there's some more input code, robot updates, and circuit simulation.
# Skip all of that. This leaves off just before SBTHALT_LOAD_ROOM_ID kicks in.
b.patch('0DAB:670B', 'jmp 0x674C')

# Skip soldering
b.patch('0DAB:677C', 'jmp 0x6790')

# Skip sound
b.patch('0DAB:67BC', 'jmp 0x67bf')

# Skip grabbing
b.patch('0DAB:679c', 'jmp 0x67ad')

# Skip level-specific functions. (These are all main loop routines which
# don't appear in every level's main loop)
b.patch('0DAB:6721', 'jmp 0x672a')
b.patch('0DAB:675b', 'jmp 0x675e')
b.patch('0DAB:6761', 'jmp 0x6764')
b.patch('0DAB:676a', 'jmp 0x6776')

# Skip text rendering. We don't need it.
b.patch('0DAB:6758', 'jmp 0x675b')

b.writeCodeToFile('build/bt_renderer.cpp', 'RendererEXE')
