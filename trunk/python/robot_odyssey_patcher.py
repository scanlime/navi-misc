#!/usr/bin/env python
#
# Binary patcher for Robot Odyssey. May also work on games which
# use the same engine.
#
# For Robot Odyssey, you only need to patch the GAME.EXE, LAB.EXE, and
# TUT.EXE binaries. The others do not need any patch, and this patcher
# will not run on them.
#
# Patcher version 1.0, April 4 2009.
# Requires Python 2.5 and nasm.
#
# This patcher knows how to make four changes to the Robot Odyssey engine:
#
#   1. The copy protection (which only works on 5.25" floppies)
#      is disabled, if any is present.
#
#   2. A frame rate limiter is installed. You can specify the
#      target frame rate on the command line.
#
#   3. Optionally, the frame rate limiter can be disabled temporarily
#      when keyboard input is pending. This is the "-k" option. This
#      option makes the game feel more responsive.
#
#   4. We patch the game's keyboard handler, so it now works with the
#      normal (non-numeric-keypad) arrow keys. Without this patch,
#      the shift-arrow combination (for walking slowly) only worked
#      with the numpad arrows, so the game was impossible to play on
#      a laptop.
#
# -- Micah Dowty <micah@navi.cx>
#

import optparse
import sys
import os
import binascii
import atexit
import tempfile
import subprocess
import shutil


def unhex(str):
    """Simple wrapper to remove whitespace from a string and convert hex to binary."""
    return binascii.a2b_hex(str.replace(" ", "").replace("\n", "").replace("\t", ""))

def asm(str, bits=16):
    """Assemble some code, returning the raw code bytes.
       This implementation calls nasm in a subprocess.
       """
    asmFile = tempfile.NamedTemporaryFile()
    asmFile.write(" bits %d\n %s\n" % (bits, str))
    asmFile.flush()
    outputDir = tempfile.mkdtemp()
    try:
        outputFile = os.path.join(outputDir, 'out')
        subprocess.check_call(('nasm', asmFile.name, '-o', outputFile))
        outputData = open(outputFile, 'rb').read()
    finally:
        shutil.rmtree(outputDir)
    return outputData

class CodeMismatchError(Exception):
    pass


def copyPatch(exe):
    """Patch the Robot Odyssey copy protection"""

    # The GAME.EXE and LAB.EXE binaries on the original 5.25" disk
    # have copy protection which searches for a "flaky bit". On the
    # original disk, the last byte of several sectors has been written
    # in such a way that reading it is nondeterministic. (There are
    # various ways of doing this, either by misaligning the track,
    # writing high-frequency noise onto the disk, writing a very weak
    # magnetic signal...)
    #
    # There is a self-contained chunk of code in a separate code
    # segment near the end of the binary, which is wholely responsible
    # for the copy protection scheme. The main program calls this
    # code, which uses Int 13h to read these flaky sectors. Based on
    # the number of times these flaky bits read different values, this
    # function returns a 0 on success or -1 on failure.
    #
    # Back at the beginning of the binary, the main program stores
    # this result (after inverting it) to a global variable. If this
    # variable is 1, many parts of the game's main loop are skipped.
    #
    # The result: If the copy protection fails, your soldering iron
    # won't turn red, and you can't solder.
    #
    # There are many trivial ways to defeat this copy protection: The
    # easiest way: the entire block of code/data at the end of the
    # binary can be removed and replaced with a single 'retf'.
    #
    # We locate this code using three heuristics:
    #
    #   - Just prior to the copy protection code is the binary's stack,
    #     which is a big region that's all initialized to zero.
    #
    #   - Since this code is used as an alternate CS/DS, it must be
    #     paragraph aligned.
    #
    #   - The beginning of the code consists of a single jump instruction
    #     followed by some initialized data. Some of it we don't want to
    #     rely on (like the number of sectors to read), but the frist few
    #     bytes at least look pretty safe.
    #
    # The first few bytes of the copy protection segment:
    #
    #   Offset  Data     Notes
    #   ------  ----     -----
    #       0   EB 18    Jump to the entry point below.
    #       2   90       Unknown (dead no-op?)
    #       3   00       Vestigial disable flag? This is always zero, but
    #                    it's used below as part of a comparison that looks
    #                    like at one time it may have skipped the copy
    #                    protection if this value was nonzero. But the
    #                    jump after the comparison is unconditional. Perhaps
    #                    this was a flag that the developers used, which was
    #                    subsequently "disabled" by replacing the conditional
    #                    jump with an unconditional one?
    #       4   00       Unused?
    #       5   08       Disk track
    #       6   08       Last sector to read
    #       7   00       Counter for sectors which were different on the
    #                    first read vs. the second read.
    #       8   00       Counter for sectors that end with 0xF7
    #       9   07       Minimum successful value for the above two counters
    #       A   00 00    Unused?
    #       C   00 00    Saved BX
    #       E   00 00    Saved CX
    #      10   00 00    Saved DX
    #      12   00 00    Saved DI
    #      14   00 00    Saved SI
    #      16   00 00    Saved DS
    #      18   00 00    Saved ES
    #      1A            Beginning of code (Function prologue)
    #      1A   55         push bp
    #      1B   8B EC      mov  bp, sp
    #

    stackSegment = chr(0) * 1024

    p = exe.find(stackSegment + unhex("EB 18 90 00"))
    if (p
        and (p._offset & 0xF) == 0
        and p[0x41A] == 0x55
        and p[0x41B] == 0x8B
        and p[0x41C] == 0xEC
        ):
        print "Found copy protection. Disabling..."
    else:
        print "Copy protection not found."
        return

    patch = asm("""
        xor  ax, ax
        retf
    """)

    p[len(stackSegment)] = patch


