/*
 * Gamecube Controller Hub driver
 *
 * This is a Linux kernel module that exposes linux input devices corresponding
 * to all controllers attached to the gchub. It handles basic calibration, and rumble
 * support is provided using the linux force feedback interface's FF_RUMBLE effect.
 * Every port has an LED that by default glows green when a controller is connected.
 * Input events for LED_MISC can be used to change this LED to red or back to green.
 *
 * Each controller is given a name that both identifies it as a GC controller and
 * gives the port numer it's connected to. Additionally, the physical location of
 * each input device is of the form "<usb-device>/port%d", also identifying the
 * port number of a controller on the gchub.
 *
 * The button/axis mapping is as follows:
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
 * Thanks to Zinx Verituse for writing the hid-tmff module, which served as a good
 * reference for implementing force feedback support in this module.
 *
 * Copyright(c) 2004 Micah Dowty <micah@navi.cx>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#include <linux/config.h>
#include <linux/version.h>
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

#define LED_OFF        0
#define LED_RED        1
#define LED_GREEN      2
#define LED_ORANGE     (LED_RED | LED_GREEN)   /* On the prototype at least, this
						* looks orange rather than yellow */

#define EFFECTS_MAX    8

/* Effect flags */
#define EFFECT_STARTED 0         /* effect is going to play after some time */
#define EFFECT_PLAYING 1	 /* effect is playing */
#define EFFECT_USED    2         /* effect has been uploaded */

#define CHECK_OWNERSHIP(f)              (current->pid == 0 \
                                        || (f).owner == current->pid)

#define EFFECT_CHECK_ID(id)	        ((id) >= 0 && (id) < EFFECTS_MAX)

#define EFFECT_CHECK_OWNERSHIP(id, ctl) (test_bit(EFFECT_USED, (ctl)->ff_effects[id].flags) \
                                        && CHECK_OWNERSHIP((ctl)->ff_effects[id]))

/* Compatibility with Linux 2.4's task queues. Makes them look just like 2.6 work queues */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,44))
#define work_struct            tq_struct
#define flush_scheduled_work   flush_scheduled_tasks
#define schedule_work          schedule_task
static inline void INIT_WORK(struct tq_struct *tq,
                             void (*routine)(void *), void *data)
{
        INIT_LIST_HEAD(&tq->list);
        tq->sync = 0;
        tq->routine = routine;
        tq->data = data;
}
#endif

/* Version checks for other major differences betwen 2.4 and 2.6 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
#define NEW_USB_SUBSYSTEM
#define NEW_INPUT_SUBSYSTEM
#endif

/* Compatibility macros for the USB subsystem */
#ifdef NEW_USB_SUBSYSTEM
#define gchub_submit_urb(urb, mem)     usb_submit_urb(urb, mem)
#define gchub_set_intfdata(inf, dev)   usb_set_intfdata(inf, dev)
#define gchub_alloc_urb(iso, mem)      usb_alloc_urb(iso, mem)
#define gchub_usb_buffer_alloc(dev, size, mem, dma) usb_buffer_alloc(dev, size, mem, dma)
#define gchub_usb_buffer_free(dev, size, data, dma) usb_buffer_free(dev, size, data, dma)
#define gchub_fill_int_urb             usb_fill_int_urb
#define gchub_fill_control_urb         usb_fill_control_urb
#else
#define gchub_submit_urb(urb, mem)     usb_submit_urb(urb)
#define gchub_set_intfdata(inf, dev)
#define gchub_alloc_urb(iso, mem)      usb_alloc_urb(iso)
#define gchub_usb_buffer_alloc(dev, size, mem, dma) kmalloc(size, mem)
#define gchub_usb_buffer_free(dev, size, data, dma) kfree(data)
#define gchub_fill_int_urb             FILL_INT_URB
#define gchub_fill_control_urb         FILL_CONTROL_URB
#endif

struct gchub_controller_status {
	int buttons;
	int joystick[2];
	int cstick[2];
	int triggers[2];
};

struct gchub_controller_outputs {
	int rumble;
	int led_color;

	int dirty;
	spinlock_t lock;
};

struct gchub_ff_effect {
	struct ff_effect effect;

	pid_t owner;
	unsigned long flags[1];
};

struct gchub_dev;

struct gchub_controller {
	struct gchub_dev *hub;

	struct input_dev dev;
	int attached;            /* Nonzero if this controller is physically present.
				  * When it's zero, the controller is assumed to be disconnected.
				  * This is set to CONTROLLER_RETRY_ATTEMPTS every time a valid
				  * controller status update is received, and decremented every
				  * time we don't get any data. This lets us tolerate infrequent
				  * glitches.
				  */
	int dev_registered;      /* Nonzero if 'dev' has been registered */
	int reg_in_progress;     /* Nonzero if a registration/unregistration is pending */
	int calibration_valid;   /* If zero, we'll sample calibration_status next chance we get */

