/*
 * Raster Wand driver
 *
 * This is a module for the Linux kernel, providing a userspace interface
 * to the Raster Wand hardware via the USB protocol described in efs_protocol.h.
 *
 * This driver exposes three basic interfaces:
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


/* Timing calculated from the current status and settings */
struct rwand_timings {
	int    column_width;
	int    gap_width;
	int    fwd_phase;
	int    rev_phase;
	int    coil_begin;
	int    coil_end;
};

/* Settings peculiar to each model of rasterwand clock */
struct model_intrinsics {
	int                  model;
	const char *         name;
	int                  coil_center;
	int                  coil_width;
	int                  fine_adjust;
	int                  starting_edges;  /* Number of edges to successfully exit startup */
	struct rwand_startup startup;
};


#define FILTER_SIZE      256    /* Increasing this will smooth out display jitter
				 * at the expense of responding more slowly to real changes
				 */
#define PERIOD_TOLERANCE 20     /* Increasing this will reduce the frequency of small jumps
				 * in the display alignment at the increased risk of having
				 * slightly incorrect timing.
				 */

#define STABILIZER_EDGES 8      /* Number of edges to successfully exit stabilization */
#define STABILIZER_TIME  HZ     /* Number of jiffies to unsuccessfully exit stabilization */

/* A simple averaging low-pass filter, O(1) */
struct filter {
	int                     buffer[FILTER_SIZE];    /* Circular buffer */
	int                     total;                  /* Total of all values currently in the buffer */
	int                     n_values;               /* Number of values currently in the buffer */
	int                     pointer;                /* Location to store the next new value in */
};

#define STATE_OFF         0      /* Don't even try to start the display, we're off */
#define STATE_STARTING    1      /* We have no timing feedback, we're just blindly trying to get it started.
				  * This state is done as soon as we get STARTING_EDGES edges from the device.
				  */
#define STATE_STABILIZING 2      /* We have timing feedback, but we aren't sure it's stable yet.
				  * This moves to STATE_RUNNING if it gets a sufficient number
				  * of sync edges, or back to STATE_STARTING if it times out.
				  */
#define STATE_RUNNING     3      /* Yay, we're running */


struct rwand_dev {
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct input_dev *       input;                  /* Input device for the rwand's buttons */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char		minor;			/* the starting minor number for this device */

	struct urb *		irq;		        /* URB for interrupt transfers*/
	unsigned char *         irq_data;
	dma_addr_t              irq_dma;
	struct usb_ctrlrequest  status_request;

	struct rwand_settings   settings;
	int                     settings_dirty;         /* Set whenever the settings have changed but
							 * timing has not been updated to reflect them.
							 */

	struct model_intrinsics*intrinsics;             /* Values intrinsic to this model of rasterwand */
	struct rwand_startup    startup;                /* Startup timings. Normally these are just copied
							 * from the intrinsics, but they may be tweaked from userspace.
							 */

	unsigned char           fb[RWAND_FB_BYTES+4];   /* Our local copy of the device framebuffer. The 4 here gives
							 * us some slack for reading past the end of the framebuffer
							 * when doing writes in blocks of 4, 8, or 12 bytes.
							 */
	struct rwand_status     status;
	int                     state;

	struct filter           period_filter;          /* We store the last several periods recorded, and actually
							 * perform the timing calculation using their mean. This prevents
							 * small electrical or mechanical glitches from having much
							 * effect on the display.
							 */
	int                     filtered_period;

