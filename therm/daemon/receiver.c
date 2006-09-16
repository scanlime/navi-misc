/*
 * receiver.c - Host-side abstraction for the USB-attached base station
 *
 * Wireless therm system
 * Copyright (C) 2004-2005 Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <stdio.h>
#include <therm-rx-protocol.h>
#include <string.h>
#include <assert.h>
#include "therm-daemon.h"

#define FIRMWARE_FILENAME     "therm-rx.bin"

#define CODESPACE_SIZE        0x2000
#define FIRMWARE_HEADER_SIZE  3


static void receiver_download_firmware(usb_dev_handle* self);


void  packet_free(struct rx_packet* self)
{
  if (self) {
    if (self->buffer)
      free(self->buffer);
    free(self);
  }
}

struct rx_packet* packet_dup(struct rx_packet* self)
{
  struct rx_packet *new_packet;

  /* Allocate the new packet */
  new_packet = malloc(sizeof(struct rx_packet));
  assert(new_packet != NULL);
  memcpy(new_packet, self, sizeof(struct rx_packet));

  /* Allocate and copy the contents */
  new_packet->buffer = malloc(new_packet->buffer_bytes);
  assert(new_packet->buffer != NULL);
  memcpy(new_packet->buffer, self->buffer, new_packet->buffer_bytes);

  return new_packet;
}

int packet_cmp(struct rx_packet* a, struct rx_packet* b)
{
  /* Compare two packets' contents. The packets may be different sizes,
   * and one or both may even be NULL.
   */

  int saved_current_a, saved_current_b;
  int i, retval;
  int bit_a, bit_b;

  if (b && !a)
    return 1;
  if (a && !b)
    return -1;
  if (!(a || b))
    return 0;

  if (a->buffer_bits < b->buffer_bits)
    return -1;
  if (a->buffer_bits > b->buffer_bits)
    return -1;

  saved_current_a = a->current_bit;
  saved_current_b = b->current_bit;
  packet_rewind(a);
  packet_rewind(b);

  retval = 0;
  for (i=0; i<a->buffer_bits; i++) {
    bit_a = packet_read_bit(a);
    bit_b = packet_read_bit(b);

    if (bit_b && !bit_a) {
      retval = 1;
      break;
    }
    if (bit_a && !bit_b) {
      retval = -1;
      break;
    }
  }

  a->current_bit = saved_current_a;
  b->current_bit = saved_current_b;
  return retval;
}

void  packet_rewind(struct rx_packet* self)
{
  self->current_bit = 0;
}

int   packet_read_bit(struct rx_packet* self)
{
  int offset = self->current_bit >> 3;
  int mask = 1 << (self->current_bit & 7);

  if (self->current_bit >= self->buffer_bits)
    return 0;
  self->current_bit++;

  if (self->buffer[offset] & mask)
    return 1;
  else
    return 0;
}

int   packet_read_int(struct rx_packet* self, int width)
{
  int i = 0;
  int mask = 1;

  assert(width <= (sizeof(int)*8));

  while (width > 0) {
    if (packet_read_bit(self))
      i |= mask;
    mask <<= 1;
    width--;
  }
  return i;
}

int packet_read_signed_int(struct rx_packet* self, int width)
{
  int i = packet_read_int(self, width);

  assert(width <= (sizeof(int)*8) - 1);

  /* Is it negative? */
  if (i & (1<<(width-1))) {
    /* Sign extend */
    i -= 1<<width;
  }

  return i;
}

usb_dev_handle* receiver_open(usb_dev_handle* self)
{
  struct usb_bus *busses;
  struct usb_bus *bus;
  struct usb_device *dev;
  static int usb_has_initialized = 0;

  /* Find the device */
  if (!usb_has_initialized) {
    usb_init();
    usb_has_initialized = 1;
  }
  usb_find_busses();
  usb_find_devices();
  busses = usb_get_busses();

  for (bus=busses; bus; bus=bus->next) {
    for (dev=bus->devices; dev; dev=dev->next) {

      if (dev->descriptor.idVendor == THERMRX_VENDOR_ID &&
	  dev->descriptor.idProduct == THERMRX_PRODUCT_ID) {
	return usb_open(dev);
      }

      if (dev->descriptor.idVendor == THERMRX_VENDOR_ID &&
	  dev->descriptor.idProduct == THERMRX_BOOT_PRODUCT_ID) {
	usb_dev_handle* devh = usb_open(dev);
	receiver_download_firmware(devh);
	usb_close(devh);
      }
    }
  }
  return NULL;
}

struct rx_packet* receiver_read(usb_dev_handle* self, int timeout)
{
  unsigned char buffer[THERMRX_BUFFER_SIZE];
  int noise, total, retval;
  struct rx_packet *packet;

