/*
 * USB Video Switch driver
 *
 * This is a module for the Linux kernel, providing a userspace interface
 * to the uvswitch hardware via the USB protocol described in uvswitch_protocol.h.
 *
 * Copyright (C) 2003-2004 Micah Dowty <micah@picogui.org>
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
#include <linux/poll.h>
#include <linux/completion.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include <uvswitch_dev.h>

#define DRIVER_VERSION "v0.3"
#define DRIVER_AUTHOR "Micah Dowty <micah@navi.cx>"
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

/* Status of all the device's switches */
struct uvswitch_status {
	unsigned char		video_channel;
	unsigned char		white_audio_channel;
	unsigned char		red_audio_channel;
	unsigned char		bypass_switch;
};

/* Structure to hold all of our device specific stuff */
struct usb_uvswitch {
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char		minor;			/* the starting minor number for this device */
	int			open_count;		/* number of times this port has been opened */

	struct uvswitch_status	status;			/* Status of the device's switches */
	int                     status_mode;            /* A MODE_* constant */
	struct urb*		status_urb;		/* URB for setting switch status */
	struct usb_ctrlrequest  status_request;

	struct urb*		adc_urb;		/* URB for the video detector A/D converters */
       	unsigned char*		adc_buffer;
	dma_addr_t              adc_dma;
	int			adc_accumulator[UVSWITCH_CHANNELS]; /* Values being accumulated after each packet received */
	int			adc_accumulated[UVSWITCH_CHANNELS]; /* Finished ADC values */
	int			active_inputs[UVSWITCH_CHANNELS]; /* Nonzero values for active video inputs */
	int			adc_samples;		/* Number of samples in adc_accumulator */
	wait_queue_head_t	adc_wait;		/* Processes waiting for video detection input changes */
	int			adc_change_id;		/* Incremented every time active_inputs changes */
	struct uvswitch_calibration calibration;	/* Tweakable values for the video detector */

	struct semaphore	sem;			/* locks this structure */
};

/* Private data for each file descriptor */
struct uvswitch_fd_private {
	struct usb_uvswitch	*dev;
	int			not_first_read;		/* Set after the first successful read() */
	int			last_change_id;		/* The adc_change_id last time a read() was done */
};

/* prevent races between open() and disconnect() */
static DECLARE_MUTEX (disconnect_sem);

/* 1 second timeout for control requests */
#define REQUEST_TIMEOUT HZ

/* Switch modes */
#define MODE_MANUAL     0
#define MODE_AUTOMATIC  1


static int          uvswitch_updateStatus      (struct usb_uvswitch *dev);
static int          uvswitch_updateCalibration (struct usb_uvswitch *dev);
static int          uvswitch_initCalibration   (struct usb_uvswitch *dev);
static void         uvswitch_adc_irq           (struct urb *urb, struct pt_regs *regs);
static void         uvswitch_status_irq        (struct urb *urb, struct pt_regs *regs);
static void         uvswitch_updateAutoStatus  (struct usb_uvswitch *dev);
static int          uvswitch_getFirstActiveChannel(struct usb_uvswitch *dev);

