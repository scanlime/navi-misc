/*
 * Media Infrawidget 6000 driver
 *
 * This is a module for the Linux kernel, providing a userspace interface
 * as described in mi6k_dev.h to the USB protocol described in mi6k_protocol.h
 *
 * Copyright(c) 2003 Micah Dowty <micah@picogui.org>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */


/************************************************** Main Definitions **********/

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
#include <mi6k_protocol.h>
#include <mi6k_dev.h>

#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR "Micah Dowty <micah@picogui.org>"
#define DRIVER_DESC "Media Infrawidget 6000 driver"

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

/* Table of devices that work with this driver */
static struct usb_device_id mi6k_table [] = {
	{ USB_DEVICE(MI6K_VENDOR_ID, MI6K_PRODUCT_ID) },
	{ } /* End table */
};
MODULE_DEVICE_TABLE (usb, mi6k_table);

/* Structure to hold all of our device specific stuff */
struct usb_mi6k {
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct usb_interface *	interface;		/* the interface for this device */
	devfs_handle_t		devfs;			/* devfs device node */
	unsigned char		minor;			/* the starting minor number for this device */
	int			open_count;		/* number of times this port has been opened */
	struct semaphore	sem;			/* locks this structure */
};

/* the global usb devfs handle */
extern devfs_handle_t usb_devfs_handle;

/* we can have up to this number of device plugged in at once */
#define MAX_DEVICES 16

/* array of pointers to our devices that are currently connected */
static struct usb_mi6k *minor_table[MAX_DEVICES];

/* lock to protect the minor_table structure */
static DECLARE_MUTEX (minor_table_mutex);

/* 1/2 second timeout for control requests */
#define REQUEST_TIMEOUT  (HZ / 2)

/* Wait 1/8 second after powering the VFD on or off */
#define VFD_POWER_DELAY  (HZ / 8)


/************************************************** Function Prototypes *******/

static void mi6k_request (struct usb_mi6k *dev, unsigned short request,
			  unsigned short wValue, unsigned short wIndex);
static int mi6k_open (struct inode *inode, struct file *file);
static int mi6k_release (struct inode *inode, struct file *file);
static ssize_t mi6k_write (struct file *file, const char *buffer, size_t count, loff_t *ppos);
static int mi6k_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static inline void mi6k_delete (struct usb_mi6k *dev);
static void * mi6k_probe(struct usb_device *udev, unsigned int ifnum, const struct usb_device_id *id);
static void mi6k_disconnect(struct usb_device *udev, void *ptr);
static int __init usb_mi6k_init(void);
static void __exit usb_mi6k_exit(void);


/************************************************** Device Communications *****/

static void mi6k_request (struct usb_mi6k *dev, unsigned short request,
			  unsigned short wValue, unsigned short wIndex)
{
	/* Send a control request to the device synchronously */
	usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
			request, 0x40, wValue, wIndex, NULL, 0, REQUEST_TIMEOUT);
}


/************************************************** Main character device *****/

static int mi6k_open (struct inode *inode, struct file *file)
{
	struct usb_mi6k *dev = NULL;
	int subminor;
	int retval = 0;

	subminor = MINOR (inode->i_rdev) - MI6K_MINOR_BASE;
	if ((subminor < 0) ||
	    (subminor >= MAX_DEVICES)) {
		return -ENODEV;
	}

	/* Increment our usage count for the module.
	 * This is redundant here, because "struct file_operations"
	 * has an "owner" field. This line is included here soley as
	 * a reference for drivers using lesser structures... ;-)
	 */
	MOD_INC_USE_COUNT;

	/* lock our minor table and get our local data for this minor */
	down (&minor_table_mutex);
	dev = minor_table[subminor];
	if (dev == NULL) {
		up (&minor_table_mutex);
		MOD_DEC_USE_COUNT;
		return -ENODEV;
	}

	/* lock this device */
	down (&dev->sem);

	/* unlock the minor table */
	up (&minor_table_mutex);

	/* increment our usage count for the driver */
	++dev->open_count;

	/* save our object in the file's private structure */
	file->private_data = dev;

	/* unlock this device */
	up (&dev->sem);

	return retval;
}

