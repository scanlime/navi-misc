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
 *   ABS_Z     : Left trigger axis
 *   ABS_RZ    : Right trigger axis
 *   ABS_RX    : C-stick X axis
 *   ABS_RY    : C-stick Y axis
 *   ABS_HAT0X : D-pad X axis
 *   ABS_HAT0Y : D-pad Y axis
 *
 * Copyright(c) 2004 Micah Dowty <micah@navi.cx>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

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
#define DEVICE_DESC    "Gamecube Controller Hub"
#define DRIVER_DESC    DEVICE_DESC " driver"

#define NUM_PORTS      4

struct gchub_controller_status {
	int buttons;
	int joystick[2];
	int cstick[2];
	int triggers[2];
};

struct gchub_controller {
	struct input_dev dev;
	int attached;            /* Nonzero if this controller is physically present */
	int dev_registered;      /* Nonzero if 'dev' has been registered */
	int reg_in_progress;     /* Nonzero if a registration/unregistration is pending */

	struct work_struct reg_work, unreg_work;
	spinlock_t reg_lock;

	struct gchub_controller_status current_status;
	struct gchub_controller_status prev_status;
	struct gchub_controller_status calibrated_status;
};

struct gchub_dev {
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct input_dev        input;                  /* Input device for the gchub's buttons */
	struct usb_interface *	interface;		/* the interface for this device */

	struct urb *		irq;		        /* URB for interrupt transfers*/
	unsigned char *         irq_data;
	dma_addr_t              irq_dma;

	struct gchub_controller ports[NUM_PORTS];

	struct semaphore	sem;			/* locks this structure */
};

#define REQUEST_TIMEOUT    (HZ/10)  /* 1/10 second timeout for control requests */

#define STATUS_PACKET_SIZE     8    /* This must always be 8, since that's what the hardware
				     * gives us. If it's smaller, the HCD will probably crash horribly.
				     */

#define STATUS_PACKET_INTERVAL 1    /* Polling interval, in milliseconds per packet */


static int    gchub_probe          (struct usb_interface*       interface,
				    const struct usb_device_id* id);
static void   gchub_disconnect     (struct usb_interface*       interface);
static void   gchub_delete         (struct gchub_dev*           dev);

static void   gchub_irq            (struct urb*                 urb,
				    struct pt_regs*             regs);
static void   gchub_process_status (struct gchub_dev*           dev,
				    const unsigned char*        packet);

static int    buttons_to_axis      (int                         buttons,
				    int                         neg_mask,
				    int                         pos_mask);
static int    controller_init      (struct gchub_controller*    ctl,
				    struct usb_interface*       interface,
				    const char*                 format,
				    int                         port_number);
static void   controller_delete    (struct gchub_controller*    ctl);
static void   controller_sync      (struct gchub_controller*    ctl);
static void   controller_attach    (void*                       data);
static void   controller_detach    (void*                       data);


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
/************************************************************* Controller *****/
/******************************************************************************/

static int controller_init(struct gchub_controller* ctl,
			   struct usb_interface* interface,
			   const char* name_format, int port_number)
{
	char name_buf[80];
	int axis;
	struct usb_device* usb = interface_to_usbdev(interface);

	memset(ctl, 0, sizeof(*ctl));
	spin_lock_init(&ctl->reg_lock);

	/* Create a name string for this controller, and copy it to
	 * a dynamically allocated buffer.
	 */
	snprintf(name_buf, sizeof(name_buf)-1, name_format, port_number+1);
	name_buf[sizeof(name_buf)-1] = '\0';
	ctl->dev.name = kmalloc(strlen(name_buf)+1, GFP_KERNEL);
	if (!ctl->dev.name)
		return -ENOMEM;
	strcpy(ctl->dev.name, name_buf);

	/* Like the name, create a physical path naming the controller port */
	usb_make_path(usb, name_buf, sizeof(name_buf));
	snprintf(name_buf, sizeof(name_buf)-1, "%s/port%d", name_buf, port_number);
	name_buf[sizeof(name_buf)-1] = '\0';
	ctl->dev.phys = kmalloc(strlen(name_buf)+1, GFP_KERNEL);
	if (!ctl->dev.phys)
		return -ENOMEM;
	strcpy(ctl->dev.phys, name_buf);

	/* Copy USB bus info to our input device */
	ctl->dev.id.bustype = BUS_USB;
	ctl->dev.id.vendor  = usb->descriptor.idVendor;
	ctl->dev.id.product = usb->descriptor.idProduct;
	ctl->dev.id.version = usb->descriptor.bcdDevice;
	ctl->dev.dev = &interface->dev;

	/* Set up callbacks */
	ctl->dev.private = ctl;

	/* Set up work structures for queueing attach/detach operations */
	INIT_WORK(&ctl->reg_work, controller_attach, ctl);
	INIT_WORK(&ctl->unreg_work, controller_detach, ctl);

	/* Set up this input device's capabilities */
	set_bit(EV_KEY, ctl->dev.evbit);
	set_bit(EV_ABS, ctl->dev.evbit);

	set_bit(BTN_A, ctl->dev.keybit);
	set_bit(BTN_B, ctl->dev.keybit);
	set_bit(BTN_X, ctl->dev.keybit);
	set_bit(BTN_Y, ctl->dev.keybit);
	set_bit(BTN_Z, ctl->dev.keybit);
	set_bit(BTN_TL, ctl->dev.keybit);
	set_bit(BTN_TR, ctl->dev.keybit);
	set_bit(BTN_START, ctl->dev.keybit);

	for (axis=0; axis<ABS_MAX; axis++) {
		switch (axis) {

		case ABS_X:
		case ABS_Y:
		case ABS_RX:
		case ABS_RY:
			ctl->dev.absmax[axis]  =  127;
			ctl->dev.absmin[axis]  = -127;
			ctl->dev.absflat[axis] =  5;
			break;

		case ABS_HAT0X:
		case ABS_HAT0Y:
			ctl->dev.absmax[axis]  =  1;
			ctl->dev.absmin[axis]  = -1;
			break;

		case ABS_Z:
		case ABS_RZ:
			ctl->dev.absmax[axis]  = 255;
			ctl->dev.absmin[axis]  = 0;
			break;

		default:
			continue;
		}
		set_bit(axis, ctl->dev.absbit);
	}

	return 0;
}