	struct gchub_ff_effect ff_effects[EFFECTS_MAX];
	spinlock_t ff_lock;

	struct work_struct reg_work, unreg_work;
	spinlock_t reg_lock;

	struct gchub_controller_status  current_status;
	struct gchub_controller_status  calibration_status;
	struct gchub_controller_outputs outputs;
};

struct gchub_dev {
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct input_dev        input;                  /* Input device for the gchub's buttons */
	struct usb_interface *	interface;		/* the interface for this device */

	struct urb *		irq;		        /* URB for interrupt transfers*/
	unsigned char *         irq_data;
#ifdef NEW_USB_SUBSYSTEM
	dma_addr_t              irq_dma;
#endif

	struct urb *            out_status;             /* URB for output status updates (LEDs, rumble) */
	struct usb_ctrlrequest  out_status_request;
	int                     out_status_in_progress;  /* Nonzero if a status update is in progress */
	int                     out_status_pending_sync; /* Another URB needs to be sent right after this one */
	spinlock_t              out_status_lock;         /* Lock for serializing status updates */

	struct gchub_controller ports[NUM_PORTS];
};

#define REQUEST_TIMEOUT       (HZ/10)  /* 1/10 second timeout for control requests */

#define STATUS_PACKET_SIZE        8    /* This must always be 8, since that's what the hardware
				        * gives us. If it's smaller, the HCD will probably crash horribly.
				        */

#define STATUS_PACKET_INTERVAL    1  /* Polling interval, in milliseconds per packet */

#define CONTROLLER_RETRY_ATTEMPTS 5  /* Number of times a controller must report no data to be
				      * considered disconnected. This gives us tolerance for glitches
				      * in the received or transmitted packets.
				      */


#ifdef NEW_USB_SUBSYSTEM
static int    gchub_probe              (struct usb_interface*           interface,
					const struct usb_device_id*     id);
static void   gchub_disconnect         (struct usb_interface*           interface);
static void   gchub_out_request_irq    (struct urb*                     urb,
					struct pt_regs*                 regs);
static void   gchub_irq                (struct urb*                     urb,
					struct pt_regs*                 regs);
#else
static void  *gchub_probe              (struct usb_device*              udev,
					unsigned int                    ifnum,
			                const struct usb_device_id*     id);
static void   gchub_disconnect         (struct usb_device*              udev,
					void*                           ptr);
static void   gchub_out_request_irq    (struct urb*                     urb);
static void   gchub_irq                (struct urb*                     urb);
#endif

static void   gchub_delete             (struct gchub_dev*               dev);

static void   gchub_process_status     (struct gchub_dev*               dev,
					const unsigned char*            packet);
static void   gchub_sync_output_status (struct gchub_dev*               dev);
static void   gchub_fill_output_request(struct gchub_dev*               dev);

static int    buttons_to_axis          (int                             buttons,
					int                             neg_mask,
					int                             pos_mask);
static int    controller_init          (struct gchub_controller*        ctl,
					struct gchub_dev*               hub,
					struct usb_interface*           interface,
					const char*                     format,
					int                             port_number);
static void   controller_init_inputdev (struct gchub_controller*        ctl);
static void   controller_delete        (struct gchub_controller*        ctl);
static void   controller_sync          (struct gchub_controller*        ctl);
static void   controller_report_status (struct gchub_controller*        ctl,
					struct gchub_controller_status* status);
static void   controller_status_diff   (struct gchub_controller_status* a,
					struct gchub_controller_status* b,
					struct gchub_controller_status* out);
static void   controller_attach        (void*                           data);
static void   controller_detach        (void*                           data);
static void   controller_set_rumble    (struct gchub_controller*        ctl,
					int                             rumble);
static void   controller_set_led       (struct gchub_controller*        ctl,
					int                             led_color);

/* Input device callbacks */
static int    controller_event         (struct input_dev*               dev,
					unsigned int                    type,
					unsigned int                    code,
					int                             value);
static int    controller_upload_effect (struct input_dev*               dev,
					struct ff_effect*               effect);
static int    controller_erase_effect  (struct input_dev*               dev,
					int                             effect_id);
static int    controller_ff_event      (struct gchub_controller*        ctl,
					unsigned int                    code,
					int                             value);
static int    controller_led_event     (struct gchub_controller*        ctl,
					unsigned int                    code,
					int                             value);
#ifdef NEW_INPUT_SUBSYSTEM
static int    controller_flush         (struct input_dev*               dev,
					struct file*                    file);
