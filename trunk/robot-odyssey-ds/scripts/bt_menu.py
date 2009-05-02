#!/usr/bin/env python
#
# Patches and hooks for the binary translation of MENU.EXE.
# Micah Dowty <micah@navi.cx>
#

import sys
import sbt86

b = sbt86.DOSBinary('build/menu.exe')

# XXX: Dynamic branch, looks sound related.
b.patch('010E:0778', 'nop', 2)

# XXX: For now, implement screen updates by tracing the framebuffer.
#      Ideally we'd hook all the drawing routines, but this isn't
#      actually that awful. Another approach would be to poll on
#      a separate thread.

b.decl("#include <stdio.h>")
b.trace('w', '''
   return segment == 0xB800;
''', '''
   static uint32_t hit = 0;

   hit++;
   if ((hit & 0x3F) == 0) {
       hw->drawScreen(proc->memSeg(0xB800));
   }
''')

b.writeCodeToFile('build/bt_menu.cpp', 'MenuEXE')
