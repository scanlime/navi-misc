/*
 * USB Video Switch driver
 *
 * This is a module for the Linux kernel, providing a userspace interface
 * to the uvswitch hardware via the USB protocol described in uvswitch_protocol.h.
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
#include <uvswitch_dev.h>

#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR "Micah Dowty <micah@picogui.org>"
#define DRIVER_DESC "USB Video Switch driver"

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

/* Table of devices that work with this driver */
static struct usb_device_id uvswitch_table [] = {
	{ USB_DEVICE(UVSWITCH_VENDOR_ID, UVSWITCH_PRODUCT_ID) },
	{ } /* End table */
};
MODULE_DEVICE_TABLE(usb, uvswitch_table);


/* Structure to hold all of our device specific stuff */
struct usb_uvswitch {
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
static struct usb_uvswitch *minor_table[MAX_DEVICES];

/* lock to protect the minor_table structure */
static DECLARE_MUTEX(minor_table_mutex);

/* 1/2 second timeout for control requests */
#define REQUEST_TIMEOUT (HZ / 2)


/******************************************************************************/
/************************************************** Function Prototypes *******/
/******************************************************************************/

static int uvswitch_switch(struct usb_uvswitch *dev,
			   unsigned char video,
			   unsigned char w_audio,
			   unsigned char r_audio,
			   unsigned char bypass);

static int uvswitch_open(struct inode *inode, struct file *file);
static int uvswitch_release(struct inode *inode, struct file *file);
static int uvswitch_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t uvswitch_read(struct file *file, char *buffer, size_t count, loff_t *ppos);

static inline void uvswitch_delete(struct usb_uvswitch *dev);
static void * uvswitch_probe(struct usb_device *udev, unsigned int ifnum, const struct usb_device_id *id);
static void uvswitch_disconnect(struct usb_device *udev, void *ptr);
static int __init usb_uvswitch_init(void);
static void __exit usb_uvswitch_exit(void);


/******************************************************************************/
/************************************************** Device Communications *****/
/******************************************************************************/

static int uvswitch_switch(struct usb_uvswitch *dev,
			   unsigned char video,
			   unsigned char w_audio,
			   unsigned char r_audio,
			   unsigned char bypass)
{
	return usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
			       UVSWITCH_CTRL_SWITCH, 0x40,
			       video | (((int)bypass)<<8),
			       r_audio | (((int)w_audio)<<8),
			       NULL, 0, REQUEST_TIMEOUT);
}


/******************************************************************************/
/************************************************** Main character device *****/
/******************************************************************************/

static int uvswitch_open(struct inode *inode, struct file *file)
{
	struct usb_uvswitch *dev = NULL;
	int subminor;
	int retval = 0;

	subminor = MINOR(inode->i_rdev) - UVSWITCH_MINOR_BASE;
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

static int uvswitch_release(struct inode *inode, struct file *file)
{
	struct usb_uvswitch *dev;
	int retval = 0;

	dev =(struct usb_uvswitch *)file->private_data;
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
		uvswitch_delete(dev);
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


static int uvswitch_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct usb_uvswitch *dev;
	int retval=0;

	dev =(struct usb_uvswitch *)file->private_data;

	/* lock this object */
	down(&dev->sem);

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		up(&dev->sem);
		return -ENODEV;
	}

	switch(cmd) {


	default:
		/* Indicate that we didn't understand this ioctl */
		retval = -ENOTTY;
		break;
	}

	/* unlock the device */
	up(&dev->sem);

	return retval;
}

static ssize_t uvswitch_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	struct usb_uvswitch *dev = (struct usb_uvswitch *)file->private_data;
	int retval = 0;

	/* lock this object */
	down (&dev->sem);

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		retval = -ENODEV;
		goto exit;
	}

	/* Not implemented yet :) */
	retval = -EFAULT;

exit:
	/* unlock the device */
	up(&dev->sem);
	return retval;
}

