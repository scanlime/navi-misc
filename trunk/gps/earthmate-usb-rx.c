/*
 * Simple libusb-based NMEA receiver for the DeLORME Earthmate USB
 * GPS module. This is the one that somewhat pretends to be a HID
 * device. Decoded data is output to stdout.
 *
 * Copyright (C) 2005 Micah Dowty
 *
 */

#include <usb.h>
#include <stdio.h>

#define GPS_VENDOR_ID   0x1163
#define GPS_PRODUCT_ID  0x0200

void received_packet(unsigned char *buffer) {
  if (buffer[0] == 0x00 && buffer[1] != 0x00) {
	fwrite(buffer+2, 6, 1, stdout);
  }
}

usb_dev_handle *open_gps(void) {
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
	    if (dev->descriptor.idVendor == GPS_VENDOR_ID &&
		dev->descriptor.idProduct == GPS_PRODUCT_ID) {
		return usb_open(dev);
	    }
	}
    }
    printf("No device found\n");
    return NULL;
}

int main(int argc, char **argv) {
    usb_dev_handle *d;

    /* This magickal packet seems to wake it up */
    unsigned char ep2_packet[8] = {
        0x20, 0x10, 0x24, 0x4c, 0x54, 0x43, 0x2c, 0x31,
    };
    unsigned char buffer[8];
    int retval;

    if (!(d = open_gps())) {
	printf("Error opening device\n");
	return 1;
    }

    while (1) {
      retval = usb_interrupt_write(d, 0x02, ep2_packet,
				   sizeof(ep2_packet), 5000);
      if (retval < 0) {
        perror("usb_interrupt_write");
        return 1;
      }

      retval = usb_interrupt_read(d, 0x81, buffer, sizeof(buffer), 5000);
      if (retval < 0) {
	perror("usb_interrupt_read");
	return 1;
      }

      received_packet(buffer);
    }

    return 0;
}

/* The End */