#endif


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
			   struct gchub_dev* hub,
			   struct usb_interface* interface,
			   const char* name_format, int port_number)
{
	char name_buf[80];

	memset(ctl, 0, sizeof(*ctl));
	spin_lock_init(&ctl->reg_lock);
	spin_lock_init(&ctl->ff_lock);
	spin_lock_init(&ctl->outputs.lock);

	ctl->outputs.dirty = 1;
	ctl->hub = hub;

	/* Create a name string for this controller, and copy it to
	 * a dynamically allocated buffer.
	 */
	snprintf(name_buf, sizeof(name_buf)-1, name_format, port_number+1);
	name_buf[sizeof(name_buf)-1] = '\0';
	ctl->dev.name = kmalloc(strlen(name_buf)+1, GFP_KERNEL);
	if (!ctl->dev.name)
		return -ENOMEM;
	strcpy(ctl->dev.name, name_buf);

#ifdef NEW_INPUT_SUBSYSTEM
	/* In Linux 2.6, we can give the input device info about the physical
	 * device supplying it with data. We attach information about our USB
	 * device, and about which controller port this is referring to.
	 */
	{
		struct usb_device* usb = interface_to_usbdev(interface);

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
	}
#endif

	return 0;
}

static void controller_init_inputdev(struct gchub_controller* ctl)
{
	/* This performs input device initialization that we should
	 * do each time we attach the device. The callbacks will get
	 * reset on unregistration, and the LED bits might be in a
	 * strange state. The rest just doesn't hurt.
	 */

	int axis;

	/* Set up callbacks */
	ctl->dev.private = ctl;
	ctl->dev.event = controller_event;
	ctl->dev.upload_effect = controller_upload_effect;
	ctl->dev.erase_effect = controller_erase_effect;
#ifdef NEW_INPUT_SUBSYSTEM
	ctl->dev.flush = controller_flush;
#endif

	/* We support one LED, to change the color of the port from green to red */
	set_bit(EV_LED, ctl->dev.evbit);
	set_bit(LED_MISC, ctl->dev.ledbit);
	memset(ctl->dev.led, 0, sizeof(ctl->dev.led));

	/* Support the rumble motor via the FF_RUMBLE force feedback bit */
	set_bit(EV_FF, ctl->dev.evbit);
	set_bit(FF_RUMBLE, ctl->dev.ffbit);
	ctl->dev.ff_effects_max = EFFECTS_MAX;

	/* Add our buttons */
	set_bit(EV_KEY, ctl->dev.evbit);
	set_bit(BTN_A, ctl->dev.keybit);
	set_bit(BTN_B, ctl->dev.keybit);
	set_bit(BTN_X, ctl->dev.keybit);
	set_bit(BTN_Y, ctl->dev.keybit);
	set_bit(BTN_Z, ctl->dev.keybit);
	set_bit(BTN_TL, ctl->dev.keybit);
	set_bit(BTN_TR, ctl->dev.keybit);
	set_bit(BTN_START, ctl->dev.keybit);

	/* Add the axes, which includes the D-pad mapped onto a hat */
	set_bit(EV_ABS, ctl->dev.evbit);
	for (axis=0; axis<ABS_MAX; axis++) {
		switch (axis) {

		case ABS_X:
		case ABS_Y:
		case ABS_RX:
		case ABS_RY:
			ctl->dev.absmax[axis]  =  127;
			ctl->dev.absmin[axis]  = -128;
			ctl->dev.absflat[axis] =  10;
			ctl->dev.absfuzz[axis] =  2;
			break;

		case ABS_HAT0X:
		case ABS_HAT0Y:
			ctl->dev.absmax[axis]  =  1;
			ctl->dev.absmin[axis]  = -1;
			break;

		case ABS_Z:
		case ABS_RZ:
			/* Important: Our minimum is actaully zero,
			 * but we lie here for the sake of apps that assume
			 * the center position is halfway between max and
			 * min. Specifically, this is necessary to keep
			 * zsnes happy. Yes this is dirty, but easier than
			 * fixing all those broken apps...
			 */
			ctl->dev.absmax[axis]  = 255;
			ctl->dev.absmin[axis]  = -256;
			ctl->dev.absflat[axis] =  10;
			ctl->dev.absfuzz[axis] =  2;
			break;

		default:
			continue;
		}
		set_bit(axis, ctl->dev.absbit);
	}
}

static void controller_delete(struct gchub_controller* ctl)
{
	dbg("controller_delete '%s'", ctl->dev.name);
	flush_scheduled_work();

	if (ctl->dev_registered) {
		controller_detach(ctl);
	}
	if (ctl->dev.name) {
		kfree(ctl->dev.name);
		ctl->dev.name = NULL;
	}
#ifdef NEW_INPUT_SUBSYSTEM
	if (ctl->dev.phys) {
		kfree(ctl->dev.phys);
		ctl->dev.phys = NULL;
	}
#endif
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

			/* Note that in 2.6 it isn't required that we
			 * reinitialize the work struct each time, but in
			 * 2.4 with task queues this is necessary.
			 */
			INIT_WORK(&ctl->reg_work, controller_attach, ctl);
			schedule_work(&ctl->reg_work);
		}
		else if (ctl->dev_registered && !ctl->attached) {
			ctl->reg_in_progress = 1;
			spin_unlock_irqrestore(&ctl->reg_lock, flags);

	                INIT_WORK(&ctl->unreg_work, controller_detach, ctl);
			schedule_work(&ctl->unreg_work);
		}
		else {
			spin_unlock_irqrestore(&ctl->reg_lock, flags);
		}
	}
	else {
		spin_unlock_irqrestore(&ctl->reg_lock, flags);
	}

	if (ctl->dev_registered && ctl->attached) {
		struct gchub_controller_status calibrated;

		if (!ctl->calibration_valid) {
			ctl->calibration_status = ctl->current_status;
			ctl->calibration_valid = 1;
		}

		controller_status_diff(&ctl->current_status,
				       &ctl->calibration_status,
				       &calibrated);
		controller_report_status(ctl, &calibrated);
	}
}

