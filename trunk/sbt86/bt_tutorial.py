#!/usr/bin/env python
#
# Patches and hooks for the binary translation of TUT.EXE.
# Micah Dowty <micah@navi.cx>
#

import sbt86
import bt_common

b = sbt86.DOSBinary('TUT.EXE')

bt_common.patch(b)

b.writeCodeToFile("bt_tutorial.c", "tutorial_main")
