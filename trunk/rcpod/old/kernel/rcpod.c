/*
 * Remote Controlled PIC of Doom driver
 *
 * This is a module for the Linux kernel, providing a userspace interface
 * to the rcpod hardware via the USB protocol described in rcpod_protocol.h.
 *
 * Copyright(c) 2003 Micah Dowty <micah@picogui.org>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */


/******************************************************************************/
/************************************************** Local Definitions *********/
/******************************************************************************/

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/smp_lock.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/usb.h>
#include <rcpod_dev.h>

#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR "Micah Dowty <micah@picogui.org>"
#define DRIVER_DESC "Remote Controlled PIC of Doom driver"

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

/* Table of devices that work with this driver */
static struct usb_device_id rcpod_table [] = {
	{ USB_DEVICE(RCPOD_VENDOR_ID, RCPOD_PRODUCT_ID) },
	{ } /* End table */
};
MODULE_DEVICE_TABLE(usb, rcpod_table);


/* Structure to hold all of our device specific stuff */
struct usb_rcpod {
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct usb_interface *	interface;		/* the interface for this device */
	devfs_handle_t		devfs;			/* main devfs device node */
	unsigned char		minor;			/* the starting minor number for this device */
	int			open_count;		/* number of times this port has been opened */
	struct semaphore	sem;			/* locks this structure */
};

/* the global devfs handle for the USB subsystem */
extern devfs_handle_t usb_devfs_handle;

/* we can have up to this number of device plugged in at once */
#define MAX_DEVICES 16

/* array of pointers to our devices that are currently connected */
static struct usb_rcpod *minor_table[MAX_DEVICES];

/* lock to protect the minor_table structure */
static DECLARE_MUTEX(minor_table_mutex);

/* 1/2 second timeout for control requests */
#define REQUEST_TIMEOUT (HZ / 2)


/******************************************************************************/
/************************************************** Function Prototypes *******/
/******************************************************************************/

static int rcpod_poke(struct usb_rcpod *dev, unsigned short address, unsigned short data);
static int rcpod_peek(struct usb_rcpod *dev, unsigned short address);
static int rcpod_analog_all(struct usb_rcpod *dev, unsigned char *buffer);
static int rcpod_usart_txrx(struct usb_rcpod *dev, unsigned short address, unsigned char tx_bytes, unsigned char rx_bytes);

static int rcpod_open(struct inode *inode, struct file *file);
static int rcpod_release(struct inode *inode, struct file *file);
static int rcpod_dev_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static inline void rcpod_delete(struct usb_rcpod *dev);
static void * rcpod_probe(struct usb_device *udev, unsigned int ifnum, const struct usb_device_id *id);
static void rcpod_disconnect(struct usb_device *udev, void *ptr);
static int __init usb_rcpod_init(void);
static void __exit usb_rcpod_exit(void);


/******************************************************************************/
/************************************************** Device Communications *****/
/******************************************************************************/

static int rcpod_poke(struct usb_rcpod *dev, unsigned short address, unsigned short data)
{
	return usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
			       RCPOD_CTRL_POKE, 0x40, data, address,
			       NULL, 0, REQUEST_TIMEOUT);
}

static int rcpod_peek(struct usb_rcpod *dev, unsigned short address)
{
	unsigned char c;
	int result;
	result = usb_control_msg(dev->udev, usb_rcvctrlpipe(dev->udev, 0),
				 RCPOD_CTRL_PEEK, 0x40 | USB_DIR_IN, 0, address, &c, sizeof(c),
				 REQUEST_TIMEOUT);
	if (result < 0)
	  return result;
	return c;
}

static int rcpod_analog_all(struct usb_rcpod *dev, unsigned char *buffer)
{
	return usb_control_msg(dev->udev, usb_rcvctrlpipe(dev->udev, 0),
			       RCPOD_CTRL_ANALOG_ALL, 0x40 | USB_DIR_IN, 0, 0, buffer, 8,
			       REQUEST_TIMEOUT);
}

