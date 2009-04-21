#
# Experimental DOS 8086 -> C static binary translator.
#
# This isn't a general-purpose static binary translator. That wouldn't
# be possible, since self-modifying code and dynamic branches make it
# impossible to create a static translation of any arbitrary program.
# Instead, this is a toolkit which does *most* of the work in creating
# a static translation of one particular program. You'll still need to
# write some program-specific code in order to fix up any untranslateable
# parts in the binary.
#
# These fixups come in the form of a separate per-program Python file
# where you will create a DOSBinary() class to represent your input
# file, apply patches to that in-memory representation, then generate
# code. The patches can either replace original assembly instructions
# (a 'patch') or add C code which executes just prior to an original
# instruction (a 'hook').
#
# Patches affect the analysis phase, where we discover subroutines and
# figure out which parts of the binary are executable code. So, if you
# stub out a function by patching its first instruction with a 'ret',
# or if you patch out any call sites, no code will be generated for
# that function. If you want to replace an original subroutine with a
# C-language routine, for example, you can patch in a 'ret'
# instruction, then hook the 'ret'.
#
# Requires Python 2.5 and the ndisasm disassembler.
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

import sys
import struct
import subprocess
import re
import tempfile


class Signature:
    """A binary signature, which can be used to identify a code fragment
       at an unknown address. The signature consists of a list of
       bytes before and after the matching address, as well as an
       optional list of wildcard bytes to exclude from the search.
       The signature is formatted as a hexadecimal string. A ':'
       represents the search address. Characters prior to the colon
       occur before the search address, and characters afterward occur
       at/after the address. Underscores represent 'blanks', that can
       contain any value.  Underscores must by byte
       aligned. Whitespace is optional. The '#' character marks the
       beginning of a comment, all text until the next newline is
       ignored.
       """
    def __init__(self, text):
        self.longText = text

        # Remove comments
        t = re.sub(r"#.*", "", text)

        # Remove whitespace
        t = t.replace('\n','').replace('\t','').replace(' ','')

        self.shortText = t

        m = re.search(r"[^0-9a-fA-F_:]", t)
        if m:
            raise SignatureFormatError("Invalid character %r in pattern %r" %
                                       (m.group(0), self.shortText))

        parts = t.split(':')
        if len(parts) != 2:
            raise SignatureFormatError("Pattern must have exactly one ':' to"
                                       " mark the search address.")
        pre, post = parts
        self.preLength = len(pre) / 2
        t = pre + post

        if len(t) % 2:
            raise SignatureFormatError("Pattern must have an even "
                                       "number of digits: %r" % self.shortText)

        regex = ""
        for byte in xrange(len(t) / 2):
            hexByte = t[byte * 2: (byte + 1) * 2]
            if '_' in hexByte:
                if hexByte == '__':
                    regex += '.'
                else:
                    SignatureFormatError("Pattern has blanks which are "
                                         "not byte aligned: %r" % self.shortText)
            else:
                char = chr(int(hexByte, 16))
                if char in ".^$*+?\\{}[]()|":
                    # Special character. Escape it.
                    regex += '\\' + char
                else:
                    regex += char

        self.re = re.compile(regex)

    def find(self, str):
        """Locate the pattern in a string, with zero or more occurrances.
           Returns a list of offsets, in bytes from the beginning of the string.
           """
        return [m.start(0) + self.preLength for m in
                self.re.finditer(str)]


class SignatureFormatError(Exception):
    pass

class SignatureMatchError(Exception):
    pass


class Addr16:
    """A 16-bit Real Mode address.
       Requires 2 out of 3 parameters, or 'str' to parse an
       address in hexadecimal string form.
       """
    width = 4

    def __init__(self, segment=None, offset=None, linear=None, str=None):
        if str is not None:
            if ':' in str:
                segment, offset = str.split(':')
                segment = int(segment, 16)
                offset = int(offset, 16)
            else:
                segment = 0
                offset = int(str, 16)

        if linear is None:
            linear = (segment << 4) + offset
        elif segment is None:
            s = linear - offset
            assert (s & 0xF) == 0
            segment = s >> 4
        elif offset is None:
            offset = linear - (segment << 4)

        segment += (offset >> 16) << 12
        offset &= 0xFFFF

        self.segment = segment
        self.offset = offset
        self.linear = linear

    def __cmp__(self, other):
        return cmp(self.linear, other.linear)

    def add(self, x):
        if isinstance(x, Addr16):
            return Addr16(self.segment + x.segment, self.offset + x.offset)
        else:
            return Addr16(self.segment, self.offset + x)

    def __repr__(self):
        return str(self)

    def __str__(self):
        return "%04X:%04X" % (self.segment, self.offset)

    def label(self):
        return "loc_%X" % self.linear


class Literal(int):
    def __new__(self, x):
        # We never really want a signed literal.
        if x < 0:
            x += 0x10000

        obj = int.__new__(self, x)
        obj.width = 2
        return obj

    def __repr__(self):
        return self.codegen()

    def codegen(self):
        if self < 16:
            return str(self)
        elif self < 0x100:
            return "0x%02x" % self
        else:
            return "0x%04x" % self


class Register:
    def __init__(self, name):
        name = name.lower()
        self.name = name

        if name[-1] in 'lh':
            self.width = 1
        else:
            self.width = 2

    def __repr__(self):
        return "Reg(%s)" % self.name

    def codegen(self):
        return "reg.%s" % self.name


class Indirect:
    def __init__(self, segment, offsets, width=None):
        self.segment = segment
        self.offsets = offsets
        self.width = width

    def __repr__(self):
        return "[%s:%s w%s]" % (
            self.segment, '+'.join(map(repr, self.offsets)), self.width)

    def genAddr(self):
        """Generate a (segment, offset) tuple, where each item is
           generated C code which calculates the semgent/offset of this
           Indirect.
           """
        return (self.segment.codegen(),
                " + ".join([o.codegen() for o in self.offsets]))

    def codegen(self):
        if self.width == 1:
            return "MEM8(%s,%s)" % self.genAddr()
        elif self.width == 2:
            return "MEM16(%s,%s)" % self.genAddr()
        else:
            raise InternalError("Indirect %r has unknown width" % self)