static int          uvswitch_open              (struct inode *inode, struct file *file);
static int          uvswitch_release           (struct inode *inode, struct file *file);
static int          uvswitch_ioctl             (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t      uvswitch_read              (struct file *file, char *buffer, size_t count, loff_t *ppos);
static ssize_t      uvswitch_write             (struct file *file, const char *buffer, size_t count, loff_t *ppos);
static unsigned int uvswitch_poll              (struct file *file, poll_table *wait);

static inline void  uvswitch_delete            (struct usb_uvswitch *dev);
static int          uvswitch_probe             (struct usb_interface *interface, const struct usb_device_id *id);
static void         uvswitch_disconnect        (struct usb_interface *interface);
static int __init   usb_uvswitch_init          (void);
static void __exit  usb_uvswitch_exit          (void);


static struct file_operations uvswitch_dev_fops = {
	.owner      =  THIS_MODULE,
	.ioctl      =  uvswitch_ioctl,
	.open       =  uvswitch_open,
	.release    =  uvswitch_release,
	.read       =  uvswitch_read,
	.write      =  uvswitch_write,
	.poll       =  uvswitch_poll,
};

static struct usb_class_driver uvswitch_class = {
	.name       =  UVSWITCH_DEV_NAMEFORMAT,
	.fops       =  &uvswitch_dev_fops,
	.mode       =  S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
	.minor_base =  UVSWITCH_MINOR_BASE,
};

static struct usb_driver uvswitch_driver = {
	.name         =	"uvswitch",
	.probe        = uvswitch_probe,
	.disconnect   = uvswitch_disconnect,
	.id_table     = uvswitch_table,
};


/******************************************************************************/
/************************************************** Device Communications *****/
/******************************************************************************/

/* Update the device's switches from the current stored status.
 * Must be callable from process or interrupt context, so it doesn't block until
 * we get a response to the command.
 */
static int uvswitch_updateStatus(struct usb_uvswitch *dev)
{
	dev->status_request.wValue = dev->status.video_channel | (((int)dev->status.bypass_switch)<<8);
	dev->status_request.wIndex = dev->status.red_audio_channel | (((int)dev->status.white_audio_channel)<<8);
	return usb_submit_urb(dev->status_urb, SLAB_ATOMIC);
}

/* Update the device's ADC settings from our calibration structure */
static int uvswitch_updateCalibration(struct usb_uvswitch *dev)
{
	dbg("Updating calibration: precharge=%d, integration=%d, interval=%d, packets=%d, threshold=%d",
	    dev->calibration.precharge_reads,
	    dev->calibration.integration_reads,
	    dev->calibration.interval,
	    dev->calibration.integration_packets,
	    dev->calibration.threshold);

	/* Set the interval next time our interrupt request is submitted */
	if (dev->adc_urb)
		dev->adc_urb->interval = dev->calibration.interval;

	/* Send a packet to change the device's ADC settings */
	return usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
			       UVSWITCH_CTRL_ADC_CYCLES, 0x40,
			       dev->calibration.integration_reads,
			       dev->calibration.precharge_reads,
			       NULL, 0, REQUEST_TIMEOUT);
}

/* Set up default values for the ADC calibration */
static int uvswitch_initCalibration(struct usb_uvswitch *dev)
{
	dev->calibration.precharge_reads = 10;
	dev->calibration.integration_reads = 50;
	dev->calibration.interval = 10;
	dev->calibration.integration_packets = 100;
	dev->calibration.threshold = 1600;

	return uvswitch_updateCalibration(dev);
}

/* Callback for handling incoming data from the device's interrupt endpoint.
 * This receives an 8-byte packet about 8 times a second with A/D converter readings.
 */
static void uvswitch_adc_irq(struct urb *urb, struct pt_regs *regs)
{
	int active_inputs[UVSWITCH_CHANNELS];
	int i;
	struct usb_uvswitch *dev = (struct usb_uvswitch*)urb->context;

	switch (urb->status) {
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		return;
	}

	if (urb->status != 0 || urb->actual_length != UVSWITCH_CHANNELS) {
		dbg("Bad URB status/size: status=%d, length=%d", urb->status, urb->actual_length);
		return;
	}

	/* Add this packet to the accumulator */
	for (i=0; i<UVSWITCH_CHANNELS; i++) {
		dev->adc_accumulator[i] += dev->adc_buffer[i];
	}
	dev->adc_samples++;

	/* Do we have enough samples to get a final analog value yet? */
	if (dev->adc_samples >= dev->calibration.integration_packets) {
		/* Copy adc_accumulator to adc_accumulated and zero the accumulator */
		memcpy(dev->adc_accumulated, dev->adc_accumulator, sizeof(dev->adc_accumulated));
		memset(dev->adc_accumulator, 0, sizeof(dev->adc_accumulator));
		dev->adc_samples = 0;

		/* Compare the final accumulated values against our threshold
		 * to detect active inputs.
		 */
		for (i=0; i<UVSWITCH_CHANNELS; i++)
			active_inputs[i] = dev->adc_accumulated[i] > dev->calibration.threshold;

		/* If our set of active inputs has changed, save it and wake up any sleeping processes */
		if (memcmp(active_inputs, dev->active_inputs, sizeof(active_inputs)) != 0) {
			memcpy(dev->active_inputs, active_inputs, sizeof(active_inputs));

			dbg("input status changed: %d %d %d %d %d %d %d %d",
			    dev->active_inputs[0], dev->active_inputs[1], dev->active_inputs[2], dev->active_inputs[3],
			    dev->active_inputs[4], dev->active_inputs[5], dev->active_inputs[6], dev->active_inputs[7]);

			dev->adc_change_id++;
			wake_up_interruptible(&dev->adc_wait);
			uvswitch_updateAutoStatus(dev);
		}
	}

	usb_submit_urb(urb, SLAB_ATOMIC);
}

