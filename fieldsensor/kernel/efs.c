/*
 * USB Electric Field Sensor driver
 *
 * This is a module for the Linux kernel, providing a userspace interface
 * to the field sensor hardware via the USB protocol described in efs_protocol.h.
 *
 * Copyright(c) 2004 Micah Dowty <micah@picogui.org>
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
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/smp_lock.h>
#include <linux/completion.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include <efs_dev.h>

#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR  "Micah Dowty <micah@navi.cx>"
#define DRIVER_DESC    "USB Electric Field Sensor driver"

/* Private data for one field sensor device */
struct field_sensor {
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char		minor;			/* the starting minor number for this device */

	struct urb		urb;		        /* URB for interrupt transfers*/

	int                     open_count;
	struct semaphore	sem;			/* locks this structure */
};

/* Private data for each file descriptor */
struct efs_fd_private {
	struct field_sensor	*dev;
};

/* prevent races between open() and disconnect() */
static DECLARE_MUTEX (disconnect_sem);

/* 1 second timeout for control requests */
#define REQUEST_TIMEOUT HZ


/******************************************************************************/
/************************************************** Function Prototypes *******/
/******************************************************************************/

static ssize_t efs_read	       (struct file *file, char *buffer, size_t count, loff_t *ppos);
static int     efs_ioctl       (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int     efs_open        (struct inode *inode, struct file *file);
static int     efs_release     (struct inode *inode, struct file *file);

static int     efs_probe       (struct usb_interface *interface, const struct usb_device_id *id);
static void    efs_disconnect  (struct usb_interface *interface);

static void    efs_irq	       (struct urb *urb, struct pt_regs *regs);

static void    efs_delete      (struct field_sensor *dev);


/******************************************************************************/
/*************************************************************** Tables *******/
/******************************************************************************/

/* Table of devices that work with this driver */
static struct usb_device_id efs_table [] = {
	{ USB_DEVICE(EFS_VENDOR_ID, EFS_PRODUCT_ID) },
	{ } /* End table */
};
MODULE_DEVICE_TABLE(usb, efs_table);


static struct file_operations efs_dev_fops = {
	.owner      =  THIS_MODULE,        /* This automates updating the module's use count */
	.ioctl      =  efs_ioctl,
	.open       =  efs_open,
	.release    =  efs_release,
	.read       =  efs_read,
};

static struct usb_class_driver efs_class = {
	.name       =  EFS_DEV_NAMEFORMAT,
	.fops       =  &efs_dev_fops,
	.mode       =  S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
	.minor_base =  EFS_MINOR_BASE,
};

static struct usb_driver efs_driver = {
	.name       =  "efs",
	.probe      =  efs_probe,
	.disconnect =  efs_disconnect,
	.id_table   =  efs_table,
};


/******************************************************************************/
/************************************************** Device Communications *****/
/******************************************************************************/

static void efs_irq (struct urb *urb, struct pt_regs *regs)
{
	/* Callback for processing incoming interrupt transfers from the IR receiver */
	struct field_sensor *dev = (struct field_sensor*)urb->context;

	if (dev && urb->status == 0 && urb->actual_length == 8) {
		dbg("Wibbly %d\n",  urb->data[0]);
	}
	else {
		dbg("Bad URB status/size: status=%d, length=%d", urb->status, urb->actual_length);
	}
}


/******************************************************************************/
/******************************************************* Character device *****/
/******************************************************************************/

static int efs_open(struct inode *inode, struct file *file)
{
	struct field_sensor *dev = NULL;
	struct efs_fd_private *prv = NULL;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);

	/* Prevent disconnects */
	down(&disconnect_sem);

	interface = usb_find_interface (&efs_driver, subminor);
	if (!interface) {
		err ("%s - error, can't find device for minor %d",
		     __FUNCTION__, subminor);
		retval = -ENODEV;
		goto exit_no_device;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		retval = -ENODEV;
		goto exit_no_device;
	}

	/* lock this device */
	down (&dev->sem);

	/* increment our usage count for the driver */
	++dev->open_count;

	/* Allocate a private data struct for this fd */
	prv = kmalloc(sizeof(struct efs_fd_private), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		retval = -ENOMEM;
		goto exit;
	}

	/* save our object in the file's private structure */
	memset(prv, 0, sizeof(struct efs_fd_private));
	prv->dev = dev;
	file->private_data = prv;

exit:
	/* unlock this device */
	up (&dev->sem);

exit_no_device:
	up (&disconnect_sem);
	return retval;
}