def signed(operand):
    """Generate signed code for an operand."""
    if operand.width == 2:
        return "((int16_t)%s)" % operand.codegen()
    elif operand.width == 1:
        return "((int8_t)%s)" % operand.codegen()
    else:
        raise InternalError("Unsupported width for signed operand (%s)" % operand.width)

def u32(operand):
    """Generate 32-bit unsigned code for an operand."""
    return "((uint32_t)%s)" % operand.codegen()


class InternalError(Exception):
    pass


class Trace:
    _args = "uint16_t segment, uint16_t offset, uint16_t cs, uint16_t ip, int width"

    def __init__(self, name, mode, probe, fire):
        self.name = name
        self.mode = mode
        self.probe = probe
        self.fire = fire

    def codegen(self):
        return ("static inline int %s_probe(%s) {\n%s\n}\n"
                "static void %s_fire(%s) {\n%s\n}\n") % (
            self.name, self._args, self.probe,
            self.name, self._args, self.fire)


def _genCycleTable():
    """Generate a table of 8086/8086 instruction timings.  This isn't
       quite accurate, as it doesn't bother calculating the number of
       cycles necessary to look up the effective address based on the
       type of Indirect() argument we have. It also doesn't account
       for instructions that take a variable number of clocks
       depending on their arguments, like MUL. In practice I doubt
       this matters, at least for the things we're using our virtual
       clock for.

       This is based on the information from:
          http://umcs.maine.edu/~cmeadow/courses/cos335/
          80x86-Integer-Instruction-Set-Clocks.pdf
       """

    table = {
        ('mov', Register, Register): 2,
        ('mov', Indirect, Register): 13,
        ('mov', Register, Indirect): 12,
        ('mov', Indirect, Literal): 14,
        ('mov', Register, Literal): 4,

        ('cmp', Register, Register): 3,
        ('cmp', Indirect, Register): 13,
        ('cmp', Register, Indirect): 12,
        ('cmp', Indirect, Literal): 14,
        ('cmp', Register, Literal): 4,

        ('test', Register, Register): 3,
        ('test', Indirect, Register): 13,
        ('test', Register, Indirect): 13,
        ('test', Indirect, Literal): 11,
        ('test', Register, Literal): 5,

        ('xchg', Register, Register): 4,
        ('xchg', Indirect, Register): 25,
        ('xchg', Register, Indirect): 25,

        ('imul', Register): 89,  # Average (8-bit)
        ('imul', Indirect): 95,

        ('mul', Register): 73,  # Average (8-bit)
        ('mul', Indirect): 79,

        ('div', Register): 85,  # Average (8-bit)
        ('div', Indirect): 91,

        ('not', Register): 3,
        ('not', Indirect): 24,

        ('neg', Register): 3,
        ('neg', Indirect): 24,

        ('inc', Register): 3,
        ('inc', Indirect): 23,

        ('dec', Register): 3,
        ('dec', Indirect): 23,

        ('les', Register, Indirect): 24,

        ('jmp', Literal): 15,
        ('loop', Literal): 17,
        ('call', Literal): 23,
        ('ret',): 20,

        ('out', Literal, Register): 14,
        ('out', Register, Register): 12,

        ('in', Register, Literal): 14,
        ('in', Register, Register): 12,

        ('push', Register): 15,
        ('push', Indirect): 24,

        ('pop', Register): 12,
        ('pop', Indirect): 25,

        ('cmc',): 2,
        ('clc',): 2,
        ('stc',): 2,
        ('cbw',): 2,

        # Stubs for instructions that take a long and variable
        # amount of time to execute. No sane programmer would
        # use these in a timing-critical loop.. (fingers crossed)

        ('int', Literal): 0,
        ('rep_stosb',): 0,
        ('rep_stosw',): 0,
        ('rep_movsb',): 0,
        ('rep_movsw',): 0,
        }

    # Conditional jumps (assume jump taken)
    for op in 'jz jnz jc jnc js jns ja jnl jl jng jna jcxz'.split():
        table[(op, Literal)] = 16

    # All shifts and rotates are the same
    for op in ('shl', 'shr', 'rcl', 'rcr', 'sar', 'ror'):
        table.update({
                (op, Register, Register): 12,
                (op, Indirect, Register): 32,   # This is why you see so many
                (op, Indirect, Literal): 23,    #    repeated shifts by 1...
                (op, Register, Literal): 2,     # <-- Much cheaper.
                })

    # 2-operand ALU operations are mostly the same.
    for op in ('xor', 'and', 'or', 'add', 'sub', 'adc', 'sbb'):
        table.update({
                (op, Register, Register): 3,
                (op, Indirect, Register): 24,
                (op, Register, Indirect): 13,
                (op, Register, Literal): 4,
                (op, Indirect, Literal): 23,
                })

    return table


