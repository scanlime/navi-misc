/*
 * core-dump.c -- DSi memory patch which writes an ELF core dump with
 *                the register and memory state at the point where the
 *                patch gains control.
 *
 *   The core is a standard ELF32 core file, but there is no standard
 *   format for the PT_NOTE data which describes the CPU state at the
 *   time we dumped core. I'm using the Linux PT_NOTE format, so you
 *   must use a Linux version of gdb (arm-eabi-linux) to load these
 *   cores.
 *
 *   Modify the section name below and/or the linker script in order
 *   to insert the patch at different addresses.
 *
 *   TODO:
 *     - Finish implementing PT_NOTE, with saved register state.
 *     - Dump both arm7 and arm9 state at once.
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define HOOK_SECTION_NAME  ".text.launcher_arm7_entry"
#define CORE_FILENAME      "core.arm7"
#define MAX_SEGMENTS       128
#define BLOCK_SIZE         (64 * 1024)
#define ADDR_BEGIN         0x00000000
#define ADDR_END           0x10000000

#include <elf.h>
#include <stdint.h>
#include <stdbool.h>
#include "iohook.h"
#include "memops.h"

/*
 * Assembly language hook, plus a struct describing the layout of the
 * stack frame it creates.
 */

typedef struct {
   uint32_t regs_0_12[13];   // Regs before sp
   uint32_t regs_14_15[2];   // Regs after sp
} hook_stackframe;

asm(".pushsection " HOOK_SECTION_NAME "\n"
    "  push {r0-r12,r14-r15} \n"    // Save all registers except SP
    "  mov r0, r13 \n"              // Pass SP as argument to main()
    "  b main \n"
    ".popsection \n");

/*
 * Templates for parts of the ELF file.
 */

static const Elf32_Ehdr ehdr_template = {
   .e_ident[EI_MAG0] = ELFMAG0,
   .e_ident[EI_MAG1] = ELFMAG1,
   .e_ident[EI_MAG2] = ELFMAG2,
   .e_ident[EI_MAG3] = ELFMAG3,
   .e_ident[EI_CLASS] = ELFCLASS32,
   .e_ident[EI_DATA] = ELFDATA2LSB,
   .e_ident[EI_VERSION] = EV_CURRENT,
   .e_ident[EI_OSABI] = ELFOSABI_LINUX,
   .e_type = ET_CORE,
   .e_machine = EM_ARM,
   .e_version = EV_CURRENT,
   .e_phoff = sizeof(Elf32_Ehdr),
   .e_flags = EF_ARM_EABI_UNKNOWN,
   .e_ehsize = sizeof(Elf32_Ehdr),
   .e_phentsize = sizeof(Elf32_Phdr),
   .e_shentsize = sizeof(Elf32_Shdr),
   .e_phnum = 1,   // PT_NOTE segment
};

static const Elf32_Phdr phdr_note_template = {
   .p_type = PT_NOTE,
   .p_offset = (sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) * MAX_SEGMENTS + 4095) & ~4095,
};

static const Elf32_Phdr phdr_seg_template = {
   .p_type = PT_LOAD,
   .p_flags = PF_R | PF_W | PF_X,
};

/*
 * Working copy of the ELF headers in read/write memory.  We also use
 * the headers to keep track of what data needs to be dumped.
 */

static Elf32_Ehdr ehdr;
static Elf32_Phdr phdr[MAX_SEGMENTS];

/*
 * First pass: Scan memory for segments worth dumping.
 */

static bool
block_is_blank(uint32_t *addr)
{
   uint32_t result;
   uint32_t addrLimit = BLOCK_SIZE + (uint32_t)addr;

   // Inline assembly to check 32 bytes at a time
   asm ("0: \n"
        "cmp %1, %2 \n"       // Check upper bound
        "bge 1f \n"
        "ldmia %1!, {r2-r8,r12} \n"
        "orr r2, r2, r3 \n"   // Tree level 3
        "orr r4, r4, r5 \n"
        "orr r6, r6, r7 \n"
        "orr r8, r8, r12 \n"
        "orr r6, r6, r8 \n"   // Tree level 2
        "orr r2, r2, r4 \n"
        "orrs r2, r2, r6 \n"  // Tree level 1
        "beq 0b \n"           // All zero? next block.
        "mov %0, #0\n"        // Nope, found a nonzero word. Exit.
        "b 2f \n"
        "1: \n"
        "mov %0, #1\n"        // Done iterating, all zero.
        "2: \n"
        : "=r" (result),
          "+r" (addr)
        : "r" (addrLimit)
        : "memory", "r1", "r2", "r3", "r4", "r5",
          "r6", "r7", "r8", "r12");

   return result;
}


static inline void
log_segment(Elf32_Phdr *seg)
{
   IOHook_LogHex(seg, 7*4);
}


static void
scan_memory(void)
{
   Elf32_Phdr *segPrev;
   Elf32_Phdr *segCurrent;
   uint32_t *addr;
   uint32_t blockBlank = 1;

   IOHook_LogStr("Scanning for segments:");

   for (addr = (uint32_t*)ADDR_BEGIN;
        addr < (uint32_t*)ADDR_END;
        addr += BLOCK_SIZE / sizeof *addr) {

      blockBlank = ((blockBlank << 1) | block_is_blank(addr)) & 3;

      switch (blockBlank) {

      case 1:    // Segment ended
         log_segment(segCurrent);
         break;

      case 3:    // No segment
         break;

      case 2:    // Start a new segment
         segPrev = &phdr[ehdr.e_phnum - 1];
         ehdr.e_phnum++;
         if (ehdr.e_phnum > MAX_SEGMENTS) {
            IOHook_Quit("Error, too many segments!");
         }
         segCurrent = segPrev + 1;
         memcpy32(segCurrent, &phdr_seg_template, sizeof *segCurrent);
         segCurrent->p_offset = segPrev->p_offset + segPrev->p_filesz;
         segCurrent->p_paddr = (uint32_t)addr;
         segCurrent->p_vaddr = (uint32_t)addr;
         // Fall through..

      case 0:    // Continue growing current segment
         segCurrent->p_memsz = segCurrent->p_filesz += BLOCK_SIZE;
         break;
      }
   }
}


/*
 * Write out the core file data.
 */

static void
write_core(void)
{
   int i;

   IOHook_FOpenW(CORE_FILENAME);
   IOHook_LogStr("Writing headers");
   IOHook_FWrite(&ehdr, sizeof ehdr);
   IOHook_FWrite(phdr, sizeof phdr[0] * ehdr.e_phnum);
   IOHook_LogStr("Writing segment data...");

   for (i = 0; i < ehdr.e_phnum; i++) {
      Elf32_Phdr *seg = &phdr[i];
      log_segment(seg);
      IOHook_FSeek(seg->p_offset);
      IOHook_FWrite((void*)seg->p_vaddr, seg->p_memsz);
   }

   IOHook_Quit("Done!");
}


/*
 * Main program
 */

void
main(uint32_t *sp)
{
   IOHook_Init();

   memcpy32(&ehdr, &ehdr_template, sizeof ehdr);

   // PT_NOTE segment
   memcpy32(&phdr[0], &phdr_note_template, sizeof phdr[0]);

   scan_memory();
   write_core();
}
