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
#include <malloc.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include "usbdevice_fs.h"
#include "therm-daemon.h"

#define FIRMWARE_FILENAME     "therm-rx.bin"

#define CODESPACE_SIZE        0x2000
#define FIRMWARE_HEADER_SIZE  3


static void receiver_download_firmware(int fd);


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

int usb_device_open(int bus, int dev)
{
  char filename[256];
  int fd;

  sprintf(filename, "/proc/bus/usb/%03d/%03d", bus, dev);
  fd = open(filename, O_RDWR);
  if (fd < 0) {
    perror("open");
    return -1;
  }

  return fd;
}

int receiver_open()
{
  const char *deviceFileName = "/proc/bus/usb/devices";
  char line[1024];
  FILE *devices;
  int devId = 0, busId = 0, vid = 0, pid = 0;
  
  /*
   * Open the usbfs device list, and scan
   * for either the programmed or unprogrammed
   * version of our hardware.
   */

  devices = fopen(deviceFileName, "r");
  if (!devices) {
    perror(deviceFileName);
    return -1;
  }

  while (fgets(line, sizeof line - 1, devices)) {
    line[sizeof line - 1] = '\0';

    switch (line[0]) {

    case 'T':
      sscanf(line, "T: Bus=%d Lev=%*d Prnt=%*d Port=%*d Cnt=%*d Dev#= %d Spd=%*d MxCh= %*d", &busId, &devId);
      break;

    case 'P':
      sscanf(line, "P: Vendor=%04x ProdID=%04x Rev= %*d.%*d", &vid, &pid);

      if (vid == THERMRX_VENDOR_ID && pid == THERMRX_PRODUCT_ID) {
	return usb_device_open(busId, devId);
      }

      if (vid == THERMRX_VENDOR_ID && pid == THERMRX_BOOT_PRODUCT_ID) {
	int fd = usb_device_open(busId, devId);
	receiver_download_firmware(fd);
	close(fd);
      }

      break;

    }
  }

  return -1;
}

struct rx_packet* receiver_read(int fd, int timeout)
{
  struct usbdevfs_bulktransfer xfer;
  unsigned char buffer[THERMRX_BUFFER_SIZE];
  int noise, total, retval;
  struct rx_packet *packet;

  xfer.ep = THERMRX_PACKET_ENDPOINT;
  xfer.len = sizeof buffer;
  xfer.timeout = timeout;
  xfer.data = buffer;

  retval = ioctl(fd, USBDEVFS_BULK, &xfer);
  if (retval <= 0)
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
  packet->signal_strength = (total * 4 - noise) / (total * 4.0f);
  if (packet->signal_strength < 0)
    packet->signal_strength = 0;

  return packet;
}

int receiver_get_local_temp(int fd, int *temperature)
{
  unsigned char b;
  struct usbdevfs_ctrltransfer xfer;
  int temp;

  xfer.bRequestType = 0xC0;
  xfer.bRequest = THERMRX_REQ_LOCAL_TEMP;
  xfer.wValue = 0;
  xfer.wIndex = 0;
  xfer.wLength = sizeof b;
  xfer.timeout = 100;
  xfer.data = &b;

  if (ioctl(fd, USBDEVFS_CONTROL, &xfer) < 0) {
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

static int send_bulk_ep_buffer(int fd, int endpoint,
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
    struct usbdevfs_bulktransfer xfer;

    if (packet_size > block_size)
      packet_size = block_size;

    xfer.ep = endpoint;
    xfer.len = packet_size;
    xfer.timeout = 200;
    xfer.data = current;

    retval = ioctl(fd, USBDEVFS_BULK, &xfer);
    if (retval <= 0)
      return retval;

    bytes_uploaded += retval;
    remaining -= retval;
    current += retval;
  }
  return bytes_uploaded;
}

static void receiver_download_firmware(int fd)
{
  int bytes_uploaded;
  int firmware_size;
  FILE *fw_file = NULL;
  unsigned char fw_buffer[CODESPACE_SIZE + FIRMWARE_HEADER_SIZE + 1];

  memset(fw_buffer, 0, sizeof(fw_buffer));

  fw_file = fopen(FIRMWARE_FILENAME, "rb");
  if (!fw_file) {
    fprintf(stderr, "Error opening firmware file, can't program device!\n");
    return;
  }

  /* Read up to the maximum firmware size plus one byte- to make sure
   * the file isn't larger than the device can handle.
   */
  firmware_size = fread(fw_buffer + FIRMWARE_HEADER_SIZE, 1, CODESPACE_SIZE+1, fw_file);
  if (firmware_size <= 0) {
    fprintf(stderr, "Error reading firmware file, can't program device!\n");
    fclose(fw_file);
    return;
  }
  if (firmware_size > CODESPACE_SIZE) {
    fprintf(stderr, "Firmware too large!\n");
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

  bytes_uploaded = send_bulk_ep_buffer(fd, THERMRX_BOOT_ENDPOINT,
				       fw_buffer, FIRMWARE_HEADER_SIZE + firmware_size);
  if (bytes_uploaded < 0) {
    fprintf(stderr, "Error writing firmware\n");
    return;
  }

  fprintf(stderr, "Firmware uploaded, device should reattach\n");
}

/* The End */
