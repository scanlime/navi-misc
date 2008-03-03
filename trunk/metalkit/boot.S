	/*
	 * boot.S -- Simple IA32 floppy/multiboot protected mode loader.
	 *           This is a tiny bootloader that can be used as either a
	 *           BIOS MBR (use the resulting binary as a raw floppy
	 *           disk image) or a GNU Multiboot image (for GRUB).
	 *
	 *           The image is loaded contiguously into memory, we
	 *           switch to protected mode with identity-mapped code
	 *           and data segments, then we call main().
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
	
	/*
	 * This begins our 16-bit DOS MBR boot sector segment. This sits in
	 * the first 512 bytes of our floppy image, and it gets loaded
	 * by the BIOS at START_ADDRESS.
	 *
	 * Our image is designed to load at START_ADDRESS, below 1MB.
	 * This is quite convenient for the MBR bootloader, but GRUB
	 * can't load images that low. For GRUB, we load at 1MB and
	 * relocate to START_ADDRESS immediately after we get control.
	 */

#define START_ADDRESS    0x7C00
#define MULTIBOOT_START  0x100000	// 1MB, minimum GRUB start address
#define STACK_ADDRESS 	 0x2000
#define CODE_SEG_32      0x08 
#define DATA_SEG_32      0x10 

#define FLAT_PTR(x)     (x - _start + START_ADDRESS)
#define FILE_OFFSET(x)	(x - _start)

	.code16
	.global	_start
	.extern _end
	.extern _edata

_start:
	ljmp	$0, $FLAT_PTR(stage1_main)

	/*
	 * GNU Multiboot header. This can come anywhere in the
	 * first 8192 bytes of the image file, but we might as
	 * well put it at the beginning since our boot sector
	 * isn't hurting for space.
	 */
	
	.p2align 2
	.code32
gnu_multiboot:

