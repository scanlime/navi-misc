/*
 * Raster Wand driver
 *
 * This is a module for the Linux kernel, providing a userspace interface
 * to the Raster Wand hardware via the USB protocol described in efs_protocol.h.
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
#include <rwand_dev.h>

#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR  "Micah Dowty <micah@navi.cx>"
#define DRIVER_DESC    "Raster Wand driver"


/* Status packets following this format are returned from the
 * device when it's asked, or from the EP1 interrupt endpoint.
 */
struct rwand_status {
	unsigned short          period;                 /* Period estimation, in 2.66us units */
	unsigned short          phase;                  /* Phase estimation, in 2.66us units */
	unsigned char           edge_count;             /* 8-bit counter of sync edges */
	unsigned char           mode;                   /* RWAND_MODE_* bits */
	unsigned char           flip_count;             /* 8-bit page flip counter */
	unsigned char           buttons;                /* RWAND_BUTTON_* bits */
};

struct rwand_dev {
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char		minor;			/* the starting minor number for this device */

	struct urb *		irq;		        /* URB for interrupt transfers*/
	unsigned char *         irq_data;
	dma_addr_t              irq_dma;

	/* Our local model of the device state */
	int                     fb_size;                /* The device's current framebuffer size */
	unsigned char           fb[RWAND_FB_BYTES];     /* Our local copy of the device framebuffer */
	struct rwand_status     status;

	int                     open_count;
	struct semaphore	sem;			/* locks this structure */
};

/* Private data for each file descriptor */
struct rwand_fd_private {
	struct rwand_dev	*dev;
};


/* prevent races between open() and disconnect() */
static DECLARE_MUTEX (disconnect_sem);

/* 1 second timeout for control requests */
#define REQUEST_TIMEOUT HZ

#define IRQ_INTERVAL       10          /* Polling interval, in milliseconds */
#define STATUS_PACKET_SIZE 8
#define IRQ_SIZE           STATUS_PACKET_SIZE


/******************************************************************************/
/************************************************** Function Prototypes *******/
/******************************************************************************/

