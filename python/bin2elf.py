#!/usr/bin/env python
#
# Tiny utility to convert a bunch of binary files, named according to
# their load address (%x.bin), into a single .ELF file. The ELF file
# is written as "memory.elf".
#
# This ELF file will have one segment for each input file. It has no
# section header. This ELF can be loaded into memory by any standard
# ELF loader, plus it's a convenient way to load many segments into
# IDA Pro at once.
#
# I couldn't get the results I wanted with normal GNU Binutils ld and
# objcopy, so this just writes an ELF header directly.
#
# -- Micah Dowty
#

import sys
import os
import struct
import binascii

# XXX: These really should be command line arguments
outputFile = "memory.elf"
entryAddr  = 0x2380000

# Parse args, and come up with a name, address, and size for each file.
files = []
for file in sys.argv[1:]:
    addr = int(file.split('.')[0], 16)
    size = os.stat(file).st_size
    files.append((file, addr, size))

# Sort files by address
files.sort()

# Output file
out = open(outputFile, 'wb')

# Header sizes
ehSize    = 0x34
phentSize = 0x20
phnum     = len(files)
phSize    = phentSize * phnum

def pageAlign(p):
    return (p + 0xfff) & ~0xfff

# Calculate a file offsets for each input binary
offsets = [pageAlign(ehSize + phSize)]
for file, addr, size in files:
    offsets.append(pageAlign(offsets[-1] + size))

# File header
out.write(binascii.a2b_hex("7f454c46010101610000000000000000"))  # e_ident
out.write(struct.pack("<H", 2))           # e_type
out.write(struct.pack("<H", 0x28))        # e_machine
out.write(struct.pack("<I", 1))           # e_version
out.write(struct.pack("<I", entryAddr))   # e_entry
out.write(struct.pack("<I", ehSize))      # e_phoff
out.write(struct.pack("<I", 0))           # e_shoff
out.write(struct.pack("<I", 0))           # e_flags
out.write(struct.pack("<H", ehSize))      # e_ehsize
out.write(struct.pack("<H", phentSize))   # e_phentsize
out.write(struct.pack("<H", phnum))       # e_phnum
out.write(struct.pack("<H", 0x28))        # e_shentsize
out.write(struct.pack("<H", 0))           # e_shnum
out.write(struct.pack("<H", 0))           # e_shstrndx

assert(out.tell() == ehSize)

# Program header
for i, (file, addr, size) in enumerate(files):
    offset = offsets[i]

    # Program header entry
    out.write(struct.pack("<I", 1))       # p_type = PT_LOAD
    out.write(struct.pack("<I", offset))  # p_offset
    out.write(struct.pack("<I", addr))    # p_vaddr
    out.write(struct.pack("<I", addr))    # p_paddr
    out.write(struct.pack("<I", size))    # p_filesz
    out.write(struct.pack("<I", size))    # p_memsz
    out.write(struct.pack("<I", 0x7))     # p_flags = read/write/exec
    out.write(struct.pack("<I", 1))       # p_align

# Write file contents
for i, (file, addr, size) in enumerate(files):
    out.seek(offsets[i])

    infile = open(file, 'rb')
    while True:
        block = infile.read(0x10000)
        if not block:
            break
        out.write(block)
