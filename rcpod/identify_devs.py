#!/usr/bin/env python
#
# A simple script to assist in identifying
# multiple rcpod devices connected to the USB bus.
# For each detected device, displays the device number
# while flashing an LED connected to RC2 (LED1 on the
# rcpod-485 board). Upon pressing enter, the next
# device flashes.
#
# -- Micah Dowty <micah@picogui.org>
#

import pyrcpod, threading, time, sys


class BlinkyThread(threading.Thread):
    def __init__(self, availableDevice):
        threading.Thread.__init__(self)
        openedDevice = availableDevice.open()
        self.led = openedDevice.rc2
        self.led.output().assert_()
        self.running = True

    def run(self):
        while self.running:
            self.led.assert_()
            time.sleep(0.1)
            self.led.deassert()
            time.sleep(0.1)
        self.led.deassert()

    def stop(self):
        self.running = False


def main():
    devs = pyrcpod.devices
    print "Found %d device%s" % (len(devs), "s"[len(devs)==1:])

    for i in xrange(len(devs)):
        dev = devs[i]
        print "Blinking LED1 on device %d... <enter> to continue" % i
        bt = BlinkyThread(dev)
        try:
            bt.start()
            sys.stdin.readline()
        finally:
            bt.stop()


if __name__ == '__main__':
    main()