static void controller_status_diff(struct gchub_controller_status* a,
				   struct gchub_controller_status* b,
				   struct gchub_controller_status* out)
{
	out->buttons = a->buttons;
	out->joystick[0] = a->joystick[0] - b->joystick[0];
	out->joystick[1] = a->joystick[1] - b->joystick[1];
	out->cstick[0] = a->cstick[0] - b->cstick[0];
	out->cstick[1] = a->cstick[1] - b->cstick[1];
	out->triggers[0] = a->triggers[0] - b->triggers[0];
	out->triggers[1] = a->triggers[1] - b->triggers[1];
}

static void controller_report_status(struct gchub_controller* ctl,
				     struct gchub_controller_status* status)
{
	input_report_key(&ctl->dev, BTN_Z,     status->buttons & GCHUB_BUTTON_Z);
	input_report_key(&ctl->dev, BTN_TR,    status->buttons & GCHUB_BUTTON_R);
	input_report_key(&ctl->dev, BTN_TL,    status->buttons & GCHUB_BUTTON_L);
	input_report_key(&ctl->dev, BTN_A,     status->buttons & GCHUB_BUTTON_A);
	input_report_key(&ctl->dev, BTN_B,     status->buttons & GCHUB_BUTTON_B);
	input_report_key(&ctl->dev, BTN_X,     status->buttons & GCHUB_BUTTON_X);
	input_report_key(&ctl->dev, BTN_Y,     status->buttons & GCHUB_BUTTON_Y);
	input_report_key(&ctl->dev, BTN_START, status->buttons & GCHUB_BUTTON_START);

	input_report_abs(&ctl->dev, ABS_X,     status->joystick[0]);
	input_report_abs(&ctl->dev, ABS_Y,    -status->joystick[1]);
	input_report_abs(&ctl->dev, ABS_Z,     status->triggers[0]);
	input_report_abs(&ctl->dev, ABS_RZ,    status->triggers[1]);
	input_report_abs(&ctl->dev, ABS_RX,    status->cstick[0]);
	input_report_abs(&ctl->dev, ABS_RY,   -status->cstick[1]);

	input_report_abs(&ctl->dev, ABS_HAT0X, buttons_to_axis(status->buttons,
							       GCHUB_BUTTON_DPAD_LEFT,
							       GCHUB_BUTTON_DPAD_RIGHT));
	input_report_abs(&ctl->dev, ABS_HAT0Y, buttons_to_axis(status->buttons,
							       GCHUB_BUTTON_DPAD_UP,
							       GCHUB_BUTTON_DPAD_DOWN));

#ifdef NEW_INPUT_SUBSYSTEM
	input_sync(&ctl->dev);
#endif
}

static void controller_attach(void *data)
{
	struct gchub_controller *ctl = (struct gchub_controller*) data;

	/* In process context, via a work queue */
	info("Attached '%s'", ctl->dev.name);
	controller_init_inputdev(ctl);
	input_register_device(&ctl->dev);
	ctl->dev_registered = 1;
	ctl->calibration_valid = 0;
	ctl->reg_in_progress = 0;

	controller_set_led(ctl, LED_GREEN);
	controller_set_rumble(ctl, 0);
	gchub_sync_output_status(ctl->hub);
}

static void controller_detach(void *data)
{
	struct gchub_controller *ctl = (struct gchub_controller*) data;

	/* Linux 2.4 at least seems to like calling our callbacks after
	 * our kernel module has been removed. Set them to NULL so that
	 * even if it does keep a copy of our input device around, it
	 * won't be able to call into removed code.
	 */
	ctl->dev.private = NULL;
	ctl->dev.event = NULL;
	ctl->dev.upload_effect = NULL;
	ctl->dev.erase_effect = NULL;
#ifdef NEW_INPUT_SUBSYSTEM
	ctl->dev.flush = NULL;
#endif

	/* In process context, via a work queue */
	info("Removed '%s'", ctl->dev.name);
	input_unregister_device(&ctl->dev);
	ctl->dev_registered = 0;
	ctl->reg_in_progress = 0;

	/* Turn off the rumble flag so it doesn't turn on the next
	 * time a controller is plugged into this port, and extinguish
	 * the LED.
	 */
	controller_set_rumble(ctl, 0);
	controller_set_led(ctl, LED_OFF);
	gchub_sync_output_status(ctl->hub);
}