def blitPatch(exe, fps, kbFast):
    """Patch the Robot Odyssey engine's blit loop"""

    # The engine used by Robot Odyssey has a giant unrolled loop which
    # does a scanline-by-scanline blit from a data segment to the CGA
    # framebuffer. There is one unrolled iteration for each scanline
    # on the playfield. Each iteration looks like:
    #
    #    mov   cx, bx     ; Re-iniitalize cx to 0x40 (# of words per line)
    #    xchg  ax, si     ; Alternate planes (for CGA interlacing)
    #    mov   di, si     ; Src/dest offsets equal
    #    rep movsw        ; Copy one scanline
    #
    # I've included this code fragment as pre-assembled hex, since
    # nasm seems to generate slightly different code than what the
    # game actually uses.

    blitLoopPattern = 191 * unhex("A5 8B CB 96 8B FE F3")
    blitLoop = exe.find(blitLoopPattern)
    if not blitLoop:
        raise CodeMismatchError()
    print "Found blitter loop. Patching..."

    # Now build our modified blit loop.
    #
    # To have space for our extra features, we write a much more
    # compact version of the original blitter. The original one takes
    # up so much space because it blits scanline-by-scanline from the
    # top of the screen to the bottom. I suspect this is to make
    # tearing artifacts less noticeable. Instead of interlaced tearing
    # artifats, you'd see a single horizontal line. But on modern
    # machines (especially with emulated CGA cards) this tearing is no
    # longer a problem, and in fact it's probably better to just copy
    # the whole frame in one go.
    #
    # Note that we'd like to just copy the whole frame in one
    # instruction, but it looks like (on DosBox, at least) you can't
    # write to both CGA planes in the same instruction. So, we have to
    # do at least two copies.

    patch = asm("""

        mov   cx, 0x2000          ; Number of words in each plane
        xchg  ax, si              ; Clear SI (First interlaced plane)
        mov   di, si              ; Src/dest offsets equal
        rep movsw                 ; Copy first half of the frame

        mov   cx, 0x2000          ; Number of words in each plane
        xchg  ax, si              ; Sets SI to 0x2000 (Second interlaced field)
        mov   di, si              ; Src/dest offsets equal
        rep movsw                 ; Copy second half of the frame

    """)

    # Add a speed control. Since we're adding this to the game's one
    # and only blitter loop, we're guaranteed that this code gets
    # called exactly once per frame. (There are several different
    # codepaths that could generate frames, and several different
    # variants of the game's main loop- so this is a pretty handy
    # location.)
    #
    # To implement the speed control, we'll use the BIOS clock
    # interrupt as a time reference. We can make an Int 1Ah call to
    # check whether the require number of ticks have elapsed, and we
    # can use the "hlt" instruction to wait for a clock
    # tick. Additionally, we can reprogram the PIT to tune the
    # frequency of this interrupt, so we get exactly the frame rate we
    # want.
    #
    # Storing global variables: We need to maintain some state that
    # persists across frames. There are many places we could stick
    # this data:
    #
    #   1. Near the beginning of the data segment. There is a table of
    #      filenames near the beginning, and these are only used during
    #      initialization, so it's okay if we overwrite one.
    #
    #   2. In the code segment, inside the space we just opened by
    #      rewriting the blitter. This is very safe, but it could
    #      cause performance problems for binary translators because
    #      it would look like self-modifying code.
    #
    #   3. At the bottom of the stack segment. Assuming the game doesn't
    #      use absolutely all of its stack, this is pretty safe.
    #
    #   4. At the end of the data segment. This is more tricky, because
    #      the game is already sticking other large data structures
    #      (like the framebuffer) there. We'd have to be careful to avoid
    #      all of these.
    #
    # Right now we use approach (3), since it's simple and pretty safe.
    #

    # Timing calculations: Figure out how many ticks per frame, and
    # what PIT divisor to use.

    pitHZ = 1193182
    numTicks = 1

    while True:
        tickHZ = fps * numTicks

        divisor = int(pitHZ / tickHZ)

        if divisor < 0x10000:
            # The divisor is fine, we're done
            break
        else:
            # Divisor is too large, increase the
            # ticks-per-frame ratio.
            numTicks += 1

    divHigh = divisor >> 8
    divLow = divisor & 0xFF

    patch += asm("""

        pusha                     ; Save state, use DS=SS
        push  ds
        push  ss
        pop   ds
        sti                       ; Make sure we don't hlt with interrupts off

        cmp   byte [tmr_init], 1  ; Have we already initialized the timer?
        jz    waitLoop
        mov   byte [tmr_init], 1

        mov   dx, 0x43            ; Program the divisor for PIT channel 0
        mov   al, 0x34
        out   dx, al
        mov   dx, 0x40
        mov   al, %d
        out   dx, al
        mov   al, %d
        out   dx, al

waitLoop
        xor   ax, ax              ; Get number of ticks from BIOS
        int   0x1A                ; Result is in DX
        mov   bx, dx
        sub   bx, [last_tick]     ; Count the number of elapsed ticks

        cmp   bx, %d              ; Have enough ticks passed?
        jnb   exit                ; If so, we're done.

        %%if %d                   ; Optional: If there are keys waiting
           push  dx               ; in the keyboard buffer don't wait.
           mov   ax, 0x100
           int   0x16
           pop   dx
           jnz   exit
        %%endif

        hlt                       ; Wait for a timer tick
        jmp   waitLoop

exit
        mov   [last_tick], dx     ; Save this frame's tick count
        pop   ds                  ; Restore state, exit.
        popa

        ; Variable addresses

last_tick  equ 0
tmr_init   equ 2

    """ % (divLow, divHigh, numTicks, kbFast))

    # Now overwrite the original blit loop with our modified loop.
    # We pad any excess space with NOPs

    assert len(patch) <= len(blitLoopPattern)
    blitLoop[0] = patch + (len(blitLoopPattern) - len(patch)) * asm('nop')


