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

/* Information about the MI6K's device node in /dev/usb/ */
#define MI6K_DEV_NAMEFORMAT   "mi6k%d"
#define MI6K_MINOR_BASE        230

/* The End */
