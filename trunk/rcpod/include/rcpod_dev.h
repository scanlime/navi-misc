/*
 * Remote Controlled PIC of Doom userspace interface
 *
 * This header describes the interface between the rcpod kernel module and
 * any userspace applications wishing to use the device.
 *
 * Copyright(c) 2003 Micah Dowty <micah@picogui.org>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#ifndef __RCPOD_DEV_H
#define __RCPOD_DEV_H

#include <asm/types.h>
#include "rcpod_protocol.h"

/************************************************** Device node ***************/

/* Information about the RCPOD's device node in /dev/usb/ */
#define RCPOD_DEV_NAMEFORMAT   "rcpod%d"
#define RCPOD_MINOR_BASE       240


/************************************************** ioctl()s ******************/

/* Address and data structure used for poke operations */
struct rcpod_pair {
	__u16 address;  /* The PIC's RAM address space is 9-bit */
	__u8 data;
};

/* Given an rcpod_pair with valid address and data, pokes that data into the PIC */
#define RCPODIO_POKE         0x3701

/* Given a __u16 with a valid address, retrieves the byte at that address in the PIC and returns it */
#define RCPODIO_PEEK         0x3702

#endif /* __RCPOD_DEV_H */

/* The End */