def kbPatch(exe):
    """Patch the Robot Odyssey engine's keyboard decoder."""

    # Robot Odyssey has a bug that makes it rather difficult to play
    # on an AT keyboard, particularly one without a numeric keypad:
    # While the AT-style ("gray") arrow keys do work for basic
    # movement, the game can't detect shift-arrow combinations. So,
    # you can't take single-pixel steps. This makes Robot Odyssey
    # totally unplayable on a laptop unless you have some kind of
    # keyboard remapper running externally.
    #
    # To understand the problem, we can take a closer look at the
    # game's keyboard handler. To poll the keyboard, the basic steps
    # are:
    #
    #  1. The game first uses BIOS interrupt 16h to first check
    #     for a key in the keyboard buffer (AH=1), then to retrieve
    #     a key if one exists (AH=0). It also pokes at the BIOS data
    #     area (segment 40h) directly, in order to turn off numlock,
    #     turn on caps lock, and drain the keyboard buffer.
    #
    #  2. This function returns with the value of AX from Int 16h
    #     preserved. This means we have a scan code in AH, and a
    #     character code in AL. Now we're in the game's keyboard
    #     mapping function, which we'll be patching here.
    #
    #  3. This function stores a translated key in a global variable.
    #     If there are no keys down, it stores zero. For most keys,
    #     it stores an ASCII value. But there are several special
    #     cases: Arrow keys, Shift-Arrow keys, and Insert/Delete.
    #
    # This arrow key remapping is of the form:
    #
    #  if (input_scancode == LEFT) {
    #     if (al == 0) {
    #        output_scancode = TRANSLATED_SHIFT_LEFT;
    #     } else {
    #        output_scancode = TRANSLATED_LEFT;
    #     }
    #  } else {
    #    ...
    #  }
    #
    # So, they're relying on the fact that an un-shifted arrow has
    # no ASCII equivalent, while a shift-arrow on the numpad turns
    # into a number key when the BIOS translates it.
    #
    # This is a clever hack, but it won't do for gray arrow keys.
    # Instead, we'd rather look at the actual status of the shift
    # keys. We can get this from the BIOS data area. This will
    # increase the code size a bit, but we can make room by removing
    # support for very old legacy scancodes.

    # XXX: This patcher works on Robot Odyssey, but it does not
    #      work on Gertrude's Secrets, since that game uses different
    #      internal keycode values.

    # This is the original keyboard handler snippet that we'll be
    # replacing, starting right after the BIOS polling function
    # returns, and ending at a point where the translated key is
    # expected to be in AL. This section has been carefully chosen
    # to avoid any non-relative addresses.

    origMapperLen = 137
    origMapperPrefix = unhex("""
        75 03 E9 81 00 80 FC 88 74 34 80 FC 4D 74 2B 80 FC 86 74 3A 80
        FC 50 74 31 80 FC 87 74 40 80 FC 4B 74 37 80 FC 85 74 46 80 FC
        48 74 3D 80 FC 52 74 48 80 FC 53 74 49 EB 54 90 3C 00 74 06
        """)

    kbMapper = exe.find(origMapperPrefix)
    if not kbMapper:
        raise CodeMismatchError()
    print "Found keyboard mapper. Patching..."

    patch = asm("""

        ; On entry:
        ;
        ;   AH = BIOS Scancode
        ;   AL = ASCII Key
        ;   Z=0 if a key is waiting,
        ;   Z=1 if there is no key.

        jz    no_key

        cmp   ah, 0x48
        jz      key_up
        cmp   ah, 0x50
        jz      key_down
        cmp   ah, 0x4B
        jz      key_left
        cmp   ah, 0x4D
        jz      key_right

        cmp   ah, 0x52      ; NB: I don't think these are used by Robot Odyssey,
        jz      key_insert  ;     but they're used by the shape editor in
        cmp   ah, 0x53      ;     Gertrude's Secrets.
        jz      key_delete

        ; Other key: Leave it in ASCII, we're done.
        jmp   done

key_insert
        mov   al, 0x86
        jmp   done
key_delete
        mov   al, 0x87
        jmp   done

        ; Each arrow key has two codes. They happen to always be
        ; separated by 6. The low code is the unshifted version, the
        ; high code is shifted. We'll stick the low code in for each
        ; arrow key here, then we'll head to a shared block of code
        ; below which checks the shift modifierss.

key_up
        mov   al, 0x82
        jmp   arrow_shift_test
key_down
        mov   al, 0x83
        jmp   arrow_shift_test
key_left
        mov   al, 0x84
        jmp   arrow_shift_test
key_right
        mov   al, 0x85
        ; Fall through

arrow_shift_test
        push  ds
        mov   bx, 0x40     ; BIOS Data Area
        mov   ds, bx
        mov   cl, [0x17]   ; Keyboard Flags 0
        pop   ds
        and   cl, 0x3      ; Left/right shift mask
        jz    done         ; Not shifted
        add   al, 6        ; Shifted
        jmp done

no_key
        xor al, al
done
        ; Immediately afterwards will be some code that saves AL
        ; and returns.

    """)

    # Insert the patch, and pad any extra space with NOPs

    assert len(patch) <= origMapperLen
    kbMapper[0] = patch + (origMapperLen - len(patch)) * asm('nop')


