#include <usb.h>
#include <stdio.h>

#define VENDOR_ID   0xE461
#define PRODUCT_ID  0x0007

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
  int i;

  if (!(d = open_dev())) {
    printf("Error opening device\n");
    return 1;
  }

  if (usb_claim_interface(d, 0)) {
    perror("usb_claim_interface");
    return 1;
  }

  /* Turn on the LED */
  usb_control_msg(d, USB_TYPE_VENDOR, 13, 1, 0, NULL, 0, 100);

  /* Test EP0 IN */
  if (usb_control_msg(d, USB_TYPE_VENDOR | 0x80, 123,
		      0, 0, &c, 1, 500) < 0) {
    perror("usb_control_msg");
    return 1;
  }
  printf("Sent a vendor request 123, received %d\n", c);

  /* Test EP1 OUT */
  for (i=0; i<sizeof(bulk_buffer); i++)
    bulk_buffer[i] = i;
  if (usb_bulk_write(d, 1, bulk_buffer, 1000, 5000) < 0) {
    perror("usb_bulk_write");
    return 1;
  }
  printf("Bulk write sent\n");

  /* Turn off the LED */
  usb_control_msg(d, USB_TYPE_VENDOR, 13, 0, 0, NULL, 0, 100);

  return 0;
}
