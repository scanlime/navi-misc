/*
 * context.S -- Implements Intr_SaveContext and Intr_RestoreContext.
 *              These two functions are primitives which can be
 *              used like setjmp/longjmp, or they can be used for
 *              simple multithreading.
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

	/*
	 * Intr_SaveContext --
	 *
	 *    This is a C-callable function which constructs an
	 *    IntrContext representing the current execution
	 *    context. This is nearly equivalent to invoking
	 *    software interrupt and saving the interrupt's
	 *    IntrContext, but this implementation doesn't have the
	 *    overhead of an actual interrupt invocation.
	 */
	
	.global	Intr_SaveContext
Intr_SaveContext:
	pusha

	// Adjust the saved stack pointer. IntrContexts always
	// store an %esp which has three words on the stack
	// prior to the general-purpose regs, but since we don't
	// use cs or eflags we only have 1.

	sub	$8, 12(%esp)
	
	// The stack now matches the layout of the first 9 words
	// of IntrContext. Copy these, then manually save CS and
	// eflags.

	mov	%esp, %esi
	mov	36(%esp), %edi
	mov	$9, %ecx
	rep movsl
	xor	%eax, %eax
	mov	%cs, %ax
	stosl
	pushf
	pop	%eax
	stosl
	
	// Return 0 when this function is called directly.
	popa
	xor	%eax, %eax
	ret


	/*
	 * Intr_RestoreContext --
	 *
	 *    This is the inverse of Intr_SaveContext: copy the
	 *    IntrContext onto the target context's stack frame,
	 *    switch stacks, then restore the rest of the context's
	 *    saved state.
	 */
	
	.global	Intr_RestoreContext
Intr_RestoreContext:
	mov	4(%esp), %esi	// Load pointer to IntrContext
	mov	12(%esi), %esp  // Switch stacks

	// esp was saved with 3 words on the stack (eip, cs, cflags).
	// Position esp so we have 9 words instead. (General purpose
	// regs plus eip, but no cs/eflags.)

	sub	$24, %esp
	
	// Copy the first 9 words of Intrcontext back onto the stack.

	mov	%esp, %edi
	mov	$9, %ecx
	rep movsl

	// Restore the general purpose regs and eip
	popa
	ret