static void uvswitch_status_irq(struct urb *urb, struct pt_regs *regs)
{
	/* This is invoked when a status update completes. Currently we
	 * don't care about this.
	 */
}

/* Update the device status when entering a different automatic switch
 * mode, or when input status changes. May be called from interrupt or
 * process context.
 */
static void uvswitch_updateAutoStatus  (struct usb_uvswitch *dev)
{
	int channel;
	switch (dev->status_mode) {

	case MODE_AUTOMATIC:
		/* Basic automatic switching- turn on the first active input.
		 * If no inputs are active, enable the bypass switch.
		 */
		channel = uvswitch_getFirstActiveChannel(dev);
		dev->status.video_channel = channel;
		dev->status.white_audio_channel = channel;
		dev->status.red_audio_channel = channel;
		dev->status.bypass_switch = channel == 0 ? 1 : 0;
		uvswitch_updateStatus(dev);
		break;

	}
}

static int uvswitch_getFirstActiveChannel(struct usb_uvswitch *dev)
{
	int i;
	for (i=0; i<UVSWITCH_CHANNELS; i++)
		if (dev->active_inputs[i])
			return i+1;
	return 0;
}


/******************************************************************************/
/************************************************** Main character device *****/
/******************************************************************************/

static int uvswitch_open(struct inode *inode, struct file *file)
{
	struct usb_uvswitch *dev = NULL;
	struct uvswitch_fd_private *prv = NULL;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);

	/* Prevent disconnects */
	down(&disconnect_sem);


	interface = usb_find_interface (&uvswitch_driver, subminor);
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
	prv = kmalloc(sizeof(struct uvswitch_fd_private), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		retval = -ENOMEM;
		goto exit;
	}

	/* save our object in the file's private structure */
	memset(prv, 0, sizeof(struct uvswitch_fd_private));
	prv->dev = dev;
	file->private_data = prv;

exit:
	/* unlock this device */
	up (&dev->sem);

exit_no_device:
	up (&disconnect_sem);
	return retval;
}

static int uvswitch_release(struct inode *inode, struct file *file)
{
	struct uvswitch_fd_private *prv;
	struct usb_uvswitch *dev;
	int retval = 0;

	prv =(struct uvswitch_fd_private *)file->private_data;
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
		uvswitch_delete(dev);
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


static int uvswitch_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct uvswitch_fd_private *prv;
	struct usb_uvswitch *dev;
	int retval = 0;

	prv =(struct uvswitch_fd_private *)file->private_data;
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

	case UVSWITCHIO_ADC_CALIBRATE:
		if (copy_from_user(&dev->calibration, (struct uvswitch_calibration*) arg, sizeof(dev->calibration))) {
			retval = -EFAULT;
			break;
		}
		uvswitch_updateCalibration(dev);
		break;

	case UVSWITCHIO_ADC_READ_RAW:
		if (copy_to_user((int*) arg, dev->adc_accumulated, sizeof(dev->adc_accumulated))) {
			retval = -EFAULT;
			break;
		}
		break;

	case UVSWITCHIO_ADC_RESET:
		retval = uvswitch_initCalibration(dev);
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

static ssize_t uvswitch_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	DECLARE_WAITQUEUE(wait, current);
	struct uvswitch_fd_private *prv;
	struct usb_uvswitch *dev;
	int retval = 0;
	char kbuffer[512];
	char *p;
	int i;

	prv =(struct uvswitch_fd_private *)file->private_data;
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

	/* If this isn't the first read or there's no new data, wait */
	while (prv->not_first_read && prv->last_change_id == dev->adc_change_id) {
		if (file->f_flags & O_NONBLOCK) {
			retval = -EAGAIN;
			goto exit;
		}

		add_wait_queue(&dev->adc_wait, &wait);
		current->state = TASK_INTERRUPTIBLE;

		up(&dev->sem);
		schedule();
		down(&dev->sem);

		current->state = TASK_RUNNING;
		remove_wait_queue(&dev->adc_wait, &wait);

		if (signal_pending(current)) {
			retval = -ERESTARTSYS;
			goto exit;
		}
	}


	/* Scan for active inputs */
	p = kbuffer;
	for (i=0; i<UVSWITCH_CHANNELS; i++) {
		if (dev->active_inputs[i]) {
			/* If this isn't the first find, separate with a space */
			if (p != kbuffer) {
				*(p++) = ' ';
			}

			/* Add this input number */
			p += sprintf(p, "%d", i+1);
		}
	}

	/* Add a newline */
	*(p++) = '\n';

	/* Check length and copy to userspace */
	if (count > p - kbuffer)
		count = p - kbuffer;
	if (copy_to_user(buffer, kbuffer, count)) {
		retval = -EFAULT;
		goto exit;
	}
	retval = count;

	prv->not_first_read = 1;
	prv->last_change_id = dev->adc_change_id;

exit:
	/* unlock the device */
	up(&dev->sem);
	return retval;
}

