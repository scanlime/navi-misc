/*
 * device.c - Interfaces that deal with libusb directly, for opening and closing
 *            the rcpod and sending low-level commands to it.
 *
 * Remote Controlled PIC of Doom
 * Copyright (C) 2003 Micah Dowty <micah@picogui.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <usb.h>
#include <stdio.h>
#include <unistd.h>

#define RCPOD_VENDOR_ID   0xE461
#define RCPOD_PRODUCT_ID  0x0002
#define RCPOD_CTRL_POKE	  0x01


usb_dev_handle *rcpod_open(void) {
  struct usb_bus *bus;
  struct usb_device *device;

  /* Init libusb */
  usb_init();
  usb_find_busses();
  usb_find_devices();

  /* Loop through all attached devices... */
  for (bus=usb_get_busses(); bus; bus=bus->next) {
    for (device=bus->devices; device; device=device->next) {

      /* Have we found an rcpod? */
      if (device->descriptor.idVendor == RCPOD_VENDOR_ID &&
	  device->descriptor.idProduct == RCPOD_PRODUCT_ID) {
	return usb_open(device);
      }
    }
  }

  /* No rcpod, darn... */
  return NULL;
}


int rcpod_poke(usb_dev_handle *rcpod, int address, int data) {
  return usb_control_msg(rcpod, USB_TYPE_VENDOR, RCPOD_CTRL_POKE, data, address, NULL, 0, 500);
}


int main() {
  usb_dev_handle *rcpod;
  int r;

  rcpod = rcpod_open();
  if (!rcpod) {
    printf("No rcpod device found\n");
    return 1;
  }

  /* Make PORTC all outputs */
  r = rcpod_poke(rcpod, 0x87, 0x00);
  if (r < 0) {
    perror("poke");
    return 1;
  }

  /* Flash PORTC */
  while (1) {
    r = rcpod_poke(rcpod, 0x07, 0xFF);
    if (r < 0) {
      perror("poke");
      return 1;
    }

    usleep(50000);

    r = rcpod_poke(rcpod, 0x07, 0x00);
    if (r < 0) {
      perror("poke");
      return 1;
    }

    usleep(50000);
  }

  return 0;
}