static ssize_t uvswitch_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	struct usb_uvswitch *dev = (struct usb_uvswitch *)file->private_data;
	int retval = 0;
	char kbuffer[256];
	unsigned char video, w_audio, r_audio, bypass;

	/* lock this object */
	down(&dev->sem);

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		retval = -ENODEV;
		goto exit;
	}

	/* Copy the write to our buffer, without overflowing it */
	if (count > sizeof(kbuffer)-1)
		count = sizeof(kbuffer)-1;
	if (copy_from_user(kbuffer, buffer, count)) {
		retval = -EFAULT;
		goto exit;
	}
	kbuffer[count] = 0;
	retval = count;

	video = simple_strtol(kbuffer, NULL, 10);
	w_audio = video;
	r_audio = video;
	bypass = 0;

	uvswitch_switch(dev, video, w_audio, r_audio, bypass);

exit:
	/* unlock the device */
	up(&dev->sem);

	return retval;
}


static struct file_operations uvswitch_dev_fops = {
	owner:		THIS_MODULE,        /* This automates updating the module's use count */

	ioctl:		uvswitch_ioctl,
	open:		uvswitch_open,
	release:	uvswitch_release,
	read:		uvswitch_read,
	write:		uvswitch_write,
};


/******************************************************************************/
/************************************************** USB Housekeeping **********/
/******************************************************************************/

static inline void uvswitch_delete(struct usb_uvswitch *dev)
{
	minor_table[dev->minor] = NULL;
	kfree(dev);
}

static struct usb_driver uvswitch_driver = {
	name:		"uvswitch",
	probe:		uvswitch_probe,
	disconnect:	uvswitch_disconnect,
	fops:		&uvswitch_dev_fops,
	minor:		UVSWITCH_MINOR_BASE,
	id_table:	uvswitch_table,
};

static void * uvswitch_probe(struct usb_device *udev, unsigned int ifnum, const struct usb_device_id *id)
{
	struct usb_uvswitch *dev = NULL;
	struct usb_interface *interface;
	struct usb_endpoint_descriptor *endpoint;
	int minor;
	int buffer_size;
	int i;
	char name[10];
	struct usb_interface_descriptor *iface_desc;

	/* See if the device offered us matches what we can accept */
	if ((udev->descriptor.idVendor != UVSWITCH_VENDOR_ID) ||
	   (udev->descriptor.idProduct != UVSWITCH_PRODUCT_ID)) {
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
	dev = kmalloc(sizeof(struct usb_uvswitch), GFP_KERNEL);
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
	sprintf(name, UVSWITCH_DEV_NAMEFORMAT, dev->minor);
	dev->devfs = devfs_register(usb_devfs_handle, name,
				     DEVFS_FL_DEFAULT, USB_MAJOR,
				    UVSWITCH_MINOR_BASE + dev->minor,
				     S_IFCHR | S_IRUSR | S_IWUSR |
				     S_IRGRP | S_IWGRP | S_IROTH,
				     &uvswitch_dev_fops, NULL);
	info("uvswitch device now attached to %s", name);

	goto exit;

error:
	uvswitch_delete(dev);
	dev = NULL;

exit:
	up(&minor_table_mutex);
	return dev;
}

static void uvswitch_disconnect(struct usb_device *udev, void *ptr)
{
	struct usb_uvswitch *dev;
	int minor;

	dev =(struct usb_uvswitch *)ptr;

	down(&minor_table_mutex);
	down(&dev->sem);

	minor = dev->minor;

	/* remove our devfs node */
	devfs_unregister(dev->devfs);

	/* if the device is not opened, then we clean up right now */
	if (!dev->open_count) {
		up(&dev->sem);
		uvswitch_delete(dev);
	} else {
		dev->udev = NULL;
		up(&dev->sem);
	}

	info("uvswitch #%d now disconnected", minor);
	up(&minor_table_mutex);
}

static int __init usb_uvswitch_init(void)
{
	int result;

	/* register this driver with the USB subsystem */
	result = usb_register(&uvswitch_driver);
	if (result < 0) {
		err("usb_register failed for the "__FILE__" driver. Error number %d",
		    result);
		return -1;
	}

	info(DRIVER_DESC " " DRIVER_VERSION " - " DRIVER_AUTHOR);
	return 0;
}

static void __exit usb_uvswitch_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&uvswitch_driver);
}

module_init(usb_uvswitch_init);
module_exit(usb_uvswitch_exit);

/* The End */