class Patchable:
    """A wrapper around some binary data that we'd like to patch.

       - patchable[address] = 42
         Writes the byte value '42' to 'address

       - patchable[address] = 'foo'
         Write the raw binary string 'foo' to address through address+2.

       This can be combined with tools like struct.pack and 'unhex' to
       generate useful binary strings.
       """

    def __init__(self, string, array=None, offset=0):
        if array is None:
            array = map(ord, string)

        # We keep the data in both string and array form: The string
        # is immutable, but fast to search. The array is the only
        # place where we can make modifications.

        self._array = array
        self._string = string
        self._offset = offset

    def offset(self, offset):
        """Create a second Patchable that references a portion of this
           Patchable, starting at a particular offset. Modifications to
           the new object will affect this one.
           """
        return Patchable(self._string, self._array, self._offset + offset)

    def find(self, data):
        """Find a string in the original data associated with this
           Patchable. On success, returns a new Patchable object which
           represents the section we found. On failure, returns None.
           """
        index = self._string.find(data)
        if index >= 0:
            return self.offset(index)

    def __getitem__(self, x):
        return self._array[x + self._offset]

    def __setitem__(self, x, value):
        if type(value) in (int, long):
            if value < 0 or value > 255:
                raise ValueError("Byte value out of range: %d" % value)
            self._array[x + self._offset] = value

        elif type(value) is str:
            self._array[x + self._offset:
                        x + self._offset + len(value)] = map(ord, value)

        else:
            raise TypeError("Unsupported type for Patchable: %r" % value)

    def __repr__(self):
        return "<%s at offset 0x%x>" % (self.__class__.__name__, self._offset)

    def __str__(self):
        return ''.join(map(chr, self._array[self._offset:]))


