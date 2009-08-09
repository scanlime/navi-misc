#!/usr/bin/env python

import spime
import struct

port = spime.PacketPort("/dev/ttyUSB1")
spime.action_ping(port)

def command(cmd, arg0=0, arg1=0, arg2=0):
    spime.memWrite(port, 4, struct.pack("<III", arg0, arg1, arg2))
    spime.memWrite(port, 0, cmd)
    while spime.memRead(port, 0, 1) != '\0':
        pass

def readBytes(addr, size=0x40):
    command('R', 0x10, addr, size)
    return spime.memRead(port, 0x10, size)

def writeBytes(addr, data):
    spime.memWrite(port, 0x10, data)
    command('W', 0x10, addr, len(data))

def write16(addr, word):
    command('H', addr, word)

def write32(addr, word):
    command('L', addr, word)

hd = spime.hexDump
hdl = lambda s: hd(s, wordLE=True)
rb = readBytes
wb = writeBytes
wh = write16
wl = write32
