/*
 * USB Video Switch userspace interface
 *
 * This header describes the interface between the uvswitch kernel module and
 * any userspace applications wishing to use the device.
 *
 * Copyright(c) 2003 Micah Dowty <micah@picogui.org>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#ifndef __UVSWITCH_DEV_H
#define __UVSWITCH_DEV_H

#include <asm/types.h>
#include "uvswitch_protocol.h"

/************************************************** Device node ***************/

/* Information about the uvswitch's device node in /dev/usb/ */
#define UVSWITCH_DEV_NAMEFORMAT   "uvswitch%d"
#define UVSWITCH_MINOR_BASE       220


/************************************************** ioctl()s ******************/

struct uvswitch_calibration {
	__u8 precharge_reads;
	__u8 integration_reads;
	__u8 threshold;
};

/* Send a uvswitch_calibration structure */
#define UVSWITCHIO_CALIBRATE      0x3901

/************************************************** read/write ******************/

/* Write a number in ASCII to the device to switch channels. Channels start at 1,
 * a value of 0 indicates to disconnect the output. If one value is written, audio
 * and video will both switch to that channel. If two values (space separated) are
 * written, video switches to the first indicated channel and audio to the second.
 * If three values are written, they specify video, white audio, and red audio
 * respectively.
 */

/* Reading from the device yields a space-separated list of active channel numbers.
 * The line is resent every time it changes.
 */

#endif /* __UVSWITCH_DEV_H */

/* The End */
