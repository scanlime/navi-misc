#!/usr/bin/env python
#
# Patches and hooks for the binary translation of LAB.EXE.
# Micah Dowty <micah@navi.cx>
#

import sbt86
import bt_common

b = sbt86.DOSBinary('LAB.EXE')

bt_common.patch(b)

b.writeCodeToFile("bt_lab.c", "lab_main")