	int                     edge_count;             /* A larger resettable extension of the device's
							 * tiny edge counter */
	unsigned long           state_timer;

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

#define STATUS_PACKET_SIZE 8    /* This must always be 8, since that's what the hardware
				 * gives us. If it's smaller, the HCD will probably crash horribly.
				 */

#ifdef VERTICAL_FLIP
#define CONVERT_FB_BYTE(b)   (reverse_bits(b))
#else
#define CONVERT_FB_BYTE(b)   (b)
#endif


/******************************************************************************/
/************************************************** Function Prototypes *******/
/******************************************************************************/

static ssize_t rwand_write       (struct file *file, const char *buffer, size_t count, loff_t *ppos);
static int     rwand_ioctl       (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int     rwand_open        (struct inode *inode, struct file *file);
static int     rwand_release     (struct inode *inode, struct file *file);

static int     rwand_probe       (struct usb_interface *interface, const struct usb_device_id *id);
static void    rwand_disconnect  (struct usb_interface *interface);

static void    rwand_status_irq  (struct urb *urb, struct pt_regs *regs);
static void    rwand_nb_irq      (struct urb *urb, struct pt_regs *regs);

static void    rwand_delete      (struct rwand_dev *dev);

static int     rwand_read_byte   (struct rwand_dev *dev, unsigned short request,
				  unsigned short wValue, unsigned short wIndex);
static void    rwand_nb_request  (struct rwand_dev *dev, unsigned short request,
				  unsigned short wValue, unsigned short wIndex);

static int     rwand_wait_for_fb (struct rwand_dev *dev);
static void    rwand_update_fb   (struct rwand_dev *dev, int bytes, unsigned char *fb);
static void    rwand_request_flip(struct rwand_dev *dev);

static void    rwand_decode_status  (const unsigned char *in, struct rwand_status *out);
static void    rwand_process_status (struct rwand_dev *dev, const unsigned char *packet);
static void    rwand_set_model      (struct rwand_dev *dev);
static void    rwand_reset_settings (struct rwand_dev *dev);
static void    rwand_calc_timings   (struct rwand_settings *settings, int period,
				     struct rwand_timings *timings);

static void    rwand_update_state_off         (struct rwand_dev *dev, struct rwand_status *new_status);
static void    rwand_update_state_starting    (struct rwand_dev *dev, struct rwand_status *new_status);
static void    rwand_update_state_stabilizing (struct rwand_dev *dev, struct rwand_status *new_status);
static void    rwand_update_state_running     (struct rwand_dev *dev, struct rwand_status *new_status);

static void    rwand_enter_state_off          (struct rwand_dev *dev);
static void    rwand_enter_state_starting     (struct rwand_dev *dev);
static void    rwand_enter_state_stabilizing  (struct rwand_dev *dev);
static void    rwand_enter_state_running      (struct rwand_dev *dev);

static void    rwand_ensure_mode       (struct rwand_dev *dev, struct rwand_status *new_status, int mode);

static unsigned char reverse_bits      (unsigned char b);

static int     filter_push       (struct filter *filter, int new_value);
static void    filter_reset      (struct filter *filter);

static int     buttons_to_axis(int buttons, int neg_mask, int pos_mask);


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
	.write      =  rwand_write,
};

static struct usb_class_driver rwand_class = {
	.name       =  RWAND_DEV_NAMEFORMAT,
	.fops       =  &rwand_dev_fops,
	.minor_base =  RWAND_MINOR_BASE,
};

static struct usb_driver rwand_driver = {
	.name       =  "rwand",
	.probe      =  rwand_probe,
	.disconnect =  rwand_disconnect,
	.id_table   =  rwand_table,
};

static struct model_intrinsics model_table[] = {
	{
		.model          = 1,
		.name           = "Original Ravinia",
		.coil_center    = 0x4000,
		.coil_width     = 0x7000,
		.fine_adjust    = -185,
		.starting_edges = 20,
		.startup        = {
			.min_period = 45000,
			.max_period = 50000,
			.climb_rate = 700,
		},
	},
	{
		.model          = 2,
		.name           = "Fascinations XP3",
		.coil_center    = 0x4000,
		.coil_width     = 0x7000,
		.fine_adjust    = -80,
		.starting_edges = 100,
		.startup        = {
			.min_period = 42316,
			.max_period = 42818,
			.climb_rate = 4662,
		},
	},
	{ }
};


/******************************************************************************/
/************************************************************** Utilities *****/
/******************************************************************************/

/* Add a new value to the filter, returning the filter's current value */
static int filter_push(struct filter *filter, int new_value)
{
	if (filter->n_values > FILTER_SIZE) {
		/* Remove the old value if we're full */
		filter->total -= filter->buffer[filter->pointer];
	}
	else {
		filter->n_values++;
	}

	/* Add the new value */
	filter->buffer[filter->pointer] = new_value;
	filter->total += new_value;

	filter->pointer = (filter->pointer + 1) & (FILTER_SIZE-1);
	return filter->total / filter->n_values;
}

static void filter_reset(struct filter *filter)
{
	filter->total = 0;
	filter->n_values = 0;
	filter->pointer = 0;
}

static unsigned char reverse_bits(unsigned char b)
{
	/* Developed by Sean Anderson in July 13, 2001.
	 * Taken from http://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
	 */
	return ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
}


/******************************************************************************/
/******************************************************** State Handlers ******/
/******************************************************************************/

static void    rwand_enter_state_off          (struct rwand_dev *dev)
{
	dev->state = STATE_OFF;
}

static void    rwand_enter_state_starting     (struct rwand_dev *dev)
{
	dev->state = STATE_STARTING;
	dev->edge_count = 0;
	dev->state_timer = jiffies;
}

static void    rwand_enter_state_stabilizing  (struct rwand_dev *dev)
{
	dev->state = STATE_STABILIZING;
	dev->edge_count = 0;
	dev->state_timer = jiffies + STABILIZER_TIME;
}

static void    rwand_enter_state_running      (struct rwand_dev *dev)
{
	dev->state = STATE_RUNNING;
}

static void rwand_update_state_off(struct rwand_dev *dev, struct rwand_status *new_status)
{
	/* Everything off */
	rwand_ensure_mode(dev, new_status, 0);
}

static void rwand_update_state_starting(struct rwand_dev *dev, struct rwand_status *new_status)
{
	int new_period;
	struct rwand_timings timings;
	unsigned long now = jiffies;
	long dt = now - dev->state_timer;
	dev->state_timer = now;

	rwand_ensure_mode(dev, new_status,
			  RWAND_MODE_ENABLE_COIL);

	/* Cycle through different frequencies trying to get our wand to start up */
	new_period = new_status->period;

	if (new_period > dev->startup.max_period ||
	    new_period < dev->startup.min_period) {
		if (dev->startup.climb_rate >= 0)
			new_period = dev->startup.min_period;
		else
			new_period = dev->startup.max_period;
	}

	new_period += dt * dev->startup.climb_rate / HZ;

	/* Set the new period and coil phase */
	rwand_calc_timings(&dev->settings, new_period, &timings);
	rwand_nb_request(dev, RWAND_CTRL_SET_PERIOD, new_period, 0);
	rwand_nb_request(dev, RWAND_CTRL_SET_COIL_PHASE, timings.coil_begin, timings.coil_end);

	if (dev->edge_count > dev->intrinsics->starting_edges)
		rwand_enter_state_stabilizing(dev);
}

static void rwand_update_state_stabilizing(struct rwand_dev *dev, struct rwand_status *new_status)
{
	rwand_ensure_mode(dev, new_status,
			  RWAND_MODE_ENABLE_SYNC |
			  RWAND_MODE_ENABLE_COIL);

	if (dev->edge_count > STABILIZER_EDGES) {
		/* Successful exit */
		rwand_enter_state_running(dev);
		return;
	}
	if (time_after(jiffies, dev->state_timer)) {
		/* Unsuccessful exit */
		rwand_enter_state_starting(dev);
		return;
	}
}

static void rwand_update_state_running(struct rwand_dev *dev, struct rwand_status *new_status)
{
	int new_filtered_period;

	/* We want the works */
	rwand_ensure_mode(dev, new_status,
			  RWAND_MODE_STALL_DETECT |
			  RWAND_MODE_ENABLE_SYNC |
			  RWAND_MODE_ENABLE_COIL |
			  RWAND_MODE_ENABLE_DISPLAY);

	/* If our display just turned off, the firmware detected a stall. Go back
	 * to the 'starting' state and give up this.
	 */
	if ((dev->status.mode & RWAND_MODE_ENABLE_DISPLAY) &&
	    !(new_status->mode & RWAND_MODE_ENABLE_DISPLAY)) {
		rwand_enter_state_starting(dev);
		return;
	}

	/* If our mode has changed, both force a timing update and reset the period filter */
	if (new_status->mode != dev->status.mode) {
		dev->settings_dirty = 1;
		filter_reset(&dev->period_filter);
	}

	new_filtered_period = filter_push(&dev->period_filter, new_status->period);

	/* Only actually update the timings if our filtered period is noticeably
	 * different than the last set period. This is mainly here to reduce
	 * the bus bandwidth used by all the rwand_nb_request()s below when possible.
	 */
	if (abs(new_filtered_period - dev->filtered_period) > PERIOD_TOLERANCE) {
		dev->settings_dirty = 1;
		dev->filtered_period = new_filtered_period;
	}

	if (dev->settings_dirty) {
		struct rwand_timings timings;
		rwand_calc_timings(&dev->settings, new_filtered_period, &timings);
		rwand_nb_request(dev, RWAND_CTRL_SET_COIL_PHASE, timings.coil_begin, timings.coil_end);
		rwand_nb_request(dev, RWAND_CTRL_SET_COLUMN_WIDTH, timings.column_width, timings.gap_width);
		rwand_nb_request(dev, RWAND_CTRL_SET_DISPLAY_PHASE, timings.fwd_phase, timings.rev_phase);
		rwand_nb_request(dev, RWAND_CTRL_SET_NUM_COLUMNS, dev->settings.num_columns, 0);
		dev->settings_dirty = 0;
	}

}


/******************************************************************************/
/************************************************** Device Communications *****/
/******************************************************************************/


/* Change modes if necessary */
static void rwand_ensure_mode(struct rwand_dev *dev, struct rwand_status *new_status, int mode)
{
	if (new_status->mode != mode)
		rwand_nb_request(dev, RWAND_CTRL_SET_MODES, mode, 0);
}


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


/* Completion handler for anonymous nonblocking requests. prints out
 * errors, but normally just frees the URB and goes on. Frees the
 * additional data in urb->context if not NULL.
 */
static void rwand_nb_irq(struct urb *urb, struct pt_regs *regs)
{
	if (urb->status < 0)
		err("Bad URB status/size: status=%d, length=%d", urb->status, urb->actual_length);
	if (urb->context) {
		kfree(urb->context);
		urb->context = NULL;
	}
	usb_free_urb(urb);
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

static int buttons_to_axis(int buttons, int neg_mask, int pos_mask)
{
	int x = 0;
	if (buttons & neg_mask)
		x -= 1;
	if (buttons & pos_mask)
		x += 1;
	return x;
}

/* Given an encoded status packet, process it and update the
 * device's state in any way necessary. This should be called
 * from interrupt context.
 */
static void rwand_process_status(struct rwand_dev *dev, const unsigned char *packet)
{
	struct rwand_status new_status;
	int power;
	rwand_decode_status(packet, &new_status);

	/* Report button status to the input subsystem */
	input_report_key(dev->input, BTN_1, new_status.buttons & RWAND_BUTTON_SQUARE);
	input_report_abs(dev->input, ABS_X, buttons_to_axis(new_status.buttons,
							     RWAND_BUTTON_LEFT,
							     RWAND_BUTTON_RIGHT));
	input_report_abs(dev->input, ABS_Y, buttons_to_axis(new_status.buttons,
							     RWAND_BUTTON_UP,
							     RWAND_BUTTON_DOWN));
	input_sync(dev->input);

	/* If the page flip counter has incremented, clear our flip pending flag
	 * and wake up any processes that might be waiting on it.
	 */
	if (new_status.flip_count != dev->status.flip_count) {
		dev->page_flip_pending = 0;
		wake_up_interruptible(&dev->page_flip_waitq);
	}

	/* Update our edge counter */
	dev->edge_count += (new_status.edge_count - dev->status.edge_count) & 0xFF;

	/* Is the display supposed to be on now? */
	switch (dev->settings.power_mode) {
	case RWAND_POWER_ON:
		power = 1;
		break;
	case RWAND_POWER_SWITCH:
		power = (new_status.buttons & RWAND_BUTTON_POWER) != 0;
		break;
	case RWAND_POWER_AUTO:
		power = dev->open_count != 0;
		break;
	default:
		power = 0;
	}
	if (power) {
		if (dev->state == STATE_OFF)
			rwand_enter_state_starting(dev);
	}
	else {
		if (dev->state != STATE_OFF)
			rwand_enter_state_off(dev);
	}

	/* Now just go to a state-specific handler */
	switch (dev->state) {
	case STATE_OFF:
		rwand_update_state_off(dev, &new_status);
		break;
	case STATE_STARTING:
		rwand_update_state_starting(dev, &new_status);
		break;
	case STATE_STABILIZING:
		rwand_update_state_stabilizing(dev, &new_status);
		break;
	case STATE_RUNNING:
		rwand_update_state_running(dev, &new_status);
		break;
	}

	dev->status = new_status;
}


/* Send a blocking control request, read one byte of data back */
static int rwand_read_byte(struct rwand_dev *dev, unsigned short request,
			   unsigned short wValue, unsigned short wIndex)
{
	int retval;
	unsigned char byte;
	retval = usb_control_msg(dev->udev, usb_rcvctrlpipe(dev->udev, 0),
				 request, USB_TYPE_VENDOR | USB_DIR_IN,
				 wValue, wIndex, &byte, 1, REQUEST_TIMEOUT);
	if (retval != 1) {
		err("Error in rwand_request, retval=%d", retval);
	}
	return byte;
}


/* Send a generic nonblocking control request, without completion notification.
 * This is safe for use in interrupt context.
 */
static void rwand_nb_request(struct rwand_dev *dev, unsigned short request,
			  unsigned short wValue, unsigned short wIndex)
{
	int retval;
	struct urb *urb;
	struct usb_ctrlrequest *dr = kmalloc(sizeof(struct usb_ctrlrequest), GFP_ATOMIC);
	if (!dr) {
		err("Out of memory in rwand_nb_request");
		return;
	}

	dr->bRequestType = USB_TYPE_VENDOR;
	dr->bRequest = request;
	dr->wValue = cpu_to_le16p(&wValue);
	dr->wIndex = cpu_to_le16p(&wIndex);
	dr->wLength = 0;

	urb = usb_alloc_urb(0, SLAB_ATOMIC);
	if (!urb) {
		kfree(dr);
		err("Out of memory in rwand_nb_request");
		return;
	}

	usb_fill_control_urb(urb, dev->udev, usb_sndctrlpipe(dev->udev, 0),
			     (unsigned char*)dr, NULL, 0,
			     rwand_nb_irq, dr);

	retval = usb_submit_urb(urb, SLAB_ATOMIC);
	if (retval) {
		dbg("Error in rwand_nb_request, retval=%d", retval);
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
		dev->settings.num_columns = bytes;
		dev->settings_dirty = 1;
	}

	/* Currently this does the whole update using SEQ_WRITE4.
	 * This might be fast enough, but there's plenty of room for improvement.
	 */
	memcpy(dev->fb, fb, bytes);
	for (i=0; i<dev->settings.num_columns; i+=4) {
		retval = usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
					 RWAND_CTRL_SEQ_WRITE4, USB_TYPE_VENDOR,
					 CONVERT_FB_BYTE(fb[i]) | (CONVERT_FB_BYTE(fb[i+1]) << 8),
					 CONVERT_FB_BYTE(fb[i+2]) | (CONVERT_FB_BYTE(fb[i+3]) << 8),
					 NULL, 0, REQUEST_TIMEOUT);
		if (retval != 0) {
			err("Unexpected usb_control_msg retval in rwand_update_fb: %d", retval);
		}
	}
}


/* Request a page flip. No display writes should be made until the
 * page flip completes. This can be checked for using rwand_wait_for_fb().
 * This issues a nonblocking control request, since it's assumed the caller
 * will use rwand_wait_for_fb().
 */
static void rwand_request_flip(struct rwand_dev *dev)
{
	dev->page_flip_pending = 1;
	rwand_nb_request(dev, RWAND_CTRL_FLIP, 0, 0);
}


/* Look up the device's model and find it in our table */
static void rwand_set_model(struct rwand_dev *dev)
{
	int model;
	struct model_intrinsics *intrinsics;

	model = rwand_read_byte(dev, RWAND_CTRL_GET_HW_MODEL, 0, 0);

	for (intrinsics=model_table; intrinsics->model; intrinsics++) {
		if (intrinsics->model == model) {
			/* Found it */
			dbg("Found model %d hardware", model);
			dev->intrinsics = intrinsics;
			return;
		}
	}

	/* Oops, can't find it. Generate an error, and use the first
	 * model in the table whether it's right or not.
	 */
	dev->intrinsics = model_table;
	err("Found unknown hardware model %d", model);
}


/* Change our timing and mode settings back to the defaults */
static void rwand_reset_settings(struct rwand_dev *dev)
{
	dev->settings.display_center = 0x8000;
	dev->settings.display_width  = 0x6000;
	dev->settings.coil_center    = dev->intrinsics->coil_center;
	dev->settings.coil_width     = dev->intrinsics->coil_width;
	dev->settings.duty_cycle     = 0xA000;
	dev->settings.fine_adjust    = dev->intrinsics->fine_adjust;
	dev->settings.power_mode     = RWAND_POWER_SWITCH;
	dev->startup                 = dev->intrinsics->startup;
}

/* Calculate all the fun little timing parameters needed by the hardware */
static void rwand_calc_timings(struct rwand_settings *settings, int period,
			       struct rwand_timings *timings)
{
	int col_and_gap_width, total_width;

