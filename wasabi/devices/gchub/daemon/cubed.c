/*
 * cubed.c - Gamecube daemon. This communicates with the gchub using libusb,
 *           and creates input devices corresponding with all connected
 *           controllers.
 *
 *           Requires at least libusb 0.1.9. This currently acts as an
 *           inputpipe client, rather than creating the devices using
 *           /dev/uinput or another method.
 *
 * The Gamecube Controller Hub project
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <usb.h>
#include <stdio.h>
#include <math.h>
#include <gchub_protocol.h>

usb_dev_handle *open_gchub(void) {
  struct usb_bus *busses;
  struct usb_bus *bus;
  struct usb_device *dev;

  /* Find the device */
  usb_init();
  usb_find_busses();
  usb_find_devices();
  busses = usb_get_busses();

  for (bus=busses; bus; bus=bus->next) {
    for (dev=bus->devices; dev; dev=dev->next) {
      if (dev->descriptor.idVendor == GCHUB_VENDOR_ID &&
	  dev->descriptor.idProduct == GCHUB_PRODUCT_ID) {
	return usb_open(dev);
      }
    }
  }
  printf("No device found\n");
  return NULL;
}

void control_write(usb_dev_handle *d, int request, int value, int index) {
  if (usb_control_msg(d, USB_TYPE_VENDOR, request,
		      value, index, NULL, 0, 500) < 0) {
    perror("usb_control_msg");
    exit(1);
  }
}

int main(int argc, char **argv) {
  usb_dev_handle *d;

  if (!(d = open_gchub())) {
    printf("Error opening device\n");
    return 1;
  }

  while (1) {
    unsigned char buffer[8];
    int result;
    /*
    result = usb_interrupt_read(d, 1, buffer, 8, 500);

    if (result < 0) {
      perror("usb_interrupt_read");
      exit(1);
    }

    printf("[%d] %d %d %d\n", result, buffer[0], buffer[1], buffer[2]);
    */

    control_write(d, GCHUB_CTRL_SET_STATUS, 0, 0x40);
    usleep(100000);
    control_write(d, GCHUB_CTRL_SET_STATUS, 0, 0x80);
    usleep(100000);
  }

  return 0;
}

/* The End */