static int efs_release(struct inode *inode, struct file *file)
{
	struct efs_fd_private *prv;
	struct field_sensor *dev;
	int retval = 0;

	prv =(struct efs_fd_private *)file->private_data;
	if (prv == NULL) {
		dbg("object is NULL");
		return -ENODEV;
	}
	dev = prv->dev;

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
		efs_delete(dev);
		return 0;
	}

	/* decrement our usage count for the device */
	--dev->open_count;
	if (dev->open_count <= 0) {
		dev->open_count = 0;
	}

	/* Free this device's private data */
	kfree(prv);
	file->private_data = NULL;

exit_not_opened:
	up(&dev->sem);
	return retval;
}


static int efs_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct efs_fd_private *prv;
	struct field_sensor *dev;
	int retval = 0;

	prv =(struct efs_fd_private *)file->private_data;
	if (prv == NULL) {
		dbg("object is NULL");
		return -ENODEV;
	}
	dev = prv->dev;

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

static ssize_t efs_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	struct efs_fd_private *prv;
	struct field_sensor *dev;
	int retval = 0;

	prv =(struct efs_fd_private *)file->private_data;
	if (prv == NULL) {
		dbg("object is NULL");
		return -ENODEV;
	}
	dev = prv->dev;

	/* lock this object */
	down (&dev->sem);

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		retval = -ENODEV;
		goto exit;
	}

	retval = 0;

exit:
	/* unlock the device */
	up(&dev->sem);
	return retval;
}


/******************************************************************************/
/************************************************** USB Housekeeping **********/
/******************************************************************************/

static void efs_delete(struct field_sensor *dev)
{
	kfree(dev);
}

static int efs_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct field_sensor *dev = NULL;
	int retval;

	/* See if the device offered us matches what we can accept */
	if ((udev->descriptor.idVendor != EFS_VENDOR_ID) ||
	   (udev->descriptor.idProduct != EFS_PRODUCT_ID)) {
		return -ENODEV;
	}

	/* allocate memory for our device state and intialize it */
	dev = kmalloc(sizeof(struct field_sensor), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		return -ENOMEM;
	}
	memset(dev, 0, sizeof(*dev));

	init_MUTEX(&dev->sem);
	dev->udev = udev;
	dev->interface = interface;

	usb_set_intfdata(interface, dev);
	retval = usb_register_dev(interface, &efs_class);
	if (retval) {
		/* something prevented us from registering this driver */
		err ("Not able to get a minor for this device.");
		usb_set_intfdata (interface, NULL);
		goto error;
	}

	dev->minor = interface->minor;
	info("Electric Field Sensor device now attached to /dev/" EFS_DEV_NAMEFORMAT, dev->minor);
	return 0;

error:
	efs_delete(dev);
	return retval;
}

static void efs_disconnect(struct usb_interface *interface)
{
	struct field_sensor *dev;
	int minor;

	/* prevent races with open() */
	down (&disconnect_sem);

	dev = usb_get_intfdata (interface);
	usb_set_intfdata (interface, NULL);

	down (&dev->sem);

	minor = dev->minor;
	usb_deregister_dev(interface, &efs_class);

	/* if the device is not opened, then we clean up right now */
	if (!dev->open_count) {
		up(&dev->sem);
		efs_delete(dev);
	} else {
		dev->udev = NULL;
		up(&dev->sem);
	}

	up(&disconnect_sem);
	info("efs #%d now disconnected", minor);
}

static int __init efs_init(void)
{
	int result = 0;

	result = usb_register(&efs_driver);
	if (result < 0) {
		err("usb_register failed. Error number %d", result);
	}
	else {
		info(DRIVER_DESC " " DRIVER_VERSION " - " DRIVER_AUTHOR);
	}

	return result;
}

static void __exit efs_exit(void)
{
	usb_deregister(&efs_driver);
}

module_init(efs_init);
module_exit(efs_exit);

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

/* The End */