static ssize_t rwand_write       (struct file *file, const char *buffer, size_t count, loff_t *ppos);
static ssize_t rwand_read        (struct file *file, char *buffer, size_t count, loff_t *ppos);
static int     rwand_ioctl       (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int     rwand_open        (struct inode *inode, struct file *file);
static int     rwand_release     (struct inode *inode, struct file *file);

static int     rwand_probe       (struct usb_interface *interface, const struct usb_device_id *id);
static void    rwand_disconnect  (struct usb_interface *interface);

static void    rwand_irq         (struct urb *urb, struct pt_regs *regs);

static void    rwand_delete      (struct rwand_dev *dev);

static void    rwand_request     (struct rwand_dev *dev, unsigned short request,
				  unsigned short wValue, unsigned short wIndex);
static int     rwand_wait_for_fb (struct rwand_dev *dev);
static void    rwand_update_fb   (struct rwand_dev *dev, int bytes, unsigned char *fb);
static void    rwand_request_flip(struct rwand_dev *dev);

static void    rwand_decode_status (const unsigned char *in, struct rwand_status *out);
static void    rwand_process_status(struct rwand_dev *dev, const unsigned char *packet);
static void    rwand_poll_status   (struct rwand_dev *dev);


/******************************************************************************/
/*************************************************************** Tables *******/
/******************************************************************************/

/* Table of devices that work with this driver */
static struct usb_device_id rwand_table [] = {
	{ USB_DEVICE(RWAND_VENDOR_ID, RWAND_PRODUCT_ID) },
	{ } /* End table */
};
MODULE_DEVICE_TABLE(usb, rwand_table);


static struct file_operations rwand_dev_fops = {
	.owner      =  THIS_MODULE,
	.ioctl      =  rwand_ioctl,
	.open       =  rwand_open,
	.release    =  rwand_release,
	.read       =  rwand_read,
	.write      =  rwand_write,
};

static struct usb_class_driver rwand_class = {
	.name       =  RWAND_DEV_NAMEFORMAT,
	.fops       =  &rwand_dev_fops,
	.mode       =  S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
	.minor_base =  RWAND_MINOR_BASE,
};

static struct usb_driver rwand_driver = {
	.name       =  "rwand",
	.probe      =  rwand_probe,
	.disconnect =  rwand_disconnect,
	.id_table   =  rwand_table,
};


/******************************************************************************/
/************************************************** Device Communications *****/
/******************************************************************************/

static void rwand_irq (struct urb *urb, struct pt_regs *regs)
{
	/* Callback for processing incoming interrupt transfers from the IR receiver */
	struct rwand_dev *dev = (struct rwand_dev*)urb->context;

	dbg("update: %d %d %d %d %d %d %d %d",
	    dev->irq_data[0], dev->irq_data[1], dev->irq_data[2], dev->irq_data[3],
	    dev->irq_data[4], dev->irq_data[5], dev->irq_data[6], dev->irq_data[7]);

	switch (urb->status) {
	case -ECONNRESET:
		err("ECONNRESET in rwand_irq, connection reset?");
		break;
	case -ENOENT:
		err("ENOENT in rwand_irq");
		return;
	case -ESHUTDOWN:
		err("ESHUTDOWN in rwand_irq");
		return;
	}

	if (urb->status != 0 || urb->actual_length != IRQ_SIZE) {
		err("Bad URB status/size: status=%d, length=%d", urb->status, urb->actual_length);
		//return;
	}

	/* Resubmit the URB to get another interrupt transfer going */
	usb_submit_urb(urb, SLAB_ATOMIC);
}


/* Decode a status packet, store the results in 'out' */
static void    rwand_decode_status(const unsigned char *in, struct rwand_status *out)
{
	out->period = in[0] + (in[1] << 8);
	out->phase = in[2] + (in[3] << 8);
	out->edge_count = in[4];
	out->mode = in[5];
	out->flip_count = in[6];
	out->buttons = in[7];
}


/* Given an encoded status packet, process it and update the
 * device's state in any way necessary. This may be called from interrupt
 * context or from process context, since status packets can be received
 * by request or through our interrupt endpoint.
 */
static void    rwand_process_status(struct rwand_dev *dev, const unsigned char *packet)
{
	rwand_decode_status(packet, &dev->status);
	dbg("Buttons: %02X", dev->status.buttons);
}


/* Perform a blocking request for a new status packet,
 * processing it on arrival.
 */
static void    rwand_poll_status   (struct rwand_dev *dev)
{
	unsigned char packet[STATUS_PACKET_SIZE];
	int retval;

	retval = usb_control_msg(dev->udev, usb_rcvctrlpipe(dev->udev, 0),
				 RWAND_CTRL_READ_STATUS, USB_TYPE_VENDOR | USB_DIR_IN,
				 0, 0, packet, sizeof(packet), REQUEST_TIMEOUT);
	if (retval) {
		err("Error polling status, retval=%d", retval);
		return;
	}
	rwand_process_status(dev, packet);
}


/* Send a generic blocking control request */
static void rwand_request(struct rwand_dev *dev, unsigned short request,
			  unsigned short wValue, unsigned short wIndex)
{
	int retval;
	retval = usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
				 request, 0x40, wValue, wIndex, NULL, 0, REQUEST_TIMEOUT);
	if (retval) {
		dbg("Error in rwand_request, retval=%d", retval);
	}
}


/* Wait for any pending page flips to finish. This can return an error
 * if the device is disconnected or we're interrupted while waiting.
 * This should be called while holding the device lock.
 */
static int rwand_wait_for_fb(struct rwand_dev *dev)
{
	return 0;
}


/* Change the device's framebuffer size and/or contents to match the one given.
 * This should be called while holding the device lock.
 */
