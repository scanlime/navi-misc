#!/usr/bin/env python
import pyunicone, time


class ProgressOperation:
    def __init__(self, name):
        print "Starting %s:" % name,

    def report(self, completion):
        print "%.02f" % completion,

    def finish(self):
        print "Done."


class MyProgressReporter:
    def start(self, op):
        return ProgressOperation(op)


dev = pyunicone.UniconeDevice()
dev.progress = MyProgressReporter()
dev.configure("fpga/test/test.bit")

while 1:
    dev.setLed(1, 0.015)
    time.sleep(0.5)
