/*
 * Media Infrawidget 6000 driver
 *
 * Copyright(c) 2003 Micah Dowty <micah@picogui.org>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 * Currently this is just for experimentation. Eventually it will morph into
 * one or more drivers for some projects I've been cooking up...
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/devfs_fs_kernel.h>
#include "../include/protocol.h"

#define DEBUG
#include <linux/usb.h>

#define MI6K_DEV_NAME     "widget"
#define MI6K_MINOR_BASE	230

#define BUFFER_SIZE 8

#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR "Micah Dowty <micah@picogui.org>"
#define DRIVER_DESC "Media Infrawidget 6000 driver"

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

/* the global usb devfs handle */
extern devfs_handle_t usb_devfs_handle;

struct mi6k {
	char name[128];
	struct usb_device *usbdev;
	devfs_handle_t devfs;
	struct urb read_urb;
	unsigned char read_data[BUFFER_SIZE];
	int open;
	struct usb_endpoint_descriptor *endpoint;
};

static void mi6k_read_complete(struct urb *urb)
{
	struct mi6k *widget = urb->context;
	unsigned char *data = widget->read_data;

	if (urb->status) return;

       	dbg("Received %d bytes - %02X %02X %02X %02X", urb->actual_length, data[0], data[1], data[2], data[3]);
}

static int mi6k_dev_open (struct inode *inode, struct file *file)
{
	MOD_INC_USE_COUNT;
	return 0;
}

static int mi6k_dev_release (struct inode *inode, struct file *file)
{
	MOD_DEC_USE_COUNT;
	return 0;
}

/* Send a control request to the device synchronously */
static void mi6k_request (struct mi6k* widget, unsigned short request, 
				unsigned short wValue, unsigned short wIndex)
{
	usb_control_msg(widget->usbdev, usb_sndctrlpipe(widget->usbdev, 0),
			request,
			0x40,     /* Request type: vendor specific, host-to-device */
			wValue,
			wIndex,
			NULL, 0,  /* data (not used) */
			HZ / 2    /* Timeout: 1/2 second */
			);
}

static ssize_t mi6k_dev_write (struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	struct mi6k *widget = (struct mi6k*) file->private_data;

	if (count) {
		/* Pad unused bytes with zero, which the display will ignore */
		unsigned char tbuffer[] = {0, 0, 0, 0};
		int transfer_length;

		transfer_length = min(sizeof(tbuffer), count);
		dbg("%d bytes to send, sending %d bytes", count, transfer_length);
		if (copy_from_user(tbuffer, buffer, transfer_length))
			return -EFAULT;

		/* Temporary debug code: turn the VFD on */
		mi6k_request(widget, MI6K_CTRL_VFD_POWER, 1, 0);
		mi6k_request(widget, MI6K_CTRL_LED_SET, 0x50, 0x50);

		/* Pack 4 bytes of the character stream into the packet's value and index parameters */
		usb_control_msg(widget->usbdev, usb_sndctrlpipe(widget->usbdev, 0),
				MI6K_CTRL_VFD_WRITE,
				0x40,     /* Request type: vendor specific, host-to-device */
				tbuffer[0] | (((int)tbuffer[1]) << 8),  /* value */
				tbuffer[2] | (((int)tbuffer[3]) << 8),  /* index */
				NULL, 0,  /* data (not used) */
				HZ / 2    /* Timeout: 1/2 second */
				);

		return transfer_length;
	}
	else {
		/* Nothing to do */
		return 0;
	}
}

static struct file_operations mi6k_fops = {
	owner:		THIS_MODULE,
	write:          mi6k_dev_write,
	open:		mi6k_dev_open,
	release:	mi6k_dev_release,
};

static void *mi6k_probe(struct usb_device *dev, unsigned int ifnum,
			     const struct usb_device_id *id)
{
	struct usb_interface *iface;
	struct usb_interface_descriptor *interface;
	struct usb_endpoint_descriptor *endpoint;
	struct mi6k *widget;
	int read_pipe;
	const char *name;

	if ((dev->descriptor.idVendor != MI6K_VENDOR_ID) ||
	    (dev->descriptor.idProduct != MI6K_PRODUCT_ID))
		return NULL;

	iface = &dev->actconfig->interface[ifnum];
	interface = &iface->altsetting[iface->act_altsetting];

	dbg("bNumEndpoints = %d", interface->bNumEndpoints);

	if (interface->bNumEndpoints < 1) return NULL;

	endpoint = interface->endpoint + 0;

	usb_set_idle(dev, interface->bInterfaceNumber, 0, 0);

	if (!(widget = kmalloc(sizeof(struct mi6k), GFP_KERNEL))) return NULL;
	memset(widget, 0, sizeof(struct mi6k));

	widget->usbdev = dev;
	widget->endpoint = endpoint;

	/* Create a devfs entry */
	widget->devfs = devfs_register (usb_devfs_handle, MI6K_DEV_NAME,
					DEVFS_FL_DEFAULT, USB_MAJOR,
					MI6K_MINOR_BASE,
					S_IFCHR | S_IRUSR | S_IWUSR |
					S_IRGRP | S_IWGRP | S_IROTH,
					&mi6k_fops, widget);
	dbg("Registered /dev/usb/%s", MI6K_DEV_NAME);

	/* Initialize read URB */
	/*
	read_pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
	usb_fill_int_urb(&widget->read_urb, dev, read_pipe, widget->read_data,
			 min(usb_maxpacket(dev, read_pipe, usb_pipeout(read_pipe)), BUFFER_SIZE),
			 mi6k_read_complete, widget, endpoint->bInterval);
	*/

	/* Start up our interrupt read transfer */
	/*
	widget->read_urb.dev = widget->usbdev;
	if (usb_submit_urb(&widget->read_urb))
	        return NULL;
	*/

	return widget;
}

static void mi6k_disconnect(struct usb_device *dev, void *ptr)
{
	struct mi6k *widget = ptr;
	usb_unlink_urb(&widget->read_urb);
	devfs_unregister(widget->devfs);
	kfree(widget);
}

static struct usb_device_id mi6k_id_table [] = {
	{ USB_DEVICE(MI6K_VENDOR_ID, MI6K_PRODUCT_ID) },
	{ }	/* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, mi6k_id_table);

static struct usb_driver mi6k_driver = {
	name:		"mi6k",
	probe:		mi6k_probe,
	disconnect:	mi6k_disconnect,
	id_table:	mi6k_id_table,
};

static int __init mi6k_init(void)
{
	usb_register(&mi6k_driver);
	info(DRIVER_VERSION ":" DRIVER_DESC);
	dbg("Poing!");
	return 0;
}

static void __exit mi6k_exit(void)
{
	usb_deregister(&mi6k_driver);
	dbg("Exiting\n");
}

module_init(mi6k_init);
module_exit(mi6k_exit);

/* The End */