static void rwand_update_fb(struct rwand_dev *dev, int bytes, unsigned char *fb)
{
	int retval;

	if (bytes != dev->fb_size) {
		rwand_request(dev, RWAND_CTRL_SET_NUM_COLUMNS, bytes, 0);
		dev->fb_size = bytes;
	}

	/*
	retval = usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
				 RWAND_CTRL_SEQ_WRITE12, 0x40,
				 fb[0] | (fb[1] << 8),
				 fb[2] | (fb[3] << 8),
				 &fb[4], 9, REQUEST_TIMEOUT);
	dbg("retval=%d in 12-byte write", retval);
	*/

	rwand_poll_status(dev);
}


/* Request a page flip. No display writes should be made until the
 * page flip completes. This can be checked for using rwand_wait_for_fb().
 */
static void rwand_request_flip(struct rwand_dev *dev)
{
	rwand_request(dev, RWAND_CTRL_FLIP, 0, 0);
}


/******************************************************************************/
/******************************************************* Character device *****/
/******************************************************************************/

static int rwand_open(struct inode *inode, struct file *file)
{
	struct rwand_dev *dev = NULL;
	struct rwand_fd_private *prv = NULL;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);

	/* Prevent disconnects */
	down(&disconnect_sem);

	interface = usb_find_interface (&rwand_driver, subminor);
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
	prv = kmalloc(sizeof(struct rwand_fd_private), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		retval = -ENOMEM;
		goto exit;
	}

	/* save our object in the file's private structure */
	memset(prv, 0, sizeof(struct rwand_fd_private));
	prv->dev = dev;
	file->private_data = prv;

	/* If the device wasn't being used before this open, we need to submit
	 * a URB to start the interrupt requests flowing.
	 */
	if (dev->open_count == 1) {
		//usb_submit_urb(dev->irq, GFP_KERNEL);
	}

exit:
	/* unlock this device */
	up (&dev->sem);

exit_no_device:
	up (&disconnect_sem);
	return retval;
}

static int rwand_release(struct inode *inode, struct file *file)
{
	struct rwand_fd_private *prv;
	struct rwand_dev *dev;
	int retval = 0;

	prv =(struct rwand_fd_private *)file->private_data;
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
		rwand_delete(dev);
		return 0;
	}

	/* decrement our usage count for the device */
	--dev->open_count;
	if (dev->open_count == 0) {
		/* We're the last ones, stop the flow of IRQ requests */
		usb_unlink_urb(dev->irq);
	}

	/* Free this device's private data */
	kfree(prv);
	file->private_data = NULL;

exit_not_opened:
	up(&dev->sem);
	return retval;
}


static int rwand_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct rwand_fd_private *prv;
	struct rwand_dev *dev;
	int retval = 0;

	prv =(struct rwand_fd_private *)file->private_data;
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


static ssize_t rwand_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	struct rwand_fd_private *prv;
	struct rwand_dev *dev;
	int retval = 0;
	int bytes_written;
	unsigned char new_fb[RWAND_FB_BYTES];

	prv =(struct rwand_fd_private *)file->private_data;
	if (prv == NULL) {
		dbg("object is NULL");
		return -ENODEV;
	}
	dev = prv->dev;

	/* lock this object */
	down(&dev->sem);

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

	bytes_written = min(sizeof(new_fb), count);
	if (copy_from_user(dev->fb, buffer, bytes_written)) {
		retval = -EFAULT;
		goto exit;
	}

	/* Make sure the device's backbuffer is available,
	 * rather than waiting for a sync from us. This can
	 * fail, so return its error if so.
	 */
	retval = rwand_wait_for_fb(dev);
	if (retval)
		goto exit;

	/* Change the framebuffer size if necessary and write its new content */
	rwand_update_fb(dev, bytes_written, new_fb);
	rwand_request_flip(dev);
	retval = bytes_written;