class BinaryPatcher:
    def __init__(self):
        self.parser = optparse.OptionParser()
        self.parser.usage = "%prog [options] original-file patched-file"

    def load(self):
        self.options, args = self.parser.parse_args()
        if len(args) != 2:
            self.parser.print_usage()
            sys.exit(1)

        self.inFile, self.outFile = args

        self.patchable = Patchable(open(self.inFile).read())
        atexit.register(self.save)
        return self.patchable

    def save(self):
        open(self.outFile, "wb").write(str(self.patchable))


if __name__ == "__main__":
    p = BinaryPatcher()

    p.parser.add_option("-r", "--rate", dest="fps", type="float", default=8.0,
                        help="set the game's frame rate to HZ [Default: 8]",
                        metavar="HZ")
    p.parser.add_option("-f", "--fast", action="store_true", default=False,
                        dest="kbfast", help="go faster when keyboard input is pending")
    p.parser.add_option("-k", "--kbmap", action="store_true", default=False,
                        dest="kbmap",
                        help="patch the keyboard mapper, to fix shift-Arrow keys")

    exe = p.load()

    try:
        copyPatch(exe)
        blitPatch(exe, p.options.fps, p.options.kbfast)
        if p.options.kbmap:
            kbPatch(exe)

    except CodeMismatchError:
        sys.stderr.write("Error: Can't find the code to patch. EXE file is"
                         " corrupted, unsupported or already patched!\n")
        sys.exit(1)

    p.save()
