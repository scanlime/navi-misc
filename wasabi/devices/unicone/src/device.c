/*
 * device.c - Abstractions for the unicone device, including identifying connected
 *            devices, loading firmware, and loading FPGA configuration.
 *
 * Universal Controller Emulator project
 * Copyright (C) 2004 Micah Dowty
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

#include <usb.h>
#include <stdio.h>
#include <assert.h>

#include <unicone.h>
#include "device.h"
#include "progress.h"
#include "bit_file.h"
#include "sha1.h"

#define TIMEOUT               500            /* In milliseconds */
#define CODESPACE_SIZE        0x2000
#define SHA1_STAMP_SIZE       20
#define MAX_FIRMWARE_SIZE     (CODESPACE_SIZE - SHA1_STAMP_SIZE)
#define FIRMWARE_HEADER_SIZE  3

static void               unicone_device_delete_content   (struct unicone_device*     self);
static int                sha_raw_file                    (const char*                filename,
							   unsigned char*             buffer);
static int                sha_bitstream_file              (const char*                filename,
							   unsigned char*             buffer);
static char*              sha_to_string                   (unsigned char*             buffer);
static int                sha_compare                     (unsigned char*             buffer1,
							   unsigned char*             buffer2);
static int                send_bulk_ep_buffer             (struct unicone_device*     self,
							   int                        endpoint,
							   unsigned char*             buffer,
							   int                        length,
							   struct progress_operation* progress_op);
static int                arith_checksum_8bit             (const unsigned char*       buffer,
							   int                        length);


/******************************************************************************/
/************************************************************ Initialization **/
/******************************************************************************/

void                      unicone_usb_init                ()
{
  usb_init();
  usb_find_busses();
  usb_find_devices();
}

struct unicone_device*    unicone_device_new              ()
{
  struct unicone_device *self;
  struct usb_bus *busses;
  struct usb_bus *bus;
  struct usb_device *dev;
  unsigned char retbyte;

  self = malloc(sizeof(struct unicone_device));
  assert(self != NULL);
  memset(self, 0, sizeof(struct unicone_device));

  busses = usb_get_busses();
  for (bus=busses; bus; bus=bus->next) {
    for (dev=bus->devices; dev; dev=dev->next) {

      /* Bootloader device? */
      if (dev->descriptor.idVendor == UNICONE_BOOTLOAD_VENDOR_ID &&
	  dev->descriptor.idProduct == UNICONE_BOOTLOAD_PRODUCT_ID) {
	self->usbdev = usb_open(dev);

	if (usb_set_configuration(self->usbdev, 1)) {
	  goto error;
	}

	if (usb_claim_interface(self->usbdev, 0)) {
	  goto error;
	}

	return self;
      }

      /* Normal unicone device? */
      if (dev->descriptor.idVendor == UNICONE_VENDOR_ID &&
	  dev->descriptor.idProduct == UNICONE_PRODUCT_ID) {
	self->usbdev = usb_open(dev);

	if (usb_claim_interface(self->usbdev, 0)) {
	  goto error;
	}

	/* We have firmware, but check on the FPGA's status */
	self->firmware_installed = 1;
	if (usb_control_msg(self->usbdev, USB_TYPE_VENDOR | 0x80,
			    UNICONE_REQ_FPGA_STATUS,
			    0, 0, &retbyte, 1, TIMEOUT) < 0) {
	  goto error;
	}
	self->fpga_configured = (retbyte == UNICONE_STATUS_OK);

	return self;
      }
    }
  }

  /* No device found */

 error:
  unicone_device_delete(self);
  return NULL;
}

void                      unicone_device_delete_content   (struct unicone_device *self)
{
  assert(self);
  if (self->usbdev)
    usb_close(self->usbdev);
}

void                      unicone_device_delete           (struct unicone_device *self)
{
  unicone_device_delete_content(self);
  free(self);
}

int                       unicone_device_reconnect        (struct unicone_device *self,
							   struct progress_reporter* progress)
{
  struct progress_operation* progress_op;
  const char *error = NULL;
  struct unicone_device *new_dev;
  int ms_completed;
  const int ms_timeout = 700;
  const int ms_step = 20;

  progress_op = progress_operation_new(progress, "Reconnecting");

  /* Disconnect our current device, but leave the unicone_device struct allocated */
  unicone_device_delete_content(self);

