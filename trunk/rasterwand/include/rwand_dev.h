/*
 * Raster Wand userspace interface
 *
 * This header describes the interface between the Raster Wand kernel module and
 * any userspace applications wishing to use the device.
 *
 * Copyright(c) 2003 Micah Dowty <micah@picogui.org>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#ifndef __RWAND_DEV_H
#define __RWAND_DEV_H

#include <asm/types.h>
#include "rwand_protocol.h"


/************************************************** Device node ***************/

/* Information about the rwand's device node in /dev */
#define RWAND_DEV_NAMEFORMAT   "usb/rwand%d"
#define RWAND_MINOR_BASE       216

#endif /* __RWAND_DEV_H */

/* The End */