static void controller_delete(struct gchub_controller* ctl)
{
	flush_scheduled_work();

	if (ctl->dev_registered) {
		controller_detach(ctl);
	}
	if (ctl->dev.name) {
		kfree(ctl->dev.name);
		ctl->dev.name = NULL;
	}
	if (ctl->dev.phys) {
		kfree(ctl->dev.phys);
		ctl->dev.phys = NULL;
	}
}

static int buttons_to_axis(int buttons, int neg_mask, int pos_mask)
{
	int x = 0;
	if (buttons & neg_mask)
		x -= 1;
	if (buttons & pos_mask)
		x += 1;
	return x;
}

static void controller_sync(struct gchub_controller *ctl)
{
	unsigned long flags;

	/* Create and/or destroy the device as necessary. Be careful to
	 * avoid a race condition resulting in multiple schedulings of
	 * the same registration operation.
	 */
	spin_lock_irqsave(&ctl->reg_lock, flags);
	if (!ctl->reg_in_progress) {
		if (ctl->attached && !ctl->dev_registered) {
			ctl->reg_in_progress = 1;
			spin_unlock_irqrestore(&ctl->reg_lock, flags);
			schedule_work(&ctl->reg_work);
		}
		else if (ctl->dev_registered && !ctl->attached) {
			ctl->reg_in_progress = 1;
			spin_unlock_irqrestore(&ctl->reg_lock, flags);
			schedule_work(&ctl->unreg_work);
		}
		else {
			spin_unlock_irqrestore(&ctl->reg_lock, flags);
		}
	}

	if (ctl->dev_registered && ctl->attached) {
		input_report_key(&ctl->dev, BTN_Z,     ctl->current_status.buttons & GCHUB_BUTTON_Z);
		input_report_key(&ctl->dev, BTN_TR,    ctl->current_status.buttons & GCHUB_BUTTON_R);
		input_report_key(&ctl->dev, BTN_TL,    ctl->current_status.buttons & GCHUB_BUTTON_L);
		input_report_key(&ctl->dev, BTN_A,     ctl->current_status.buttons & GCHUB_BUTTON_A);
		input_report_key(&ctl->dev, BTN_B,     ctl->current_status.buttons & GCHUB_BUTTON_B);
		input_report_key(&ctl->dev, BTN_X,     ctl->current_status.buttons & GCHUB_BUTTON_X);
		input_report_key(&ctl->dev, BTN_Y,     ctl->current_status.buttons & GCHUB_BUTTON_Y);
		input_report_key(&ctl->dev, BTN_START, ctl->current_status.buttons & GCHUB_BUTTON_START);

		input_report_abs(&ctl->dev, ABS_X,     ctl->current_status.joystick[0]);
		input_report_abs(&ctl->dev, ABS_Y,     ctl->current_status.joystick[1]);
		input_report_abs(&ctl->dev, ABS_Z,     ctl->current_status.triggers[0]);
		input_report_abs(&ctl->dev, ABS_RZ,    ctl->current_status.triggers[1]);
		input_report_abs(&ctl->dev, ABS_RX,    ctl->current_status.cstick[0]);
		input_report_abs(&ctl->dev, ABS_RY,    ctl->current_status.cstick[1]);

		input_report_abs(&ctl->dev, ABS_HAT0X, buttons_to_axis(ctl->current_status.buttons,
								       GCHUB_BUTTON_DPAD_LEFT,
								       GCHUB_BUTTON_DPAD_RIGHT));
		input_report_abs(&ctl->dev, ABS_HAT0Y, buttons_to_axis(ctl->current_status.buttons,
								       GCHUB_BUTTON_DPAD_DOWN,
								       GCHUB_BUTTON_DPAD_UP));
		input_sync(&ctl->dev);
	}
}