  /* Spin our progress indicator as we wait for the device to initialize */
  for (ms_completed=0; ms_completed < ms_timeout; ms_completed+=ms_step) {
    usleep(ms_step * 1000);

    /* Are we there yet? */
    usb_find_devices();
    new_dev = unicone_device_new();
    if (new_dev)
      break;

    progress_operation_update(progress_op, ((float) ms_completed) / ms_timeout);
  }

  if (!new_dev) {
    error = "Can't open device";
    goto done;
  }

  /* Copy the content from the new device, and delete its containing struct */
  memcpy(self, new_dev, sizeof(struct unicone_device));
  free(new_dev);

 done:
  progress_operation_finish(progress_op, error);
  if (error)
    return -1;
  return 0;
}


/******************************************************************************/
/************************************************ Firmware / FPGA Versioning **/
/******************************************************************************/

static int                sha_raw_file                    (const char            *filename,
							   unsigned char         *buffer)
{
  /* Take the SHA-1 digest of an entire binary file */
  FILE* f;
  f = fopen(filename, "rb");
  if (!f)
    return -1;
  sha_stream(f, buffer);
  fclose(f);
  return 0;
}

static int                sha_bitstream_file              (const char            *filename,
							   unsigned char         *buffer)
{
  /* Take the SHA-1 digest of only the bitstream data itself, from a Xilinx bitstream file */
  struct bitfile *f;
  f = bitfile_new_from_path(filename);
  if (!f)
    return -1;
  if (bitfile_read_content(f) < 0) {
    bitfile_delete(f);
    return -1;
  }
  sha_buffer(f->data, f->length, buffer);
  bitfile_delete(f);
  return 0;
}

static char*              sha_to_string                   (unsigned char         *buffer)
{
  /* Convert an SHA-1 digest to a string, using a static buffer */
  static char result[41];
  int i;
  for (i=0; i<20; i++)
    sprintf(result+2*i, "%02x", buffer[i]);
  result[40] = 0;
  return result;
}

static int                sha_compare                     (unsigned char         *buffer1,
							   unsigned char         *buffer2)
{
  /* Compare two SHA-1 digests, returning a code compatible with unicone_device_compare_*.
   * 1 if they're different, 0 if they're the same.
   */
  if (memcmp(buffer1, buffer2, 20))
    return 1;
  else
    return 0;
}

int                       unicone_device_compare_firmware (struct unicone_device*    self,
							   const char*               filename,
							   struct progress_reporter* progress)
{
  unsigned char file_sha1[32];
  unsigned char device_sha1[32];

  assert(self);

  /* Does the device have no firmware at all? */
  if (!self->firmware_installed)
    return 1;

  /* Take the SHA-1 digest of the whole firmware file */
  if (sha_raw_file(filename, file_sha1) < 0) {
    progress_error(progress, "Can't read firmware file");
    return -1;
  }

  /* Ask the device for it's current SHA-1 digest */
  if (usb_control_msg(self->usbdev, USB_TYPE_VENDOR | 0x80,
		      UNICONE_REQ_GET_FIRMWARE_STAMP,
		      0, 0, device_sha1, 20, TIMEOUT) < 0)
    return -1;

  return sha_compare(file_sha1, device_sha1);
}

int                       unicone_device_compare_bitstream(struct unicone_device*    self,
							   const char*               filename,
							   struct progress_reporter* progress)
{
  unsigned char file_sha1[32];
  unsigned char device_sha1[32];

  assert(self);
  assert(self->firmware_installed);

  /* Does the device have no bitstream installed? */
  if (!self->fpga_configured)
    return 1;

  /* Take the SHA-1 digest of the whole bitstream file */
  if (sha_bitstream_file(filename, file_sha1) < 0) {
    progress_error(progress, "Can't read bitstream file");
    return -1;
  }

  /* Ask the device for it's current SHA-1 digest */
  if (usb_control_msg(self->usbdev, USB_TYPE_VENDOR | 0x80,
		      UNICONE_REQ_GET_FPGA_STAMP,
		      0, 0, device_sha1, 20, TIMEOUT) < 0)
    return -1;

  return sha_compare(file_sha1, device_sha1);
}

int                       unicone_device_configure        (struct unicone_device*    self,
							   const char*               firmware_file,
							   const char*               bitstream_file,
							   struct progress_reporter* progress)
{
  int retval;

