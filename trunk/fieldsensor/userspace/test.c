#include <usb.h>
#include "../include/efs_protocol.h"

usb_dev_handle *open_efs(void) {
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
      if (dev->descriptor.idVendor == EFS_VENDOR_ID &&
	  dev->descriptor.idProduct == EFS_PRODUCT_ID) {
	printf("Found an electric field sensor, device %s\n", dev->filename);
	return usb_open(dev);
      }
    }
  }
  printf("No device found");
  return NULL;
}

void set_param_byte(usb_dev_handle *d, int address, int value) {
  if (usb_control_msg(d, USB_TYPE_VENDOR, EFS_CTRL_SET_PARAM_BYTE,
		      value, address, NULL, 0, 500) < 0)
    perror("usb_control_msg");
}

int take_reading(usb_dev_handle *d) {
  unsigned char c;
  if (usb_control_msg(d, USB_TYPE_VENDOR, EFS_CTRL_TAKE_READING,
		      0, 0, &c, c, 500) < 0)
    perror("usb_control_msg");
  return c;
}

int main(int argc, char **argv) {
  int i;
  usb_dev_handle *d = open_efs();
  if (!d)
    return 1;

  set_param_byte(d, EFS_PARAM_LC_PORT_XOR,      0xFF);
  set_param_byte(d, EFS_PARAM_ADCON_INIT,       0x81);
  set_param_byte(d, EFS_PARAM_PERIOD,           256 - 50);
  set_param_byte(d, EFS_PARAM_PHASE,            256 - 20);
  set_param_byte(d, EFS_PARAM_NUM_HALF_PERIODS, 10);
  set_param_byte(d, EFS_PARAM_LC_TRIS_INIT,     0xFC);
  set_param_byte(d, EFS_PARAM_LC_PORT_INIT,     0x02);

  printf("%d\n", take_reading(d));

  return 0;
}
