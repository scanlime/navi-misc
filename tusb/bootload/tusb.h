/*  
 * TUSB2136/3210 constants
 * See http://www-s.ti.com/sc/psheets/sllu025a/sllu025a.pdf
 *
 * Copyright (c) 2001 Jim Paris <jim@jtan.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License, as published by the Free Software Foundation.  This
 * program is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of merchantability or fitness for a particular
 * purpose.  You should have received a copy of version 2 of the GNU
 * General Public License along with this program; if not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef TUSB_H
#define TUSB_H

#include <usb.h>

typedef unsigned short u16;
typedef unsigned char u8;

extern usb_dev_handle *tusb;

int tusb_init(void);
void tusb_close(void);

#define BTC_GET_BOOTCODE_STATUS		0x80
#define BTC_EXECUTE_FIRMWARE		0x81
#define BTC_GET_FIRMWARE_REVISION	0x82
#define BTC_PRE_UPDATE_HEADER		0x83
#define BTC_UPDATE_HEADER		0x84
#define BTC_REBOOT			0x85
#define BTC_FORCE_EXECUTE_FIRMWARE	0x8F
#define BTC_EXTERNAL_MEMORY_READ	0x90
#define BTC_EXTERNAL_MEMORY_WRITE	0x91
#define BTC_I2C_MEMORY_READ		0x92
#define BTC_I2C_MEMORY_WRITE		0x93
#define BTC_INTERNAL_ROM_MEMORY_READ	0x94

#endif /* TUSB_H */