  /* Check our firmware */
  retval = unicone_device_compare_firmware(self, firmware_file, progress);
  if (retval < 0)
    return retval;

  if (retval > 0) {
    /* Firmware needs upgrading or needs initial installation */

    if (self->firmware_installed) {
      progress_message(progress, "Rebooting to remove existing firmware");
      unicone_device_remove_firmware(self);
      if (unicone_device_reconnect(self, progress) < 0)
	return -1;
    }

    /* Install firmware and reconnect */
    if (unicone_device_upload_firmware(self, firmware_file, progress) < 0)
      return -1;
    if (unicone_device_reconnect(self, progress) < 0)
      return -1;
  }

  /* Decide whether we need to install a bitstream */
  retval = unicone_device_compare_bitstream(self, bitstream_file, progress);
  if (retval < 0)
    return retval;

  if (retval > 0) {
    /* FPGA needs configuration */

    if (unicone_device_upload_bitstream(self, bitstream_file, progress) < 0)
      return -1;
  }
  return 0;
}


/******************************************************************************/
/**************************************************** Firmware / FPGA Upload **/
/******************************************************************************/

static int                send_bulk_ep_buffer             (struct unicone_device*     self,
							   int                        endpoint,
							   unsigned char*             buffer,
							   int                        length,
							   struct progress_operation* progress_op)
{
  /* Send a large block of data to the given USB bulk endpoint, in small blocks,
   * giving updates to the indicated progress reporter. Returns negative on error,
   * or the number of bytes uploaded on success.
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

    retval = usb_bulk_write(self->usbdev, endpoint, current, packet_size, TIMEOUT);
    if (retval <= 0)
      return retval;

    bytes_uploaded += retval;
    remaining -= retval;
    current += retval;

    progress_operation_update(progress_op, ((float) bytes_uploaded) / length);
  }
  return bytes_uploaded;
}

static int                arith_checksum_8bit             (const unsigned char*      buffer,
							   int                       length)
{
  /* Perform a simple 8-bit arithmetic checksum, as used by the TUSB3410 bootloader */
  int i;
  unsigned char sum;
  for (i=0; i<length; i++)
    sum += buffer[i];
  return sum;
}

int                       unicone_device_upload_firmware  (struct unicone_device *self,
							   const char *filename,
							   struct progress_reporter *progress)
{
  struct progress_operation *progress_op;
  const char *error = NULL;
  int bytes_uploaded;
  int firmware_size;
  FILE *fw_file = NULL;
  unsigned char fw_buffer[CODESPACE_SIZE + FIRMWARE_HEADER_SIZE + 1];
  unsigned char sha1_digest[32];

  memset(fw_buffer, 0, sizeof(fw_buffer));

  assert(self);
  assert(!self->firmware_installed);

  progress_op = progress_operation_new(progress, "Uploading firmware");

  fw_file = fopen(filename, "rb");
  if (!fw_file) {
    error = "Error opening file";
    goto done;
  }

  /* Read up to the maximum firmware size plus one byte- to make sure
   * the file isn't larger than the device can handle.
   */
  firmware_size = fread(fw_buffer + FIRMWARE_HEADER_SIZE, 1, MAX_FIRMWARE_SIZE+1, fw_file);
  if (firmware_size <= 0) {
    error = "File read error";
    goto done;
  }
  if (firmware_size > MAX_FIRMWARE_SIZE) {
    error = "Firmware too large";
    goto done;
  }

  fclose(fw_file);
  fw_file = NULL;

  /* Take an SHA-1 message digest of the whole file, install it at the very
   * last 20 bytes of memory. This can be read later by the firmware to see
   * if it's up to date with what's on disk.
   */
  sha_buffer(fw_buffer + FIRMWARE_HEADER_SIZE, firmware_size, sha1_digest);
  memcpy(fw_buffer + FIRMWARE_HEADER_SIZE + CODESPACE_SIZE - SHA1_STAMP_SIZE,
	 sha1_digest, SHA1_STAMP_SIZE);

  /* Set up the header- a 16-bit little endian length, followed by an 8-bit checksum.
   * We always send the entire fw_buffer to our device, since we need to send it the
   * version stamp located at the end of code space.
   */
  fw_buffer[0] = CODESPACE_SIZE & 0xFF;
  fw_buffer[1] = CODESPACE_SIZE >> 8;
  fw_buffer[2] = arith_checksum_8bit(fw_buffer + FIRMWARE_HEADER_SIZE, CODESPACE_SIZE);

