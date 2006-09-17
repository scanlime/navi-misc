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
#include "usb-storage.h"

struct disk_info {
    int device_id;
    usb_dev_handle *devh;
    unsigned int sector_size, n_sectors;
};


static int usage_device_cb(int id, struct usb_device *dev, int interface, void *userData)
{
    char name[256] = "Unknown";

    if (dev->descriptor.iProduct > 0) {
	usb_dev_handle *devh = usb_open(dev);
	usb_get_string_simple(devh, dev->descriptor.iProduct, name, sizeof(name));
	usb_close(devh);
    }

    printf("[%d] %04x:%04x \"%s\"\n", id, dev->descriptor.idVendor, dev->descriptor.idProduct, name);

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

static void disk_reset(struct disk_info *di)
{
    while (1) {
	if (di->devh) 
	    usb_close(di->devh);
	di->devh = storage_open_by_id(di->device_id);
	if (di->devh)
	    break;

	printf("Device open failed, retrying...\n");
	sleep(1);
    }

    usb_reset(di->devh);
}

static int disk_init(struct disk_info *di, int device_id)
{
    di->devh = NULL;
    di->device_id = device_id;

    disk_reset(di);

    return storage_cmd_read_capacity(di->devh, &di->n_sectors, &di->sector_size);
}

static void read_or_die_trying(struct disk_info *di, unsigned int sector, unsigned int count, unsigned char *buffer)
{
    if (storage_cmd_read(di->devh, sector, count, buffer, count * di->sector_size) < 0) {
	printf("Resetting\n");
	usleep(500000);
	disk_reset(di);
	usleep(500000);	

	/* Arbitrary threshold.. */
	if (count > 64) {
	    int half = count >> 1;
	    printf("Failure to read %d sectors at %d, splitting\n", count, sector);
	    read_or_die_trying(di, sector, half, buffer);
	    read_or_die_trying(di, sector + half, count - half, buffer + (half * di->sector_size));
	}
	else {
	    printf("Bad spot at %d, skipping %d sectors\n", sector, count);
	}

    }
}


int main(int argc, char **argv)
{
    int outfile;
    struct disk_info di;
    int sector;
    int batch_size = 512;
    unsigned char *buffer;

    if (argc != 3)
	return usage(argv);

    if (disk_init(&di, atoi(argv[1])) < 0)
	return 1;    
    printf("sector_size: %d n_sectors: %d\n", di.sector_size, di.n_sectors);

    outfile = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (outfile < 0) {
	perror("open");
	return 1;
    }

    buffer = malloc(di.sector_size * batch_size);
    if (buffer == NULL) {
	perror("malloc");
	return 1;
    }

    for (sector=0; sector<di.n_sectors; sector+=batch_size) {

	printf("sector %d [%.04f%%]\n", sector, sector / (float) di.n_sectors * 100.0);

	read_or_die_trying(&di, sector, batch_size, buffer);

	if (write(outfile, buffer, di.sector_size * batch_size) < 0) {
	    perror("write");
	    return 1;
	}
	fsync(outfile);
    }

    return 0;
}


/* The End */
