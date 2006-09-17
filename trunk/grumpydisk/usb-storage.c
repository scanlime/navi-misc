/*
 * usb-storage.c - Minimalist read-only USB storage class driver
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

#include <stdio.h>
#include <string.h>
#include <scsi/scsi.h>
#include "usb-storage.h"

static int storage_probe_interfaces(struct usb_device *dev)
{
    int i;

    for (i=0; i<dev->config->bNumInterfaces; i++) {
	if (dev->config->interface[i].altsetting->bInterfaceClass == USB_CLASS_MASS_STORAGE)
	    return dev->config->interface[i].altsetting->bInterfaceNumber;
    }

    return -1;
}

int storage_scan(int (*callback)(int id, struct usb_device *dev, int interface, void *userData), void *userData)
{
    struct usb_bus *busses;
    struct usb_bus *bus;
    struct usb_device *dev;
    static int usb_has_initialized = 0;
    int retval = 0, interface;

    if (!usb_has_initialized) {
	usb_init();
	usb_has_initialized = 1;
    }
    usb_find_busses();
    usb_find_devices();
    busses = usb_get_busses();

    for (bus=busses; bus; bus=bus->next) {
	for (dev=bus->devices; dev; dev=dev->next) {    
	    interface = storage_probe_interfaces(dev);
	    if (interface >= 0) {
		/* Found a USB-storage device, yay */
		retval = callback(atoi(dev->filename), dev, interface, userData);
		if (retval) {
		    goto done;
		}
	    }
	}
    }

 done:
    return retval;
}

struct storage_open_by_id_data {
    usb_dev_handle *result;
    int id;
};

static int storage_open_by_id_cb(int id, struct usb_device *dev, int interface, void *userData)
{
    struct storage_open_by_id_data *data = (struct storage_open_by_id_data *) userData;

    if (id == data->id) {
	data->result = usb_open(dev);
	if (data->result) {
	    /* Kick other drivers (usb-storage) off */
	    usb_detach_kernel_driver_np(data->result, interface);

	    if (usb_claim_interface(data->result, interface) < 0) {
		perror("usb_claim_interface");
		data->result = NULL;
	    }
	}
	else {
	    perror("usb_open");
	}
	return 1;
    }

    return 0;
}

usb_dev_handle* storage_open_by_id(int id)
{
    struct storage_open_by_id_data data;

    data.id = id;
    if (storage_scan(storage_open_by_id_cb, &data))
	return data.result;

    return NULL;
}

static void append_le_uint32(unsigned char **buffer, unsigned int i)
{
    *((*buffer)++) = i;
    *((*buffer)++) = i >> 8;
    *((*buffer)++) = i >> 16;
    *((*buffer)++) = i >> 24;
}

static void append_uint8(unsigned char **buffer, unsigned char i)
{
    *((*buffer)++) = i;
}

static unsigned int read_be_uint32(unsigned char **buffer)
{
    unsigned int i = 0;
    i |= *((*buffer)++) << 24;
    i |= *((*buffer)++) << 16;
    i |= *((*buffer)++) << 8;
    i |= *((*buffer)++);
    return i;
}

static unsigned int read_le_uint32(unsigned char **buffer)
{
    unsigned int i = 0;
    i |= *((*buffer)++);
    i |= *((*buffer)++) << 8;
    i |= *((*buffer)++) << 16;
    i |= *((*buffer)++) << 24;
    return i;
}

static unsigned char read_uint8(unsigned char **buffer)
{
    return *((*buffer)++);
}

int storage_send_cbw(usb_dev_handle *devh, unsigned int tag, unsigned int length, unsigned char flags,
		     unsigned char *cb, int cbLength)
{
    unsigned char cbw[31];
    unsigned char *p = cbw;

    memset(cbw, 0, sizeof(cbw));

    append_le_uint32(&p, 0x43425355);
    append_le_uint32(&p, tag);
    append_le_uint32(&p, length);
    append_uint8(&p, flags);
    append_uint8(&p, 0);         /* LUN, currently not needed */
    append_uint8(&p, cbLength);

    memcpy(p, cb, cbLength);

    return usb_bulk_write(devh, 0x02, (void*) cbw, sizeof(cbw), 100);
}

int storage_read(usb_dev_handle *devh, unsigned char *buffer,
                 unsigned int length, unsigned char *cb, int cbLength)
{
    int retval;
    unsigned char packet[512];
    unsigned char csw[13], *p;
    unsigned int signature, replyTag, dataResidue, status;
    static unsigned int tag = 12345;
    tag++;

    retval = storage_send_cbw(devh, tag, length, CBW_DIR_IN, cb, cbLength);
    if (retval < 0) {
	perror("storage_send_cbw");
	return retval;
    }
    if (retval != 31) {
	fprintf(stderr, "storage_send_cbw: short read\n");
	return -1;
    }

 retry:
    retval = usb_bulk_read(devh, 0x81, (void*) packet, sizeof(packet), 500);
    if (retval < 0) {
	perror("usb_bulk_read");
	return retval;
    }
    if (retval > length) {
	fprintf(stderr, "usb_bulk_read: long read\n");
	return -1;
    }
    memcpy(buffer, packet, retval);

    if (retval != length) {
	if (retval != sizeof(packet)) {
	    fprintf(stderr, "usb_bulk_read: short read\n");
	    return -1;
	}

	/* We need more packets */
	length -= sizeof(packet);
	buffer += sizeof(packet);
	goto retry;
    }

    retval = usb_bulk_read(devh, 0x81, (void*) csw, sizeof(csw), 100);
    if (retval < 0) {
	perror("usb_bulk_read CSW");
	return retval;
    }
    if (retval != sizeof(csw)) {
	fprintf(stderr, "usb_bulk_read CSW: short read\n");
	return -1;
    }

    /* Unpack the CSW */
    p = csw;
    signature = read_le_uint32(&p);
    replyTag = read_le_uint32(&p);
    dataResidue = read_le_uint32(&p);
    status = read_uint8(&p);

    if (signature != 0x53425355) {
	fprintf(stderr, "usb_bulk_read CSW: invalid signature 0x%08X\n", signature);
	return -1;
    }
    if (tag != replyTag) {
	fprintf(stderr, "usb_bulk_read CSW: invalid tag (got 0x%08X, expected 0x%08X)\n", replyTag, tag);
	return -1;
    }
    if (status) {
	fprintf(stderr, "storage_read: command failed with status 0x%02X\n", status);
	return -1;
    }

    return 0;
}

int storage_cmd_read_capacity(usb_dev_handle *devh, unsigned int *sector_size, unsigned int *n_sectors)
{
    unsigned char cmd[] = {READ_CAPACITY};
    unsigned char result[8];
    unsigned char *p = result;

    int retval = storage_read(devh, result, sizeof(result), cmd, sizeof(cmd));

    *sector_size = read_be_uint32(&p);
    *n_sectors = read_be_uint32(&p);

    return retval;
}

int storage_cmd_read(usb_dev_handle *devh, unsigned int sector, unsigned int count, unsigned char *buffer, unsigned int length)
{
    unsigned char cmd[] = {
	READ_10,
	0,
	sector >> 24,
	sector >> 16,
	sector >> 8,
	sector,
	0,
	count >> 8,
	count,
	0,
    };

    return storage_read(devh, buffer, length, cmd, sizeof(cmd));
}

/* The End */
