/*
 * Raster Wand driver
 *
 * This is a module for the Linux kernel, providing a userspace interface
 * to the Raster Wand hardware via the USB protocol described in efs_protocol.h.
 *
 * This drive exposes three basic interfaces:
 *
 *   - Buttons turn into events for the linux input layer
 *   - Frames can be written to /dev/usb/rwand*
 *   - Timing parameters all have sane defaults, but can be tweaked with ioctl()
 *
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
#include <linux/input.h>
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

struct rwand_settings {
	int                     display_center;         /* The center of the display. 0 is full left,
							 * 0xFFFF is full right.
							 */
	int                     display_width;          /* The total width of the display, from 0 (nothing)
							 * to 0xFFFF (the entire wand sweep)
							 */
	int                     coil_center;            /* The center of the coil pulse. 0 is full left,
							 * 0x4000 is the center on the left-right pass,
							 * 0x8000 is full-right, 0xC000 is center on the
							 * right-left pass, and 0xFFFF is full left again.
							 */
	int                     coil_width;             /* The width of the coil pulse, from 0 (nothing) to
							 * 0xFFFF (the entire period)
							 */
	int                     duty_cycle;             /* The ratio of pixels to gaps. 0xFFFF has no gap,
							 * 0x0000 is all gap and no pixel.
							 */
	int                     fine_adjust;            /* Fine tuning for the front/back alignment */
	int                     power_mode;             /* RWAND_POWER_* */
	int                     num_columns;            /* The number of columns actually being displayed */
};

/* Timing calculated from the current status and settings */
struct rwand_timings {
	int                     column_width;
	int                     gap_width;
	int                     fwd_phase;
	int                     rev_phase;
	int                     coil_begin;
	int                     coil_end;
};

struct rwand_dev {
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct input_dev        input;                  /* Input device for the rwand's buttons */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char		minor;			/* the starting minor number for this device */

	struct urb *		irq;		        /* URB for interrupt transfers*/
	unsigned char *         irq_data;
	dma_addr_t              irq_dma;
	struct usb_ctrlrequest  status_request;

	struct rwand_settings   settings;

	/* Our local model of the device state */
	unsigned char           fb[RWAND_FB_BYTES+4];   /* Our local copy of the device framebuffer. The 4 here gives
							 * us some slack for reading past the end of the framebuffer
							 * when doing writes in blocks of 4, 8, or 12 bytes.
							 */
	struct rwand_status     status;

	int                     page_flip_pending;      /* Are we waiting on a page flip? */
	wait_queue_head_t       page_flip_waitq;        /* Processes waiting on a page flip */

	int                     open_count;
	struct semaphore	sem;			/* locks this structure */
};

/* Private data for each file descriptor */
struct rwand_fd_private {
	struct rwand_dev	*dev;
};


/* prevent races between open() and disconnect() */
static DECLARE_MUTEX (disconnect_sem);

/* 1/10 second timeout for control requests */
#define REQUEST_TIMEOUT    (HZ/10)

#define STATUS_PACKET_SIZE 8

/* Mapping between rwand buttons and linux input system buttons */
#define MAPPED_BTN_SQUARE    KEY_ENTER
#define MAPPED_BTN_RIGHT     KEY_RIGHT
#define MAPPED_BTN_LEFT      KEY_LEFT
#define MAPPED_BTN_UP        KEY_UP
#define MAPPED_BTN_DOWN      KEY_DOWN
#define MAPPED_BTN_POWER     KEY_POWER


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

static void    rwand_status_irq  (struct urb *urb, struct pt_regs *regs);

static void    rwand_delete      (struct rwand_dev *dev);

static void    rwand_request     (struct rwand_dev *dev, unsigned short request,
				  unsigned short wValue, unsigned short wIndex);
static int     rwand_wait_for_fb (struct rwand_dev *dev);
static void    rwand_update_fb   (struct rwand_dev *dev, int bytes, unsigned char *fb);
static void    rwand_request_flip(struct rwand_dev *dev);

