/*
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
#include <fcntl.h>
#include <unistd.h>
#include <scsi/scsi.h>
#include "usb-storage.h"

static int usage_device_cb(int id, struct usb_device *dev, int interface, void *userData)
{
    char name[256] = "Unknown";

    if (dev->descriptor.iProduct > 0) {
	usb_dev_handle *devh = usb_open(dev);
	usb_get_string_simple(devh, dev->descriptor.iProduct, name, sizeof(name));
	usb_close(devh);
    }

    printf("[%d] %04x:%04x \"%s\" %s\n", id, dev->descriptor.idVendor, dev->descriptor.idProduct, name, dev->filename);

    return 0;
}

static int usage(char **argv)
{
    printf("usage: %s device-id output-file\n"
	   "\n"
	   "Grumpydisk is a data recovery program for damaged or\n"
	   "otherwise failing USB storage disks that tend to lock\n"
	   "up the kernel under normal use. It uses libusb to access\n"
	   "these disks directly, and is aggressive about ignoring\n"
	   "errors.\n"
	   "\n"
	   "The output file is a plain file or a device to write\n"
	   "the disk's raw contents to. Available devices:\n"
	   "\n",
	   argv[0]);

    storage_scan(usage_device_cb, NULL);
}

int main(int argc, char **argv)
{
    usb_dev_handle *devh;
    int outfile;

    if (argc != 3)
	return usage(argv);
    
    devh = storage_open_by_id(atoi(argv[1]));
    if (!devh)
	return 1;
    usb_reset(devh);

    outfile = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (outfile < 0) {
	perror("open");
	return 1;
    }

    {
	unsigned char cmd[16];
	unsigned char result[8];
	unsigned int sector_size, n_sectors, sector;
	int count = 64;
	int retry;

	cmd[0] = READ_CAPACITY;
	storage_read(devh, result, sizeof(result), cmd, 1);

	n_sectors = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3];
	sector_size = (result[4] << 24) | (result[5] << 16) | (result[6] << 8) | result[7];

	printf("sector_size: %d n_sectors: %d\n", sector_size, n_sectors);

	for (sector=0; sector<n_sectors; sector+=count) {
	    unsigned char sector_buffer[sector_size * count];

	    printf("sector %d [%.04f%%]\n", sector, sector / (float) n_sectors * 100.0);

	    cmd[0] = READ_10;
	    cmd[1] = 0;
	    cmd[2] = sector >> 24;
	    cmd[3] = sector >> 16;
	    cmd[4] = sector >> 8;
	    cmd[5] = sector;
	    cmd[6] = 0;
	    cmd[7] = 0;
	    cmd[8] = count;
	    cmd[9] = 0;
	    for (retry=0; retry < 3; retry++) {
		if (storage_read(devh, sector_buffer, sizeof(sector_buffer), cmd, 10) >= 0)
		    break;
		printf("reset and retry...\n");

		usb_close(devh);
		devh = storage_open_by_id(atoi(argv[1]));
		if (!devh)
		    return 1;
		usb_reset(devh);
	    }

	    if (write(outfile, sector_buffer, sizeof(sector_buffer)) < 0) {
		perror("write");
	    }
	    fsync(outfile);
	}
    }

    return 0;
}


/* The End */
