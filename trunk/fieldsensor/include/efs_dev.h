/*
 * USB Electric Field Sensor userspace interface
 *
 * This header describes the interface between the fieldsensor kernel module and
 * any userspace applications wishing to use the device.
 *
 * Copyright(c) 2003 Micah Dowty <micah@picogui.org>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#ifndef __EFS_DEV_H
#define __EFS_DEV_H

#include <asm/types.h>
#include "efs_protocol.h"


/************************************************** Device node ***************/

/* Information about the efs's device node in /dev */
#define EFS_DEV_NAMEFORMAT   "usb/efs%d"
#define EFS_MINOR_BASE       215

/* Each parmaeter block is a set of sensor parameters stored in the board.
 * Each parameter block is sampled, and generates one byte in the resulting
 * packets. These bytes are then accumulated until read by userspace code.
 */
#define EFS_NUM_PARAM_BLOCKS 8

/* These are returned by read() */
struct efs_results {
  int accumulators[EFS_NUM_PARAM_BLOCKS];
  int num_samples;
};

#endif /* __EFS_DEV_H */

/* The End */
