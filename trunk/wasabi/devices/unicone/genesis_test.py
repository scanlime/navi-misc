#!/usr/bin/env python
import re
from PyUnicone import Device, Controller, Systems

emu = Systems.GenesisEmulator(Device.GenesisDevice())

def plug(c):
    print "Plugged in: %r" % c

    try:
        portNum = int(re.match("Gamecube Controller (\d+)", c.name).group(1)) - 1
    except:
        return
    print "\tAttaching to port %d" % portNum

    # Manually set up a mapping appropriate for Sonic games
    Controller.Mapping(c, emu.attach(portNum),
                       BTN_START = 'BTN_C',
                       BTN_A     = 'BTN_A',
                       JS_ABS    = 'JS_ABS')

def unplug(c):
    print "Unplugged: %r" % c

bus = Controller.EvdevBus()
bus.onPlug.observe(plug)
bus.onUnplug.observe(unplug)
bus.onSync.observe(emu.sync)
bus.mainLoop()