static void controller_set_rumble(struct gchub_controller* ctl,
				  int rumble)
{
	unsigned long flags;
	spin_lock_irqsave(&ctl->outputs.lock, flags);
	if (ctl->outputs.rumble != rumble) {
		ctl->outputs.rumble = rumble;
		ctl->outputs.dirty = 1;
	}
	spin_unlock_irqrestore(&ctl->outputs.lock, flags);
}

static void controller_set_led(struct gchub_controller* ctl,
				int led_color)
{
	unsigned long flags;

	spin_lock_irqsave(&ctl->outputs.lock, flags);

	/* Keep the device's LED bits in sync */
	if (led_color == LED_GREEN)
		clear_bit(LED_MISC, ctl->dev.led);
	else if (led_color == LED_RED)
		set_bit(LED_MISC, ctl->dev.led);

	dbg("Changing LED color from %d to %d", ctl->outputs.led_color, led_color);

	if (ctl->outputs.led_color != led_color) {
		ctl->outputs.led_color = led_color;
		ctl->outputs.dirty = 1;
	}

	spin_unlock_irqrestore(&ctl->outputs.lock, flags);
}


/******************************************************************************/
/*************************************************** Controller Callbacks *****/
/******************************************************************************/

static int controller_event(struct input_dev* dev, unsigned int type,
			    unsigned int code, int value)
{
	struct gchub_controller *ctl = (struct gchub_controller*) dev->private;
	switch (type) {

	case EV_FF:
		return controller_ff_event(ctl, code, value);

	case EV_LED:
		return controller_led_event(ctl, code, value);

	}
	return -EINVAL;
}

static int controller_ff_event(struct gchub_controller* ctl,
			       unsigned int code, int value)
{
        /*
        if (!EFFECT_CHECK_ID(code))
		return -EINVAL;
	if (!EFFECT_CHECK_OWNERSHIP(code, ctl))
		return -EACCES;
	if (value < 0)
		return -EINVAL;
        */

	/* FIXME: implement support for timers */
	controller_set_rumble(ctl, value);
	gchub_sync_output_status(ctl->hub);

	return 0;
}

static int controller_led_event(struct gchub_controller* ctl,
			       unsigned int code, int value)
{
	dbg("entering controller_led_event()");

	if (!ctl->attached) {
	  dbg("in controller_led_event, controller no longer attached");
	  return 0;
	}

	/* The input core has already set ctl->dev.led accordingly,
	 * we just have to update the current state of everything.
	 */
	if (test_bit(LED_MISC, ctl->dev.led))
		controller_set_led(ctl, LED_RED);
	else
		controller_set_led(ctl, LED_GREEN);

	gchub_sync_output_status(ctl->hub);

	dbg("leaving controller_led_event()");

	return 0;
}

static int controller_upload_effect(struct input_dev* dev,
				    struct ff_effect* effect)
{
	struct gchub_controller *ctl = (struct gchub_controller*) dev->private;
	unsigned long flags;
	int id;

	if (!test_bit(effect->type, dev->ffbit))
		return -EINVAL;

	spin_lock_irqsave(&ctl->ff_lock, flags);

	if (effect->id == -1) {
		/* Find a free effect ID */
		for (id=0; id < EFFECTS_MAX; id++) {
			if (!test_bit(EFFECT_USED, ctl->ff_effects[id].flags))
				break;
		}
		if (id >= EFFECTS_MAX) {
			spin_unlock_irqrestore(&ctl->ff_lock, flags);
			return -ENOSPC;
		}
		effect->id = id;
	}
	else {
		/* Validate the ID they gave us */
		id = effect->id;
		if (!EFFECT_CHECK_ID(id)) {
			spin_unlock_irqrestore(&ctl->ff_lock, flags);
			return -EINVAL;
		}
		if (!EFFECT_CHECK_OWNERSHIP(id, ctl)) {
			spin_unlock_irqrestore(&ctl->ff_lock, flags);
			return -EACCES;
		}
	}

	/* Upload the effect */
	ctl->ff_effects[id].owner = current->pid;
	ctl->ff_effects[id].flags[0] = 0;
	set_bit(EFFECT_USED, ctl->ff_effects[id].flags);
	ctl->ff_effects[id].effect = *effect;

	spin_unlock_irqrestore(&ctl->ff_lock, flags);
	return 0;
}