  retval = usb_bulk_read(self, THERMRX_PACKET_ENDPOINT, (void*) buffer,
			 sizeof(buffer), timeout);
  if (retval <= 6)
    return NULL;

  /* Allocate the new packet */
  packet = malloc(sizeof(struct rx_packet));
  assert(packet != NULL);
  memset(packet, 0, sizeof(struct rx_packet));

  /* Allocate and copy the contents */
  packet->buffer_bytes = retval - 6;
  packet->buffer = malloc(packet->buffer_bytes);
  assert(packet->buffer != NULL);
  memcpy(packet->buffer, buffer + 6, packet->buffer_bytes);

  /* Extract the header */
  packet->buffer_bits = buffer[0] | (buffer[1] << 8);
  noise = buffer[2] | (buffer[3] << 8);
  total = buffer[4] | (buffer[5] << 8);

  /* Calculate signal strength, a fraction between 0 and 1.
   * Without any sub-bit errors, the signal strength is 1. If we're getting
   * random noise, the signal strength should be about zero. Currently this
   * is scaled so an average of 4 sub-bit errors per bit brings the
   * signal to 0.
   */
  packet->signal_strength.numerator = noise;
  packet->signal_strength.denominator = total * 4;
  packet->signal_strength.numerator =
    packet->signal_strength.denominator - packet->signal_strength.numerator;
  if (packet->signal_strength.numerator < 0)
    packet->signal_strength.numerator = 0;

  return packet;
}

int receiver_get_local_temp(usb_dev_handle* self, int *temperature)
{
  unsigned char b;
  int temp;

  if (usb_control_msg(self, USB_TYPE_VENDOR | 0x80, THERMRX_REQ_LOCAL_TEMP,
		      0, 0, (void*) &b, 1, 100) < 0) {
    perror("usb_control_msg");
    return -1;
  }

  /* Sign extend */
  temp = b;
  if (temp & 0x80)
    temp -= 0x100;
  *temperature = temp;

  return 0;
}

static int arith_checksum_8bit(const unsigned char* buffer, int length)
{
  /* Perform a simple 8-bit arithmetic checksum, as used by the TUSB3410 bootloader */
  int i;
  unsigned char sum = 0;
  for (i=0; i<length; i++)
    sum += buffer[i];
  return sum;
}

static int send_bulk_ep_buffer(usb_dev_handle* self, int endpoint,
			       unsigned char* buffer, int length)
{
  /* Send a large block of data to the given USB bulk endpoint, in small blocks.
   * Returns negative on error, or the number of bytes uploaded on success.
   */

  const int block_size = 128;
  int remaining = length;
  unsigned char *current = buffer;
  int bytes_uploaded = 0;

  while (remaining > 0) {
    int packet_size = remaining;
    int retval;

    if (packet_size > block_size)
      packet_size = block_size;

    retval = usb_bulk_write(self, endpoint, (void*) current, packet_size, 200);
    if (retval <= 0)
      return retval;

    bytes_uploaded += retval;
    remaining -= retval;
    current += retval;
  }
  return bytes_uploaded;
}

static void receiver_download_firmware(usb_dev_handle* self)
{
  int bytes_uploaded;
  int firmware_size;
  FILE *fw_file = NULL;
  unsigned char fw_buffer[CODESPACE_SIZE + FIRMWARE_HEADER_SIZE + 1];

  memset(fw_buffer, 0, sizeof(fw_buffer));

  fw_file = fopen(FIRMWARE_FILENAME, "rb");
  if (!fw_file) {
    printf("Error opening firmware file, can't program device!\n");
    return;
  }

  /* Read up to the maximum firmware size plus one byte- to make sure
   * the file isn't larger than the device can handle.
   */
  firmware_size = fread(fw_buffer + FIRMWARE_HEADER_SIZE, 1, CODESPACE_SIZE+1, fw_file);
  if (firmware_size <= 0) {
    printf("Error reading firmware file, can't program device!\n");
    fclose(fw_file);
    return;
  }
  if (firmware_size > CODESPACE_SIZE) {
    printf("Firmware too large!\n");
    fclose(fw_file);
    return;
  }

  fclose(fw_file);
  fw_file = NULL;

  /* Set up the header- a 16-bit little endian length, followed by an 8-bit checksum.
   */
  fw_buffer[0] = firmware_size & 0xFF;
  fw_buffer[1] = firmware_size >> 8;
  fw_buffer[2] = arith_checksum_8bit(fw_buffer + FIRMWARE_HEADER_SIZE, firmware_size);

  bytes_uploaded = send_bulk_ep_buffer(self, THERMRX_BOOT_ENDPOINT,
				       fw_buffer, FIRMWARE_HEADER_SIZE + firmware_size);
  if (bytes_uploaded < 0) {
    printf("Error writing firmware\n");
    return;
  }

  printf("Firmware uploaded, device should reattach\n");
}

/* The End */
