#!/usr/bin/env python
#
# Patches and hooks for the binary translation of GAME.EXE.
# Micah Dowty <micah@navi.cx>
#

import sys
import sbt86
import bt_common

b = sbt86.DOSBinary('build/game.exe')

bt_common.patch(b)
bt_common.patchChips(b)
bt_common.patchLoadSave(b)

b.decl("#include <stdio.h>")
b.patchAndHook(b.findCode('2c01 :2f a2____ a2____ b12c 32ed'),
               'nop', length=1, cCode='''
   sassert(false, "Unimplemented DAS instruction\\n");
''')

b.writeCodeToFile('build/bt_game.cpp', 'GameEXE')