static int controller_erase_effect(struct input_dev* dev, int id)
{
	struct gchub_controller *ctl = (struct gchub_controller*) dev->private;
	unsigned long flags;

	spin_lock_irqsave(&ctl->ff_lock, flags);

	if (!EFFECT_CHECK_ID(id)) {
		spin_unlock_irqrestore(&ctl->ff_lock, flags);
		return -EINVAL;
	}
	if (!EFFECT_CHECK_OWNERSHIP(id, ctl)) {
		spin_unlock_irqrestore(&ctl->ff_lock, flags);
		return -EACCES;
	}

	ctl->ff_effects[id].flags[0] = 0;

	spin_unlock_irqrestore(&ctl->ff_lock, flags);
	return 0;
}

#ifdef NEW_INPUT_SUBSYSTEM
/* Erase all effects owned by the current process */
static int controller_flush(struct input_dev* dev, struct file* file)
{
	struct gchub_controller *ctl = (struct gchub_controller*) dev->private;
	unsigned long flags;
	int id;

	spin_lock_irqsave(&ctl->ff_lock, flags);

	for (id=0; id < EFFECTS_MAX; id++) {
		if (ctl->ff_effects[id].owner == current->pid &&
		    test_bit(EFFECT_USED, ctl->ff_effects[id].flags)) {

			/* We don't use controller_erase effect
			 * here because we already hold the lock,
			 * and we don't need its extra parameter validation.
			 */
			ctl->ff_effects[id].flags[0] = 0;
		}
	}

	spin_unlock_irqrestore(&ctl->ff_lock, flags);
	return 0;
}
#endif


/******************************************************************************/
/************************************************** Device Communications *****/
/******************************************************************************/

#ifdef NEW_USB_SUBSYSTEM
static void gchub_irq(struct urb *urb, struct pt_regs *regs)
#else
static void gchub_irq(struct urb *urb)
#endif
{
	struct gchub_dev *dev = (struct gchub_dev*)urb->context;

	switch (urb->status) {
	case -ECONNRESET:
	case -ESHUTDOWN:
	case -ETIMEDOUT:
		/* This is normal at rmmod or device disconnection time */
		goto cancel_urb;
	case -ENOENT:
		err("ENOENT in gchub_irq");
		goto cancel_urb;
	}

	if (urb->status != 0 || urb->actual_length != STATUS_PACKET_SIZE) {
		err("Bad URB status/size: status=%d, length=%d", urb->status, urb->actual_length);
		goto cancel_urb;
	}

	gchub_process_status(dev, dev->irq_data);

#ifdef NEW_USB_SUBSYSTEM
	/* Resubmit the URB to get another interrupt transfer going.
	 * In Linux 2.4, the URBs get automagically resubmitted (good for us,
	 * but bad for a lot of other things). cancel_urb just returns
	 * without doing this.
	 */
	gchub_submit_urb(urb, SLAB_ATOMIC);
 cancel_urb:
	return;
#else
	/* The URB auto-resubmits, we have to cancel it explicitly */
	return;
 cancel_urb:
	usb_unlink_urb(urb);
#endif
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
		/* We get a disconnect packet when the firmware timed out
		 * reading a response from the indicated controller. This
		 * generally means the controller was unplugged, but there
		 * are some special circumstances we handle here.
		 *
		 * First, we don't actually disconnect a controller until
		 * it has many consecutive timeouts. This filters out
		 * signal glitches.
		 *
		 * Second, if you hold down X, Y, and Start for three
		 * seconds, the controller will stop responding to packets.
		 * This is the button combination used by the Gamecube
		 * for recalibration- they implement this by having
		 * the controller appear as though it's actually been
		 * unplugged and plugged back in. This will annoy any
		 * apps that are reading from the controller though, so
		 * we cheat. If we see disconnects and the X, Y, and Start
		 * buttons were down last we saw them, assume the controller
		 * was put into calibration mode. We assume it's still
		 * connected, but invalidate our current calibration for
		 * it. Unfortunately, this means that if a controller
		 * really was unplugged while someone was holding down X, Y,
		 * and Start we won't see it.
		 */

		if ((port->current_status.buttons & GCHUB_BUTTON_X) &&
		    (port->current_status.buttons & GCHUB_BUTTON_Y) &&
		    (port->current_status.buttons & GCHUB_BUTTON_START)) {

			port->calibration_valid = 0;
		}
		else if (port->attached > 0) {
			port->attached--;
		}
		else {
			port->attached = 0;
		}
		break;

	case GCHUB_PACKET_ANALOG:
		port->attached = CONTROLLER_RETRY_ATTEMPTS;
		port->current_status.joystick[0] = packet[1];
		port->current_status.joystick[1] = packet[2];
		port->current_status.cstick[0]   = packet[3];
		port->current_status.cstick[1]   = packet[4];
		port->current_status.triggers[0] = packet[5];
		port->current_status.triggers[1] = packet[6];
		break;

	case GCHUB_PACKET_BUTTONS:
		port->attached = CONTROLLER_RETRY_ATTEMPTS;
		port->current_status.buttons = (packet[1] << 8) | packet[2];
		break;

	default:
		dbg("Received unknown packet type 0x%02X", packet_type);

	}

	if (sync)
		controller_sync(port);
}

