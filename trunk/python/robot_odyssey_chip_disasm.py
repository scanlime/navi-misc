#!/usr/bin/env python
#
# Robot Odyssey chip disassembler.
#
# This program can disassemble chip designs saved by Robot Odyssey and
# the stock chips which come with the game. The chips are stored in a
# bytecode format that resembles an electrical netlist. Our output is
# a textual representation of this bytecode.
#
# Understanding the output
# ------------------------
#
# The resulting listing is similar to an electrical netlist. It lists
# each component's type, inputs, and outputs. The listing could also
# be viewed as a kind of assembly language. It is a series of
# instructions, in the same order they are executed during actual chip
# simulation in-game.
#
# The game parses through the chip's bytecode once per 'clock cycle'.
# Each instruction reads its inputs, flip-flops update their internal
# state, then one or two output values are produced. The order outputs
# are applied in depends on the instruction:
#
#   - All Node outputs propagate immediately.
#
#   - Outputs which go to pins on the *current* chip are propagated
#     immediately.
#
#   - All other outputs are put on a queue for later propagation.
#     They are propagated just prior to the Nodes at the end of
#     the chip.
#
# There may be slight differences between the way circuits are
# simulated in chips and in the rest of the game. Flip-flops in chips
# will not toggle if the inputs are both high. There is also an extra
# clock of propagation delay for signals that cross a nested chip
# boundary, since all I/O to a nested chip goes through its pin
# values. Even though the interpreter allows this, chips are never
# compiled such that an outer chip reads or writes a nested chip's
# gates directly.
#
# Nets are assigned a unique name based on the part that they appeared
# as an input to. Values inside <> represent the current state of an
# object. All pins are labeled as <0> (off) or <1> on. Flip-flops can
# be in the <01> or <10> state.
#
# Flip-flops have two inputs and two outputs, corresponding to their
# two halves. They also have two internal 'pins' which keep track of
# their current state. These pins don't show up as inputs or outputs.
#
# License
# -------
#
# Copyright (c) 2009 Micah Dowty <micah@navi.cx>
#
#    Permission is hereby granted, free of charge, to any person
#    obtaining a copy of this software and associated documentation
#    files (the "Software"), to deal in the Software without
#    restriction, including without limitation the rights to use,
#    copy, modify, merge, publish, distribute, sublicense, and/or sell
#    copies of the Software, and to permit persons to whom the
#    Software is furnished to do so, subject to the following
#    conditions:
#
#    The above copyright notice and this permission notice shall be
#    included in all copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#    OTHER DEALINGS IN THE SOFTWARE.
#

import struct
import sys


class FormatError(Exception):
    pass

class Pin:
    """One electrical pin on a chip or gate. Pins always have a current
       state, either 0 or 1. Pins can have a name, so we can uniquely
       identify them in the disassembly.
       """
    def __init__(self, state, name=None):
        if isinstance(state, str):
            state = ord(state)

        if state > 1:
            raise FormatError("Pin state must be 0 or 1")

        self.state = state
        self.name = name

    def __repr__(self):
        return "%s<%d>" % (self.name, self.state)

    def __str__(self):
        return self.name


def _formatLine(operation, args, outputs=None):
    s = '%-10s %s' % (operation, ' '.join(map(repr, args)))
    if outputs:
        s += ' => %s' % ' '.join(map(repr, outputs))
    return s


class Gate:
    """One logic gate. A gate has a name, a list of input pins,
       and a list of output pins. This is an abstract base class,
       each type of gate has a subclass of Gate.
       """
    def __init__(self, inputs, outputs, name=None, state=()):
        if name is None:
            self.__class__._index += 1
            name = "%s%d" % (self.__class__.__name__, self.__class__._index)

        self.inputs = inputs
        self.outputs = outputs
        self.name = name
        self.state = state

        self._assignNames(self.inputs, 'in')
        self._assignNames(self.state, 'state')

    def _assignNames(self, pins, prefix):
        # Assign names to any unnamed pins
        for i, pin in enumerate(pins):
            if pin.name is None:
                pin.name = '%s_%s%d' % (self.name, prefix, i)

    def __repr__(self):
        return _formatLine(self.__class__.__name__,
                           self.inputs, self.outputs)


class AND(Gate):
    _index = 0
    def __init__(self, a, b, outputs, name=None):
        Gate.__init__(self, (a, b), (outputs,), name)

class OR(Gate):
    _index = 0
    def __init__(self, a, b, outputs, name=None):
        Gate.__init__(self, (a, b), (outputs,), name)

class XOR(Gate):
    _index = 0
    def __init__(self, a, b, outputs, name=None):
        Gate.__init__(self, (a, b), (outputs,), name)

class NOT(Gate):
    _index = 0
    def __init__(self, input, outputs, name=None):
        Gate.__init__(self, (input,), (outputs,), name)

class Node(Gate):
    _index = 0
    def __init__(self, input, outputs, name=None):
        Gate.__init__(self, (input,), (outputs,), name)