static int rcpod_usart_txrx(struct usb_rcpod *dev, unsigned short address, unsigned char tx_bytes, unsigned char rx_bytes)
{
	return usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
			       RCPOD_CTRL_USART_TXRX, 0x40, tx_bytes | (((int)rx_bytes) << 8), address,
			       NULL, 0, REQUEST_TIMEOUT);
}



/******************************************************************************/
/************************************************** Main character device *****/
/******************************************************************************/

static int rcpod_open(struct inode *inode, struct file *file)
{
	struct usb_rcpod *dev = NULL;
	int subminor;
	int retval = 0;

	subminor = MINOR(inode->i_rdev) - RCPOD_MINOR_BASE;
	if ((subminor < 0) ||
	   (subminor >= MAX_DEVICES)) {
		return -ENODEV;
	}

	/* lock our minor table and get our local data for this minor */
	down(&minor_table_mutex);
	dev = minor_table[subminor];
	if (dev == NULL) {
		up(&minor_table_mutex);
		return -ENODEV;
	}

	/* lock this device */
	down(&dev->sem);

	/* unlock the minor table */
	up(&minor_table_mutex);

	/* increment our usage count for the driver */
	++dev->open_count;

	/* save our object in the file's private structure */
	file->private_data = dev;

	/* unlock this device */
	up(&dev->sem);

	return retval;
}

static int rcpod_release(struct inode *inode, struct file *file)
{
	struct usb_rcpod *dev;
	int retval = 0;

	dev =(struct usb_rcpod *)file->private_data;
	if (dev == NULL) {
		dbg("object is NULL");
		return -ENODEV;
	}

	/* lock our minor table */
	down(&minor_table_mutex);

	/* lock our device */
	down(&dev->sem);

	if (dev->open_count <= 0) {
		dbg("device not opened");
		retval = -ENODEV;
		goto exit_not_opened;
	}

	if (dev->udev == NULL) {
		/* the device was unplugged before the file was released */
		up(&dev->sem);
		rcpod_delete(dev);
		up(&minor_table_mutex);
		return 0;
	}

	/* decrement our usage count for the device */
	--dev->open_count;
	if (dev->open_count <= 0) {
		dev->open_count = 0;
	}

exit_not_opened:
	up(&dev->sem);
	up(&minor_table_mutex);

	return retval;
}


static int rcpod_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct usb_rcpod *dev;
	int retval=0;
	struct rcpod_pair p;
	struct rcpod_txrx txrx;
	char analog_buffer[8];

	dev =(struct usb_rcpod *)file->private_data;

	/* lock this object */
	down(&dev->sem);

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		up(&dev->sem);
		return -ENODEV;
	}

	switch(cmd) {

	case RCPODIO_POKE:
		if (copy_from_user(&p,(struct rcpod_pair*) arg, sizeof(p))) {
			retval = -EFAULT;
			break;
		}
		retval = rcpod_poke(dev, p.address, p.data);
		break;

	case RCPODIO_PEEK:
		retval = rcpod_peek(dev, arg);
		break;

	case RCPODIO_ANALOG_ALL:
		retval = rcpod_analog_all(dev, analog_buffer);
		if (copy_to_user((unsigned char*) arg, analog_buffer, sizeof(analog_buffer))) {
			retval = -EFAULT;
			break;
		}
		break;

	case RCPODIO_USART_TXRX:
		if (copy_from_user(&txrx,(struct rcpod_txrx*) arg, sizeof(txrx))) {
			retval = -EFAULT;
			break;
		}
		retval = rcpod_usart_txrx(dev, txrx.address, txrx.tx_bytes, txrx.rx_bytes);
		break;


	default:
		/* Indicate that we didn't understand this ioctl */
		retval = -ENOTTY;
		break;
	}

	/* unlock the device */
	up(&dev->sem);

	return retval;
}

static struct file_operations rcpod_dev_fops = {
	owner:		THIS_MODULE,        /* This automates updating the module's use count */

	ioctl:		rcpod_ioctl,
	open:		rcpod_open,
	release:	rcpod_release,
};


