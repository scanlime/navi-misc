/*
 * boot.S --
 *
 *    This is a tiny but relatively featureful bootloader for
 *    32-bit standalone apps and kernels. It compiles into one
 *    binary that can be used either stand-alone (loaded directly
 *    by the BIOS, from a floppy or USB disk image) or as a GNU
 *    Multiboot image, loaded by GRUB.
 *
 *    This bootloader loads itself and the attached main program
 *    at 1MB, with the available portions of the first megabyte of
 *    RAM set up as stack space by default.
 *
 *    This loader is capable of loading an arbitrarily big binary
 *    image from the boot device into high memory. If you're booting
 *    from a floppy, it can load the whole 1.44MB disk. If you're
 *    booting from USB, it can load any amount of data from the USB
 *    disk.
 *
 *    This loader works by using the BIOS's disk services, so we
 *    should be able to read the whole binary image off of any device
 *    the BIOS knows how to boot from. Since we have only a tiny
 *    amount of buffer space, and we need to store the resulting image
 *    above the 1MB boundary, we have to keep switching back and forth
 *    between real mode and protected mode.
 *
 *    To avoid device-specific CHS addressing madness, we require LBA
 *    mode to boot off of anything other than a 1.44MB floppy or a
 *    Multiboot loader. We try to use the INT 13h AH=42h "Extended Read
 *    Sectors From Drive" command, which uses LBA addressing. If this
 *    doesn't work, we fall back to floppy-disk-style CHS addressing.
 *
 *
 * This file is part of Metalkit, a simple collection of modules for
 * writing software that runs on the bare metal. Get the latest code
 * at http://svn.navi.cx/misc/trunk/metalkit/
 *
 * Copyright (c) 2008 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "boot.h"

/*
 * Constants that affect our early boot memory map.
 */
#define BIOS_START_ADDRESS     0x7C00    // Defined by the BIOS
#define EARLY_STACK_ADDRESS    0x2000    // In low DOS memory
#define SECTORS_AT_A_TIME      18        // Must equal sectors per head
#define SECTOR_SIZE            512
#define DISK_BUFFER            0x2800
#define DISK_BUFFER_SIZE       (SECTORS_AT_A_TIME * SECTOR_SIZE)

/*
 * Post-boot memory map
 */
#define FINAL_START_ADDRESS    0x100000
#define FINAL_STACK_ADDRESS    0x9FFFC   // Top of usable part of first 1MB

#define BIOS_PTR(x)            (x - _start + BIOS_START_ADDRESS)
#define FILE_OFFSET(x)         (x - _start)

        .section .boot

        .global _start

        .extern _end
        .extern _edata
        .extern main

        /*
         * This begins our 16-bit DOS MBR boot sector segment. This
         * sits in the first 512 bytes of our floppy image, and it
         * gets loaded by the BIOS at START_ADDRESS.
         *
         * Until we've loaded the memory image off of disk into
         * its final location, this code is running at a different
         * address than the linker is expecting. Any absolute
         * addresses must be fixed up by the BIOS_PTR() macro.
         */

        .code16
_start:
        ljmp    $0, $BIOS_PTR(bios_main)


        /*
         * gnu_multiboot --
         *
         *    GNU Multiboot header. This can come anywhere in the
         *    first 8192 bytes of the image file.
         */

        .p2align 2
        .code32
gnu_multiboot:

#define MULTIBOOT_MAGIC         0x1BADB002
#define MULTIBOOT_FLAGS         0x00010000

        .long   MULTIBOOT_MAGIC
        .long   MULTIBOOT_FLAGS
        .long   -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
        .long   gnu_multiboot
        .long   _start
        .long   _edata
        .long   _end
        .long   entry32


        /*
         * String table, located in the boot sector.
         */

