#!/usr/bin/env python
import pyunicone, time, sys


class ProgressOperation:
    def __init__(self, name):
        sys.stderr.write("Starting %s: " % name)

    def update(self, completion):
        sys.stderr.write("%.02f " % completion)

    def finish(self, message):
        if message:
	    sys.stderr.write("[%s]\n" % message)
	else:
            sys.stderr.write("Done.\n")

class MyProgressReporter:
    def start(self, op):
        return ProgressOperation(op)


dev = pyunicone.UniconeDevice()
dev.progress = MyProgressReporter()
dev.configure("fpga/test/test.bit")

while 1:
    dev.setLed(1, 0.015)
    time.sleep(0.5)