/******************************************************************************/
/************************************************** USB Housekeeping **********/
/******************************************************************************/

static inline void rcpod_delete(struct usb_rcpod *dev)
{
	minor_table[dev->minor] = NULL;
	kfree(dev);
}

static struct usb_driver rcpod_driver = {
	name:		"rcpod",
	probe:		rcpod_probe,
	disconnect:	rcpod_disconnect,
	fops:		&rcpod_dev_fops,
	minor:		RCPOD_MINOR_BASE,
	id_table:	rcpod_table,
};

static void * rcpod_probe(struct usb_device *udev, unsigned int ifnum, const struct usb_device_id *id)
{
	struct usb_rcpod *dev = NULL;
	struct usb_interface *interface;
	struct usb_endpoint_descriptor *endpoint;
	int minor;
	int buffer_size;
	int i;
	char name[10];
	struct usb_interface_descriptor *iface_desc;

	/* See if the device offered us matches what we can accept */
	if ((udev->descriptor.idVendor != RCPOD_VENDOR_ID) ||
	   (udev->descriptor.idProduct != RCPOD_PRODUCT_ID)) {
		return NULL;
	}

	/* select a "subminor" number(part of a minor number) */
	down(&minor_table_mutex);
	for(minor = 0; minor < MAX_DEVICES; ++minor) {
		if (minor_table[minor] == NULL)
			break;
	}
	if (minor >= MAX_DEVICES) {
		info("Too many devices plugged in, can not handle this device.");
		goto exit;
	}

	/* allocate memory for our device state and intialize it */
	dev = kmalloc(sizeof(struct usb_rcpod), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		goto exit;
	}
	memset(dev, 0x00, sizeof(*dev));
	minor_table[minor] = dev;

	interface = &udev->actconfig->interface[ifnum];
	iface_desc = &interface->altsetting[interface->act_altsetting];
	endpoint = &iface_desc->endpoint[0];

	init_MUTEX(&dev->sem);
	dev->udev = udev;
	dev->interface = interface;
	dev->minor = minor;

	/* Initialize the devfs node for this device and register it */
	sprintf(name, RCPOD_DEV_NAMEFORMAT, dev->minor);
	dev->devfs = devfs_register(usb_devfs_handle, name,
				     DEVFS_FL_DEFAULT, USB_MAJOR,
				    RCPOD_MINOR_BASE + dev->minor,
				     S_IFCHR | S_IRUSR | S_IWUSR |
				     S_IRGRP | S_IWGRP | S_IROTH,
				     &rcpod_dev_fops, NULL);
	info("rcpod device now attached to %s", name);

	goto exit;

error:
	rcpod_delete(dev);
	dev = NULL;

exit:
	up(&minor_table_mutex);
	return dev;
}

static void rcpod_disconnect(struct usb_device *udev, void *ptr)
{
	struct usb_rcpod *dev;
	int minor;

	dev =(struct usb_rcpod *)ptr;

	down(&minor_table_mutex);
	down(&dev->sem);

	minor = dev->minor;

	/* remove our devfs node */
	devfs_unregister(dev->devfs);

	/* if the device is not opened, then we clean up right now */
	if (!dev->open_count) {
		up(&dev->sem);
		rcpod_delete(dev);
	} else {
		dev->udev = NULL;
		up(&dev->sem);
	}

	info("rcpod #%d now disconnected", minor);
	up(&minor_table_mutex);
}

static int __init usb_rcpod_init(void)
{
	int result;

	/* register this driver with the USB subsystem */
	result = usb_register(&rcpod_driver);
	if (result < 0) {
		err("usb_register failed for the "__FILE__" driver. Error number %d",
		    result);
		return -1;
	}

	info(DRIVER_DESC " " DRIVER_VERSION " - " DRIVER_AUTHOR);
	return 0;
}

static void __exit usb_rcpod_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&rcpod_driver);
}

module_init(usb_rcpod_init);
module_exit(usb_rcpod_exit);

/* The End */
