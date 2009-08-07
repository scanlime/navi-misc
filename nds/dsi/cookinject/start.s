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
_memclr:
	cmp	r2, r3
	beq	_memclrDone
	str	r1, [r2], #4
	b	_memclr
_memclrDone:

	b main

	.pool

	#################################################
	# Interrupt Trampoline

	.global isr_trampoline
	.global isr_original

isr_trampoline:
	push	{r0-r7, lr}
	bl	isr_hook
	pop	{r0-r7, lr}
	ldr	pc, isr_original

isr_original:
	.long   0


	#################################################
	# Fifo TX trampoline

	.global fifo_tx_trampoline

	# This 8-byte trampoline is copied over the original code
	.thumb
fifo_tx_trampoline:
	ldr	r2, =fifo_tx_trampoline_2
	bx	r2
	.pool

	# And it branches here:

	.arm
fifo_tx_trampoline_2:
	ldr	r1, [sp]	@ Load FIFO TX value

	push	{r0, r2-r7, lr}	@ Invoke hook
	mov	r0, r1
	bl	fifo_tx_hook
	mov	r1, r0
	pop 	{r0, r2-r7, lr}

	str	r1, [r3, #4]	@ Store to TX register

	mrs	r1, cpsr	@ Begin irq_restore
	bic	r2, r1, #0x80
	orr	r2, r2, r0
	msr	CPSR_c, r2
	and	r0, r1, #0x80	@ End irq_restore

	movs	r0, #0
	add	sp, sp, #4
	pop	{r3, r4, pc}
	

	#################################################
	# Main loop trampoline

	.global	mainloop_trampoline
	.thumb
mainloop_trampoline:
	ldr	r0, =mainloop_trampoline_2
	bx	r0
	.pool

	.arm
mainloop_trampoline_2:

	b mainloop_trampoline_2
	
	push	{r0-r7, lr}	@ Invoke hook
	bl	mainloop_hook
	pop 	{r2-r7, lr}

	# This hook overwrites four instructions, but three
	# of them are no-ops, since they're calling a stub
	# function. This is the one potentially meaningful
	# instruction.

	ldr	r0, =0x217b820

	# Jump back

	ldr	pc, =0x2006570
	.pool