static void controller_attach(void *data)
{
	struct gchub_controller *ctl = (struct gchub_controller*) data;

	/* In process context, via a work queue */
	dbg("Attached '%s'", ctl->dev.name);
	input_register_device(&ctl->dev);
	ctl->dev_registered = 1;
	ctl->reg_in_progress = 0;
}

static void controller_detach(void *data)
{
	struct gchub_controller *ctl = (struct gchub_controller*) data;

	/* In process context, via a work queue */
	dbg("Removed '%s'", ctl->dev.name);
	input_unregister_device(&ctl->dev);
	ctl->dev_registered = 0;
	ctl->reg_in_progress = 0;
}


/******************************************************************************/
/************************************************** Device Communications *****/
/******************************************************************************/

static void gchub_irq(struct urb *urb, struct pt_regs *regs)
{
	struct gchub_dev *dev = (struct gchub_dev*)urb->context;

	switch (urb->status) {
	case -ECONNRESET:
	case -ESHUTDOWN:
		/* This is normal at rmmod or device disconnection time */
		return;
	case -ENOENT:
		err("ENOENT in gchub_irq");
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
	struct gchub_controller *port;
	int sync;
	int packet_type;

	/* The first byte of the packet holds the controller number,
	 * packet type, and sync bit.
	 */
	port = &dev->ports[packet[0] & 0x03];
	sync = packet[0] & GCHUB_SYNC;
	packet_type = packet[0] & 0x70;

	switch (packet_type) {

	case GCHUB_PACKET_DISCONNECT:
		port->attached = 0;
		break;

	case GCHUB_PACKET_ANALOG:
		port->attached = 1;
		port->current_status.joystick[0] = packet[1];
		port->current_status.joystick[1] = packet[2];
		port->current_status.cstick[0]   = packet[3];
		port->current_status.cstick[1]   = packet[4];
		port->current_status.triggers[0] = packet[5];
		port->current_status.triggers[1] = packet[6];
		break;

	case GCHUB_PACKET_BUTTONS:
		port->attached = 1;
		port->current_status.buttons = (packet[1] << 8) | packet[2];
		break;

	default:
		dbg("Received unknown packet type 0x%02X", packet_type);

	}

	if (sync)
		controller_sync(port);
}


/******************************************************************************/
/************************************************** USB Housekeeping **********/
/******************************************************************************/

static void gchub_delete(struct gchub_dev *dev)
{
	int i;

	if (dev->irq) {
		usb_unlink_urb(dev->irq);
		usb_free_urb(dev->irq);
	}
	if (dev->irq_data) {
		usb_buffer_free(dev->udev, STATUS_PACKET_SIZE, dev->irq_data, dev->irq_dma);
	}

	for (i=0; i<NUM_PORTS; i++)
		controller_delete(&dev->ports[i]);

	kfree(dev);
}

static int gchub_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct gchub_dev *dev = NULL;
	int retval, i;

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

	for (i=0; i<NUM_PORTS; i++) {
		retval = controller_init(&dev->ports[i], interface, "Gamecube Controller %d", i);
		if (retval)
			goto error;
	}

	dev->irq_data = usb_buffer_alloc(udev, 8, SLAB_ATOMIC, &dev->irq_dma);
	if (!dev->irq_data) {
		retval = -ENOMEM;
		goto error;
	}

	dev->irq = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->irq) {
		retval = -ENOMEM;
		goto error;
	}

	usb_fill_int_urb(dev->irq, udev, usb_rcvintpipe(udev, 1),
			 dev->irq_data, STATUS_PACKET_SIZE, gchub_irq,
			 dev, STATUS_PACKET_INTERVAL);
	usb_submit_urb(dev->irq, SLAB_ATOMIC);

	info(DEVICE_DESC " now attached");
	return 0;

error:
	usb_set_intfdata (interface, NULL);
	gchub_delete(dev);
	return retval;
}

static void gchub_disconnect(struct usb_interface *interface)
{
	struct gchub_dev *dev;

	dev = usb_get_intfdata (interface);
	usb_set_intfdata (interface, NULL);

	gchub_delete(dev);

	info(DEVICE_DESC " now disconnected");
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