static void    rwand_decode_status (const unsigned char *in, struct rwand_status *out);
static void    rwand_process_status(struct rwand_dev *dev, const unsigned char *packet);
static void    rwand_reset_settings(struct rwand_dev *dev);
static void    rwand_calc_timings(struct rwand_settings *settings,
				  struct rwand_status *status,
				  struct rwand_timings *timings);


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

static void rwand_status_irq(struct urb *urb, struct pt_regs *regs)
{
	/* Callback for processing incoming interrupt transfers from the IR receiver */
	struct rwand_dev *dev = (struct rwand_dev*)urb->context;

	switch (urb->status) {
	case -ECONNRESET:
		/* This is normal at rmmod or device disconnection time */
		return;
	case -ENOENT:
		err("ENOENT in rwand_irq");
		return;
	case -ESHUTDOWN:
		err("ESHUTDOWN in rwand_irq");
		return;
	}

	if (urb->status != 0 || urb->actual_length != STATUS_PACKET_SIZE) {
		err("Bad URB status/size: status=%d, length=%d", urb->status, urb->actual_length);
		return;
	}

	rwand_process_status(dev, dev->irq_data);

	/* Resubmit the URB to get another interrupt transfer going */
	usb_submit_urb(urb, SLAB_ATOMIC);
}


/* Decode a status packet, store the results in 'out' */
static void rwand_decode_status(const unsigned char *in, struct rwand_status *out)
{
	out->period = in[0] + (in[1] << 8);
	out->phase = in[2] + (in[3] << 8);
	out->edge_count = in[4];
	out->mode = in[5];
	out->flip_count = in[6];
	out->buttons = in[7];
}


/* Given an encoded status packet, process it and update the
 * device's state in any way necessary. This should be called
 * from interrupt context.
 */
static void rwand_process_status(struct rwand_dev *dev, const unsigned char *packet)
{
	struct rwand_status new_status;
	int power, new_mode;
	rwand_decode_status(packet, &new_status);

	/* Report button status to the input subsystem */
	input_report_key(&dev->input, MAPPED_BTN_SQUARE, new_status.buttons & RWAND_BUTTON_SQUARE);
	input_report_key(&dev->input, MAPPED_BTN_RIGHT,  new_status.buttons & RWAND_BUTTON_RIGHT);
	input_report_key(&dev->input, MAPPED_BTN_LEFT,   new_status.buttons & RWAND_BUTTON_LEFT);
	input_report_key(&dev->input, MAPPED_BTN_UP,     new_status.buttons & RWAND_BUTTON_UP);
	input_report_key(&dev->input, MAPPED_BTN_DOWN,   new_status.buttons & RWAND_BUTTON_DOWN);
	input_report_key(&dev->input, MAPPED_BTN_POWER,  new_status.buttons & RWAND_BUTTON_POWER);
	input_sync(&dev->input);

	/* If the page flip counter has incremented, clear our flip pending flag
	 * and wake up any processes that might be waiting on it.
	 */
	if (new_status.flip_count != dev->status.flip_count) {
		dev->page_flip_pending = 0;
		wake_up_interruptible(&dev->page_flip_waitq);
	}

	/* Is the display supposed to be on now? */
	switch (dev->settings.power_mode) {
	case RWAND_POWER_ON:
		power = 1;
		break;
	case RWAND_POWER_AUTO:
		power = (new_status.buttons & RWAND_BUTTON_POWER) != 0;
		break;
	default:
		power = 0;
	}

	if (power) {
		/* The display should be on. Is it? */
		struct rwand_timings timings;
		rwand_calc_timings(&dev->settings, &new_status, &timings);
		dbg("%d %d %d %d %d %d",
		    timings.column_width, timings.gap_width, timings.fwd_phase,
		    timings.rev_phase, timings.coil_begin, timings.coil_end);

	}
	else {
		/* It should be off */
		new_mode = 0;
	}


	dev->status = new_status;
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
	DECLARE_WAITQUEUE(wait, current);
	int retval = 0;

	if (dev->page_flip_pending && (dev->status.mode & RWAND_MODE_ENABLE_DISPLAY)) {
		add_wait_queue(&dev->page_flip_waitq, &wait);
		current->state = TASK_INTERRUPTIBLE;

		while (dev->page_flip_pending && (dev->status.mode & RWAND_MODE_ENABLE_DISPLAY)) {
			if (signal_pending(current)) {
				retval = -ERESTARTSYS;
				break;
			}

			up(&dev->sem);
			schedule();
			down(&dev->sem);
		}

		current->state = TASK_RUNNING;
		remove_wait_queue(&dev->page_flip_waitq, &wait);
	}

	return retval;
}


