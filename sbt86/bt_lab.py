#!/usr/bin/env python
#
# Patches and hooks for the binary translation of LAB.EXE.
# Micah Dowty <micah@navi.cx>
#

import sbt86
import bt_common

b = sbt86.DOSBinary('LAB.EXE')

def chipTrace(b):
    """Debugging code: Add a memory trace which catches all *reads* to
       the memory where chip data is loaded.
       """
    bt_common.structTrace(b, structName='chip1', traceMode='r',
                          structBase=0xa400, structSize=2048,
                          itemTable=[
            ])
    bt_common.structTrace(b, structName='chip2', traceMode='r',
                          structBase=0xac00, structSize=2048,
                          itemTable=[
            ])

bt_common.patch(b)
bt_common.findSelfModifyingCode(b)
#bt_common.worldTrace(b)
chipTrace(b)

b.writeCodeToFile("bt_lab.c", "lab_main")