	/* The coil driver just needs to have its relative timings
	 * multiplied by our predictor's current period. This is fixed
	 * point math with 16 digits to the right of the binary point.
	 */
	timings->coil_begin = (period * (settings->coil_center - settings->coil_width/2)) >> 16;
	timings->coil_end   = (period * (settings->coil_center + settings->coil_width/2)) >> 16;

	if (settings->num_columns > 0) {
		/* Now calculate the display timings. We start out with the precise
		 * width of our columns, so that the width of the whole display
		 * can be calculated accurately.
		 */
		col_and_gap_width = (period / settings->num_columns * settings->display_width) >> 17;
		timings->column_width = (col_and_gap_width * settings->duty_cycle) >> 16;
		timings->gap_width = col_and_gap_width - timings->column_width;
		total_width =
		  (settings->num_columns) * timings->column_width +
		  (settings->num_columns-1) * timings->gap_width;


		/* Now that we know the true width of the display, we can calculate the
		 * two phase timings. These indicate when it starts the forward scan and the
		 * backward scan, relative to the left position. The alignment between
		 * the forward and backward scans should be calculated correctly, but it
		 * can be tweaked using settings->fine_adjust. This value is set per-model
		 * to account for latency in the interruption sensor and LED drive hardware.
		 */
		timings->fwd_phase = ((period * settings->display_center) >> 17) - total_width/2;
		timings->rev_phase = period - timings->fwd_phase - total_width + settings->fine_adjust;
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

	case RWANDIO_GET_SETTINGS:
		if (copy_to_user((struct rwand_settings*) arg, &dev->settings, sizeof(dev->settings))) {
			retval = -EFAULT;
			break;
		}
		break;

	case RWANDIO_GET_STATUS:
		if (copy_to_user((struct rwand_status*) arg, &dev->status, sizeof(dev->status))) {
			retval = -EFAULT;
			break;
		}
		break;

	case RWANDIO_PUT_SETTINGS:
		if (copy_from_user(&dev->settings, (struct rwand_settings*) arg, sizeof(dev->settings))) {
			retval = -EFAULT;
			break;
		}
		dev->settings_dirty = 1;
		break;

	case RWANDIO_GET_STARTUP:
		if (copy_to_user((struct rwand_startup*) arg, &dev->startup, sizeof(dev->startup))) {
			retval = -EFAULT;
			break;
		}
		break;

	case RWANDIO_PUT_STARTUP:
		if (copy_from_user(&dev->startup, (struct rwand_startup*) arg, sizeof(dev->startup))) {
			retval = -EFAULT;
			break;
		}
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


static ssize_t rwand_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	struct rwand_fd_private *prv;
	struct rwand_dev *dev;
	int retval = 0, wait_retval;
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

	/* Change the framebuffer size if necessary and write its new content */
	rwand_update_fb(dev, bytes_written, new_fb);
	rwand_request_flip(dev);
	retval = bytes_written;

	/* Wait for the sync to actually complete. We could return to the process
	 * faster by moving this check above, before writing the new content.
	 * However, for an app that's constantly streaming new frames to the display,
	 * as most are, this results in less latency between the app and the display
	 * refreshes.
	 */
	wait_retval = rwand_wait_for_fb(dev);
	if (wait_retval)
		retval = wait_retval;

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
		usb_kill_urb(dev->irq);
		usb_free_urb(dev->irq);
	}
	if (dev->irq_data) {
		usb_buffer_free(dev->udev, STATUS_PACKET_SIZE, dev->irq_data, dev->irq_dma);
	}
   if (dev->input) {
      input_free_device(dev->input);
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

	rwand_set_model(dev);
	rwand_reset_settings(dev);
	rwand_enter_state_off(dev);

	/* Register an input device for our buttons */
   dev->input = input_allocate_device();
	dev->input->private = dev;
	dev->input->name = "Raster Wand";
	dev->input->id.bustype = BUS_USB;
	dev->input->id.vendor = udev->descriptor.idVendor;
	dev->input->id.product = udev->descriptor.idProduct;
	dev->input->id.version = udev->descriptor.bcdDevice;

        set_bit(EV_KEY, dev->input->evbit);
        set_bit(EV_ABS, dev->input->evbit);
        set_bit(BTN_1, dev->input->keybit);
	set_bit(ABS_X, dev->input->absbit);
	dev->input->absmin[ABS_X] = -1;
	dev->input->absmax[ABS_X] = 1;
	set_bit(ABS_Y, dev->input->absbit);
	dev->input->absmin[ABS_Y] = -1;
	dev->input->absmax[ABS_Y] = 1;

	input_register_device(dev->input);

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
	input_unregister_device(dev->input);
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