class Instruction:
    """A disassembled instruction, in NASM format."""

    _widthNames = {
        "byte": 1,
        "short": 2,
        "word": 2,
        }

    _cycleTable = _genCycleTable()

    def __init__(self, line, offset=None, prefix=None):
        self.raw = line
        self._offset = offset

        # Remove jump distances and superfluous segment prefixes
        line = line.replace("far ", "")
        line = line.replace("near ", "")

        # ndisasm doesn't seem to decode prefixes...
        line = line.replace("db 0xF3", "rep")
        line = line.replace("db 0xF2", "repne")

        # Split up the ndisasm line into useful pieces.

        tokens = line.strip().split(None, 3)
        try:
            addr, encoding, op, args = tokens
            args = args.split(',')
        except ValueError:
            addr, encoding, op = tokens
            args = ()

        # Save most of the attributes we care about, then parse
        # operands. Some operands may have a dependency on the other
        # values.

        self.op = op
        self.addr = Addr16(str=addr).add(offset or 0)
        self.encoding = encoding

        # Normally we get an offset from the disassembler,
        # but if this is a manually entered (patch) instruction,
        # assume jumps are relative to the segment base.

        if offset is None:
            offset = Addr16(self.addr.segment, 0)

        self.args = map(self._decodeOperand, args)

        # Propagate known widths to operands with unknown width
        for a in self.args:
            if a.width is None:
                a.width = self._findUnknownWidth()

        # If this instruction has been attached to the prefix,
        # represent that in a prefix_opcode notation, and merge this
        # instruction's address and encoding info with the prefix.  If
        # this instruction IS a prefix, let the disassembler know.

        self.isPrefix = op in ('rep', 'repe', 'repne')
        if prefix:
            assert not prefix.args
            self.op = "%s_%s" % (prefix.op, self.op)
            self.addr = prefix.addr
            self.encoding = prefix.encoding + self.encoding
            self.raw = prefix.raw + self.raw

        # Calculate instruction length *after* merging the prefix.
        # Otherwise, we'll end up decoding the instruction after
        # the prefix twice.
        self.length = len(self.encoding) / 2

        # Calculate the next address(es) for this instruction.
        # For most instructions, this is the next instruction.
        # For unconditional jumps, it's the jump target. For
        # conditional jumps, there are two addresses.
        #
        # For subroutine calls, we treat both the return address and
        # the subroutine as 'next' addresses. Return instructions
        # have no next address.
        #
        # If one of the 'next' addresses can't be known statically
        # (it's an Indirect) we ignore it. Function pointers and
        # computed jumps can't be translated automatically.

        if op in ('jmp', 'ret', 'iret', 'retf'):
            self.nextAddrs = ()
        else:
            self.nextAddrs = (self.addr.add(self.length),)

        self.labels = ()
        if self.args and (op[0] == 'j' or op in ('call', 'loop')):
            arg = self.args[0]
            if isinstance(arg, Literal):
                # Near branch
                self.labels = (offset.add(self.args[0]),)
            elif isinstance(arg, Addr16):
                # Far branch
                self.labels = (arg,)

        self.nextAddrs += self.labels

    def _findUnknownWidth(self):
        """Figure out an operand width that wasn't explicitly specified.
        May return None if the width can't be determined.
           """
        w = None
        for a in self.args:
            if a.width is not None:
                if w is None:
                    w = a.width
                elif w != a.width:
                    raise InternalError("Contradictory widths %s and %s in %s" % (
                            w, a.width, self))
        return w

    def _decodeOperand(self, text):
        """Decode an instruction operand. If this operand includes a clue
        about the instruction width, sets self.width as a side-effect.
           """
        # Does the operand begin with a width keyword?
        firstToken = text.split(None, 2)[0]
        if firstToken in self._widthNames:
            width = self._widthNames[firstToken]
            text = text[len(firstToken):].strip()
        else:
            width = None

        # Is it an immediate value?
        try:
            return Literal(int(text, 0))
        except ValueError:
            pass

        # Does it look like a register?
        if len(text) == 2:
            return Register(text)

        # Is this in indirect?
        if text[0] == '[' and text[-1] == ']':
            return self._decodeIndirect(text[1:-1], width)

        # Is it an address?
        if ':' in text:
            return Addr16(str=text)

        raise InternalError("Unsupported operand %r in %r at %r" %
                            (text, self.raw, self.addr))

    def _decodeIndirect(self, text, width=None):
        """Decode an operand that uses indirect addressing."""
        if ':' in text:
            segment, text = text.split(':', 1)
            segment = self._decodeOperand(segment)
        else:
            segment = Register('ds')

        # Convert signed offsets to unsigned.
        if '-' in text:
            text = text.replace('-', '+-')
            if text[0] == '+':
                text = text[1:]

        if '+' in text:
            offsets = map(self._decodeOperand, text.split('+'))
        else:
            offsets = [self._decodeOperand(text)]

        return Indirect(segment, offsets, width)

    def __repr__(self):
        return "%s %s %r" % (self.addr, self.op, self.args)

    def _repeat(self, cnt, contents):
        """Repeat 'contents', 'cnt' times."""
        if cnt == 1:
            return "{ %s }" % contents;
        else:
            return "{ int c = %s; while (c--) { %s } }" % (cnt.codegen(), contents)

    def _resultShift(self, dest):
        if dest.width == 1:
            return "reg.uresult <<= 8; reg.sresult <<= 8;"
        else:
            return ""

    def _call(self, name):
        return "reg = %s(reg);" % name

    def codegen(self, traces=None, clockEnable=False):
        f = getattr(self, "codegen_%s" % self.op, None)
        if not f:
            raise NotImplementedError("Unsupported opcode in %s" % self)
        self._traces = traces

        code = f(*self.args)

        if clockEnable:
            # Come up with a signature for this instruction, which
            # consists of its opcode name and operand types. This
            # will be used to index into a table of instruction timings.

            sig = (self.op,) + tuple([arg.__class__ for arg in self.args])
            code += 'clock+=%d;' % self._cycleTable[sig]

        return code

    def _genTraces(self, *ops):
        """Generate code to fire any runtime traces. If there are no runtime
           traces, this generates no code (and there is no overhead).

           'ops' is a list of (operand, mode) tuples, which describe
           accesses to operands. Currently we only care about tracing memory,
           so we ignore operands that aren't Indirects. If any traces are in
           place, we call an inline 'probe' function to test whether we should
           call its 'fire' function.
           """
        if not self._traces:
            return ''

        code = []
        for operand, mode in ops:
            if not isinstance(operand, Indirect):
                continue

            for trace in self._traces:
                if mode not in trace.mode:
                    continue

                seg, off = operand.genAddr()
                args = "%s,%s,0x%x,0x%x,%d" % (seg, off, self.addr.segment,
                                               self.addr.offset, operand.width)

                code.append("if (%s_probe(%s)) %s_fire(%s);" % (
                        trace.name, args, trace.name, args))

        return ''.join(code)

    def codegen_mov(self, dest, src):
        return "%s = %s;%s" % (
            dest.codegen(), src.codegen(),
            self._genTraces((src, 'r'),
                            (dest, 'w')),
            )

    def codegen_xor(self, dest, src):
        return "reg.uresult = %s ^= %s; reg.sresult=0; %s%s" % (
            dest.codegen(), src.codegen(),
            self._resultShift(dest),
            self._genTraces((src, 'r'),
                            (dest, 'r'),
                            (dest, 'w')),
            )

    def codegen_or(self, dest, src):
        return "reg.uresult = %s |= %s; reg.sresult=0; %s%s" % (
            dest.codegen(), src.codegen(),
            self._resultShift(dest),
            self._genTraces((src, 'r'),
                            (dest, 'r'),
                            (dest, 'w')),
            )

    def codegen_and(self, dest, src):
        return "reg.uresult = %s &= %s; reg.sresult=0; %s%s" % (
            dest.codegen(), src.codegen(),
            self._resultShift(dest),
            self._genTraces((src, 'r'),
                            (dest, 'r'),
                            (dest, 'w')),
            )

    def codegen_add(self, dest, src):
        return "reg.sresult = %s + %s; %s = reg.uresult = %s + %s; %s%s" % (
            signed(dest), signed(src),
            dest.codegen(), u32(dest), u32(src),
            self._resultShift(dest),
            self._genTraces((src, 'r'),
                            (dest, 'r'),
                            (dest, 'w')),
            )

    def codegen_adc(self, dest, src):
        return "reg.sresult = %s + %s + CF; %s = reg.uresult = %s + %s + CF; %s%s" % (
            signed(dest), signed(src),
            dest.codegen(), u32(dest), u32(src),
            self._resultShift(dest),
            self._genTraces((src, 'r'),
                            (dest, 'r'),
                            (dest, 'w')),
            )

    def codegen_sub(self, dest, src):
        return "reg.sresult = %s - %s; %s = reg.uresult = %s - %s; %s%s" % (
            signed(dest), signed(src),
            dest.codegen(), u32(dest), u32(src),
            self._resultShift(dest),
            self._genTraces((src, 'r'),
                            (dest, 'r'),
                            (dest, 'w')),
            )

    def codegen_sbb(self, dest, src):
        return "reg.sresult=%s - (%s + CF); %s=reg.uresult=%s - (%s + CF); %s%s" % (
            signed(dest), signed(src),
            dest.codegen(), u32(dest), u32(src),
            self._resultShift(dest),
            self._genTraces((src, 'r'),
                            (dest, 'r'),
                            (dest, 'w')),
            )

    def codegen_shl(self, r, cnt):
        return self._repeat(cnt,
                            "reg.sresult=0; %s=reg.uresult = ((uint32_t)%s) << 1; %s" %
                            (r.codegen(), r.codegen(),
                             self._genTraces((cnt, 'r'),
                                             (r, 'r'),
                                             (r, 'w')),
                             )) + self._resultShift(r)

    def codegen_shr(self, r, cnt):
        return self._repeat(cnt,
                            ("reg.sresult=0; "
                             "reg.uresult = (%s & 1) << 16; "
                             "(%s) >>= 1; %s") %
                            (u32(r), r.codegen(),
                             self._genTraces((cnt, 'r'),
                                             (r, 'r'),
                                             (r, 'w'))))

    def codegen_sar(self, r, cnt):
        return self._repeat(cnt,
                            ("reg.sresult=0; "
                             "reg.uresult = (%s & 1) << 16; "
                             "(%s) = ((int16_t)%s) >> 1; %s") %
                            (u32(r), r.codegen(), r.codegen(),
                             self._genTraces((cnt, 'r'),
                                             (r, 'r'),
                                             (r, 'w'))))

    def codegen_ror(self, r, cnt):
        msbShift = r.width * 8 - 1
        return self._repeat(cnt,
                            ("reg.sresult=0; "
                             "reg.uresult = ((uint32_t)(%s) & 1) << 16;"
                             "(%s) = ((%s) >> 1) + ((%s) << %s); %s") %
                            (r.codegen(), r.codegen(),
                             r.codegen(), r.codegen(), msbShift,
                             self._genTraces((cnt, 'r'),
                                             (r, 'r'),
                                             (r, 'w'))))

    def codegen_rcr(self, r, cnt):
        msbShift = r.width * 8 - 1
        return self._repeat(cnt,
                            ("int cf = CF;"
                             "reg.sresult=0; "
                             "reg.uresult = %s << 16;"
                             "(%s) = ((%s) >> 1) + (cf << %s); %s") %
                            (r.codegen(), r.codegen(), r.codegen(), msbShift,
                             self._genTraces((cnt, 'r'),
                                             (r, 'r'),
                                             (r, 'w'))))

    def codegen_rcl(self, r, cnt):
        return self._repeat(cnt,
                            ("reg.sresult=0; "
                             "%s = reg.uresult = (%s << 1) + CF; %s%s") %
                            (r.codegen(), u32(r), self._resultShift(r),
                             self._genTraces((cnt, 'r'),
                                             (r, 'r'),
                                             (r, 'w'))))

    def codegen_xchg(self, a, b):
        return "{ uint16_t t = %s; %s = %s; %s = t; %s}" % (
            a.codegen(), a.codegen(), b.codegen(), b.codegen(),
            self._genTraces((a, 'r'),
                            (b, 'r'),
                            (a, 'w'),
                            (b, 'w')))

    def codegen_imul(self, arg):
        return "reg.ax = (int8_t)reg.al * %s;" % arg.codegen()

    def codegen_mul(self, arg):
        return "reg.ax = reg.al * %s;" % arg.codegen()

    def codegen_div(self, arg):
        return "reg.al = reg.ax / %s; reg.ah = reg.ax %% %s;" % (
            arg.codegen(), arg.codegen())

    def codegen_cmc(self):
        return "if (CF) CLR_CF; else SET_CF;"

    def codegen_clc(self):
        return "CLR_CF;"

    def codegen_stc(self):
        return "SET_CF;"

    def codegen_cbw(self):
        return "reg.ax = (int16_t)(int8_t)reg.al;"

    def codegen_cmp(self, a, b):
        return "reg.sresult = %s - %s; reg.uresult = %s - %s; %s" % (
            signed(a), signed(b),
            u32(a), u32(b),
            self._resultShift(a))

    def codegen_test(self, a, b):
        return "reg.uresult = %s & %s; reg.sresult=0; %s" % (
            a.codegen(), b.codegen(), self._resultShift(a))

    def codegen_inc(self, arg):
        return self.codegen_add(arg, Literal(1))

    def codegen_dec(self, arg):
        return self.codegen_sub(arg, Literal(1))

    def codegen_not(self, arg):
        return "%s = ~%s;%s" % (
            arg.codegen(), arg.codegen(),
            self._genTraces((arg, 'r'),
                            (arg, 'w')))

    def codegen_neg(self, arg):
        return "%s = -%s;%s" % (
            arg.codegen(), arg.codegen(),
            self._genTraces((arg, 'r'),
                            (arg, 'w')))

    def codegen_les(self, dest, src):
        assert isinstance(src, Indirect)
        assert src.width == 2
        segPtr = Indirect(src.segment, src.offsets + [Literal(2)], 2)

        return "%s = %s; %s = %s;%s" % (
            dest.codegen(), src.codegen(),
            Register('es').codegen(), segPtr.codegen(),
            self._genTraces((src, 'r'),
                            (segPtr, 'r'),
                            (dest, 'w')))

    def codegen_rep_stosw(self):
        cx = Register('cx').codegen()
        return "while (%s) { %s %s--;}" % (cx, self.codegen_stosw(), cx)

    def codegen_rep_movsw(self):
        cx = Register('cx').codegen()
        return "while (%s) { %s %s--;}" % (cx, self.codegen_movsw(), cx)

    def codegen_rep_stosb(self):
        cx = Register('cx').codegen()
        return "while (%s) { %s %s--;}" % (cx, self.codegen_stosb(), cx)

    def codegen_rep_scasb(self):
        # (Actually the 'repe' prefix, repeat while equal.)
        cx = Register('cx').codegen()
        return "while (%s) { %s %s--; if (!ZF) break; }" % (
            cx, self.codegen_scasb(), cx)

    def codegen_movsw(self):
        dest = Indirect(Register('es'), (Register('di'),), 2)
        src = Indirect(Register('ds'), (Register('si'),), 2)
        return "%s = %s; %s += 2; %s += 2;%s" % (
            dest.codegen(), src.codegen(),
            Register('si').codegen(), Register('di').codegen(),
            self._genTraces((src, 'r'),
                            (dest, 'w')))

    def codegen_stosb(self):
        dest = Indirect(Register('es'), (Register('di'),), 1)
        return "%s = %s; %s++;%s" % (
            dest.codegen(),
            Register('al').codegen(), Register('di').codegen(),
            self._genTraces((dest, 'w')))

    def codegen_stosw(self):
        dest = Indirect(Register('es'), (Register('di'),), 2)
        return "%s = %s; %s += 2;%s" % (
            dest.codegen(),
            Register('ax').codegen(), Register('di').codegen(),
            self._genTraces((dest, 'w')))

    def codegen_scasb(self):
        return "%s; %s++;" % (
            self.codegen_cmp(Register('al'),
                             Indirect(Register('es'), (Register('di'),), 1)),
            Register('di').codegen())

    def codegen_lodsb(self):
        src = Indirect(Register('ds'), (Register('si'),), 1)
        return "%s = %s; %s++;%s" % (
            Register('al').codegen(), src.codegen(),
            Register('si').codegen(),
            self._genTraces((src, 'r')))

    def codegen_push(self, arg):
        # We currently implement the stack as a totally separate memory
        # area which can hold 16-bit words or native code return addresses.
        return "stack[stackPtr++].word = %s;" % arg.codegen()

    def codegen_pop(self, arg):
        return "%s = stack[--stackPtr].word;" % arg.codegen()

    def codegen_pushaw(self):
        return ' '.join((
                self.codegen_push(Register('ax')),
                self.codegen_push(Register('cx')),
                self.codegen_push(Register('dx')),
                self.codegen_push(Register('bx')),
                self.codegen_push(Register('sp')),
                self.codegen_push(Register('bp')),
                self.codegen_push(Register('si')),
                self.codegen_push(Register('di')),
                ))

    def codegen_popaw(self):
        return ' '.join((
                self.codegen_push(Register('di')),
                self.codegen_pop(Register('si')),
                self.codegen_pop(Register('bp')),
                self.codegen_pop(Register('sp')),
                self.codegen_pop(Register('bx')),
                self.codegen_pop(Register('dx')),
                self.codegen_pop(Register('cx')),
                self.codegen_pop(Register('ax')),
                ))

    def codegen_pushfw(self):
        return ("stack[stackPtr].uresult = reg.uresult;"
                "stack[stackPtr++].sresult = reg.sresult;")

    def codegen_popfw(self):
        return ("reg.uresult = stack[--stackPtr].uresult;"
                "reg.sresult = stack[stackPtr].sresult;")

    def codegen_nop(self):
        return "/* nop */;"

    def codegen_cli(self):
        return "/* cli */;"

    def codegen_sti(self):
        return "/* sti */;"

    def codegen_jz(self, arg):
        return "if (ZF) goto %s;" % self.nextAddrs[-1].label()

    def codegen_jnz(self, arg):
        return "if (!ZF) goto %s;" % self.nextAddrs[-1].label()

    def codegen_jc(self, arg):
        return "if (CF) goto %s;" % self.nextAddrs[-1].label()

    def codegen_jnc(self, arg):
        return "if (!CF) goto %s;" % self.nextAddrs[-1].label()

    def codegen_js(self, arg):
        return "if (SF) goto %s;" % self.nextAddrs[-1].label()

    def codegen_jns(self, arg):
        return "if (!SF) goto %s;" % self.nextAddrs[-1].label()

    def codegen_ja(self, arg):
        return "if (!CF && !ZF) goto %s;" % self.nextAddrs[-1].label()

    def codegen_jnl(self, arg):
        return "if (SF == OF) goto %s;" % self.nextAddrs[-1].label()

    def codegen_jl(self, arg):
        return "if (SF != OF) goto %s;" % self.nextAddrs[-1].label()

    def codegen_jng(self, arg):
        return "if (ZF || (SF != OF)) goto %s;" % self.nextAddrs[-1].label()

    def codegen_jna(self, arg):
        return "if (CF || ZF) goto %s;" % self.nextAddrs[-1].label()

    def codegen_jcxz(self, arg):
        return "if (!reg.cx) goto %s;" % self.nextAddrs[-1].label()

    def codegen_loop(self, arg):
        return "if (--reg.cx) goto %s;" % self.nextAddrs[-1].label()

    def codegen_jmp(self, arg):
        if isinstance(arg, Literal) or isinstance(arg, Addr16):
            return "goto %s;" % self.nextAddrs[-1].label()
        else:
            raise NotImplementedError("Dynamic jmp not supported at %s" % self)

    def codegen_call(self, arg):
        if isinstance(arg, Literal) or isinstance(arg, Addr16):
            return self._call("sub_%X" % self.nextAddrs[-1].linear)
        else:
            raise NotImplementedError("Dynamic call not supported at %s" % self)

    def codegen_ret(self):
        return "return reg;"

    def codegen_retf(self):
        return self.codegen_ret()

    def codegen_int(self, arg):
        return self._call("int%x" % arg);

    def codegen_out(self, port, value):
        return "out(%s,%s,clock);" % (port.codegen(), value.codegen())

    def codegen_in(self, value, port):
        return "%s = in(%s,clock);" % (value.codegen(), port.codegen())