static int mi6k_release (struct inode *inode, struct file *file)
{
	struct usb_mi6k *dev;
	int retval = 0;

	dev = (struct usb_mi6k *)file->private_data;
	if (dev == NULL) {
		dbg ("object is NULL");
		return -ENODEV;
	}

	dbg("minor %d", dev->minor);

	/* lock our minor table */
	down (&minor_table_mutex);

	/* lock our device */
	down (&dev->sem);

	if (dev->open_count <= 0) {
		dbg ("device not opened");
		retval = -ENODEV;
		goto exit_not_opened;
	}

	if (dev->udev == NULL) {
		/* the device was unplugged before the file was released */
		up (&dev->sem);
		mi6k_delete (dev);
		up (&minor_table_mutex);
		MOD_DEC_USE_COUNT;
		return 0;
	}

	/* decrement our usage count for the device */
	--dev->open_count;
	if (dev->open_count <= 0) {
		/* Unlink URBs here once we have some */
		dev->open_count = 0;
	}

	/* decrement our usage count for the module */
	MOD_DEC_USE_COUNT;

exit_not_opened:
	up (&dev->sem);
	up (&minor_table_mutex);

	return retval;
}

static ssize_t mi6k_write (struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	/* Pad unused bytes with zero, which the display will ignore */
	unsigned char tbuffer[] = {0, 0, 0, 0};
	struct usb_mi6k *dev;
	ssize_t bytes_written = 0;
	int retval = 0;

	dev = (struct usb_mi6k *)file->private_data;

	dbg("minor %d, count = %d", dev->minor, count);

	/* lock this object */
	down (&dev->sem);

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		retval = -ENODEV;
		goto exit;
	}

	/* verify that we actually have some data to write */
	if (count == 0) {
		dbg("write request of 0 bytes");
		goto exit;
	}

	bytes_written = min(sizeof(tbuffer), count);
	dbg("%d bytes to send, sending %d bytes", count, bytes_written);
	if (copy_from_user(tbuffer, buffer, bytes_written)) {
		retval = -EFAULT;
		goto exit;
	}

	/* Pack 4 bytes of the character stream into the packet's value and index parameters */
	mi6k_request(dev, MI6K_CTRL_VFD_WRITE,
		     tbuffer[0] | (((int)tbuffer[1]) << 8),
		     tbuffer[2] | (((int)tbuffer[3]) << 8));

	retval = bytes_written;

exit:
	/* unlock the device */
	up (&dev->sem);

	return retval;
}

static int mi6k_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct usb_mi6k *dev;
	int retval=0;
	struct mi6k_leds leds;

	dev = (struct usb_mi6k *)file->private_data;

	/* lock this object */
	down (&dev->sem);

	/* verify that the device wasn't unplugged */
	if (dev->udev == NULL) {
		up (&dev->sem);
		return -ENODEV;
	}

	dbg("minor %d, cmd 0x%.4x, arg %ld", 
	    dev->minor, cmd, arg);

	switch (cmd) {

	case MI6KIO_VFD_POWER:
		/* Issue a blocking power command, then wait a bit for the device to power on/off.
		 * Note that the device is locked during this time, preventing multiple processes
		 * from potentially damaging the VFD by switching it on and off rapidly. This also
		 * prevents writes to the VFD before it's initialized.
		 */
		mi6k_request(dev, MI6K_CTRL_VFD_POWER, arg ? 1 : 0, 0);
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(VFD_POWER_DELAY);
		break;

	case MI6KIO_LED_SET:
		/* Set the MI6K's LEDs from a mi6k_leds struct. The struct uses the full range of
		 * a u16 to hide the details of the PWM resolution. We chop off 6 bits to quantize
		 * into the PWM's 10-bit resolution.
		 */
		if (copy_from_user(&leds, (struct mi6k_leds*) arg, sizeof(leds))) {
			retval = -EFAULT;
			break;
		}
		mi6k_request(dev, MI6K_CTRL_LED_SET, leds.blue >> 6, leds.white >> 6);
		break;

	default:
		/* Indicate that we didn't understand this ioctl */
		retval = -ENOTTY;
		break;
	}

	/* unlock the device */
	up (&dev->sem);

	return retval;
}

