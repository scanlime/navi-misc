#!/usr/bin/env python
#
# Patches and hooks for the binary translation of PLAY.EXE.
# Micah Dowty <micah@navi.cx>
#

import sys
import sbt86

b = sbt86.DOSBinary('build/play.exe')

b.writeCodeToFile('build/bt_play.c', "play_main")
