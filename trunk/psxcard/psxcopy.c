/*
 * Simple host application to copy memory blocks to and from a PSX
 * memory card, via the USB card reader implemented in psxcard-fw.
 *
 * --Micah Dowty <micah@navi.cx>
 */

#include <usb.h>
#include <stdio.h>

#define VENDOR_ID   0xE461
#define PRODUCT_ID  0x0013

usb_dev_handle *open_dev(void) {
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
      if (dev->descriptor.idVendor == VENDOR_ID &&
	  dev->descriptor.idProduct == PRODUCT_ID) {
	return usb_open(dev);
      }
    }
  }
  printf("No device found\n");
  return NULL;
}

int main(int argc, char **argv) {
  usb_dev_handle *d;
  unsigned char c;
  unsigned char bulk_buffer[1000];
  int retval, i;

  if (!(d = open_dev())) {
    printf("Error opening device\n");
    return 1;
  }

  if (usb_claim_interface(d, 0)) {
    perror("usb_claim_interface");
    return 1;
  }

  /* Test EP1 IN */
  while (1) {
    retval = usb_bulk_read(d, 1, bulk_buffer, 64, 5000);
    if (retval < 0) {
      perror("usb_bulk_read");
      return 1;
    }
    
    printf("[%d]", retval);
    for (i=0; i<retval; i++) {
      printf(" %02x", bulk_buffer[i]);
    }
    printf("\n");
  }

  return 0;
}

/* The End */
