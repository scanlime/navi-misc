#include <usb.h>
#include <stdio.h>
#include <rwand_protocol.h>

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

void control_write(usb_dev_handle *d, int request, int value, int index) {
  if (usb_control_msg(d, USB_TYPE_VENDOR, request,
		      value, index, NULL, 0, 500) < 0) {
    perror("usb_control_msg");
    exit(1);
  }
}

int main(int argc, char **argv) {
  usb_dev_handle *d = open_rwand();
  unsigned char c[8];
  unsigned char x[4];
  if (!d)
    return 1;

  control_write(d, RWAND_CTRL_SET_MODES,
		RWAND_MODE_ENABLE_SYNC |
		RWAND_MODE_ENABLE_COIL |
		RWAND_MODE_ENABLE_DISPLAY,
		0);
  control_write(d, RWAND_CTRL_SET_COIL_PHASE, 8000, 15000);

  return 0;
}
