#include <usb.h>
#include <stdio.h>
#include "include/rwand_protocol.h"

usb_dev_handle *open_rwand(void) {
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
      if (dev->descriptor.idVendor == RWAND_VENDOR_ID &&
	  dev->descriptor.idProduct == RWAND_PRODUCT_ID) {
	return usb_open(dev);
      }
    }
  }
  printf("No device found\n");
  return NULL;
}

int main(int argc, char **argv) {
  usb_dev_handle *d = open_rwand();
  unsigned char c[8];
  unsigned char x[4];
  if (!d)
    return 1;

  if (usb_control_msg(d, USB_TYPE_VENDOR, RWAND_CTRL_SET_PREDICTION,
		      0, 43000, NULL, 0, 500) < 0)
    perror("usb_control_msg");
  if (usb_control_msg(d, USB_TYPE_VENDOR, RWAND_CTRL_SET_COIL_PHASE,
		      8000, 15000, NULL, 0, 500) < 0)
    perror("usb_control_msg");

  while (1) {
    if (usb_control_msg(d, USB_TYPE_VENDOR | USB_ENDPOINT_IN, 1,
			0, 0, c, 8, 500) < 0)
      perror("usb_control_msg");
    if (usb_control_msg(d, USB_TYPE_VENDOR | USB_ENDPOINT_IN, 2,
			0, 0, x, 4, 500) < 0)
      perror("usb_control_msg");

    printf("Poing %d %d %d %d - %d %d\n",
	   c[0] | (c[1] << 8),
	   c[2] | (c[3] << 8),
	   c[4] | (c[5] << 8),
	   c[6] | (c[7] << 8),
	   x[0] | (x[1] << 8),
	   x[2] | (x[3] << 8));

    usleep(1000);
  }

  return 0;
}
