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
 * Copyright (c) 2008-2009 Micah Dowty
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

#define ASM
#include "boot.h"

/*
 * Constants that affect our early boot memory map.
 */
#define BIOS_START_ADDRESS     0x7C00    // Defined by the BIOS
#define EARLY_STACK_ADDRESS    0x2000    // In low DOS memory
#define SECTORS_AT_A_TIME      18        // Must equal CHS sectors per head
#define SECTOR_SIZE            512
#define DISK_BUFFER            0x2800
#define DISK_BUFFER_SIZE       (SECTORS_AT_A_TIME * SECTOR_SIZE)

#define BIOS_PTR(x)            (x - _start + BIOS_START_ADDRESS)

        .section .boot

        .global _start

        /*
         * External symbols. main() is self-explanatory, but these
         * other symbols must be provided by the linker script. See
         * "image.ld" for the actual partition size and LDT calculations.
         */
        .extern main
        .extern _end
        .extern _edata
        .extern _bss_size
        .extern _stack
        .extern _partition_chs_head
        .extern _partition_chs_sector_byte
        .extern _partition_chs_cylinder_byte
        .extern _partition_blocks
        .extern _ldt_byte0
        .extern _ldt_byte1
        .extern _ldt_byte2
        .extern _ldt_byte3

        /*
         * Other modules can optionally define an LDT in uninitialized
         * memory.  By default this LDT will be all zeroes, but this
         * is a simple and code-size-efficient way of letting other
         * Metalkit modules allocate segment descriptors when they
         * need to.
         *
         * Note that we page-align the LDT. This isn't strictly
         * necessary, but it might be useful for performance in
         * some environments.
         */
        .comm   LDT, BOOT_LDT_SIZE, 4096
	.comm	LDTAllocNext, 4, 4

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

loading_str:            .string "\r\nMETALKIT "
disk_error_str:         .string " err!"

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
         * of memory. There are multiple ways to do this: The
         * original way was to write to bit 1 of the keyboard
         * controller's output port. There's also a bit on PS2
         * System Control port A to enable A20.
         *
         * The keyboard controller method should always work, but
         * it's kind of slow and it takes a lot of code space in
         * our already-cramped bootloader. Instead, we ask the BIOS
         * to enable A20.
         *
         * If your computer doesn't support this BIOS interface,
         * you'll see our "err!" message before "METAL" appears.
         *
         * References:
         *    http://www.win.tue.nl/~aeb/linux/kbd/A20.html
         */

        mov     $0x2401, %ax    // Enable A20
        int     $0x15
        jc      fatal_error

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

fatal_error:
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
         *
         * Note that we reset CS, DS, and ES, but we don't
         * modify the stack at all.
         */

        cli
        lgdt    BIOS_PTR(bios_gdt_desc)
        movl    %cr0, %eax
        orl     $1, %eax
        movl    %eax, %cr0
        ljmp    $BOOT_CODE_SEG, $BIOS_PTR(copy_enter32)
        .code32
copy_enter32:
        movw    $BOOT_DATA_SEG, %ax
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
        ljmp    $BOOT_CODE_SEG, $entry32
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

        /* 1. Load protected-mode segment registers (CS, DS, ES) */

        movw    $BOOT_DATA16_SEG, %ax
        movw    %ax, %ds
        movw    %ax, %es
        ljmp    $BOOT_CODE16_SEG, $BIOS_PTR(copy_enter16)

        /* (We're entering a 16-bit code segment now) */
        .code16
copy_enter16:

        /* 2. Disable protected mode */

        movl    %cr0, %eax
        andl    $(~1), %eax
        movl    %eax, %cr0

        /*
         * 3. Load real-mode segment registers. (CS, DS, ES)
         */

        xorw    %ax, %ax
        movw    %ax, %ds
        movw    %ax, %es
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
ret_label:
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
        jz      ret_label
        call    print_char
        jmp     print_str


        /*
         * entry32 --
         *
         *    Main 32-bit entry point. To be here, we require that:
         *
         *      - We're running in protected mode
         *      - The A20 gate is enabled
         *      - The entire image is loaded at _start
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

        lgdt    boot_gdt_desc
        movl    %cr0, %eax
        orl     $1, %eax
        movl    %eax, %cr0
        ljmp    $BOOT_CODE_SEG, $entry32_gdt_done
entry32_gdt_done:

        movw    $BOOT_DATA_SEG, %ax
        movw    %ax, %ds
        movw    %ax, %ss
        movw    %ax, %es
        movw    %ax, %fs
        movw    %ax, %gs
        mov     $_stack, %esp

        /*
         * Zero out the BSS segment.
         */

        xor     %eax, %eax
        mov     $_bss_size, %ecx
        mov     $_edata, %edi
        rep stosb

        /*
         * Set our LDT segment as the current LDT.
         */
        mov     $BOOT_LDT_SEG, %ax
        lldt    %ax

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
         * boot_gdt --
         *
         *    This is a Global Descriptor Table that gives us a
         *    code and data segment, with a flat memory model.
         *
         *    See section 3.4.5 of the Intel IA32 software developer's manual.
         */

        .code32
        .p2align 3
