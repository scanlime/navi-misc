#include <usb.h>
#include <stdio.h>
#include <unistd.h>

#include <unicone.h>
#include "bit_file.h"

usb_dev_handle *open_dev(void);
int write_bitstream(usb_dev_handle *d, struct bitfile *bf);

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

  if (strcmp(bf->part_number, "4010xlpc84")) {
    printf("This design is for an incompatible part\n");
    return 1;
  }

  /* Read the bitstream itself */
  if (bitfile_read_content(bf) < 0) {
    perror("bitfile_read_content");
    return 1;
  }

  d = open_dev();
  if (write_bitstream(d, bf) < 0) {
    perror("write_bitstream");
    return 1;
  }

  bitfile_delete(bf);
  return 0;
}

int write_bitstream(usb_dev_handle *d, struct bitfile *bf)
{
  unsigned char *p;
  int remaining, retval, write_size;
  unsigned char retbyte;

  /* Begin configuration */
  if (usb_control_msg(d, USB_TYPE_VENDOR, UNICONE_REQ_FPGA_CONFIG_BEGIN,
		      0, 0, NULL, 0, 1000) < 0)
    return 1;

  /* Wait while the FPGA clears itself */
  usleep(100000);

  /* Send the bitstream */
  p = bf->data;
  remaining = bf->length;

  while (remaining > 0) {
    /* Write in small chunks so we can use a smaller timeout */
    write_size = remaining;
    if (write_size > 256)
      write_size = 256;

    retval = usb_bulk_write(d, 1, p, write_size, 500);
    if (retval < 0)
      return retval;

    remaining -= retval;
    p += retval;
  }

  /* End configuration and show the status code */
  if (usb_control_msg(d, USB_TYPE_VENDOR | 0x80, UNICONE_REQ_FPGA_CONFIG_END,
		      0, 0, &retbyte, 1, 1000) < 0)
    return -1;
  printf("configuration returned: 0x%02X\n", retbyte);

  return 0;
}

usb_dev_handle *open_dev(void)
{
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
      if (dev->descriptor.idVendor == UNICONE_VENDOR_ID &&
	  dev->descriptor.idProduct == UNICONE_PRODUCT_ID) {
	return usb_open(dev);
      }
    }
  }

  printf("No device found\n");
  exit(1);
}