/* Check for changes in controller outputs that haven't been sent
 * yet. If there are any and an update hasn't already been started,
 * we start one. If an update had already been started, once complete
 * it will automatically check again for dirty outputs and send
 * another update if necessary.
 */
static void gchub_sync_output_status(struct gchub_dev* dev)
{
	unsigned long flags;
	int dirty = 0;
	int i;

	dbg("sync_output_status");

	/* Are any outputs still dirty? */
	for (i=0; i<NUM_PORTS; i++)
		if (dev->ports[i].outputs.dirty)
			dirty++;
	dbg("dirty outputs: %d", dirty);

	if (!dirty)
		return;

	/* Send out a status packet if one isn't already on its way */
	spin_lock_irqsave(&dev->out_status_lock, flags);
	if (dev->out_status_in_progress) {
		/* A packet is already on its way, but we'll need
		 * another one to be sent right afterward.
		 */
		dev->out_status_pending_sync = 1;
		spin_unlock_irqrestore(&dev->out_status_lock, flags);

		dbg("Status packet already pending");
	}
	else {
		dev->out_status_in_progress = 1;
		spin_unlock_irqrestore(&dev->out_status_lock, flags);

		gchub_fill_output_request(dev);
		gchub_submit_urb(dev->out_status, SLAB_ATOMIC);

		dbg("Submitted status packet");
	}
}

/* Collect output data from all ports into our
 * out_status_request, clearing dirty flags as we go.
 * Must work in interrupt context!
 */
static void   gchub_fill_output_request(struct gchub_dev* dev)
{
	struct gchub_controller_outputs outputs[NUM_PORTS];
	int port;
	unsigned long flags;
	unsigned short led_bits, rumble_bits;

	gchub_fill_control_urb(dev->out_status, dev->udev, usb_sndctrlpipe(dev->udev, 0),
			       (unsigned char*) &dev->out_status_request, NULL,
			       0, gchub_out_request_irq, dev);

	/* Take a snapshot of each port's outputs and clear dirty flags,
	 * while holding the outputs' spinlock.
	 */
	for (port=0; port<NUM_PORTS; port++) {
		spin_lock_irqsave(&dev->ports[port].outputs.lock, flags);
		outputs[port].rumble = dev->ports[port].outputs.rumble;
		outputs[port].led_color = dev->ports[port].outputs.led_color;
		dev->ports[port].outputs.dirty = 0;
		spin_unlock_irqrestore(&dev->ports[port].outputs.lock, flags);
	}

	rumble_bits = 0;
	if (outputs[0].rumble) rumble_bits |= GCHUB_RUMBLE_PORT0;
	if (outputs[1].rumble) rumble_bits |= GCHUB_RUMBLE_PORT1;
	if (outputs[2].rumble) rumble_bits |= GCHUB_RUMBLE_PORT2;
	if (outputs[3].rumble) rumble_bits |= GCHUB_RUMBLE_PORT3;

	led_bits = 0;
	if (outputs[0].led_color & LED_RED) led_bits |= GCHUB_LED_PORT0_RED;
	if (outputs[1].led_color & LED_RED) led_bits |= GCHUB_LED_PORT1_RED;
	if (outputs[2].led_color & LED_RED) led_bits |= GCHUB_LED_PORT2_RED;
	if (outputs[3].led_color & LED_RED) led_bits |= GCHUB_LED_PORT3_RED;
	if (outputs[0].led_color & LED_GREEN) led_bits |= GCHUB_LED_PORT0_GREEN;
	if (outputs[1].led_color & LED_GREEN) led_bits |= GCHUB_LED_PORT1_GREEN;
	if (outputs[2].led_color & LED_GREEN) led_bits |= GCHUB_LED_PORT2_GREEN;
	if (outputs[3].led_color & LED_GREEN) led_bits |= GCHUB_LED_PORT3_GREEN;

	memset(&dev->out_status_request, 0, sizeof(dev->out_status_request));
	dev->out_status_request.bRequestType = USB_TYPE_VENDOR;
	dev->out_status_request.bRequest = GCHUB_CTRL_SET_STATUS;
	dev->out_status_request.wValue = cpu_to_le16p(&rumble_bits);
	dev->out_status_request.wIndex = cpu_to_le16p(&led_bits);
}

