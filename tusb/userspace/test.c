#include <usb.h>
#include <stdio.h>

#define VENDOR_ID   0x0451
#define PRODUCT_ID  0x3410

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

  if (!(d = open_dev())) {
    printf("Error opening device\n");
    return 1;
  }

#if 0
  if (usb_control_msg(d, USB_TYPE_VENDOR, 42,
		      0, 0, "Boing", 6, 500) < 0) {
    perror("usb_control_msg");
    return 1;
  }

  if (usb_control_msg(d, USB_TYPE_VENDOR, 42,
		      0, 0, "Whee", 5, 500) < 0) {
    perror("usb_control_msg");
    return 1;
  }
#endif

#if 1
  if (usb_control_msg(d, USB_TYPE_VENDOR | 0x80, 123,
		      0, 0, &c, 1, 500) < 0) {
    perror("usb_control_msg");
    return 1;
  }
  printf("Received %d\n", c);
#endif

  return 0;
}