class FF(Gate):
    _index = 0
    def __init__(self, in0, in1, state0, state1, out0, out1, name=None):
        Gate.__init__(self, (in0, in1,), (out0, out1), name, (state0, state1))

    def __repr__(self):
        name = '%s<%d%d>' % (self.__class__.__name__,
                             self.state[0].state, self.state[1].state)
        return _formatLine(name, self.inputs, self.outputs)


class AddressSpace:
    """An address space for pins. Maintains a mapping from offset to
       Pin object. Address spaces may be carved up into nested address
       spaces, in order to support nested chips.
       """
    def __init__(self):
        self._offset = 0
        self._map = {}

    def offset(self, x):
        """Create a linked address space which is offset by 'x'."""
        space = self.__class__()
        space._offset = self._offset + x
        space._map = self._map
        return space

    def lookupPin(self, data, offset, name=None):
        """Return a Pin object for the specified offset in the chip data.
           If it exists, return it. Otherwise, create the pin.
           If the pin is not yet named, optionally name it.
           """
        myOffset = self._offset + offset

        if len(data) <= offset:
            raise FormatError("End of chip data or invalid pin address 0x%04x" % offset)

        if myOffset in self._map:
            pin = self._map[myOffset]
            if pin.name is None:
                pin.name = name
        else:
            pin = Pin(data[offset], name)
            self._map[myOffset] = pin

        return pin


