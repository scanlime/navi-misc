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

#define TIMEOUT               500            /* In milliseconds */
#define MAX_FIRMWARE_SIZE     8192
#define FIRMWARE_HEADER_SIZE  3

static void               unicone_device_delete_content   (struct unicone_device *self);

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
	  perror("Error setting bootloader device configuration");
	  goto error;
	}

	if (usb_claim_interface(self->usbdev, 0)) {
	  perror("Error claiming bootloader device interfce");
	  goto error;
	}

	return self;
      }

      /* Normal unicone device? */
      if (dev->descriptor.idVendor == UNICONE_VENDOR_ID &&
	  dev->descriptor.idProduct == UNICONE_PRODUCT_ID) {
	self->usbdev = usb_open(dev);

	if (usb_claim_interface(self->usbdev, 0)) {
	  perror("Error claiming device interfce");
	  goto error;
	}

	/* We have firmware, but check on the FPGA's status */
	self->firmware_installed = 1;
	if (usb_control_msg(self->usbdev, USB_TYPE_VENDOR | 0x80,
			    UNICONE_REQ_FPGA_STATUS,
			    0, 0, &retbyte, 1, TIMEOUT) < 0) {
	  perror("Error checking FPGA status");
	  goto error;
	}
	self->fpga_configured = (retbyte == UNICONE_STATUS_OK);

	return self;
      }
    }
  }

  fprintf(stderr, "No Unicone device found\n");

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
							   struct progress_reporter* progress,
							   int delay_milliseconds)
{
  void *progress_op;
  const char *error = NULL;
  struct unicone_device *new_dev;
  int ms_completed;
  const int ms_step = 20;

  if (progress)
    progress_op = progress->start(progress, "Reconnecting");

  /* Disconnect our current device, but leave the unicone_device struct allocated */
  unicone_device_delete_content(self);

  /* Spin our progress indicator as we wait for the device to initialize */
  for (ms_completed=0; ms_completed < delay_milliseconds; ms_completed+=ms_step) {
    usleep(ms_step * 1000);
    if (progress)
      progress->report(progress, progress_op, ((float) ms_completed) / delay_milliseconds);
  }

  usb_find_devices();
  new_dev = unicone_device_new();
  if (!new_dev) {
    error = "Can't open device";
    goto done;
  }

  /* Copy the content from the new device, and delete its containing struct */
  memcpy(self, new_dev, sizeof(struct unicone_device));
  free(new_dev);

 done:
  if (progress)
    progress->finish(progress, progress_op, error);
  if (error)
    return -1;
  return 0;
}

int                       unicone_device_upload_firmware  (struct unicone_device *self,
							   const char *filename,
							   struct progress_reporter *progress)
{
  void *progress_op;
  const char *error = NULL;
  int bytes_uploaded = 0;
  int firmware_size, remaining;
  FILE *fw_file = NULL;
  unsigned char fw_buffer[MAX_FIRMWARE_SIZE + FIRMWARE_HEADER_SIZE + 1];
  unsigned char *current;

  assert(self);
  assert(!self->firmware_installed);

  if (progress)
    progress_op = progress->start(progress, "Uploading firmware");

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

  /* Set up the header- a 16-bit little endian length, followed by an 8-bit checksum */
  fw_buffer[0] = firmware_size & 0xFF;
  fw_buffer[1] = firmware_size >> 8;
  fw_buffer[2] = 0;
  remaining = firmware_size;
  current = fw_buffer + FIRMWARE_HEADER_SIZE;
  while (remaining > 0) {
    fw_buffer[2] += *current;
    current++;
    remaining--;
  }

  /* Send the firmware in chunks of 128 bytes */
  remaining = firmware_size + FIRMWARE_HEADER_SIZE;
  current = fw_buffer;
  while (remaining > 0) {
    int packet_size = remaining;
    int retval;

    if (packet_size > 128)
      packet_size = 128;

    retval = usb_bulk_write(self->usbdev, UNICONE_EP_BOOTLOAD, current, packet_size, TIMEOUT);
    if (retval <= 0) {
      error = "USB write error";
      goto done;
    }

    bytes_uploaded += retval;
    remaining -= retval;
    current += retval;

    if (progress)
      progress->report(progress, progress_op, ((float) bytes_uploaded) / firmware_size);
  }

 done:
  if (fw_file)
    fclose(fw_file);
  if (progress)
    progress->finish(progress, progress_op, error);
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
							   struct progress_reporter* progress,
							   int                       verbose)
{
  void *progress_op;
  const char *error = NULL;
  int bytes_uploaded = 0;
  struct bitfile* bf = NULL;
  int remaining;
  unsigned char *current;
  unsigned char retbyte;

  assert(self);
  assert(self->firmware_installed);

  bf = bitfile_new_from_path(filename);
  if (!bf)
    error = "Error opening bitstream";

  if (verbose && bf) {
    fprintf(stderr, "Bitstream %s:\n", filename);
    fprintf(stderr, "  NCD file    : %s\n", bf->ncd_filename);
    fprintf(stderr, "  Part number : %s\n", bf->part_number);
    fprintf(stderr, "  Date        : %s\n", bf->date);
    fprintf(stderr, "  Time        : %s\n", bf->time);
    fprintf(stderr, "  Length      : %d bytes\n", bf->length);
  }

  if (progress)
    progress_op = progress->start(progress, "Uploading FPGA bitstream");

  if (!bf)
    goto done;

  if (strcmp(bf->part_number, "4010xlpc84")) {
    error = "This design is for an incompatible part";
    goto done;
  }

  if (bitfile_read_content(bf) < 0) {
    error = "Error reading bitstream content";
    goto done;
  }

  /* Prepare the FPGA for programming */
  self->fpga_configured = 0;
  if (usb_control_msg(self->usbdev, USB_TYPE_VENDOR, UNICONE_REQ_FPGA_CONFIG_BEGIN,
		      0, 0, NULL, 0, TIMEOUT) < 0) {
    error = "Error starting FPGA configuration";
    goto done;
  }

  /* Send the bitstream in 128-byte chunks */
  remaining = bf->length;
  current = bf->data;
  while (remaining > 0) {
    int packet_size = remaining;
    int retval;

    if (packet_size > 128)
      packet_size = 128;

    retval = usb_bulk_write(self->usbdev, UNICONE_EP_FPGA_CONFIG, current, packet_size, TIMEOUT);
    if (retval <= 0) {
      error = "USB write error";
      goto done;
    }

    bytes_uploaded += retval;
    remaining -= retval;
    current += retval;

    if (progress)
      progress->report(progress, progress_op, ((float) bytes_uploaded) / bf->length);
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
  if (progress)
    progress->finish(progress, progress_op, error);
  if (error)
    return -1;
  return bytes_uploaded;
}

/* The End */
