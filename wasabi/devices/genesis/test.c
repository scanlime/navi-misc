#include <usb.h>
#include <stdio.h>

#define GENESIS_VENDOR_ID   0xE461
#define GENESIS_PRODUCT_ID  0x0009

usb_dev_handle *open_genesis(void) {
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
      if (dev->descriptor.idVendor == GENESIS_VENDOR_ID &&
	  dev->descriptor.idProduct == GENESIS_PRODUCT_ID) {
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
  int value, index;
  char line[256];

  if (!(d = open_genesis())) {
    printf("Error opening device\n");
    return 1;
  }

  while (fgets(line, sizeof(line), stdin)) {
    sscanf(line, "%d %d", &value, &index);
    control_write(d, 0x01, value, index);
  }

  return 0;
}