class BinaryImage:
    def __init__(self, filename=None, file=None, offset=0, data=None):
        if file is None:
            file = open(filename, "rb")

        if data is None:
            file.seek(offset)
            data = file.read()
            offset = 0

        if offset:
            data = data[offset:]

        self.filename = filename
        self._data = data
        self.loadSegment = 0
        self.parseHeader()
        self._iCache = {}
        self._tempFile = None

    def relocate(self, segment, addrs):
        """Apply a list of relocations to this image. 'addrs' is a list of
           Addr16s which describe 16-bit relocations, 'segment' is the
           amount we'll add to each relocation. This function modifies
           the BinaryImage's data buffer.

           This implementation is horribly inefficient, but given how
           few relocations we have and how slow gcc is at compiling
           our output anyway, I don't care.
           """

        print "Applying relocations at %r" % addrs
        self.loadSegment = segment

        if self._tempFile:
            raise Exception("Must relocate before starting disassembly!")

        for addr in addrs:
            offset = addr.linear
            pre = self._data[:offset]
            reloc = self._data[offset:offset+2]
            post = self._data[offset+2:]

            reloc = struct.unpack("<H", reloc)[0]
            reloc = (reloc + segment) & 0xFFFF
            reloc = struct.pack("<H", reloc)

            self._data = ''.join((pre, reloc, post))

    def offset(self, offset):
        return BinaryImage(self.filename, offset=offset, data=self._data)

    def parseHeader(self):
        pass

    def read(self, offset, length):
        return self._data[offset:offset + length]

    def unpack(self, offset, fmt):
        return struct.unpack(fmt, self.read(offset, struct.calcsize(fmt)))

    def le8(self, offset):
        return self.unpack(offset, "<B")[0]

    def le16(self, offset):
        return self.unpack(offset, "<H")[0]

    def le32(self, offset):
        return self.unpack(offset, "<I")[0]

    def disasm(self, addr, bits=16):
        """This is an iterator which disassembles starting at the specified
           memory address. Uses a temporary file to pass the relocated
           binary image to ndisasm.

           If the memory address is different from the address on disk,
           self.loadSegment must be set to the segment where this image
           is loaded.
           """

        if not self._tempFile:
            self._tempFile = tempfile.NamedTemporaryFile()
            self._tempFile.write(self._data)
            self._tempFile.flush()

        args = ["ndisasm",
                "-o", str(addr.offset),
                "-b", str(bits),
                "-e", str(addr.linear - (self.loadSegment << 4)),
                self._tempFile.name]
        proc = subprocess.Popen(args, stdout=subprocess.PIPE)

        prefix = None
        base = Addr16(addr.segment, 0)

        for line in proc.stdout:
            i = Instruction(line, base, prefix)
            prefix = None
            if i.isPrefix:
                prefix = i
            else:
                yield i

    def iFetch(self, addr):
        """Fetch the next instruction, via the instruction cache.  If there's
           a cache miss, disassemble a block of code starting at this
           address.
           """
        if addr.linear not in self._iCache:
            self._disasmToCache(addr)
        return self._iCache[addr.linear]

    def _disasmToCache(self, addr, instructions=100):
        for i in self.disasm(addr):
            if i.addr.linear not in self._iCache:
                self._iCache[i.addr.linear] = i
            instructions -= 1
            if instructions <= 0:
                break


