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

# Sort files by address
files.sort()

objcopy = 'arm-eabi-objcopy -S -O elf32-littlearm'

# Make an empty ELF file by adding a dummy Binary section, then removing it.
os.system('%s -B arm -I binary %s memory.elf --remove-section .data'
          % (objcopy, files[0][0]))

# Add sections one at a time- objcopy chokes on long lists of added sections.

for i, (file, addr) in enumerate(files):
    os.system(('%s -I elf32-littlearm memory.elf memory.elf ' +
               '--add-section .data.%d=%s --change-section-address .data.%d=0x%x ' +
               '--set-section-flags .data.%d=CONTENTS,ALLOC,LOAD,DATA')
              % (objcopy, i, file, i, addr, i))
