/*
 * Media Infrawidget 6000 userspace interface
 *
 * This header describes the interface between the MI6K kernel module and
 * any userspace applications wishing to use the device.
 *
 * Copyright(c) 2003 Micah Dowty <micah@picogui.org>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#ifndef __MI6K_DEV_H
#define __MI6K_DEV_H

#include <asm/types.h>

/************************************************** Device node ***************/

/* Information about the MI6K's device node in /dev/usb/ */
#define MI6K_DEV_NAMEFORMAT   "mi6k%d"
#define MI6K_MINOR_BASE       230


/************************************************** ioctl()s ******************/

/* Turns the VFD on if the parameter is nonzero, off if it is zero.
 * This will block for a sufficient amount of time to let the VFD hardware
 * initialize or safely power down.
 */
#define MI6KIO_VFD_POWER      0x3600

/* Sets the state of all LEDs. Brightnesses are given in fixed point such
 * that 0xFFFF is the brightest value and 0x0000 is the darkest.
 */
struct mi6k_leds {
	__u16 white;
	__u16 blue;
};
#define MI6KIO_LED_SET        0x3601


#endif /* __MI6K_DEV_H */

/* The End */
