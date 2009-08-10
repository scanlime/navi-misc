#!/usr/bin/env python
#
# Tiny utility to convert a bunch of binary files, named according to
# their load address (%x.bin), into a single .ELF file. The ELF file
# is written as "memory.elf".
#
# -- Micah Dowty
#

import sys
import os

# Parse args
files = []
for file in sys.argv[1:]:
    addr = int(file.split('.')[0], 16)
    files.append((file, addr))

args = ['arm-eabi-objcopy',
        '-O', 'elf32-littlearm',
        '-I', 'binary']

# First file is converted as a data segment
args.extend([
        files[0][0],
        '--change-section-address',
        '.data=0x%x' % files[0][1],
        ])

# Other files are added sections.
# Objcopy seems to reverse them, so we'll reverse again.
for i, (file, addr) in reversed(list(enumerate(files[1:]))):
    args.extend([
            '--add-section', '.sec%d=%s' % (i, file),
            '--change-section-address', '.sec%d=0x%x' % (i, addr),
            ])

# Output file
args.append("memory.elf")

# XXX: No quoting, I'm lazy..
os.system(' '.join(args))