exit:
	/* unlock the device */
	up(&dev->sem);

	return retval;
}


static ssize_t rwand_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	struct rwand_fd_private *prv;
	struct rwand_dev *dev;
	int retval = 0;

	prv =(struct rwand_fd_private *)file->private_data;
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

	/* FIXME */
	retval = count;

exit:
	/* unlock the device */
	up(&dev->sem);
	return retval;
}


/******************************************************************************/
/************************************************** USB Housekeeping **********/
/******************************************************************************/

static void rwand_delete(struct rwand_dev *dev)
{
	if (dev->irq) {
		usb_unlink_urb(dev->irq);
		usb_free_urb(dev->irq);
	}
	if (dev->irq_data) {
		usb_buffer_free(dev->udev, IRQ_SIZE, dev->irq_data, dev->irq_dma);
	}
	kfree(dev);
}

static int rwand_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct rwand_dev *dev = NULL;
	struct usb_host_interface *host_interface = &interface->altsetting[interface->act_altsetting];
	int retval;

	/* See if the device offered us matches what we can accept */
	if ((udev->descriptor.idVendor != RWAND_VENDOR_ID) ||
	   (udev->descriptor.idProduct != RWAND_PRODUCT_ID)) {
		return -ENODEV;
	}

	/* allocate memory for our device state and intialize it */
	dev = kmalloc(sizeof(struct rwand_dev), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		return -ENOMEM;
	}
	memset(dev, 0, sizeof(*dev));

	init_MUTEX(&dev->sem);
	dev->udev = udev;
	dev->interface = interface;

	usb_set_intfdata(interface, dev);
	retval = usb_register_dev(interface, &rwand_class);
	if (retval) {
		/* something prevented us from registering this driver */
		err ("Not able to get a minor for this device.");
		goto error;
	}

	/* Allocate some DMA-friendly memory and a URB used for our interrupt
	 * transfers carrying predictor status, button status, and page flip status.
	 */
	dev->irq_data = usb_buffer_alloc(udev, IRQ_SIZE, SLAB_ATOMIC, &dev->irq_dma);
	if (!dev->irq_data) {
		retval = -ENOMEM;
		goto error;
	}
	dev->irq = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->irq) {
		retval = -ENODEV;
		goto error;
	}

	usb_fill_int_urb(dev->irq, udev,
			 usb_rcvintpipe(udev, host_interface->endpoint[0].desc.bEndpointAddress),
			 dev->irq_data, IRQ_SIZE,
			 rwand_irq, dev, IRQ_INTERVAL);

	dev->minor = interface->minor;
	info("Raster Wand device now attached");
	return 0;

error:
	usb_set_intfdata (interface, NULL);
	rwand_delete(dev);
	return retval;
}

static void rwand_disconnect(struct usb_interface *interface)
{
	struct rwand_dev *dev;
	int minor;

	/* prevent races with open() */
	down (&disconnect_sem);

	dev = usb_get_intfdata (interface);
	usb_set_intfdata (interface, NULL);

	down (&dev->sem);

	minor = dev->minor;
	usb_deregister_dev(interface, &rwand_class);

	/* if the device is not opened, then we clean up right now */
	if (!dev->open_count) {
		up(&dev->sem);
		rwand_delete(dev);
	} else {
		dev->udev = NULL;
		up(&dev->sem);
	}

	up(&disconnect_sem);
	info("rwand #%d now disconnected", minor);
}

static int __init rwand_init(void)
{
	int result = 0;

	result = usb_register(&rwand_driver);
	if (result < 0) {
		err("usb_register failed. Error number %d", result);
	}
	else {
		info(DRIVER_DESC " " DRIVER_VERSION " - " DRIVER_AUTHOR);
	}

	return result;
}

static void __exit rwand_exit(void)
{
	usb_deregister(&rwand_driver);
}

module_init(rwand_init);
module_exit(rwand_exit);

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

/* The End */