  bytes_uploaded = send_bulk_ep_buffer(self, UNICONE_EP_BOOTLOAD,
				       fw_buffer, FIRMWARE_HEADER_SIZE + CODESPACE_SIZE,
				       progress_op);
  if (bytes_uploaded < 0)
    error = "USB write error";

 done:
  if (fw_file)
    fclose(fw_file);
  progress_operation_finish(progress_op, error);
  if (error)
    return -1;
  return bytes_uploaded;
}

int                       unicone_device_remove_firmware  (struct unicone_device *self)
{
  assert(self);
  assert(self->firmware_installed);
  return usb_control_msg(self->usbdev, USB_TYPE_VENDOR,
			 UNICONE_REQ_REBOOT,
			 0, 0, NULL, 0, TIMEOUT);
}

int                       unicone_device_upload_bitstream (struct unicone_device*    self,
							   const char*               filename,
							   struct progress_reporter* progress)
{
  struct progress_operation* progress_op;
  const char *error = NULL;
  int bytes_uploaded = 0;
  struct bitfile* bf = NULL;
  int retval;
  unsigned char retbyte;
  unsigned char *buffer = NULL;
  int buffer_size;

  assert(self);
  assert(self->firmware_installed);

  progress_op = progress_operation_new(progress, "Configuring FPGA");

  bf = bitfile_new_from_path(filename);
  if (!bf) {
    error = "Error opening bitstream";
    goto done;
  }

  if (strcmp(bf->part_number, "4010xlpc84")) {
    error = "This design is for an incompatible part";
    goto done;
  }

  if (bitfile_read_content(bf) < 0) {
    error = "Error reading bitstream content";
    goto done;
  }

  /* Allocate a buffer large enough for both the bitstream and our SHA-1 header */
  buffer_size = bf->length + 20;
  buffer = malloc(buffer_size);
  assert(buffer);

  /* Fill the buffer, with first the SHA-1 then the bitstream itself */
  sha_buffer(bf->data, bf->length, buffer);
  memcpy(buffer+20, bf->data, bf->length);

  /* Prepare the FPGA for programming */
  self->fpga_configured = 0;
  if (usb_control_msg(self->usbdev, USB_TYPE_VENDOR, UNICONE_REQ_FPGA_CONFIG_BEGIN,
		      0, 0, NULL, 0, TIMEOUT) < 0) {
    error = "Error starting FPGA configuration";
    goto done;
  }

  /* Send the whole buffer, giving progress updates on the way */
  bytes_uploaded = send_bulk_ep_buffer(self, UNICONE_EP_FPGA_CONFIG,
				       buffer, buffer_size,
				       progress_op);
  if (bytes_uploaded < 0) {
    error = "USB write error";
    goto done;
  }

  /* Finish programming the device, and get its status */
  if (usb_control_msg(self->usbdev, USB_TYPE_VENDOR | 0x80, UNICONE_REQ_FPGA_CONFIG_END,
		      0, 0, &retbyte, 1, TIMEOUT) < 0) {
    error = "Error finishing FPGA configuration";
    goto done;
  }
  if (retbyte == UNICONE_STATUS_OK) {
    self->fpga_configured = 1;
  }
  else {
    error = "No response from FPGA";
    goto done;
  }

 done:
  if (bf)
    bitfile_delete(bf);
  if (buffer)
    free(buffer);
  progress_operation_finish(progress_op, error);
  if (error)
    return -1;
  return bytes_uploaded;
}


/******************************************************************************/
/************************************************** Low-level Communications **/
/******************************************************************************/

int                       unicone_device_i2c_write        (struct unicone_device*    self,
							   int                       i2c_addr,
							   const unsigned char*      data,
							   int                       length)
{
  unsigned char packet[64];

  if (length > sizeof(packet)+2)
    return -1;

  packet[0] = i2c_addr;
  packet[1] = length;
  memcpy(packet+2, data, length);

  if (usb_bulk_write(self->usbdev, UNICONE_EP_FPGA_WRITE, packet, length + 2, TIMEOUT) <= 0)
    return -1;

  return 0;
}


/* The End */
