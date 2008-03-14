/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * intr.c - Interrupt vector management, interrupt routing,
 *          and low-level building blocks for multithreading.
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

#include "intr.h"
#include "boot.h"
#include "io.h"

/*
 * Definitions for the two PIC chips.
 */

#define PIC1_COMMAND_PORT  0x20
#define PIC1_DATA_PORT     0x21
#define PIC2_COMMAND_PORT  0xA0
#define PIC2_DATA_PORT     0xA1


/*
 * IDT table and IDT table descriptor. The table itself lives in the
 * BSS segment, the descriptor lives in the data segment.
 */

static struct {
   uint16 offsetLow;
   uint16 segment;
   uint16 flags;
   uint16 offsetHigh;
} __attribute__ ((__packed__, aligned (8))) IDT[NUM_INTR_VECTORS];

const struct {
   uint16 limit;
   void *address;
} __attribute__ ((__packed__)) IDTDesc = {
   .limit = NUM_INTR_VECTORS * 8 - 1,
   .address = IDT,
};

/*
 * To save space, we don't include assembly-language trampolines for
 * each interrupt vector. Instead, we allocate a table in the BSS
 * segment which we can fill in at runtime with simple trampoline
 * functions. This structure actually describes executable 32-bit
 * code.
 */

static struct {
   uint16      code1;
   uint32      arg;
   uint8       code2;
   IntrHandler handler;
   uint32      code3;
   uint32      code4;
   uint32      code5;
   uint32      code6;
   uint32      code7;
   uint32      code8;
} __attribute__ ((__packed__, aligned (4))) IntrTrampoline[NUM_INTR_VECTORS];


/*
 * IntrDefaultHandler --
 *
 *    Default no-op interrupt handler.
 */

static void
IntrDefaultHandler(int vector)
{
   /* Do nothing. */
}


/*
 * Intr_Init --
 *
 *    Initialize the interrupt descriptor table and the programmable
 *    interrupt controller (PIC). On return, interrupts are enabled
 *    but all handlers are no-ops.
 */

