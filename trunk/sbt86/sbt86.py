#!/usr/bin/env python
#
# Experimental 8086 -> C static binary translator.
# Micah Dowty <micah@navi.cx>
#

import sys
import struct
import subprocess


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

    def codegen(self):
        addr = "SEG(%s,%s)" % (
            self.segment.codegen(), " + ".join([o.codegen() for o in self.offsets]))

        if self.width == 1:
            return "mem[%s]" % addr
        elif self.width == 2:
            return "(*(uint16_t*)(mem + %s))" % addr
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


class Instruction:
    """A disassembled instruction, in NASM format."""

    _widthNames = {
        "byte": 1,
        "short": 2,
        "word": 2,
        }

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
        return "{ Regs r = reg; %s(&r); reg = r; }" % name

    def codegen(self):
        f = getattr(self, "codegen_%s" % self.op, None)
        if not f:
            raise NotImplementedError("Unsupported opcode in %s" % self)
        return f(*self.args)

    def codegen_mov(self, dest, src):
        return "%s = %s;" % (dest.codegen(), src.codegen())

    def codegen_xor(self, dest, src):
        return "reg.uresult = %s ^= %s; reg.sresult=0; %s" % (
            dest.codegen(), src.codegen(), self._resultShift(dest))

    def codegen_or(self, dest, src):
        return "reg.uresult = %s |= %s; reg.sresult=0; %s" % (
            dest.codegen(), src.codegen(), self._resultShift(dest))

    def codegen_and(self, dest, src):
        return "reg.uresult = %s &= %s; reg.sresult=0; %s" % (
            dest.codegen(), src.codegen(), self._resultShift(dest))

    def codegen_add(self, dest, src):
        return "reg.sresult = %s + %s; %s = reg.uresult = %s + %s; %s" % (
            signed(dest), signed(src),
            dest.codegen(), u32(dest), u32(src),
            self._resultShift(dest))

    def codegen_adc(self, dest, src):
        return "reg.sresult = %s + %s + CF; %s = reg.uresult = %s + %s + CF; %s" % (
            signed(dest), signed(src),
            dest.codegen(), u32(dest), u32(src),
            self._resultShift(dest))

    def codegen_sub(self, dest, src):
        return "reg.sresult = %s - %s; %s = reg.uresult = %s - %s; %s" % (
            signed(dest), signed(src),
            dest.codegen(), u32(dest), u32(src),
            self._resultShift(dest))

    def codegen_sbb(self, dest, src):
        return "reg.sresult = %s - (%s + CF); %s = reg.uresult = %s - (%s + CF); %s" % (
            signed(dest), signed(src),
            dest.codegen(), u32(dest), u32(src),
            self._resultShift(dest))

    def codegen_shl(self, r, cnt):
        return self._repeat(cnt,
                            ("reg.sresult=0; %s = reg.uresult = ((uint32_t)%s) << 1;") %
                            (r.codegen(), r.codegen())) + self._resultShift(r)

    def codegen_shr(self, r, cnt):
        return self._repeat(cnt,
                            ("reg.sresult=0; "
                             "reg.uresult = (%s & 1) << 16; "
                             "(%s) >>= 1;") %
                            (u32(r), r.codegen()))

    def codegen_sar(self, dest, cnt):
        return self._repeat(cnt,
                            ("reg.sresult=0; "
                             "reg.uresult = (%s & 1) << 16; "
                             "(%s) = ((int16_t)%s) >> 1;") %
                            (u32(r), r.codegen(), r.codegen()))

    def codegen_ror(self, r, cnt):
        msbShift = r.width * 8 - 1
        return self._repeat(cnt,
                            ("reg.sresult=0; "
                             "reg.uresult = ((uint32_t)(%s) & 1) << 16;"
                             "(%s) = ((%s) >> 1) + ((%s) << %s);") %
                            (r.codegen(), r.codegen(),
                             r.codegen(), r.codegen(), msbShift))

    def codegen_rcr(self, r, cnt):
        msbShift = r.width * 8 - 1
        return self._repeat(cnt,
                            ("int cf = CF;"
                             "reg.sresult=0; "
                             "reg.uresult = %s << 16;"
                             "(%s) = ((%s) >> 1) + (cf << %s);") %
                            (r.codegen(), r.codegen(), r.codegen(), msbShift))

    def codegen_rcl(self, r, cnt):
        return self._repeat(cnt,
                            ("reg.sresult=0; "
                             "%s = reg.uresult = (%s << 1) + CF; %s") %
                            (r.codegen(), u32(r), self._resultShift(r)))

    def codegen_xchg(self, a, b):
        return "{ uint16_t t = %s; %s = %s; %s = t; }" % (
            a.codegen(), a.codegen(), b.codegen(), b.codegen())

    def codegen_imul(self, arg):
        return "reg.ax = (int8_t)reg.al * %s;" % arg.codegen()

    def codegen_mul(self, arg):
        return "reg.ax = reg.al * %s;" % arg.codegen()

    def codegen_div(self, arg):
        return "reg.al = reg.ax / %s; reg.ah = reg.ax %% %s;" % (
            arg.codegen(), arg.codegen())

    def codegen_cmc(self):
        return "if (CF) CLR_CF(reg); else SET_CF(reg);"

    def codegen_clc(self):
        return "CLR_CF(reg);"

    def codegen_stc(self):
        return "SET_CF(reg);"

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
        return "%s = ~%s;" % (arg.codegen(), arg.codegen())

    def codegen_neg(self, arg):
        return "%s = -%s;" % (arg.codegen(), arg.codegen())

    def codegen_les(self, dest, src):
        assert isinstance(src, Indirect)
        assert src.width == 2
        segPtr = Indirect(src.segment, src.offsets + [Literal(2)], 2)

        return "%s = %s; %s = %s;" % (
            dest.codegen(), src.codegen(),
            Register('es').codegen(), segPtr.codegen())

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
        return "%s = %s; %s += 2; %s += 2;" % (
            Indirect(Register('es'), (Register('di'),), 2).codegen(),
            Indirect(Register('ds'), (Register('si'),), 2).codegen(),
            Register('si').codegen(), Register('di').codegen())

    def codegen_stosb(self):
        return "%s = %s; %s++;" % (
            Indirect(Register('es'), (Register('di'),), 1).codegen(),
            Register('al').codegen(), Register('di').codegen())

    def codegen_stosw(self):
        return "%s = %s; %s += 2;" % (
            Indirect(Register('es'), (Register('di'),), 2).codegen(),
            Register('ax').codegen(), Register('di').codegen())

    def codegen_scasb(self):
        return "%s; %s++;" % (
            self.codegen_cmp(Register('al'),
                             Indirect(Register('es'), (Register('di'),), 1)),
            Register('di').codegen())

    def codegen_lodsb(self):
        return "%s = %s; %s++;" % (
            Register('al').codegen(),
            Indirect(Register('ds'), (Register('si'),), 1).codegen(),
            Register('si').codegen())

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
        return "*callerRegs = reg; return;"

    def codegen_retf(self):
        return self.codegen_ret()

    def codegen_int(self, arg):
        return self._call("int%x" % arg);

    def codegen_out(self, port, value):
        return "out(%s,%s);" % (port.codegen(), value.codegen())

    def codegen_in(self, value, port):
        return "%s = in(%s);" % (value.codegen(), port.codegen())


