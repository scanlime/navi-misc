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


/* Information about the rwand's device node in /dev */
#define RWAND_DEV_NAMEFORMAT   "usb/rwand%d"
#define RWAND_MINOR_BASE       216

/* Size of the device's framebuffer, in columns and in bytes.
 */
#define RWAND_FB_COLUMNS       80
#define RWAND_FB_BYTES         RWAND_FB_COLUMNS

/* Power modes */
#define RWAND_POWER_AUTO       0   /* Controlled by switch */
#define RWAND_POWER_OFF        1
#define RWAND_POWER_ON         2


struct rwand_settings {
  int display_center;         /* The center of the display. 0 is full left,
			       * 0xFFFF is full right.
			       */
  int display_width;          /* The total width of the display, from 0 (nothing)
			       * to 0xFFFF (the entire wand sweep)
			       */
  int coil_center;            /* The center of the coil pulse. 0 is full left,
			       * 0x4000 is the center on the left-right pass,
			       * 0x8000 is full-right, 0xC000 is center on the
			       * right-left pass, and 0xFFFF is full left again.
			       */
  int coil_width;             /* The width of the coil pulse, from 0 (nothing) to
			       * 0xFFFF (the entire period)
			       */
  int duty_cycle;             /* The ratio of pixels to gaps. 0xFFFF has no gap,
			       * 0x0000 is all gap and no pixel.
			       */
  int fine_adjust;            /* Fine tuning for the front/back alignment */
  int power_mode;             /* RWAND_POWER_* */
  int num_columns;            /* The number of columns actually being displayed.
			       * This is set automatically on write().
			       */
};

#define RWANDIO_GET_SETTINGS    0x3B01
#define RWANDIO_PUT_SETTINGS    0x3B02


#endif /* __RWAND_DEV_H */

/* The End */
