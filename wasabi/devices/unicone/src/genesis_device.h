/*
 * genesis_device.h - Abstractions for the Sega Genesis controller
 *                    emulator hardware. This can identify and open
 *                    devices, then send status updates.
 *
 * Universal Controller Emulator project
 * Copyright (C) 2004 Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _H_GENESIS_DEVICE
#define _H_GENESIS_DEVICE

#include <usb.h>

struct genesis_device {
  usb_dev_handle *usbdev;
};

/* Look for the first Sega Genesis controller emulator device found, and open it.
 * This requires that libusb be initialized beforehand.
 */
struct genesis_device*    genesis_device_new     ();
void                      genesis_device_delete  (struct genesis_device *self);

/* Send a status update packet to the device. This one packet
 * includes the full state of both controllers, and the status
 * of the power relay and reset button.
 */
int                       genesis_device_update  (struct genesis_device*    self,
						  int wValue, int wIndex);

#endif /* _H_GENESIS_DEVICE */

/* The End */
