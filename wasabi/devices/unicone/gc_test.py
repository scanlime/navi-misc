#!/usr/bin/env python
import re
from PyUnicone import Device, Controller, Systems

unicone = Device.UniconeDevice()
emu = Systems.GamecubeEmulator(unicone)

def plug(c):
    print "Plugged in: %r" % c

    try:
        portNum = int(re.match("Gamecube Controller (\d+)", c.name).group(1)) - 1
    except:
        return
    print "\tAttaching to port %d" % portNum
    emu.attach(c, portNum)

def unplug(c):
    print "Unplugged: %r" % c
    try:
        emu.remove(c)
    except ValueError:
        pass
    else:
        print "\tRemoved from emulator"

bus = Controller.EvdevBus()
bus.onPlug.observe(plug)
bus.onUnplug.observe(unplug)
bus.onSync.observe(emu.sync)
bus.mainLoop()