loading_str:            .string "\r\nMETALKIT - Loading"
disk_error_str:         .string " err!"


        /*
         * wait_for_8042 --
         *
         *    Utility function used by the A20 gate code. Waits for
         *    the keyboard microcontroller's I/O queues to empty. (On
         *    modern hardware this is probably not necessary, but we
         *    do it anyway for good luck.)
         */

        .code16
wait_for_8042:
        inb     $0x64, %al
        test    $0x02, %al
        jnz     wait_for_8042
        ret

        /*
         * bios_main --
         *
         *    Main routine for our BIOS MBR based loader. We set up the
         *    stack, display some welcome text, then load the rest of
         *    the boot image from disk. We have to use real mode to
         *    call the BIOS's floppy driver, then protected mode to
         *    copy each disk block to its final location above the 1MB
         *    barrier.
         */

        .code16
bios_main:

        /*
         * Early init: setup our stack and data segments, make sure
         * interrupts are off.
         */
        cli
        xorw    %ax, %ax
        movw    %ax, %ss
        movw    %ax, %ds
        movw    %ax, %es
        movw    $EARLY_STACK_ADDRESS, %sp

        /*
         * Save parameters that the BIOS gave us via registers.
         */
        mov     %dl, BIOS_PTR(disk_drive)

        /*
         * Switch on the A20 gate, so we can access more than 1MB
         * of memory.
         *
         * The A20 line is part of the keyboard controller, for
         * amusing legacy reasons. Modern chipsets support a "Fast
         * A20" line which requires only a single I/O cycle, but we
         * can't rely on its presence.
         *
         * There are a few different descriptions of the keyboard
         * controller and the A20 line floating around on the 'net.
         * This uses the first sequence described by:
         *
         * http://www.win.tue.nl/~aeb/linux/kbd/A20.html
         */

        call    wait_for_8042
        mov     $0xd1, %al      // Command: Write to output port
        outb    %al, $0x64
        call    wait_for_8042
        mov     $0xdf, %al      // Output port value. Bit 1 is A20.
        outb    %al, $0x60
        call    wait_for_8042

        /*
         * Load our image, starting at the beginning of whatever disk
         * the BIOS told us we booted from. The Disk Address Packet
         * (DAP) has already been initialized statically.
         */

        mov     $BIOS_PTR(loading_str), %si
        call    print_str

        /*
         * Fill our DISK_BUFFER, reading SECTORS_AT_A_TIME sectors.
         *
         * First, try to use LBA addressing. This is required in
         * order to boot off of non-floppy devices, like USB drives.
         */

disk_copy_loop:
        mov     $0x42, %ah
        mov     BIOS_PTR(disk_drive), %dl
        mov     $BIOS_PTR(dap_buffer), %si
        int     $0x13
        jnc     disk_success

        /*
         * If LBA fails, fall back to old fashioned CHS addressing.
         * This works everywhere, but only if we're on a 1.44MB floppy.
         */

        mov     $(0x0200 | SECTORS_AT_A_TIME), %ax
        mov     BIOS_PTR(chs_sector), %cx               // Sector and cylinder
        mov     BIOS_PTR(disk_drive), %dx               // Drive and head
        mov     $DISK_BUFFER, %bx
        int     $0x13
        jnc     disk_success

        /*
         * If both CHS and LBA fail, the error is fatal.
         */

        mov     $BIOS_PTR(disk_error_str), %si
        call    print_str
        cli
        hlt

disk_success:
        mov     $'.', %al
        call    print_char

        /*
         * Enter protected mode, so we can copy this sector to
         * memory above the 1MB boundary.
         */

        cli
        lgdt    BIOS_PTR(bios_gdt_desc)
        movl    %cr0, %eax
        orl     $1, %eax
        movl    %eax, %cr0
        ljmp    $BOOT_CODE_SEGMENT, $BIOS_PTR(copy_enter32)
        .code32