/* Change the device's framebuffer size and/or contents to match the one given.
 * This should be called while holding the device lock.
 */
static void rwand_update_fb(struct rwand_dev *dev, int bytes, unsigned char *fb)
{
	int retval, i;

	/* Change the display width if necessary */
	if (bytes != dev->settings.num_columns) {
		rwand_request(dev, RWAND_CTRL_SET_NUM_COLUMNS, bytes, 0);
		dev->settings.num_columns = bytes;
	}

	/* Currently this does the whole update using SEQ_WRITE4.
	 * This might be fast enough, but there's plenty of room for improvement.
	 */
	memcpy(dev->fb, fb, bytes);
	for (i=0; i<dev->settings.num_columns; i+=4) {
		retval = usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
					 RWAND_CTRL_SEQ_WRITE4, USB_TYPE_VENDOR,
					 fb[i] | (fb[i+1] << 8),
					 fb[i+2] | (fb[i+3] << 8),
					 NULL, 0, REQUEST_TIMEOUT);
		if (retval != 0) {
			err("Unexpected usb_control_msg retval in rwand_update_fb: %d", retval);
		}
	}
}


/* Request a page flip. No display writes should be made until the
 * page flip completes. This can be checked for using rwand_wait_for_fb().
 */
static void rwand_request_flip(struct rwand_dev *dev)
{
	dev->page_flip_pending = 1;
	rwand_request(dev, RWAND_CTRL_FLIP, 0, 0);
}


/* Change our timing and mode settings back to the defaults */
static void rwand_reset_settings(struct rwand_dev *dev)
{
	dev->settings.display_center = 0x8000;
	dev->settings.display_width  = 0xE000;
	dev->settings.coil_center    = 0x4000;
	dev->settings.coil_width     = 0x7000;
	dev->settings.duty_cycle     = 0x8000;
	dev->settings.fine_adjust    = 0;
	dev->settings.power_mode     = RWAND_POWER_AUTO;
}

/* Calculate all the fun little timing parameters needed by the hardware */
static void    rwand_calc_timings(struct rwand_settings *settings,
				  struct rwand_status *status,
				  struct rwand_timings *timings)
{
	int col_and_gap_width, total_width;

	/* The coil driver just needs to have its relative timings
	 * multiplied by our predictor's current period. This is fixed
	 * point math with 16 digits to the right of the binary point.
	 */
	timings->coil_begin = (status->period * (settings->coil_center - settings->coil_width/2)) >> 16;
	timings->coil_end   = (status->period * (settings->coil_center + settings->coil_width/2)) >> 16;