class BinaryImage:
    def __init__(self, filename=None, file=None, offset=0):
        if file is None:
            file = open(filename, "rb")
        self.filename = filename
        self._file = file
        self._offset = offset
        self.parseHeader()
        self._iCache = {}

    def offset(self, offset):
        return BinaryImage(self.filename, self._file, self._offset + offset)

    def parseHeader(self):
        pass

    def read(self, offset, length):
        self._file.seek(offset + self._offset)
        return self._file.read(length)

    def unpack(self, offset, fmt):
        return struct.unpack(fmt, self.read(offset, struct.calcsize(fmt)))

    def le8(self, offset):
        return self.unpack(offset, "<B")[0]

    def le16(self, offset):
        return self.unpack(offset, "<H")[0]

    def le32(self, offset):
        return self.unpack(offset, "<I")[0]

    def disasm(self, addr, bits=16):
        args = ["ndisasm",
                "-o", str(addr.offset),
                "-b", str(bits),
                "-e", str(addr.linear + self._offset),
                self.filename]
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

    def patch(self, line):
        """Manually stick a line of assembly into the iCache,
           in order to replace an instruction we would have loaded
           from the EXE file.
           """
        i = Instruction(line)
        self._iCache[i.addr.linear] = i

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

            # Remember all label targets, and remember future
            # addresses to analyze. Treat subroutine calls specially.

            for next in i.labels:
                self.labels[next.linear] = True

            if i.op == 'call':
                assert len(i.nextAddrs) == 2
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

    def codegen(self):
        body = []
        for i in self.instructions:
            if i.addr.linear in self.labels:
                label = i.addr.label() + ': '
            else:
                label = ''
            body.append("/* %-60s R[%-9s] */ %15s %s%s" % (
                    i, i.referent, label,
                    self.hooks.get(i.addr.linear, ''),
                    i.codegen()))
        return """
void
%s(Regs *callerRegs)
{
  register Regs reg = *callerRegs;

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

%(subDecls)s

%(subCode)s
void
body(Regs initRegs)
{
  initRegs.cs = 0x%(entryCS)04x;
  sub_%(entryLinear)X(&initRegs);
}
"""

    def toLinear(self, segmentOffset):
        return ((segmentOffset >> 16) << 4) | (segmentOffset & 0xFFFF)

    def parseHeader(self):
        self._hooks = {}

        (signature, bytesInLastPage, numPages, numRelocations,
         headerParagraphs, minMemParagraphs, maxMemParagraphs,
         initSS, initSP, checksum, entryIP, entryCS, relocTable,
         overlayNumber) = self.unpack(0, "<2sHHHHHHHHHHHHH")

        if signature != "MZ":
            raise ValueError("Input file is not a DOS executable")

        self.exeSize = (numPages - 1) * 512 + bytesInLastPage

        self.headerSize = headerParagraphs * 16
        self.memorySize = minMemParagraphs * 16 + self.exeSize
        self.entryPoint = Addr16(entryCS, entryIP)
        self.image = self.offset(self.headerSize)

    def _toHexArray(self, data):
        return ''.join(["0x%02x,%s" % (ord(b), "\n"[:(i&15)==15])
                         for i, b in enumerate(data)])

    def hook(self, addr, code):
        """Install a C-language hook, to be executed just before the
           instruction at address 'addr'.  The address and C-code are
           both specified as strings.
           """
        self._hooks[Addr16(str=addr).linear] = "{\n%s\n } " % code

    def analyze(self, verbose=False):
       """Analyze the whole program. This breaks it up into
          subroutines, and analyzes each routine.
          """

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

    def codegen(self):
        vars = dict(self.__dict__)
        vars['subCode'] = '\n'.join([s.codegen()
                                     for s in self.subroutines.itervalues()])
        vars['subDecls'] = '\n'.join(["void %s(Regs *r);" % s.name
                                     for s in self.subroutines.itervalues()])
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