void
Intr_Init(void)
{
   int i;

   Intr_Disable();

   for (i = 0; i < NUM_INTR_VECTORS; i++) {

      uint32 trampolineAddr = (uint32) &IntrTrampoline[i];

      /*
       * Set up the IDT entry as a 32-bit interrupt gate, pointing at
       * our trampoline for this vector.
       */

      IDT[i].offsetLow = (uint16)trampolineAddr;
      IDT[i].segment = BOOT_CODE_SEGMENT;
      IDT[i].flags = 0x8E00;
      IDT[i].offsetHigh = trampolineAddr >> 16;

      /*
       * Set up the trampoline, pointing it at the default handler.
       * The trampoline function wraps our C interrupt handler, and
       * handles placing a vector number onto the stack. It also allows
       * interrupt handlers to switch stacks upon return by writing
       * to the saved 'esp' register.
       *
       * Note that the old stack and new stack may actually be different
       * stack frames on the same stack. We require that the new stack
       * is in a higher or equal stack frame, but the two stacks may
       * overlap. This is why the trampoline does its copy in reverse.
       *
       * Keep the trampoline function consistent with the definition
       * of IntrContext in intr.h.
       *
       * Stack layout:
       *
       *     8   eflags
       *     4   cs
       *     0   eip        <- esp on entry to IRQ handler
       *    -4   eax
       *    -8   ecx
       *   -12   edx
       *   -16   ebx
       *   -20   esp
       *   -24   ebp
       *   -28   esi
       *   -32   edi
       *   -36   <arg>      <- esp on entry to handler function
       *
       * Our trampolines each look like:
       *
       *    60                 pusha                   // Save general-purpose regs
       *    68 <32-bit arg>    push   <arg>            // Call handler(arg)
       *    b8 <32-bit addr>   mov    <addr>, %eax
       *    ff d0              call   *%eax
       *    58                 pop    %eax             // Remove arg from stack
       *    8b 7c 24 0c        mov    12(%esp), %edi   // Load new stack address
       *    8d 74 24 28        lea    40(%esp), %esi   // Addr of eflags on old stack
       *    83 c7 08           add    $8, %edi         // Addr of eflags on new stack
       *    fd                 std                     // Copy backwards
       *    a5                 movsl                   // Copy eflags
       *    a5                 movsl                   // Copy cs
       *    a5                 movsl                   // Copy eip
       *    61                 popa                    // Restore general-purpose regs
       *    8b 64 24 ec        mov    -20(%esp), %esp  // Switch stacks
       *    cf                 iret                    // Restore eip, cs, eflags
       *
       */

      IntrTrampoline[i].code1 = 0x6860;
      IntrTrampoline[i].code2 = 0xb8;
      IntrTrampoline[i].code3 = 0x8b58d0ff;
      IntrTrampoline[i].code4 = 0x8d0c247c;
      IntrTrampoline[i].code5 = 0x83282474;
      IntrTrampoline[i].code6 = 0xa5fd08c7;
      IntrTrampoline[i].code7 = 0x8b61a5a5;
      IntrTrampoline[i].code8 = 0xcfec2464;

      IntrTrampoline[i].handler = IntrDefaultHandler;
      IntrTrampoline[i].arg = i;
   }

   asm volatile ("lidt IDTDesc");

   /*
    * Program the PIT to map all IRQs linearly starting at
    * IRQ_VECTOR_BASE.
    */

   IO_Out8(PIC1_COMMAND_PORT, 0x11);       // Begin init, use 4 command words
   IO_Out8(PIC2_COMMAND_PORT, 0x11);
   IO_Out8(PIC1_DATA_PORT, IRQ_VECTOR_BASE);
   IO_Out8(PIC2_DATA_PORT, IRQ_VECTOR_BASE + 8);
   IO_Out8(PIC1_DATA_PORT, 0x04);
   IO_Out8(PIC2_DATA_PORT, 0x02);
   IO_Out8(PIC1_DATA_PORT, 0x03);          // 8086 mode, auto-end-of-interrupt.
   IO_Out8(PIC2_DATA_PORT, 0x03);

   /*
    * All IRQs start out masked, except for the cascade IRQs 2 and 4.
    */
   IO_Out8(PIC1_DATA_PORT, 0xEB);
   IO_Out8(PIC2_DATA_PORT, 0xFF);

   Intr_Enable();
}


/*
 * Intr_SetHandler --
 *
 *    Set a C-language interrupt handler for a particular vector.
 *    Note that the argument is a vector number, not an IRQ.
 */

void
Intr_SetHandler(int vector, IntrHandler handler)
{
   Intr_Disable();
   IntrTrampoline[vector].handler = handler;
   Intr_Enable();
}


/*
 * Intr_SetMask --
 *
 *    (Un)mask a particular IRQ.
 */

void
Intr_SetMask(int irq, Bool enable)
{
   uint8 port, bit, mask;

   if (irq >= 8) {
      bit = 1 << (irq - 8);
      port = PIC2_DATA_PORT;
   } else {
      bit = 1 << irq;
      port = PIC1_DATA_PORT;
   }

   mask = IO_In8(port);

   /* A '1' bit in the mask inhibits the interrupt. */
   if (enable) {
      mask &= ~bit;
   } else {
      mask |= bit;
   }

   IO_Out8(port, mask);
}


/*
 * Intr_SetFaultHandlers --
 *
 *    Set all processor fault handlers to the provided function.
 */

void
Intr_SetFaultHandlers(IntrHandler handler)
{
   int vector;

   for (vector = 0; vector < NUM_FAULT_VECTORS; vector++) {
      Intr_SetHandler(vector, handler);
   }
}


/*
 * Intr_InitContext --
 *
 *    Create an IntrContext representing a brand new thread of
 *    execution. This can be used as a primitive to implement
 *    light-weight cooperative or pre-emptive multithreading.
 *
 *    'Stack' points to the initial value of the stack pointer.
 *    Stacks grow downward, so this should point to the top word of
 *    the allocated stack memory.
 */

void
Intr_InitContext(IntrContext *ctx, uint32 *stack, IntrContextFn main)
{
   Intr_SaveContext(ctx);
   ctx->esp = (uint32) stack;
   ctx->eip = (uint32) main;
}