copy_enter32:
        movw    $BOOT_DATA_SEGMENT, %ax
        movw    %ax, %ds
        movw    %ax, %es

        /*
         * Copy the buffer to high memory.
         */

        mov     $DISK_BUFFER, %esi
        mov     BIOS_PTR(dest_address), %edi
        mov     $(DISK_BUFFER_SIZE / 4), %ecx
        rep movsl

        /*
         * Next...
         *
         * Even though the CHS and LBA addresses are mutually exclusive,
         * there's no harm in incrementing them both. The LBA increment
         * is pretty straightforward, but CHS is of course less so.
         * We only support CHS on 1.44MB floppies. We always copy one
         * head at a time (SECTORS_AT_A_TIME must equal 18), so we have
         * to hop between disk head 0 and 1, and increment the cylinder
         * on every other head.
         *
         * When we're done copying, branch to entry32 while we're
         * still in protected mode. Also note that we do a long branch
         * to its final address, not it's temporary BIOS_PTR() address.
         */

        addl    $DISK_BUFFER_SIZE, BIOS_PTR(dest_address)
        addl    $SECTORS_AT_A_TIME, BIOS_PTR(dap_sector)

        xorb    $1, BIOS_PTR(chs_head)
        jnz     same_cylinder
        incb    BIOS_PTR(chs_cylinder)
same_cylinder:

        cmpl    $_edata, BIOS_PTR(dest_address)
        jl      not_done_copying
        ljmp    $BOOT_CODE_SEGMENT, $entry32
not_done_copying:

        /*
         * Back to 16-bit mode for the next copy.
         *
         * To understand this code, it's important to know the difference
         * between how segment registers are treated in protected-mode and
         * in real-mode. Loading a segment register in PM is actually a
         * request for the processor to fill the hidden portion of that
         * segment register with data from the GDT. When we switch to
         * real-mode, the segment registers change meaning (now they're
         * paragraph offsets again) but that hidden portion of the
         * register remains set.
         */

        /* 1. Load all protected-mode segment registers */

        movw    $BOOT_DATA16_SEGMENT, %ax
        movw    %ax, %ds
        movw    %ax, %es
        movw    %ax, %ss
        ljmp    $BOOT_CODE16_SEGMENT, $BIOS_PTR(copy_enter16)

        /* (We're entering a 16-bit code segment now) */
        .code16
copy_enter16:

        /* 2. Disable protected mode */

        movl    %cr0, %eax
        andl    $(~1), %eax
        movl    %eax, %cr0

        /*
         * 3. Load all real-mode segment registers.
         *    The %cs load occurs when we do a long
         *    branch back to disk_copy_loop.
         */

        xorw    %ax, %ax
        movw    %ax, %ss
        movw    %ax, %ds
        movw    %ax, %es
        movw    $EARLY_STACK_ADDRESS, %sp
        ljmp    $0, $BIOS_PTR(disk_copy_loop)


        /*
         * print_char --
         *
         *    Use the BIOS's TTY emulation to output one character, from %al.
         */

        .code16
print_char:
        mov     $0x0E, %ah
        mov     $0x0001, %bx
        int     $0x10
        ret

        /*
         * print_str --
         *
         *    Print a NUL-terminated string, starting at %si.
         */

        .code16
print_str:
        lodsb
        test    %al, %al
        jz      print_str_done
        call    print_char
        jmp     print_str
print_str_done:
        ret


        /*
         * entry32 --
         *
         *    Main 32-bit entry point. To be here, we require that:
         *
         *      - We're running in protected mode
         *      - The A20 gate is enabled
         *      - The entire image is loaded at FINAL_START_ADDRESS
         *
         *    We jump directly here from GNU Multiboot loaders (like
         *    GRUB), and this is where we jump directly from our
         *    protected mode disk block copy routine after we've copied
         *    the lask block.
         *
         *    We still need to set up our final stack and GDT.
         */

        .code32
entry32:

        cli

        lgdt    final_gdt_desc
        movl    %cr0, %eax
        orl     $1, %eax
        movl    %eax, %cr0
        ljmp    $BOOT_CODE_SEGMENT, $entry32_gdt_done