static struct file_operations mi6k_fops = {
	/*
	 * The owner field is part of the module-locking
	 * mechanism. The idea is that the kernel knows
	 * which module to increment the use-counter of
	 * BEFORE it calls the device's open() function.
	 * This also means that the kernel can decrement
	 * the use-counter again before calling release()
	 * or should the open() function fail.
	 *
	 * Not all device structures have an "owner" field
	 * yet. "struct file_operations" and "struct net_device"
	 * do, while "struct tty_driver" does not. If the struct
	 * has an "owner" field, then initialize it to the value
	 * THIS_MODULE and the kernel will handle all module
	 * locking for you automatically. Otherwise, you must
	 * increment the use-counter in the open() function
	 * and decrement it again in the release() function
	 * yourself.
	 */
	owner:		THIS_MODULE,

	write:		mi6k_write,
	ioctl:		mi6k_ioctl,
	open:		mi6k_open,
	release:	mi6k_release,
};


/************************************************** USB Housekeeping **********/

static inline void mi6k_delete (struct usb_mi6k *dev)
{
	minor_table[dev->minor] = NULL;
	kfree (dev);
}

static struct usb_driver mi6k_driver = {
	name:		"mi6k",
	probe:		mi6k_probe,
	disconnect:	mi6k_disconnect,
	fops:		&mi6k_fops,
	minor:		MI6K_MINOR_BASE,
	id_table:	mi6k_table,
};

static void * mi6k_probe(struct usb_device *udev, unsigned int ifnum, const struct usb_device_id *id)
{
	struct usb_mi6k *dev = NULL;
	struct usb_interface *interface;
	struct usb_interface_descriptor *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int minor;
	int buffer_size;
	int i;
	char name[10];

	/* See if the device offered us matches what we can accept */
	if ((udev->descriptor.idVendor != MI6K_VENDOR_ID) ||
	    (udev->descriptor.idProduct != MI6K_PRODUCT_ID)) {
		return NULL;
	}

	/* select a "subminor" number (part of a minor number) */
	down (&minor_table_mutex);
	for (minor = 0; minor < MAX_DEVICES; ++minor) {
		if (minor_table[minor] == NULL)
			break;
	}
	if (minor >= MAX_DEVICES) {
		info ("Too many devices plugged in, can not handle this device.");
		goto exit;
	}

	/* allocate memory for our device state and intialize it */
	dev = kmalloc (sizeof(struct usb_mi6k), GFP_KERNEL);
	if (dev == NULL) {
		err ("Out of memory");
		goto exit;
	}
	memset (dev, 0x00, sizeof (*dev));
	minor_table[minor] = dev;

	interface = &udev->actconfig->interface[ifnum];

	init_MUTEX (&dev->sem);
	dev->udev = udev;
	dev->interface = interface;
	dev->minor = minor;

	/* initialize the devfs node for this device and register it */
	sprintf(name, MI6K_DEV_NAMEFORMAT, dev->minor);
	dev->devfs = devfs_register (usb_devfs_handle, name,
				     DEVFS_FL_DEFAULT, USB_MAJOR,
				     MI6K_MINOR_BASE + dev->minor,
				     S_IFCHR | S_IRUSR | S_IWUSR |
				     S_IRGRP | S_IWGRP | S_IROTH,
				     &mi6k_fops, NULL);

	/* let the user know what node this device is now attached to */
	info ("MI6K device now attached to %s", name);
	goto exit;

error:
	mi6k_delete (dev);
	dev = NULL;

exit:
	up (&minor_table_mutex);
	return dev;
}

static void mi6k_disconnect(struct usb_device *udev, void *ptr)
{
	struct usb_mi6k *dev;
	int minor;

	dev = (struct usb_mi6k *)ptr;

	down (&minor_table_mutex);
	down (&dev->sem);

	minor = dev->minor;

	/* remove our devfs node */
	devfs_unregister(dev->devfs);

	/* if the device is not opened, then we clean up right now */
	if (!dev->open_count) {
		up (&dev->sem);
		mi6k_delete (dev);
	} else {
		dev->udev = NULL;
		up (&dev->sem);
	}

	info("MI6K #%d now disconnected", minor);
	up (&minor_table_mutex);
}

static int __init usb_mi6k_init(void)
{
	int result;

	/* register this driver with the USB subsystem */
	result = usb_register(&mi6k_driver);
	if (result < 0) {
		err("usb_register failed for the "__FILE__" driver. Error number %d",
		    result);
		return -1;
	}

	info(DRIVER_DESC " " DRIVER_VERSION);
	return 0;
}

static void __exit usb_mi6k_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&mi6k_driver);
}

module_init (usb_mi6k_init);
module_exit (usb_mi6k_exit);

/* The End */
