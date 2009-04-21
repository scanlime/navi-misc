#!/usr/bin/env python
#
# Patches and hooks for the binary translation of GAME.EXE.
# Micah Dowty <micah@navi.cx>
#

import sbt86
import bt_common

b = sbt86.DOSBinary('GAME.EXE')

bt_common.patch(b)

b.decl("#include <stdio.h>")
b.patchAndHook("0DAB:BC3F", 'nop', length=1, cCode='''
   printf("XXX: Skipping unimplemented 'das' instruction.\\n");
''')

b.writeCodeToFile("bt_game.c", "game_main")
