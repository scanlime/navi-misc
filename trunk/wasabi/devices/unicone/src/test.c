#include <usb.h>
#include <stdio.h>
#include "bit_file.h"

#define VENDOR_ID   0xE461
#define PRODUCT_ID  0x0007

usb_dev_handle *open_dev(void);

int main()
{
  struct bitfile* bf;
  usb_dev_handle *d;

  /* Read our bitstream header */
  bf = bitfile_new_from_path("fpga/test/test.bit");
  if (!bf) {
    perror("bitfile_new_from_path");
    return 1;
  }

  printf("NCD file    : %s\n", bf->ncd_filename);
  printf("Part number : %s\n", bf->part_number);
  printf("Date        : %s\n", bf->date);
  printf("Time        : %s\n", bf->time);
  printf("Length      : %d bytes\n", bf->length);

  /* Read the bitstream itself */
  if (bitfile_read_content(bf) < 0) {
    perror("bitfile_read_content");
    return 1;
  }

  /* Send the bitstream over EP1 OUT */
  d = open_dev();
  if (usb_bulk_write(d, 1, bf->data, bf->length, 5000) < 0) {
    perror("usb_bulk_write");
    return 1;
  }

  bitfile_delete(bf);
  return 0;
}

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
  exit(1);
}