class Chip:
    """A Robot Odyssey chip. 'data' is the chip's electrical data,
       in the same format used in CSV files, CHP files, and in memory.
       CSV files also contain pin directions, bytecode length, and
       help text.

       If 'data' is at least 1KB, we'll assume it's in CSV
       format. Otherwise we assume it's in CHP format.

       Chips can optionally have a name, so they can be identified in
       the disassembly.
       """

    name = None
    chipData = None
    bcOffset = None
    helpText = None
    pinDirections = None
    pins = None
    parts = None
    addrMap = None

    _pinDirEnum = (None, 'in', 'out')
    _index = 0

    def __init__(self, data, name=None, format=None, addrSpace=None):
        if format is None:
            if len(data) >= 1024:
                format = 'CSV'
            else:
                format = 'CHP'

        if name is None:
            self.__class__._index += 1
            name = "%s%d" % (self.__class__.__name__, self.__class__._index)

        self.name = name
        self.addrSpace = addrSpace or AddressSpace()
        getattr(self, '_parse%s' % format)(data)

    def _parsePinObjects(self, data):
        # Parse a chip's group of electrical Pins.
        # Populates self.pins, and adds the pins to addrMap.

        # WARNING: The pins aren't actually in the normal order.
        #          Pins are labeled like they are in electronics:
        #          counterclockwise from the top-left. But in the
        #          file, they're stored 1-4 then 8-5 (two top-down
        #          rows). We keep the pins in this order in our
        #          'pins' and 'pinDirections' lists, but we rename
        #          them according to electronics conventions.

        self.pins = []
        for addr, byte in enumerate(data[:8]):
            if addr < 4:
                pinNum = addr + 1
            else:
                pinNum = 4 + 8 - addr

            self.pins.append(self.addrSpace.lookupPin(
                    data, addr, '%s_pin%d' % (self.name, pinNum)))

    def _parseCSV(self, data):
        # CSV format. Normal electrical data occupies the first
        # kilobyte of the file. The rest is:
        #
        #  - A table of pin directions
        #
        #  - The length of the CHP data. The rest of the first
        #    kilobyte appears to be unused, though it isn't always
        #    zeroed.
        #
        #  - The help screen

        if len(data) != 1333:
            raise FormatError("Length of CSV file must always be 1333 bytes")

        self._parsePIN(data[0x400:0x408])
        self._parseHelpText(data[0x40A:])

        chpSize = struct.unpack("<H", data[0x408:0x40A])[0]
        self._parseCHP(data[:chpSize])

        # XXX: I'd like to be able to verify that len(self.chipData)
        #      equals chpSize here, but that isn't always the case.
        #      The original WALLHUG.CSV, for example, claims the
        #      bytecode is one byte longer than it really is.

    def _parsePIN(self, data):
        # Parse the pin direction table included in CSV files
        # or as a stand-alone PIN file.

        if len(data) != 8:
            raise FormatError("Pin direction table must be 8 bytes long")
        self.pinDirections = []

        for byte in data:
            byte = ord(byte)
            if byte > 2:
                raise FormatError("Invalid byte 0x%02 in pin direction table" % byte)
            self.pinDirections.append(self._pinDirEnum[byte])

    def _parseHelpText(self, data):
        # Parse the table of help text. This is an array of 9
        # zero-terminated strings, all padded with spaces. The first
        # line (title) is 18 characters. The others are 34 chars.

        lines = data.split('\x00')

        if len(lines) != 10:
            raise FormatError("Incorrect number of lines in help text")
        if lines[-1] != '':
            raise FormatError("Help text missing final NUL byte")

        if len(lines[0]) != 18:
            raise FormatError("Help text title line has incorrect length")
        self.helpText = [lines[0].rstrip()]

        for line in lines[1:-1]:
            if len(line) != 34:
                raise FormatError("Help text line has incorrect length")
            self.helpText.append(line.rstrip())

    def _parseCHP(self, data):
        # Chip electrical data. The first 8 bytes are the pin states,
        # the rest is bytecode. Offsets are relative to the beginning
        # of the chip data, so store the whole thing.
        #
        # In a nested chip, we don't know how long the chip data is
        # until parsing the bytecode. So 'data' may be longer than the
        # actual bytecode data for this chip. We'll truncate chipData
        # to the length of this chip, so callers can check
        # len(chipData) to see how long this chip actually was.

        # Start a map which tracks pins by address
        self.addrMap = {}

        # Create pins. Bytecode starts immediately afterward.
        self._parsePinObjects(data[:8])
        bcOffset = 8

        # Bytecode parsing loop. Each bytecode has a handler function
        # that returns the next offset to parse from. After bytecode
        # 0x07 has finished, this chip is done.

        self.parts = []
        while True:
            byte = ord(data[bcOffset])
            fn = getattr(self, '_bytecode_%02x' % byte, None)
            if not fn:
                raise FormatError("Unknown bytecode 0x%02x" % byte)
            bcOffset = fn(data, bcOffset + 1)
            if byte == 0x07:
                break

        # Now we know the end of the bytecode. Store chipData.
        self.chipData = data[:bcOffset]

    def _bytecodePin(self, data, offset):
        # Parse a pin from the bytecode.
        # Returns an offset, pin tuple.

        pin = self.addrSpace.lookupPin(data, offset)
        return offset + 1, pin

    def _bytecodeAddrList(self, data, offset):
        # Parse a list of pin addresses, terminated by an 0xFF byte.
        # Returns an offset, pinList tuple.

        pinList = []
        while True:
            if len(data) <= offset:
                raise FormatError("Premature end of chip data")

            if data[offset] == '\xFF':
                return offset + 1, pinList

            addr = struct.unpack('>H', data[offset:offset+2])[0]
            pinList.append(self.addrSpace.lookupPin(data, addr))
            offset += 2

    def _bytecode_01(self, data, offset):
        # AND gate
        offset, in0 = self._bytecodePin(data, offset)
        offset, in1 = self._bytecodePin(data, offset)
        offset, outs = self._bytecodeAddrList(data, offset)
        self.parts.append(AND(in0, in1, outs))
        return offset

    def _bytecode_02(self, data, offset):
        # OR gate
        offset, in0 = self._bytecodePin(data, offset)
        offset, in1 = self._bytecodePin(data, offset)
        offset, outs = self._bytecodeAddrList(data, offset)
        self.parts.append(OR(in0, in1, outs))
        return offset

    def _bytecode_03(self, data, offset):
        # XOR gate
        offset, in0 = self._bytecodePin(data, offset)
        offset, in1 = self._bytecodePin(data, offset)
        offset, outs = self._bytecodeAddrList(data, offset)
        self.parts.append(XOR(in0, in1, outs))
        return offset

    def _bytecode_04(self, data, offset):
        # NOT gate
        offset, input = self._bytecodePin(data, offset)
        offset, outs = self._bytecodeAddrList(data, offset)
        self.parts.append(NOT(input, outs))
        return offset

    def _bytecode_05(self, data, offset):
        # Flip flop
        offset, in0 = self._bytecodePin(data, offset)
        offset, in1 = self._bytecodePin(data, offset)
        offset, state0 = self._bytecodePin(data, offset)
        offset, state1 = self._bytecodePin(data, offset)
        offset, out0 = self._bytecodeAddrList(data, offset)
        offset, out1 = self._bytecodeAddrList(data, offset)
        self.parts.append(FF(in0, in1, state0, state1, out0, out1))
        return offset

    def _bytecode_06(self, data, offset):
        # Nested chip.
        chip = Chip(data[offset:], format='CHP', addrSpace=self.addrSpace.offset(offset))
        self.parts.append(chip)
        return offset + len(chip.chipData)

    def _bytecode_07(self, data, offset):
        # Propagate inputs, end chip.
        #
        # The arguments are a list of lists. Each inner list is a
        # value to propagate. The first address is the source pin, the
        # rest are destination pins.

        while True:
            offset, addrs = self._bytecodeAddrList(data, offset)
            if not addrs:
                break
            self.parts.append(Node(addrs[0], addrs[1:]))
        return offset

    def __repr__(self):
        lines = []
        indent = '    '

        lines.append('%s {' % (self.name or 'CHIP'))

        if self.helpText:
            for line in self.helpText:
                lines.append(indent + _formatLine('HELP', [line]))
            lines.append('')

        for i, pin in enumerate(self.pins):
            tokens = [pin]
            if self.pinDirections:
                dir = self.pinDirections[i]
                if not dir:
                    continue
                tokens.append(dir)
            lines.append(indent + _formatLine('PIN', tokens))
        lines.append('')

        for part in self.parts:
            for line in repr(part).split('\n'):
                lines.append(indent + line)

        lines.append('}')
        return '\n'.join(lines)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.stderr.write("usage: %s <CHP or CSV file>\n" % sys.argv[0])
    else:
        print Chip(open(sys.argv[1], 'rb').read())
