#
# Startup code for the C-language portion of cookinject.
#
# This code doesn't have any of the restrictions that exploit.s
# has. It is compiled AND linked, it has virtually unlimited
# memory, and it can contain NUL bytes :)
#
# This just clears the .bss section, and branches to main().
#

	.global _start
	.section .boot

_start:
	eor	r1, r1
	ldr	r2, =__bss_start
	ldr	r3, =_end
loop:
	strb	r1, [r2]!
	cmp	r2, r3
	bne	loop

	b main

	.pool
