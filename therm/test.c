#include <usb.h>
#include <stdio.h>
#include <errno.h>
#include <therm-rx-protocol.h>


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
      if (dev->descriptor.idVendor == THERMRX_VENDOR_ID &&
	  dev->descriptor.idProduct == THERMRX_PRODUCT_ID) {
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

unsigned char control_read1(usb_dev_handle *d, int request, int value, int index) {
  unsigned char b;
  if (usb_control_msg(d, USB_TYPE_VENDOR | 0x80, request,
		      value, index, &b, 1, 500) < 0) {
    perror("usb_control_msg");
    exit(1);
  }
  return b;
}

int main(int argc, char **argv) {
  usb_dev_handle *d;

  if (!(d = open_dev())) {
    printf("Error opening device\n");
    return 1;
  }

  if (usb_claim_interface(d, 0)) {
    perror("usb_claim_interface");
    return 1;
  }

  printf("Local temperature: %d\n", control_read1(d, THERMRX_REQ_LOCAL_TEMP, 0, 0));

  while (1) {
    unsigned char buffer[64];
    int retval;
    int byte, bit;
    int buffered, noise, total;

    retval = usb_bulk_read(d, 0x81, buffer, sizeof(buffer), 100000);
    if (retval < 0) {
      perror("usb_bulk_read");
      continue;
    }

    buffered = buffer[0] | (buffer[1] << 8);
    noise    = buffer[2] | (buffer[3] << 8);
    total    = buffer[4] | (buffer[5] << 8);

    printf("%d bytes, %d content bits, %d noise, %d total bits\n\t",
	   retval, buffered, noise, total);

    for (byte=6; byte<retval; byte++) {
      for (bit=1; bit&0xFF; bit<<=1) {
	printf("%c", buffer[byte]&bit ? '1' : '0');
      }
    }

    printf("\n");
  }

  return 0;
}