class Subroutine:
    """Information about a single subroutine. We define a subroutine as
       the section of a control flow graph which begins just after a
       'call' and ends at all 'ret's. One code fragment may be used
       by multiple subroutines.
       """

    # Should this subroutine generate clock cycle counting code?
    clockEnable = False

    def __init__(self, image, entryPoint, hooks=None):
        self.image = image
        self.entryPoint = entryPoint
        self.hooks = hooks or {}
        self.name = "sub_%X" % self.entryPoint.linear

    def analyze(self, verbose=False):
        """Analyze the code in this subroutine.
           Stores analysis results in member attributes.
           """

        # Address memo: linear -> Addr16
        memo = {}

        # Label flags: linear -> True
        self.labels = { self.entryPoint.linear: True }

        # Routines that are called by this one
        self.callsTo = {}

        # Local jump stack: (referent, level, ptr)
        stack = [(None, self.entryPoint)]

        while stack:
            referent, ptr = stack.pop()
            if ptr.linear in memo:
                if verbose:
                    sys.stderr.write("Address %s already visited\n" % ptr)
                continue

            memo[ptr.linear] = ptr
            i = self.image.iFetch(ptr)
            i.referent = referent

            if verbose:
                sys.stderr.write("%s R[%-9s] D%-3d %-50s -> %-22s || %s\n" % (
                        self.name, referent, len(stack), i, i.nextAddrs, i.raw.strip()))

            # I/O instructions enable cycle counting, so that we can
            # give in() and out() accurate timestamps.

            if i.op in ('in', 'out'):
                self.clockEnable = True

            # Remember all label targets, and remember future
            # addresses to analyze. Treat subroutine calls specially.

            for next in i.labels:
                self.labels[next.linear] = True

            if i.op == 'call' and len(i.nextAddrs) == 2:
                stack.append((ptr, i.nextAddrs[0]))
                self.callsTo[i.nextAddrs[1].linear] = i.nextAddrs[1]
            else:
                for next in i.nextAddrs:
                    stack.append((ptr, next))

        # Sort the instruction memo, and store a list
        # of instructions sorted by address.
        addrs = memo.values()
        addrs.sort()
        self.instructions = map(self.image.iFetch, addrs)

    def codegen(self, traces=None):
        body = []
        for i in self.instructions:
            if i.addr.linear in self.labels:
                label = i.addr.label() + ': '
            else:
                label = ''
            body.append("/* %-60s R[%-9s] */ %15s %s%s" % (
                    i, i.referent, label,
                    self.hooks.get(i.addr.linear, ''),
                    i.codegen(traces=traces, clockEnable=self.clockEnable)))
        return """
Regs
%s(Regs reg)
{
  goto %s;
%s
}""" % (self.name, self.entryPoint.label(), '\n'.join(body))