b = DOSBinary(sys.argv[1])

# Patches for dynamic jumps in LAB.EXE
if 0:
    b.image.patch("0C63:040D - int 3")
    b.image.patch("0C63:041B - int 3")
    b.image.patch("0C63:11B5 - int 3")
    b.image.patch("0C63:1271 - int 3")
    b.image.patch("0C63:1CB2 - jmp 0x21B3")
    b.image.patch("0C63:0F89 - int 3")
    b.image.patch("0C63:004B 00 nop")   # Copy protection

# Dynamic call in MENU.EXE
if 0:
    b.image.patch("009E:0778 - int 3")

# Patches for TUT.EXE
if 1:
    # Good patches
    b.image.patch("098F:03F2 - ret")                   # Stub video_set_cursor_position,
                                                       # avoid push/pop_ax_bx_cx_dx
    b.image.patch("098F:1CB2 - jmp 0x21b3")            # video_draw_playfield
    b.image.patch("098F:0D72 - ret")                   # Self-modifying (largetext font)
    b.image.patch("098F:0D19 - ret")                   # Self-modifying (text font)
    b.image.patch("098F:0F89 00112233  call 0x4E03")   # game_func_ptr
    b.hook("098F:1C93", "SDL_Delay(15);")              # Per-frame delay

    # XXX: Problematic patches
    b.image.patch("098F:11B5 - ret")           # unknown func ptr
    b.image.patch("098F:1271 - ret")           # unknown func ptr (text-related?)
    b.image.patch("098F:0B7B - ret")           # Self-modifying code (text rendering XXX)


b.analyze()
print b.codegen()
