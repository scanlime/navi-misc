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
    data = []
    while size > 0:
        chunkSize = min(size, 0xF0000)
        command('R', 0x10, addr, chunkSize)
        data.append(spime.memRead(port, 0x10, chunkSize))
        addr += chunkSize
        size -= chunkSize
    return ''.join(data)

def writeBytes(addr, data):
    while data:
        chunkSize = min(len(data), 0xF0000)
        spime.memWrite(port, 0x10, data[:chunkSize])
        command('W', 0x10, addr, chunkSize)
        addr += chunkSize
        data = data[chunkSize:]

def write16(addr, word):
    command('H', addr, word)

def write32(addr, word):
    command('L', addr, word)

def cont():
    command('C')

hd = spime.hexDump
hdl = lambda s: hd(s, wordLE=True)
rb = readBytes
wb = writeBytes
wh = write16
wl = write32
