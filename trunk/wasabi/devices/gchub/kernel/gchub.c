/*
 * Gamecube Controller Hub driver
 *
 * This is a Linux 2.6 kernel module that exposes linux input devices corresponding
 * to all controllers attached to the gchub. It handles basic calibration, and rumble
 * support is provided using the linux force feedback interface.
 *
 * Each controller is given a name that both identifies it as a GC controller and
 * gives the port numer it's connected to. The button/axis mapping is as follows:
 *
 *   BTN_A     : A button
 *   BTN_B     : B button
 *   BTN_X     : X button
 *   BTN_Y     : Y button
 *   BTN_Z     : Z button
 *   BTN_TL    : L button
 *   BTN_TR    : R button
 *   BTN_START : Start button
 *
 *   ABS_X     : Main joystick X axis
 *   ABS_Y     : Main joystick Y axis
 *   ABS_BRAKE : Left trigger axis
 *   ABS_GAS   : Right trigger axis
 *   ABS_HAT0X : C-stick X axis
 *   ABS_HAT0Y : C-stick Y axis
 *   ABS_HAT1X : D-pad X axis
 *   ABS_HAT1Y : D-pad Y axis
 *
 * Copyright(c) 2004 Micah Dowty <micah@navi.cx>
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
#include <linux/usb.h>
#include <gchub_protocol.h>

#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR  "Micah Dowty <micah@navi.cx>"
#define DRIVER_DESC    "Gamecube Controller Hub driver"

struct gchub_dev {
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct input_dev        input;                  /* Input device for the gchub's buttons */
	struct usb_interface *	interface;		/* the interface for this device */

	struct urb *		irq;		        /* URB for interrupt transfers*/
	unsigned char *         irq_data;
	dma_addr_t              irq_dma;

	struct semaphore	sem;			/* locks this structure */
};

/* prevent races between open() and disconnect() */
static DECLARE_MUTEX (disconnect_sem);

/* 1/10 second timeout for control requests */
#define REQUEST_TIMEOUT    (HZ/10)

#define STATUS_PACKET_SIZE 8    /* This must always be 8, since that's what the hardware
				 * gives us. If it's smaller, the HCD will probably crash horribly.
				 */

/* Map the left and right trigger axes to something
 * that exists and makes at least a little sense.
 */
#ifndef ABS_TL
#define ABS_TL   ABS_BRAKE
#endif
#ifndef ABS_TR
#define ABS_TR   ABS_GAS
#endif


/******************************************************************************/
/************************************************** Function Prototypes *******/
/******************************************************************************/

static int    gchub_probe          (struct usb_interface*       interface,
				    const struct usb_device_id* id);
static void   gchub_disconnect     (struct usb_interface*       interface);

static void   gchub_irq            (struct urb*                 urb,
				    struct pt_regs*             regs);
static void   gchub_process_status (struct gchub_dev*           dev,
				    const unsigned char*        packet);

static void   gchub_delete         (struct gchub_dev*           dev);


/******************************************************************************/
/*************************************************************** Tables *******/
/******************************************************************************/

/* Table of devices that work with this driver */
static struct usb_device_id gchub_table [] = {
	{ USB_DEVICE(GCHUB_VENDOR_ID, GCHUB_PRODUCT_ID) },
	{ } /* End table */
};
MODULE_DEVICE_TABLE(usb, gchub_table);

static struct usb_driver gchub_driver = {
	.owner      =  THIS_MODULE,
	.name       =  "gchub",
	.probe      =  gchub_probe,
	.disconnect =  gchub_disconnect,
	.id_table   =  gchub_table,
};


/******************************************************************************/
/************************************************** Device Communications *****/
/******************************************************************************/

static void gchub_irq(struct urb *urb, struct pt_regs *regs)
{
	struct gchub_dev *dev = (struct gchub_dev*)urb->context;

	switch (urb->status) {
	case -ECONNRESET:
		/* This is normal at rmmod or device disconnection time */
		return;
	case -ENOENT:
		err("ENOENT in gchub_irq");
		return;
	case -ESHUTDOWN:
		err("ESHUTDOWN in gchub_irq");
		return;
	}

	if (urb->status != 0 || urb->actual_length != STATUS_PACKET_SIZE) {
		err("Bad URB status/size: status=%d, length=%d", urb->status, urb->actual_length);
		return;
	}

	gchub_process_status(dev, dev->irq_data);

	/* Resubmit the URB to get another interrupt transfer going */
	usb_submit_urb(urb, SLAB_ATOMIC);
}

static void gchub_process_status(struct gchub_dev *dev, const unsigned char *packet)
{
	/*
	input_report_key(&dev->input, MAPPED_BTN_SQUARE, new_status.buttons & GCHUB_BUTTON_SQUARE);
	input_report_key(&dev->input, MAPPED_BTN_RIGHT,  new_status.buttons & GCHUB_BUTTON_RIGHT);
	input_report_key(&dev->input, MAPPED_BTN_LEFT,   new_status.buttons & GCHUB_BUTTON_LEFT);
	input_report_key(&dev->input, MAPPED_BTN_UP,     new_status.buttons & GCHUB_BUTTON_UP);
	input_report_key(&dev->input, MAPPED_BTN_DOWN,   new_status.buttons & GCHUB_BUTTON_DOWN);
	input_sync(&dev->input);
	*/
}


/******************************************************************************/
/************************************************** USB Housekeeping **********/
/******************************************************************************/

static void gchub_delete(struct gchub_dev *dev)
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

static int gchub_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct gchub_dev *dev = NULL;
	int retval;

	/* See if the device offered us matches what we can accept */
	if ((udev->descriptor.idVendor != GCHUB_VENDOR_ID) ||
	   (udev->descriptor.idProduct != GCHUB_PRODUCT_ID)) {
		return -ENODEV;
	}

	/* allocate memory for our device state and intialize it */
	dev = kmalloc(sizeof(struct gchub_dev), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		return -ENOMEM;
	}
	memset(dev, 0, sizeof(*dev));

	init_MUTEX(&dev->sem);
	dev->udev = udev;
	dev->interface = interface;
	usb_set_intfdata(interface, dev);




	info("Gamecube Controller Hub device now attached");
	return 0;

error:
	usb_set_intfdata (interface, NULL);
	gchub_delete(dev);
	return retval;
}

static void gchub_disconnect(struct usb_interface *interface)
{
	struct gchub_dev *dev;

	/* prevent races with open() */
	down (&disconnect_sem);

	dev = usb_get_intfdata (interface);
	usb_set_intfdata (interface, NULL);

	gchub_delete(dev);

	up(&disconnect_sem);
	info("gchub now disconnected");
}

static int __init gchub_init(void)
{
	int result = 0;

	result = usb_register(&gchub_driver);
	if (result < 0) {
		err("usb_register failed. Error number %d", result);
	}
	else {
		info(DRIVER_DESC " " DRIVER_VERSION " - " DRIVER_AUTHOR);
	}

	return result;
}

static void __exit gchub_exit(void)
{
	usb_deregister(&gchub_driver);
}

module_init(gchub_init);
module_exit(gchub_exit);

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

/* The End */