/* An output request just completed, but we might need another right away. */
#ifdef NEW_USB_SUBSYSTEM
static void gchub_out_request_irq(struct urb* urb, struct pt_regs* regs)
#else
static void gchub_out_request_irq(struct urb* urb)
#endif
{
	struct gchub_dev *dev = (struct gchub_dev*)urb->context;
	unsigned long flags;

	dbg("out_request_irq, status=%d", urb->status);

	if (urb->status != 0) {
		/* Some sort of error, or an intentional
		 * cancellation.  we're not pending any more,
		 * but don't try to start anything else.
		 */
		dev->out_status_pending_sync = 0;
		dev->out_status_in_progress = 0;
	}
	else {

		spin_lock_irqsave(&dev->out_status_lock, flags);
		if (dev->out_status_pending_sync) {
			/* Yep, we need another. Leave out_status_in_progress
			 * set, and resumbit this URB with new data.
			 */
			dev->out_status_pending_sync = 0;
			spin_unlock_irqrestore(&dev->out_status_lock, flags);

			gchub_fill_output_request(dev);
			gchub_submit_urb(dev->out_status, SLAB_ATOMIC);

			dbg("sending another output request");
		}
		else {
			/* Done for now. */
			dev->out_status_pending_sync = 0;
			dev->out_status_in_progress = 0;
			spin_unlock_irqrestore(&dev->out_status_lock, flags);

			dbg("no more pending output requests");
		}
	}
}


/******************************************************************************/
/************************************************** USB Housekeeping **********/
/******************************************************************************/

static void gchub_delete(struct gchub_dev *dev)
{
	int i;

	dbg("gchub_delete");

	if (dev->irq) {
		usb_unlink_urb(dev->irq);
		usb_free_urb(dev->irq);
		dev->irq = NULL;
	}
	if (dev->irq_data) {
		gchub_usb_buffer_free(dev->udev, STATUS_PACKET_SIZE, dev->irq_data, dev->irq_dma);
		dev->irq_data = NULL;
	}

	/* It's important that we delete controllers before the out_status URB,
	 * since a controller might still be in the process of trying to update
	 * rumble or LED status.
	 */
	for (i=0; i<NUM_PORTS; i++)
		controller_delete(&dev->ports[i]);

	if (dev->out_status) {
		usb_unlink_urb(dev->out_status);
		usb_free_urb(dev->out_status);
		dev->out_status = NULL;
	}

	kfree(dev);
}

#ifdef NEW_USB_SUBSYSTEM
static int gchub_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
#else
static void *gchub_probe(struct usb_device *udev, unsigned int ifnum,
			 const struct usb_device_id *id)
{
	struct usb_interface *interface = &udev->actconfig->interface[ifnum];
#endif
	struct gchub_dev *dev = NULL;
	int retval, i;

	/* See if the device offered us matches what we can accept */
	if ((udev->descriptor.idVendor != GCHUB_VENDOR_ID) ||
	   (udev->descriptor.idProduct != GCHUB_PRODUCT_ID)) {
		retval = -ENODEV;
		goto early_error;
	}

	/* allocate memory for our device state and intialize it */
	dev = kmalloc(sizeof(struct gchub_dev), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		retval = -ENOMEM;
		goto early_error;
	}
	memset(dev, 0, sizeof(*dev));

	spin_lock_init(&dev->out_status_lock);
	dev->udev = udev;
	dev->interface = interface;
	gchub_set_intfdata(interface, dev);

	for (i=0; i<NUM_PORTS; i++) {
		retval = controller_init(&dev->ports[i], dev, interface,
					 "Gamecube Controller %d", i);
		if (retval)
			goto error;
	}

	/* Prepere the irq URB */
	dev->irq_data = gchub_usb_buffer_alloc(udev, 8, GFP_KERNEL, &dev->irq_dma);
	if (!dev->irq_data) {
		retval = -ENOMEM;
		goto error;
	}
	dev->irq = gchub_alloc_urb(0, GFP_KERNEL);
	if (!dev->irq) {
		retval = -ENOMEM;
		goto error;
	}

	/* Prepare the output status URB */
	dev->out_status = gchub_alloc_urb(0, GFP_KERNEL);
	if (!dev->out_status) {
		retval = -ENOMEM;
		goto error;
	}

	/* Use it to make sure the device has our default state */
	gchub_sync_output_status(dev);

	/* Start the interrupt data flowing */
	gchub_fill_int_urb(dev->irq, udev, usb_rcvintpipe(udev, 1),
			   dev->irq_data, STATUS_PACKET_SIZE, gchub_irq,
			   dev, STATUS_PACKET_INTERVAL);
	gchub_submit_urb(dev->irq, SLAB_ATOMIC);

	info(DEVICE_DESC " now attached");

#ifdef NEW_USB_SUBSYSTEM
	return 0;
error:
	gchub_set_intfdata (interface, NULL);
	gchub_delete(dev);
early_error:
	return retval;
#else
	return dev;
error:
	gchub_set_intfdata (interface, NULL);
	gchub_delete(dev);
early_error:
	return NULL;
#endif
}


#ifdef NEW_USB_SUBSYSTEM
static void gchub_disconnect(struct usb_interface *interface)
{
	struct gchub_dev *dev;
	dev = usb_get_intfdata (interface);

#else
static void gchub_disconnect(struct usb_device *udev, void *ptr)
{
	struct gchub_dev *dev = ptr;
#endif

	dbg("entering gchub_disconnect");

	gchub_set_intfdata (interface, NULL);
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
