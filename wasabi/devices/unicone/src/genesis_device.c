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

#include "genesis_device.h"
#include <assert.h>
#include <usb.h>

/* The Genesis controller emulator has a very simple
 * protocol. It's a low-speed USB device, with one
 * vendor-specific request to send a packed 32-bit
 * update for all controller state.
 */
#define GENESIS_VENDOR_ID   0xE461
#define GENESIS_PRODUCT_ID  0x0009
#define GENESIS_CTRL_SET_STATUS 	0x01

struct genesis_device*    genesis_device_new     ()

{
  struct genesis_device *self;
  struct usb_bus *busses;
  struct usb_bus *bus;
  struct usb_device *dev;
  unsigned char retbyte;

  self = malloc(sizeof(struct genesis_device));
  assert(self != NULL);
  memset(self, 0, sizeof(struct genesis_device));

  busses = usb_get_busses();
  for (bus=busses; bus; bus=bus->next) {
    for (dev=bus->devices; dev; dev=dev->next) {

      if (dev->descriptor.idVendor == GENESIS_VENDOR_ID &&
	  dev->descriptor.idProduct == GENESIS_PRODUCT_ID) {
	self->usbdev = usb_open(dev);

	if (usb_claim_interface(self->usbdev, 0)) {
	  goto error;
	}

	return self;
      }
    }
  }

  /* No device found */

 error:
  genesis_device_delete(self);
  return NULL;
}

void                      genesis_device_delete  (struct genesis_device *self)
{
  assert(self);
  if (self->usbdev)
    usb_close(self->usbdev);
  free(self);
}

int                       genesis_device_update  (struct genesis_device*    self,
						  int wValue, int wIndex)
{
  return usb_control_msg(self->usbdev, USB_TYPE_VENDOR,
			 GENESIS_CTRL_SET_STATUS,
			 wValue, wIndex, NULL, 0, 500);
}

/* The End */
