#!/usr/bin/env python
#
# A utility to interactively train the learning remote used with Wasabi
#

import os, sys, time
from Wasabi import IR

remoteDir = "remotes"
remoteName = "wasabi"

# Open our remote config file and scan for code names
conf = open(os.path.join(remoteDir, remoteName))
while conf.readline().strip() != 'begin codes':
    pass
codes = []
while True:
    line = conf.readline().strip()
    if line == 'end codes':
        break
    codes.append(line.split(" ")[0])

print """Interactive utility for training Wasabi's learning remote:
After a code name is given:
   - On the remote, press 'learn' then the named button
   - Once it displays 'ok', press enter
"""

tx = IR.Transmitter()

try:
    for code in codes:
        print code
        tx.sendStart(remoteName, code)
        sys.stdin.readline()
        tx.sendStop(remoteName, code)
        time.sleep(0.5)
finally:
    # Make sure to stop repeating the code if we were interrupted
    try:
        tx.sendStop(remoteName, code)
    except:
        pass

