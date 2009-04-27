#!/usr/bin/env python
#
# Patches and hooks for the binary translation of TUT.EXE.
# Micah Dowty <micah@navi.cx>
#

import sbt86
import bt_common

b = sbt86.DOSBinary('TUT.EXE')

bt_common.patch(b)
#bt_common.findSelfModifyingCode(b)

# Debug: Trace all IPs which access memory (for diagnosing hangs)
#b.trace('rw', 'return 1;', 'printf("%04x ", ip);')

b.writeCodeToFile("../build/tutorial.bt.c", "tutorial_main")
