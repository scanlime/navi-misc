#!/usr/bin/env python
#
# Patches and hooks for the binary translation of LAB.EXE.
# Micah Dowty <micah@navi.cx>
#

import sys
import sbt86
import bt_common

b = sbt86.DOSBinary('build/lab.exe')

bt_common.patch(b)
bt_common.patchChips(b)
bt_common.patchLoadSave(b)

b.writeCodeToFile('build/bt_lab.cpp', 'LabEXE')
