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

void control_read(usb_dev_handle *d, int request, unsigned char* buffer, int bytes) {
  if (usb_control_msg(d, USB_TYPE_VENDOR | USB_ENDPOINT_IN, request,
		      0, 0, buffer, bytes, 500) < 0) {
    perror("usb_control_msg");
    exit(1);
  }
}

void control_read16(usb_dev_handle *d, int request, short unsigned int *buffer, int words) {
  unsigned char *bytes = (unsigned char*) buffer;
  int i;
  control_read(d, request, bytes, words*2);
  for (i=0; i<words; i++)
    buffer[i] = bytes[i*2] + (bytes[i*2+1] << 8);
}

int main(int argc, char **argv) {
  usb_dev_handle *d;
  unsigned short int values[4];
  int period;

  if (!(d = open_rwand()))
    return 1;

  control_write(d, RWAND_CTRL_SET_MODES,
		RWAND_MODE_ENABLE_SYNC |
		RWAND_MODE_ENABLE_COIL |
		RWAND_MODE_COIL_DEBUG |
		//		RWAND_MODE_ENABLE_DISPLAY |
		0,
		0);

  while (1) {
    /* Read the current period prediction */
    control_read16(d, RWAND_CTRL_READ_PREDICTION, values, 2);
    period = values[0];
    printf("Wand period: %d (%.02f Hz)\n", period,
	   1/(period * 16 / 6000000.0));

    /* Update the coil driver phase */
    {
      const float pulse_center = 0.25;
      const float pulse_width  = 0.2;
      control_write(d, RWAND_CTRL_SET_COIL_PHASE,
		    period * (pulse_center - pulse_width),
		    period * (pulse_center + pulse_width));
    }

    control_write(d, RWAND_CTRL_SET_DISPLAY_PHASE, 100, 0x4FFF);
    control_write(d, RWAND_CTRL_SET_COLUMN_WIDTH, 100, 0);
    control_write(d, RWAND_CTRL_FLIP, 0,0);

    usleep(10000);
  }

  return 0;
}
