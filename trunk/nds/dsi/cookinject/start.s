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
	# ARM7 trampoline

	.global	arm7_trampoline
	.arm
arm7_trampoline:
	ldr	pc, =arm7_trampoline_2
	.pool

arm7_trampoline_2:
	push	{r0-r7, lr}	@ Invoke hook
	bl	arm7_hook
	pop 	{r0-r7, lr}

	push	{r4, lr}	@ Back to hooked function
	cmp	r2, #0
	ldr	pc, =0x2fe0378
	.pool