static ssize_t uvswitch_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	struct uvswitch_fd_private *prv;
	struct usb_uvswitch *dev;
	int retval = 0;
	char kbuffer[256];
	char *token;
	char *pkbuf;
	static char delim[] = " \t\n";
	int v;

	prv =(struct uvswitch_fd_private *)file->private_data;
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

	/* Copy the write to our buffer, without overflowing it */
	if (count > sizeof(kbuffer)-1)
		count = sizeof(kbuffer)-1;
	if (copy_from_user(kbuffer, buffer, count)) {
		retval = -EFAULT;
		goto exit;
	}
	kbuffer[count] = 0;
	retval = count;
	pkbuf = kbuffer;

	/* Assume manual mode unless we find otherwise... */
	dev->status_mode = MODE_MANUAL;

	/* First token - video/audio channels */
	token = strsep(&pkbuf, delim);
	if (token) {

		/* Is it the special "auto" keyword? */
		if (!strcmp(token, "auto")) {
			dev->status_mode = MODE_AUTOMATIC;
			uvswitch_updateAutoStatus(dev);
			goto success;
		}

		v = simple_strtol(token, NULL, 10);
		if (v < 0 || v > 8) {
			retval = -EINVAL;
			goto exit;
		}
		dev->status.video_channel = v;
		dev->status.white_audio_channel = v;
		dev->status.red_audio_channel = v;
		dbg("Setting video/audio channels to %d", v);
	}

	/* Second token - bypass enable */
	token = strsep(&pkbuf, delim);
	if (token) {
		v = simple_strtol(token, NULL, 10);
		if (v < 0 || v > 1) {
			retval = -EINVAL;
			goto exit;
		}
		dev->status.bypass_switch = v;
		dbg("Setting bypass switch to %d", v);
	}

	/* Third token - white/red audio channel */
	token = strsep(&pkbuf, delim);
	if (token) {
		v = simple_strtol(token, NULL, 10);
		if (v < 0 || v > 8) {
			retval = -EINVAL;
			goto exit;
		}
		dev->status.white_audio_channel = v;
		dev->status.red_audio_channel = v;
		dbg("Setting audio channels to %d", v);
	}

	/* Fourth token - red audio channel */
	token = strsep(&pkbuf, delim);
	if (token) {
		v = simple_strtol(token, NULL, 10);
		if (v < 0 || v > 8) {
			retval = -EINVAL;
			goto exit;
		}
		dev->status.red_audio_channel = v;
		dbg("Setting red channel to %d", v);
	}

	retval = uvswitch_updateStatus(dev);

success:
	if (retval == 0) {
		/* On success indicate that we've written all bytes */
		retval = count;
	}

exit:
	/* unlock the device */
	up(&dev->sem);

	return retval;
}

static unsigned int uvswitch_poll(struct file *file, poll_table *wait)
{
	struct uvswitch_fd_private *prv;
	struct usb_uvswitch *dev;

	prv =(struct uvswitch_fd_private *)file->private_data;
	if (prv == NULL) {
		dbg("object is NULL");
		return -ENODEV;
	}
	dev = prv->dev;

	poll_wait(file, &dev->adc_wait, wait);

	/* Was the device unplugged? */
	if (dev->udev == NULL)
		return POLLERR | POLLHUP;

	if (dev->adc_change_id != prv->last_change_id)
		return POLLIN | POLLRDNORM;

	return 0;
}


/******************************************************************************/
/************************************************** USB Housekeeping **********/
/******************************************************************************/

static inline void uvswitch_delete(struct usb_uvswitch *dev)
{
	if (dev->adc_urb) {
		usb_unlink_urb(dev->adc_urb);
		usb_free_urb(dev->adc_urb);
	}
	if (dev->adc_buffer) {
		usb_buffer_free(dev->udev, UVSWITCH_CHANNELS, dev->adc_buffer, dev->adc_dma);
	}
	if (dev->status_urb) {
		usb_unlink_urb(dev->status_urb);
		usb_free_urb(dev->status_urb);
	}
	kfree(dev);
}