boot_gdt:

        /*
         * This is BOOT_NULL_SEG, the unusable segment zero.
         * Reuse this memory as bios_gdt_desc, a GDT descriptor
         * which uses our pre-relocation (BIOS_PTR) GDT address.
         */
bios_gdt_desc:
        .word   (boot_gdt_end - boot_gdt - 1)
        .long   BIOS_PTR(boot_gdt)
        .word   0  // Unused

        .word   0xFFFF, 0x0000                  // BOOT_CODE_SEG
        .byte   0x00, 0x9A, 0xCF, 0x00

        .word   0xFFFF, 0x0000                  // BOOT_DATA_SEG
        .byte   0x00, 0x92, 0xCF, 0x00

        .word   0xFFFF, 0x0000                  // BOOT_CODE16_SEG
        .byte   0x00, 0x9A, 0x00, 0x00

        .word   0xFFFF, 0x0000                  // BOOT_DATA16_SEG
        .byte   0x00, 0x92, 0x00, 0x00

        .word   0xFFFF                          // BOOT_LDT_SEG
        .byte   _ldt_byte0
        .byte   _ldt_byte1
        .byte   _ldt_byte2
        .byte   0x82, 0x40
        .byte   _ldt_byte3
boot_gdt_end:

boot_gdt_desc:                                  // Uses final address
        .word   (boot_gdt_end - boot_gdt - 1)
        .long   boot_gdt


        /*
         * dap_buffer --
         *
         *    The Disk Address Packet buffer holds the current LBA
         *    disk address. We pass this to BIOS INT 13h, and we
         *    statically initialize it here.
         *
         *    Note that the DAP is only used in LBA mode, not CHS mode.
         *
         * References:
         *    http://en.wikipedia.org/wiki/INT_13
         *        #INT_13h_AH.3D42h:_Extended_Read_Sectors_From_Drive
         */

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

        /*
         * Statically initialized disk addressing variables.  The CHS
         * address here is only used in CHS mode, not LBA mode, but
         * the disk drive number and dest address are always used.
         */
chs_sector:                             // Order matters. Cylinder/sector and head/drive
        .byte   0x01                    //   are packed into words together.
chs_cylinder:
        .byte   0x00
disk_drive:
        .byte   0x00
chs_head:
        .byte   0x00
dest_address:
        .long   _start                  // Initial dest address for 16-to-32-bit copy.


        /*
         * Partition table and Boot Signature --
         *
         *    This must be at the end of the first 512-byte disk
         *    sector. The partition table marks the end of the
         *    portion of this binary which is loaded by the BIOS.
         *
         *    Each partition record is 16 bytes.
         *
         *    After installing Metalkit, a disk can be partitioned as
         *    long as the space used by the Metalkit binary itself is
         *    reserved. By default, we create a single "Non-FS data"
         *    partition which holds the Metalkit binary. Note that
         *    this default partition starts at sector 1 (the first
         *    sector) so it covers the entire Metalkit image including
         *    bootloader.
         *
         *    Partitions 2 through 4 are unused, and must be all zero
         *    or fdisk will complain.
         *
         * References:
         *    http://en.wikipedia.org/wiki/Master_boot_record
         */

        .org    0x1BE           // Partition 1
boot_partition_table:
        .byte   0x80                     // Status (Bootable)
        .byte   0x00                     // First block (head, sector/cylinder, cylinder)
        .byte   0x01
        .byte   0x00
        .byte   0xda                     // Partition type ("Non-FS data" in fdisk)
        .byte   _partition_chs_head      // Last block (head, sector/cylinder, cylinder)
        .byte   _partition_chs_sector_byte
        .byte   _partition_chs_cylinder_byte
        .long   0                        // LBA of first sector
        .long   _partition_blocks        // Number of blocks in partition

        .org    0x1CE           // Partition 2 (Unused)
        .org    0x1DE           // Partition 3 (Unused)
        .org    0x1EE           // Partition 4 (Unused)
        .org    0x1FE           // Boot signature
        .byte   0x55, 0xAA      //   This marks the end of the 512-byte MBR.