entry32_gdt_done:

        movw    $BOOT_DATA_SEGMENT, %ax
        movw    %ax, %ds
        movw    %ax, %ss
        movw    %ax, %es
        movw    %ax, %fs
        movw    %ax, %gs
        mov     $FINAL_STACK_ADDRESS, %esp

        /*
         * Zero out the BSS segment.
         */

        xor     %eax, %eax
        mov     $_end, %ecx
        sub     $_edata, %ecx
        mov     $_edata, %edi
        rep stosb

        /*
         * Call main().
         *
         * If it returns, put the machine in a halt loop. We don't
         * disable interrupts: if the main program is in fact done
         * with, but the application is still doing useful work in its
         * interrupt handlers, no reason to stop them.
         */

        call    main
halt_loop:
        hlt
        jmp     halt_loop

        /*
         * gdt --
         *
         *    This is a Global Descriptor Table that gives us a
         *    code and data segment, with a flat memory model.
         *
         *    See section 3.4.5 of the Intel IA32 software developer's manual.
         */

        .code32
        .p2align 3
gdt:
        .word   0x0000, 0x0000                  // Unused
        .byte   0x00, 0x00, 0x00, 0x00

        .word   0xFFFF, 0x0000                  // 32-bit Code segment
        .byte   0x00, 0x9A, 0xCF, 0x00

        .word   0xFFFF, 0x0000                  // 32-bit Data segment
        .byte   0x00, 0x92, 0xCF, 0x00

        .word   0xFFFF, 0x0000                  // 16-bit Code segment
        .byte   0x00, 0x9A, 0x00, 0x00

        .word   0xFFFF, 0x0000                  // 16-bit Data segment
        .byte   0x00, 0x92, 0x00, 0x00
gdt_end:

bios_gdt_desc:                                  // Uses BIOS_PTR
        .word   (gdt_end - gdt - 1)
        .long   BIOS_PTR(gdt)

final_gdt_desc:                                 // Uses final address
        .word   (gdt_end - gdt - 1)
        .long   gdt


        /*
         * Partition table and Boot Signature --
         *
         *    This must be at the end of the first 512-byte disk
         *    sector. The partition table marks the end of the
         *    portion of this binary which is loaded by the BIOS.
         *
         *    Each partition record is 16 bytes. We reuse partition 4
         *    as a 16-byte Disk Address Packet buffer. (On disk, this
         *    looks like yet another unused partition entry. At runtime,
         *    this is the memory we use to store the DAP.) We reuse
         *    partition 3 to hold our default CHS addressing values.
         *
         *    Note that the DAP is only used in LBA mode, not CHS mode.
         *
         * References:
         *
         *    http://en.wikipedia.org/wiki/Master_boot_record
         *
         *    http://en.wikipedia.org/wiki/INT_13
         *        #INT_13h_AH.3D42h:_Extended_Read_Sectors_From_Drive
         */

        .org    0x1BE           // Partition 1
        .org    0x1CE           // Partition 2

        .org    0x1DE           // Partition 3

chs_sector:                             // Order matters. Cylinder/sector and head/drive
        .byte   0x01                    //   are packed into words together.
chs_cylinder:
        .byte   0x00
disk_drive:
        .byte   0x00
chs_head:
        .byte   0x00
dest_address:
        .long   FINAL_START_ADDRESS     // Initial dest address for 16-to-32-bit copy.

        .org    0x1EE           // Partition 4
dap_buffer:
        .byte   0x10                    // DAP structure size
        .byte   0x00                    // (Unused)
        .byte   SECTORS_AT_A_TIME       // Number of sectors to read
        .byte   0x00                    // (Unused)
        .word   DISK_BUFFER             // Buffer offset
        .word   0x00                    // Buffer segment
dap_sector:
        .long   0x00000000              // Disk sector number
        .long   0x00000000

        .org    0x1FE           // Boot signature
        .byte   0x55, 0xAA      //   This marks the end of the 512-byte MBR.
