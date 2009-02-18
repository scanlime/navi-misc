/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * apm.c - Support for the legacy Advanced Power Management (APM) BIOS
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

#ifndef __APM_H__
#define __APM_H__

#include "types.h"
#include "bios.h"

#define SIGNATURE_APM    0x504d    // "PM"

#define APM_FLAG_16BIT             (1 << 0)
#define APM_FLAG_32BIT             (1 << 1)
#define APM_FLAG_SLOW_CPU_ON_IDLE  (1 << 2)
#define APM_FLAG_DISABLED          (1 << 3)
#define APM_FLAG_DISENGAGED        (1 << 4)

/* APM power states */
#define POWER_ON        0
#define POWER_STANDBY   1
#define POWER_SUSPEND   2
#define POWER_OFF       3

typedef struct {
   Bool         connected;         // Are we successfully connected to APM?
   uint16       version;           // Supported APM version in BCD, 0 if not supported
   uint16       flags;
} APMState;

extern APMState gAPM;

fastcall void APM_Init();
fastcall void APM_Idle();
fastcall Bool APM_SetPowerState(uint16 state);

#endif /* __APM_H_ */