#define	MULTIBOOT_MAGIC		0x1BADB002
#define MULTIBOOT_FLAGS		0x00010000
#define MULTIBOOT_ADDR(sym)	(sym + MULTIBOOT_START - START_ADDRESS)
	
	.long	MULTIBOOT_MAGIC
	.long	MULTIBOOT_FLAGS
	.long	-(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
	.long	MULTIBOOT_ADDR(gnu_multiboot)
	.long	MULTIBOOT_ADDR(_start)
	.long	MULTIBOOT_ADDR(_edata)
	.long	MULTIBOOT_ADDR(_end)
	.long	MULTIBOOT_ADDR(multiboot_main)
	
	/*
	 * Store the data segment end address in a well-known
	 * location.  This is inserted at link-time, so we can't
	 * calculate anything at assembly-time with this value. To use
	 * it effectively in 16-bit code, we perform calculations on
	 * it at runtime in order to convert it back to a paragraph
	 * address.
	 */
end_addr32:
	.long	_edata
end_segment:
	.word	0
		
	/*
	 * String table, located in the boot sector.
	 */
	
hello_str:		.string "\n\rHello, Computer. (>\")>\n\n\rLoading disk image..."
done_loading_str:	.string "\n\rDone.\n\r"
disk_error_str:		.string "\n\n\rDisk error!"
pci_error_str:		.string "\n\n\rNo PCI BIOS!"
halting_str:		.string "\n\rHalting.\n\r"
	
	/*
	 * stage1_main --
	 *
	 *    Main routine for the 512-byte boot sector. We set up the
	 *    stack, display some welcome text, then load the rest of
	 *    the boot image from disk. After loading, we jump to
	 *    stage2_main.
	 */

	.code16
stage1_main:

	xorw	%ax, %ax
	movw	%ax, %ss
	movw	%ax, %ds
	movw	$STACK_ADDRESS, %sp

	mov	$hello_str, %si
	call	print_str

	/*
	 * Convert end_addr32 from a 32-bit linear address to a 16-bit
	 * paragraph number.
	 */
	add	$3, end_addr32		// Add 3 (round up to next paragraph)
	adc	$0, end_addr32 + 2
	mov	end_addr32, %ax		// Shift low word right by 4	
	shr	$4, %ax
	mov	(end_addr32 + 2), %bx	// Shift high word left by 12
	shl	$12, %bx
	or	%ax, %bx		// Combine the two
	mov	%bx, end_segment
	
	/*
	 * Load the whole disk image contiguously, using BIOS
	 * Cylinder/Head/Sector addressing.  A 1.44MB floppy disk has
	 * 80 cylinders, 2 heads, and 18 sectors. Note that sector
	 * indices begin at 1, and that the very first (boot) sector is
	 * already loaded. We start at sector 2, and end when we've
	 * passed our image's _edata address.
	 */

	mov	$((START_ADDRESS + 512) >> 4), %bx
	mov	%bx, %es

	xor	%dx, %dx
	xor	%ch, %ch
	mov	$2, %cl			// Start on sector 2

cylinder_loop:	

	xor	%dh, %dh
head_sector_loop:

	xor	%bx, %bx		// Low nybble of dest address	
	mov	$0x0201, %ax		// Command: Read 1 sector
	int	$0x13
	jc	disk_error

	mov	%es, %bx		// Advance the destination address
	add	$(512 >> 4), %bx
	mov	%bx, %es
	cmp	end_segment, %bx	// We're finished when we've copied the _edata address
	jg	done_loading	
	
	inc	%cl			// Next sector (Loop over 1..18)
	cmp	$19, %cl
	jnz	head_sector_loop
	mov	$1, %cl
	
	inc	%dh			// Next head
	cmp	$2, %dh
	jnz	head_sector_loop
		
	inc	%ch			// Next cylinder
	jnz	cylinder_loop
	
done_loading:	
	mov	$done_loading_str, %si
	call	print_str
	jmp	stage2_main

	/*
	 * disk_error --
	 * pci_error --
	 * error_halt --
	 *
	 *    Handlers for fatal errors. Display a message and halt.
	 */
pci_error:
	mov	$pci_error_str, %si
	call	print_str
	jmp	error_halt	
disk_error:	
	mov	$disk_error_str, %si
	call	print_str
error_halt:
	mov	$halting_str, %si
	call	print_str
	cli
	hlt
	
	/*
	 * print_char --
	 *
	 *    Use the BIOS's TTY emulation to output one character, from %al.
	 */
print_char:	
	mov	$0x0E, %ah
	mov	$0x0001, %bx
	int	$0x10
	ret

	/*
	 * print_str --
	 *
	 *    Print a NUL-terminated string, starting at %si.
	 */
print_str:
	lodsb
	test	%al, %al
	jz	print_str_done
	call	print_char
	jmp	print_str
print_str_done:
	ret

	/*
	 * stage2_main --
	 *
	 *    Second-stage initialization, after the disk image has
	 *    been loaded. The operating environment here is
	 *    essentially the same as in stage1_main (16-bit real
	 *    mode) except that code after this point no longer needs
	 *    to fit in 512 bytes.
	 *
	 *    The stage 2 loader's main job is to initialize the CPU in
	 *    protected mode, so that we can jump into our 32-bit C
	 *    code. We follow the steps outlined in section 8.9.1 of
	 *    the Intel IA32 software developer's manual.
	 *
	 *    Before we hit protected mode, we could take this opportunity
	 *    to make real-mode BIOS calls that we won't be able to
	 *    make later.
	 */
stage2_main:

	/*
	 * Set up protected mode, without paging.
	 */
	
	cli
	lgdt	gdt_desc
	movl	%cr0, %eax
	orl	$1, %eax
	movl	%eax, %cr0
	ljmp	$CODE_SEG_32, $FLAT_PTR(code32_main)
	
	/*
	 * Global Descriptor Table: See section 3.4.5 of the Intel
	 * IA32 software developer's manual.
	 *
	 * The code/data segments are both identity-mapped and read/write.
	 */
	
	.code32
	.p2align 3		// Intel recommends 8-byte GDT alignment
gdt:
	.word	0x0000, 0x0000			// Unused
	.byte	0x00, 0x00, 0x00, 0x00

	.word	0xFFFF, 0x0000			// 32-bit Code segment
	.byte	0x00, 0x9A, 0xCF, 0x00

	.word	0xFFFF, 0x0000			// 32-bit Data segment
	.byte	0x00, 0x92, 0xCF, 0x00
	
gdt_desc:
	.word	(gdt_desc - gdt - 1)
	.long	gdt

	/*
	 * multiboot_main --
	 *
	 *    Entry point for GNU Multiboot. We need to relocate ourselves
	 *    from MULTIBOOT_START down to START_ADDRESS, then jump to
	 *    code32_main. The bootloader has already performed all the
	 *    setup we did in stage2_main above, but we'll load our own GDT
	 *    anyway just to make sure we use the same one in both boot modes.
	 */

multiboot_main:

	mov	$(_edata - _start), %ecx
	mov	$MULTIBOOT_START, %esi
	mov	$START_ADDRESS, %edi
	rep movsb
	jmp	multiboot_relocated
multiboot_relocated:

	/*
	 * This is just like in stage2_main above, except that this
	 * version is executing as 32-bit code already. This just serves
	 * to make sure interrupts are off, then to load our real GDT.
	 */
	
	cli
	lgdt	gdt_desc
	movl	%cr0, %eax
	orl	$1, %eax
	movl	%eax, %cr0
	ljmp	$CODE_SEG_32, $FLAT_PTR(code32_main)
	
	/*
	 * code32_main --
	 *
	 *    Protected mode initialization. Now we have 32 whole bits, yay.
	 *    Set up the stack and segment registers, then jump to C code.
	 */

code32_main:
	movw	$DATA_SEG_32, %ax
	movw	%ax, %ds
	movw	%ax, %ss
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	mov	$STACK_ADDRESS, %esp

	/*
	 * Zero out the BSS segment.
	 */

	xor	%eax, %eax
	mov	$_end, %ecx
	sub	$_edata, %ecx
	mov	$_edata, %edi
	rep stosb
	
	/*
	 * Call main(). If it returns, halt the machine.
	 */	

	.extern	main
	call	main
	cli
	hlt

	/*
	 * Partition table and boot signature go at the end of the
	 * boot sector.  These aren't needed by VMware's BIOS
	 * apparently, but they would be needed by most machines
	 * (including Bochs).
	 *
	 * Currently this entire file fits in the boot sector,
	 * but only the code above stage2_main actually needs to.
	 * Feel free to move this up, in order to stick it at the
	 * end of the boot sector without wasting much space on
	 * padding.
	 *
	 * (Run 'objdump -D boot.o', and examine the resulting
	 * memory layout.)
	 */

	.org	0x1BE		// Blank partition table	
	.org	0x1FE		// Boot signature
	.byte	0x55, 0xAA
	