class DOSBinary(BinaryImage):
    # Skeleton for output C code
    _skel = """/*
 * Static binary translation by sbt86
 *
 * Filename: %(filename)r
 * EXE size: %(exeSize)r
 * Memory size: %(memorySize)r
 * Entry point: %(entryPoint)s
 */

#include <stdint.h>

static uint8_t dataImage[] = {
%(dataImage)s};

#include "sbt86.h"

static StackItem stack[64];
static int stackPtr;
static uint32_t clock;

%(subDecls)s

%(_decls)s
%(traceDecls)s

%(subCode)s

uint8_t
%(mainFunction)s(const char *cmdLine)
{
    const uint32_t relocSegment = 0x%(relocSegment)04x;
    const uint32_t pspSegment = 0x%(pspSegment)04x;
    const uint32_t memorySize = 0x%(memorySize)08x;
    Regs reg = {{ 0 }};
    int retval;

    // Set up our DOS Exit handler

    retval = setjmp(dosExitJump);
    if (retval) {
        return (uint8_t)retval;
    }

    memset(mem, 0, memorySize + SEG(relocSegment, 0));
    memcpy(mem + SEG(relocSegment, 0), dataImage, sizeof dataImage);

    // Memory size (32-bit)
    reg.bx = memorySize >> 16;
    reg.cx = (uint16_t) memorySize;

    // XXX: Stack is fake.
    stackPtr = 0;
    reg.ss = 0;
    reg.sp = 0xFFFF;

    // Beginning of EXE image, after relocation
    reg.ds = relocSegment;

    // Program Segment Prefix.
    // Zero it, and copy in our command line.

    reg.es = pspSegment;
    memset(mem + SEG(reg.es, 0), 0, 0x100);

    mem[SEG(reg.es, 0x80)] = strlen(cmdLine);
    strcpy(mem + SEG(reg.es, 0x81), cmdLine);

    // Jump to the entry point

    reg.cs = 0x%(entryCS)04x;
    sub_%(entryLinear)X(reg);

    return 0xFF;
}
"""

    # Memory map:
    #
    #  relocSegment -- Segment to relocate binary to. This must be
    #  after the BIOS data area, IVT, and other low-memory areas.
    #
    #  pspSegment - Segment to place the Program Segment Prefix in.
    #  Generally this is 256 bytes prior to the relocSegment.
    #
    relocSegment = 0x70
    pspSegment = 0x60

    subroutines = None

    def toLinear(self, segmentOffset):
        return ((segmentOffset >> 16) << 4) | (segmentOffset & 0xFFFF)

    def parseHeader(self):
        self._hooks = {}
        self._traces = []
        self._decls = ''

        (signature, bytesInLastPage, numPages, numRelocations,
         headerParagraphs, minMemParagraphs, maxMemParagraphs,
         initSS, initSP, checksum, entryIP, entryCS, relocTable,
         overlayNumber) = self.unpack(0, "<2sHHHHHHHHHHHHH")

        if signature != "MZ":
            raise ValueError("Input file is not a DOS executable")

        self.exeSize = (numPages - 1) * 512 + bytesInLastPage

        self.headerSize = headerParagraphs * 16
        self.memorySize = minMemParagraphs * 16 + self.exeSize
        self.entryPoint = Addr16(self.relocSegment + entryCS, entryIP)
        self.image = self.offset(self.headerSize)

        relocs = []
        for i in range(numRelocations):
            offset, segment = self.unpack(relocTable + i * 4, "<HH")
            relocs.append(Addr16(segment, offset))
        self.image.relocate(self.relocSegment, relocs)

    def _toHexArray(self, data):
        return ''.join(["0x%02x,%s" % (ord(b), "\n"[:(i&15)==15])
                         for i, b in enumerate(data)])

    def decl(self, code):
        """Add code to the declarations in the generated C file.
           This is useful if patches require global variables or
           shared code.
           """
        self._decls += code

    def patch(self, addr, code, length=0):
        """Manually stick a line of assembly into the iCache,
           in order to replace an instruction we would have loaded
           from the EXE file.

           If the instruction is not a return or an unconditional
           jump, you must specify the instruction's length (in bytes)
           so we know where to find the next instruction. The length
           doesn't have to match the actual length of the assembly
           code you provide, it's just used to locate the next
           instruction to disassemble.
           """
        i = Instruction("%s %s %s" % (addr, ("--" * length) or '-', code))
        self.image._iCache[i.addr.linear] = i

    def hook(self, addr, code):
        """Install a C-language hook, to be executed just before the
           instruction at address 'addr'.  The address and C-code are
           both specified as strings.
           """
        self._hooks[Addr16(str=str(addr)).linear] = "{\n%s\n } " % code

    def patchAndHook(self, addr, asmCode, cCode, length=0):
        """This is a shorthand for patching and hooking the same address."""
        self.patch(addr, asmCode, length)
        self.hook(addr, cCode)

    def trace(self, mode, probe, fire):
        """Define a memory trace.

           'mode' is 'r' for memory reads, 'w' for writes, and 'rw' for either.

           'probe' is the body of a C function which is inlined at every
           memory access of the specified type. In this function, 'segment'
           and 'offset' will be a far pointer to the memory that was modified,
           and 'width' will be the 1 or 2 bytes. 'cs' and 'ip' identify the
           instruction performing the memory operation.

           'fire' is the function to be called when 'probe' returns TRUE.
           The parameters are identical to 'probe'.
           """
        self._traces.append(Trace("trace%d" % len(self._traces),
                                  mode, probe, fire))

    def analyze(self, verbose=False):
       """Analyze the whole program. This breaks it up into
          subroutines, and analyzes each routine.
          """

       print "Analyzing %s..." % self.filename

       self.subroutines = {}
       stack = [ self.entryPoint ]

       while stack:
           ptr = stack.pop()
           if ptr.linear in self.subroutines:
               if verbose:
                   sys.stderr.write("Subroutine %s already visited\n" % ptr)
               continue

           sub = Subroutine(self.image, ptr, self._hooks)
           sub.analyze(verbose)
           stack.extend(sub.callsTo.values())
           self.subroutines[ptr.linear] = sub

    def findCode(self, signature):
        """Find a signature in the binary's code segment.
           Returns an Addr16, using the binary's entry
           point as a segment reference.
           """
        return self.findCodeMultiple(signature, 1)[0]

    def findCodeMultiple(self, signature, expectedCount=None):
        """Like findCode(), but allows the signature to appear zero or more times.
           If expectedCount is specified, we require it to occur exactly that many
           times. Returns a list of Addr16s.
           """
        sig = Signature(signature)
        addrs = [self.entryPoint.add(o + (self.relocSegment << 4)
                                     - self.entryPoint.linear)
                 for o in sig.find(self.image._data)]
        if expectedCount is not None and len(addrs) != expectedCount:
            raise SignatureMatchError("Signature found %d times, expected to "
                                      "find %d. Matches: %r" %
                                      (len(addrs), expectedCount, addrs))
        print "Found patch location %r in %s for: %r" % (
            addrs, self.filename, sig.shortText)
        return addrs

    def codegen(self, mainFunction):
        vars = dict(self.__class__.__dict__)
        vars.update(self.__dict__)

        print "Code generating %s (%d subroutines)..." % (
            self.filename, len(self.subroutines.values()))

        vars['mainFunction'] = mainFunction
        vars['subCode'] = '\n'.join([s.codegen(traces=self._traces)
                                     for s in self.subroutines.itervalues()])
        vars['subDecls'] = '\n'.join(["static Regs %s(Regs r);" % s.name
                                     for s in self.subroutines.itervalues()])
        vars['traceDecls'] = '\n'.join([t.codegen() for t in self._traces])
        vars['entryLinear'] = self.entryPoint.linear
        vars['entryCS'] = self.entryPoint.segment

        # The data image contains everything prior to the entry point
        # (discard most of the code segment). This assumes the program
        # doesn't have any initialized data in the code segment, or any
        # self-modifying or self-referential code. Any exceptions will
        # need to be patched.

        dataSize = min(self.exeSize, self.entryPoint.linear)
        vars['dataImage'] = self._toHexArray(self.image.read(0, dataSize))

        return self._skel % vars

    def writeCodeToFile(self, filename, mainFunction='body', verbose=False):
        """Run analysis if necessary, then generate code to a file."""
        f = open(filename, 'w')
        if self.subroutines is None:
            self.analyze(verbose)
        f.write(self.codegen(mainFunction))
