#!/usr/bin/env python
#
# A cheesy little tool to save a CSV file with
# analog readings to the name given on the command line.
#
# -- Micah Dowty <micah@navi.cx>
#

import gtk, pyrcpod, time, sys

if not pyrcpod.devices:
    print "No rcpod device detected"
    sys.exit(1)
rcpod = pyrcpod.devices[0].open()

if len(sys.argv) != 2:
    print "usage: %s output.csv" % sys.argv[0]
    sys.exit(1)

logfile = open(sys.argv[1], "w")
def log(values):
    formatted = ', '.join(map(str, values))
    logfile.write(formatted + "\n")

print "Collecting data to %s, press CTRL-C to stop" % (sys.argv[1])

try:
    startTime = time.time()
    log(["Time (s)"] + ["Channel %s (V)" % i for i in range(8)])
    while 1:
       readings = rcpod.analogReadAll()
       log([time.time() - startTime] + [c / 255.0 * 5.0 for c in readings])
except KeyboardInterrupt:
    pass
print "Finished successfully"

### The End ###
