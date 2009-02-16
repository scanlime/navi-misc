/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * apm.c - Support for the legacy Advanced Power Management (APM) BIOS
 *
 * This file is part of Metalkit, a simple collection of modules for
 * writing software that runs on the bare metal. Get the latest code
 * at http://svn.navi.cx/misc/trunk/metalkit/
 *
 * Copyright (c) 2009 Micah Dowty
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

#include "apm.h"
#include "bios.h"
#include "intr.h"

APMState gAPM;

/*
 * APM_Init --
 *
 *    Probe for APM support. If APM is available, this connects to APM.
 *
 *    It would be easier to use the 16-bit real mode interface via
 *    Metalkit's BIOS module, but that wouldn't work very well for us
 *    because we can't handle interrupts during a real-mode BIOS call.
 *    So, any APM_Idle() call would hang!
 *
 *    Instead, we need to use 16-bit BIOS calls to bootstrap APM, then
 *    we do our real work via the 32-bit APM interface that is present
 *    in all APM 1.2 BIOSes.
 *
 *    On exit, gAPM will have a valid 'connected' flag, APM version,
 *    and flags.
 */

fastcall void
APM_Init()
{
   APMState *self = &gAPM;
   Regs reg = {};

   /* Real mode "APM Installation Check" call */
   reg.ax = 0x5300;
   reg.bx = 0x0000;
   BIOS_Call(0x15, &reg);
   if (reg.bx == SIGNATURE_APM && reg.cf == 0) {
      self->version = reg.ax;
      self->flags = reg.cx;
   } else {
      return;
   }

   /* Protected mode 32-bit interface connect */
   reg.ax = 0x5303;
   reg.bx = 0x0000;
   BIOS_Call(0x15, &reg);
   if (reg.cf == 0) {
      self->connected = TRUE;
   }
}


/*
 * APM_Idle --
 *
 *    If we're connected to APM, issue a "CPU Idle" call. The BIOS may
 *    halt the CPU until the next interrupt and/or slow or stop the
 *    CPU clock.
 *
 *    If we aren't connected to APM or the APM call is unsuccessful,
 *    this issue a CPU HLT instruction.
 */

fastcall void
APM_Idle()
{
   /*
    * XXX: This doesn't actually work, because BIOS_Call disables
    *      interrupts!  To get idle calls working, we'll need to use
    *      the real 32-bit APM interface.
    */
#if 0

   APMState *self = &gAPM;

   if (self->connected) {
      Regs reg = {};

      /* Real mode "CPU Idle" call */
      reg.ax = 0x5305;
      BIOS_Call(0x15, &reg);
      if (reg.cf == 0) {
         /* Success */
         return;
      }
   }

#endif

   /* Fall back to CPU HLT */
   Intr_Halt();
}


/*
 * APM_SetPowerState --
 *
 *    Set the power state of all APM-managed devices.
 *    If we aren't connected to APM, has no effect.
 */

fastcall void
APM_SetPowerState(uint16 state)
{
   APMState *self = &gAPM;

   if (self->connected) {
      Regs reg = {};

      reg.ax = 0x5307;   // APM Set Power State
      reg.bx = 0x0001;   // All devices
      reg.cx = state;
      BIOS_Call(0x15, &reg);
   }
}