	if (settings->num_columns > 0) {
		/* Now calculate the display timings. We start out with the precise
		 * width of our columns, so that the width of the whole display
		 * can be calculated accurately.
		 */
		col_and_gap_width = (status->period / settings->num_columns * settings->display_width) >> 17;
		timings->column_width = (col_and_gap_width * settings->duty_cycle) >> 16;
		timings->gap_width = col_and_gap_width - timings->column_width;
		total_width = (settings->num_columns+1) * timings->column_width + settings->num_columns * timings->gap_width;

		/* Now that we know the true width of the display, we can calculate the
		 * two phase timings. These indicate when it starts the forward scan and the
		 * backward scan, relative to the left position. The alignment between
		 * the forward and backward scans should be calculated correctly, but it
		 * can be tweaked using settings->fine_adjust.
		 */
		timings->fwd_phase = ((status->period * settings->display_center) >> 17) - total_width/2;
		timings->rev_phase = status->period - timings->fwd_phase - total_width + settings->fine_adjust;
	}
	else {
		/* We can't calculate timings for a zero-width display without dividing by
		 * zero, so just fill in some invalid timings that will blank the display.
		 */
		timings->column_width = 1;
		timings->gap_width = 1;
		timings->fwd_phase = 0xFFFF;
		timings->rev_phase = 0xFFFF;
	}
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
	if (copy_from_user(new_fb, buffer, bytes_written)) {
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
		usb_buffer_free(dev->udev, STATUS_PACKET_SIZE, dev->irq_data, dev->irq_dma);
	}
	kfree(dev);
}

static int rwand_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct rwand_dev *dev = NULL;
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
	rwand_reset_settings(dev);

	init_MUTEX(&dev->sem);
	init_waitqueue_head(&dev->page_flip_waitq);
	dev->udev = udev;
	dev->interface = interface;

	usb_set_intfdata(interface, dev);
	retval = usb_register_dev(interface, &rwand_class);
	if (retval) {
		/* something prevented us from registering this driver */
		err ("Not able to get a minor for this device.");
		goto error;
	}

	/* Register an input device for our buttons */
	dev->input.private = dev;
	dev->input.name = "Raster Wand";
	dev->input.id.bustype = BUS_USB;
	dev->input.id.vendor = udev->descriptor.idVendor;
	dev->input.id.product = udev->descriptor.idProduct;
	dev->input.id.version = udev->descriptor.bcdDevice;
        set_bit(EV_KEY, dev->input.evbit);
        set_bit(MAPPED_BTN_SQUARE, dev->input.keybit);
        set_bit(MAPPED_BTN_RIGHT, dev->input.keybit);
        set_bit(MAPPED_BTN_LEFT, dev->input.keybit);
        set_bit(MAPPED_BTN_UP, dev->input.keybit);
        set_bit(MAPPED_BTN_DOWN, dev->input.keybit);
        set_bit(MAPPED_BTN_POWER, dev->input.keybit);
	input_register_device(&dev->input);

	/* Allocate some DMA-friendly memory and a URB used for periodic
	 * transfers carrying predictor status, button status, and page flip status.
	 * Currently these are implemented as periodic control requests. Interrupt
	 * requests make more sense, but for some reason I haven't been able to get
	 * any data out of the EP1 endpoint successfully.
	 */
	dev->status_request.bRequestType = USB_TYPE_VENDOR;
	dev->status_request.bRequest = RWAND_CTRL_READ_STATUS;

	dev->irq_data = usb_buffer_alloc(udev, STATUS_PACKET_SIZE, SLAB_ATOMIC, &dev->irq_dma);
	if (!dev->irq_data) {
		retval = -ENOMEM;
		goto error;
	}
	dev->irq = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->irq) {
		retval = -ENODEV;
		goto error;
	}

	usb_fill_control_urb(dev->irq, udev, usb_rcvctrlpipe(udev, 0),
			     (unsigned char*) &dev->status_request,
			     dev->irq_data, STATUS_PACKET_SIZE, rwand_status_irq, dev);

	/* Keep the flow of interrupt requests going as long as the device is attached.
	 * Even when our device node isn't open we need to be able to deal with input
	 * events, predictor updates, and such.
	 */
	dbg("First URB submitted");
	usb_submit_urb(dev->irq, GFP_KERNEL);

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
	input_unregister_device(&dev->input);
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
