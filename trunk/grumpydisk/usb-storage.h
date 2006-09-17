/*
 * usb-storage.h - Minimalist read-only USB storage class driver
 *                 implemented in userspace using libusb.
 *
 * Grumpydisk
 * Copyright (C) 2005 Micah Dowty
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

/* Look for all available storage-class devices, calling the provided callback for each
 * one we find. This automatically initializes libusb and scans busses if necessary.
 * Each device is assigned a small sequential ID for reference later.
 */
int storage_scan(int (*callback)(int id, struct usb_device *dev, int interface, void *userData), void *userData);

/* Open a storage device, given the ID assigned to it earlier */
usb_dev_handle* storage_open_by_id(int id);

#define CBW_DIR_IN  0x80
#define CBW_DIR_OUT 0x00

/* Send a command block wrapper to the device */
int storage_send_cbw(usb_dev_handle *devh, unsigned int tag, unsigned int length, unsigned char flags,
		     unsigned char *cb, int cbLength);

/* Perform a very simplified CBW-read-CSW cycle on the device */
int storage_read(usb_dev_handle *devh, unsigned char *buffer, unsigned int length,
                 unsigned char *cb, int cbLength);

/* Issue a READ_CAPACITY command, to check the disk's sector size and total number of sectors */
int storage_cmd_read_capacity(usb_dev_handle *devh, unsigned int *sector_size, unsigned int *n_sectors);

/* Issue a READ_10 command, reading data with a 32-bit sector number and 16-bit count */
int storage_cmd_read(usb_dev_handle *devh, unsigned int sector, unsigned int count,
                     unsigned char *buffer, unsigned int length);

/* The End */