static int uvswitch_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct usb_uvswitch *dev = NULL;
	struct usb_host_interface *host_interface = interface->cur_altsetting;
	int retval, i;

	/* See if the device offered us matches what we can accept */
	if ((udev->descriptor.idVendor != UVSWITCH_VENDOR_ID) ||
	   (udev->descriptor.idProduct != UVSWITCH_PRODUCT_ID)) {
		return -ENODEV;
	}

	/* allocate memory for our device state and intialize it */
	dev = kmalloc(sizeof(struct usb_uvswitch), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		retval = -ENOMEM;
		goto error;
	}
	memset(dev, 0x00, sizeof(*dev));
	init_waitqueue_head(&dev->adc_wait);

	init_MUTEX(&dev->sem);
	dev->udev = udev;
	dev->interface = interface;

	usb_set_intfdata(interface, dev);
	retval = usb_register_dev(interface, &uvswitch_class);
	if (retval) {
		/* something prevented us from registering this driver */
		err ("Not able to get a minor for this device.");
		goto error;
	}

	/* Allocate some DMA-friendly memory and a URB used for our interrupt
	 * transfers carrying actual sensor readings back to us.
	 */
	dev->adc_buffer = usb_buffer_alloc(udev, UVSWITCH_CHANNELS, SLAB_ATOMIC, &dev->adc_dma);
	if (!dev->adc_buffer) {
		retval = -ENOMEM;
		goto error;
	}
	dev->adc_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->adc_urb) {
		retval = -ENODEV;
		goto error;
	}

	/* We need another URB for updating video switch status */
	dev->status_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->status_urb) {
		retval = -ENODEV;
		goto error;
	}

	/* Fill in most of the URB we'll use to update switch status */
	dev->status_request.bRequestType = USB_TYPE_VENDOR;
	dev->status_request.bRequest = UVSWITCH_CTRL_SWITCH;
	usb_fill_control_urb(dev->status_urb, udev, usb_sndctrlpipe(udev, 0),
			     (unsigned char*) &dev->status_request,
			     NULL, 0, uvswitch_status_irq, dev);

	/* Set up default ADC calibration */
	uvswitch_initCalibration(dev);

	/* Begin our interrupt transfer polling the video detector ADC */
	usb_fill_int_urb(dev->adc_urb, dev->udev,
			 usb_rcvintpipe(dev->udev, host_interface->endpoint[0].desc.bEndpointAddress),
			 dev->adc_buffer, UVSWITCH_CHANNELS,
			 uvswitch_adc_irq, dev, dev->calibration.interval);

	dbg("Submitting adc_urb, interval %d", dev->calibration.interval);
	i = usb_submit_urb(dev->adc_urb, GFP_KERNEL);
	if (i) {
		dbg("Error submitting URB: %d", i);
	}

	/* Put all the switches into a known default state.
	 * In this default, no input is selected but the bypass switch
	 * is on, so we get video from the computer's output.
	 */
	dev->status.video_channel = 0;
	dev->status.white_audio_channel = 0;
	dev->status.red_audio_channel = 0;
	dev->status.bypass_switch = 1;
	uvswitch_updateStatus(dev);

	info("uvswitch device now attached");
	return 0;

error:
	usb_set_intfdata(interface, NULL);
	uvswitch_delete(dev);
	return retval;
}

static void uvswitch_disconnect(struct usb_interface *interface)
{
	struct usb_uvswitch *dev;
	int minor;

	/* prevent races with open() */
	down (&disconnect_sem);

	dev = usb_get_intfdata (interface);
	usb_set_intfdata (interface, NULL);

	down(&dev->sem);

	minor = dev->minor;
	usb_deregister_dev(interface, &uvswitch_class);

	/* if the device is not opened, then we clean up right now */
	if (!dev->open_count) {
		up(&dev->sem);
		uvswitch_delete(dev);
	} else {
		dev->udev = NULL;
		up(&dev->sem);
	}

	up(&disconnect_sem);
	info("uvswitch #%d now disconnected", minor);
}

static int __init usb_uvswitch_init(void)
{
	int result;

	/* register this driver with the USB subsystem */
	result = usb_register(&uvswitch_driver);
	if (result < 0) {
		err("usb_register failed for the "__FILE__" driver. Error number %d",
		    result);
	} else {
		info(DRIVER_DESC " " DRIVER_VERSION " - " DRIVER_AUTHOR);
	}

	return result;
}

static void __exit usb_uvswitch_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&uvswitch_driver);
}

module_init(usb_uvswitch_init);
module_exit(usb_uvswitch_exit);

/* The End */
