/*
 * Media Infrawidget 6000 driver
 *
 * This is a module for the Linux kernel, providing a userspace interface
 * to the MI6K hardware via the USB protocol described in mi6k_protocol.h.
 * In addition to the new interfaces described in mi6k_dev.h, this driver
 * creates an LIRC-compatible device for accessing the MI6K's infrared
 * transmitter and receiver.
 *
 * Copyright (C) 2003-2004 Micah Dowty <micah@navi.cx>
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
#include <mi6k_protocol.h>
#include <mi6k_dev.h>
#include <lirc.h>

#define DRIVER_VERSION "v0.2"
#define DRIVER_AUTHOR "Micah Dowty <micah@navi.cx>"
#define DRIVER_DESC "Media Infrawidget 6000 driver"

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

/* Table of devices that work with this driver */
static struct usb_device_id mi6k_table [] = {
	{ USB_DEVICE(MI6K_VENDOR_ID, MI6K_PRODUCT_ID) },
	{ } /* End table */
};
MODULE_DEVICE_TABLE(usb, mi6k_table);

/* Size of the ring buffer for received IR data. Must be a power of two */
#define IR_RECV_BUFFER_SIZE 512

/* Size of the URB buffer for incoming interrupt transfers. This should match
 * the maximum transfer size used by the hardware, making it any larger just
 * wastes memory.
 */
#define IR_URB_BUFFER_SIZE  8

/* Structure to hold all of our device specific stuff */
struct usb_mi6k {
	/* Device administration */
	struct usb_device *	udev;			/* save off the usb device pointer */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char		minor;			/* the starting minor number for this device */
	int			open_count;		/* number of times this port has been opened */
	struct semaphore	sem;			/* locks this structure */

	/* IR receive */
	lirc_t			ir_rx_buffer[IR_RECV_BUFFER_SIZE]; /* ring buffer for incoming IR data */
	int			ir_rx_head;		/* empty slot for the next received value */
	int			ir_rx_tail;		/* slot for the oldest value in the buffer */
	wait_queue_head_t	ir_rx_wait;		/* wait queue for processes reading IR data */
	struct urb*		ir_rx_urb;		/* URB for receiver interrupt transfers */
	unsigned char*		ir_rx_tbuffer;          /* Buffer holding one interrupt transfer */
	dma_addr_t		ir_rx_dma;
	lirc_t			pulse_flag;		/* flag that alternates, indicating whether we're currently
							 * receiving a pulse or a space. */

	/* IR transmit */
	unsigned char		ir_tx_pulse;		/* Buffer for a pulse awaiting transmission.
							 * Once two pulses await transmission, they
							 * are sent and this is cleared.
							 */
	unsigned char		ir_tx_space;		/* Corresponds with ir_tx_pulse */
};

/* Private data for each file descriptor */
struct mi6k_fd_private {
	struct usb_mi6k	*dev;
};

/* 1/2 second timeout for control requests */
#define REQUEST_TIMEOUT (HZ / 2)

/* Wait 1/8 second after powering the VFD on or off */
#define VFD_POWER_DELAY (HZ / 8)

/* Number of minor numbers we use per physical device */
#define MINORS_PER_DEVICE 2

/* prevent races between open() and disconnect() */
static DECLARE_MUTEX (disconnect_sem);


/******************************************************************************/
/************************************************** Function Prototypes *******/
/******************************************************************************/

static int          mi6k_status               (struct usb_mi6k* dev);
static void         mi6k_request              (struct usb_mi6k* dev,
					       unsigned short   request,
					       unsigned short   wValue,
					       unsigned short   wIndex);

static int          mi6k_ir_rx_available      (struct usb_mi6k* dev);
static void         mi6k_ir_rx_push           (struct usb_mi6k* dev,
					       lirc_t           x);
static int          mi6k_ir_rx_pop            (struct usb_mi6k* dev);
static void         mi6k_ir_rx_store          (struct usb_mi6k* dev,
					       unsigned char*   buffer,
					       size_t           count);
static void         mi6k_ir_tx_send           (struct usb_mi6k* dev,
					       lirc_t           pulse,
					       lirc_t           space);
static void         mi6k_ir_tx_send_converted (struct usb_mi6k* dev,
					       lirc_t           pulse,
					       lirc_t           space);
static void         mi6k_ir_tx_flush          (struct usb_mi6k* dev);
static void         mi6k_ir_rx_irq            (struct urb*      urb,
					       struct pt_regs*  regs);

static int          mi6k_open                 (struct inode*   inode,
					       struct file*    file);
static int          mi6k_release              (struct inode*   inode,
					       struct file*    file);
static ssize_t      mi6k_dev_write            (struct file*    file,
					       const char*     buffer,
					       size_t          count,
					       loff_t*         ppos);
static int          mi6k_dev_ioctl            (struct inode*   inode,
					       struct file*    file,
					       unsigned int    cmd,
					       unsigned long   arg);

static ssize_t      mi6k_lirc_read            (struct file*    file,
					       char*           buffer,
					       size_t          count,
					       loff_t*         ppos);
static ssize_t      mi6k_lirc_write           (struct file*    file,
					       const char*     buffer,
					       size_t          count,
					       loff_t*         ppos);
static int          mi6k_lirc_ioctl           (struct inode*   inode,
					       struct file*    file,
					       unsigned int    cmd,
					       unsigned long   arg);
static unsigned int mi6k_lirc_poll            (struct file*    file,
					       poll_table*     wait);

static inline void  mi6k_delete               (struct usb_mi6k*            dev);
static int          mi6k_probe                (struct usb_interface*       interface,
					       const struct usb_device_id* id);
static void         mi6k_disconnect           (struct usb_interface*       interface);
static int __init   usb_mi6k_init             (void);
static void __exit  usb_mi6k_exit             (void);


static struct file_operations mi6k_dev_fops = {
	.owner	  =  THIS_MODULE,
	.write	  =  mi6k_dev_write,
	.ioctl	  =  mi6k_dev_ioctl,
	.open	  =  mi6k_open,
	.release  =  mi6k_release,
};

static struct file_operations mi6k_lirc_fops = {
	.owner      =  THIS_MODULE,
	.write      =  mi6k_lirc_write,
	.read       =  mi6k_lirc_read,
	.ioctl      =  mi6k_lirc_ioctl,
	.poll       =  mi6k_lirc_poll,
	.open       =  mi6k_open,
	.release    =  mi6k_release,
};

static struct usb_class_driver mi6k_class = {
	.name       =  MI6K_DEV_NAMEFORMAT,
	.fops       =  &mi6k_dev_fops,
	.mode       =  S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
	.minor_base =  MI6K_MINOR_BASE,
};

static struct usb_driver mi6k_driver = {
	.name       =  "mi6k",
	.probe      =  mi6k_probe,
	.disconnect =  mi6k_disconnect,
	.id_table   =  mi6k_table,
};


/******************************************************************************/
/************************************************** Device Communications *****/
/******************************************************************************/

static void mi6k_request(struct usb_mi6k *dev, unsigned short request,
			  unsigned short wValue, unsigned short wIndex)
{
	/* Send a control request to the device synchronously */
	usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
			request, 0x40, wValue, wIndex, NULL, 0, REQUEST_TIMEOUT);
}

static int mi6k_ir_rx_available(struct usb_mi6k *dev)
{
	/* Return the number of bytes available in the receive ring buffer. */
	return (dev->ir_rx_head - dev->ir_rx_tail) & (IR_RECV_BUFFER_SIZE - 1);
}

static void mi6k_ir_rx_push(struct usb_mi6k *dev, lirc_t x)
{
	/* Push a value into the receive ring buffer
	 * This will overwrite data if the ring buffer is full.
	 */
	dev->ir_rx_buffer[dev->ir_rx_head] = x;
	dev->ir_rx_head = (dev->ir_rx_head + 1) & (IR_RECV_BUFFER_SIZE - 1);
}

static int mi6k_ir_rx_pop(struct usb_mi6k *dev)
{
	/* Pop a value out of the receive ring buffer.
	 * This function will return an undefined value if the ring buffer is empty.
	 */
	lirc_t result =	dev->ir_rx_buffer[dev->ir_rx_tail];
	dev->ir_rx_tail = (dev->ir_rx_tail + 1) & (IR_RECV_BUFFER_SIZE - 1);
	return result;
}

static void mi6k_ir_rx_store(struct usb_mi6k *dev, unsigned char *buffer, size_t count)
{
	/* Store a buffer full of raw IR receiver responses in the ring buffer
	 * and notify any waiting processes that new data is available.
	 * This converts from raw received values to lirc_t values in microseconds.
	 * This involves converting from the receiver's 4/3us unit to microseconds,
	 * and setting the pulse/space flag appropriately. It also extends overflowed
	 * timers to the highest value lirc_t supports.
	 */
	lirc_t value;

	while (count >= 2) {
		value = ((lirc_t)buffer[0]) + (((lirc_t)buffer[1]) << 8);
		if (value == 0xFFFF) {
			/* The mi6k's timer overflowed. Replace this with a
			 * suitably large number to make irrecord and xmode2 happy.
			 */
			value = 2000000;

			/* We also use these overflows to synchronize pulses and spaces.
			 * We assume that only a space should be long enough to cause an overflow.
			 */
			dev->pulse_flag = 0;
		}
		else {
			/* Convert the timer value to microseconds */
			value = value * 4 / 3;

			/* Alternate pulses and spaces */
			dev->pulse_flag ^= PULSE_BIT;
			value |= dev->pulse_flag;
		}

		mi6k_ir_rx_push(dev, value);
		buffer += 2;
		count -= 2;
	}
	wake_up_interruptible(&dev->ir_rx_wait);
}

/* Convert a pulse or space value from microseconds to the transmitter's ~12.8us units */
static lirc_t mi6k_ir_tx_convert(lirc_t v)
{
	return ((v & PULSE_MASK) * 100 + 50) / 1285;
}

/* Queue a pulse/space pair for transmission, converting long pulses/spaces as necessary */
static void mi6k_ir_tx_send(struct usb_mi6k *dev, lirc_t pulse, lirc_t space)
{
	dbg("ir_tx_send: %d %d", pulse, space);
	pulse = mi6k_ir_tx_convert(pulse);
	space = mi6k_ir_tx_convert(space);
	mi6k_ir_tx_send_converted(dev, pulse, space);
}

/* Recursive function used by mi6k_ir_tx_send, sends a pulse and space using already-converted values */
static void mi6k_ir_tx_send_converted(struct usb_mi6k *dev, lirc_t pulse, lirc_t space)
{
	/* Split pulses that are too long */
	while (pulse > 255) {
		mi6k_ir_tx_send_converted(dev, 255, 0);
		pulse -= 255;
	}

	dbg("ir_tx_send_converted: %d %d", pulse, space & 0xFF);

	/* If we already have one value buffered, send two pulse/space pairs */
	if (dev->ir_tx_pulse || dev->ir_tx_space) {
		mi6k_request(dev, MI6K_CTRL_IR_SEND,
			     pulse | ((space & 0xFF) << 8),
			     dev->ir_tx_pulse | (dev->ir_tx_space << 8));
		dev->ir_tx_pulse = dev->ir_tx_space = 0;
	}
	else {
		/* Nope, buffer this one so we can send two pulse/space pairs at a time later */
		dev->ir_tx_pulse = pulse;
		dev->ir_tx_space = space & 0xFF;
	}

	/* Split spaces that are too long */
	while (space > 255) {
		mi6k_ir_tx_send_converted(dev, 0, 255);
		space -= 255;
	}
}

/* Send any pending IR data. This includes the one locally buffered pulse/space pair,
 * and the data being buffered in the mi6k firmware.
 */
static void mi6k_ir_tx_flush(struct usb_mi6k *dev)
{
	if (dev->ir_tx_pulse || dev->ir_tx_space) {
		mi6k_request(dev, MI6K_CTRL_IR_SEND, 0,
			     dev->ir_tx_pulse | (dev->ir_tx_space << 8));
		dev->ir_tx_pulse = dev->ir_tx_space = 0;
	}
	mi6k_request(dev, MI6K_CTRL_IR_FLUSH, 0, 0);
}

static void mi6k_ir_rx_irq(struct urb *urb, struct pt_regs *regs)
{
	/* Callback for processing incoming interrupt transfers from the IR receiver */
	struct usb_mi6k *dev = (struct usb_mi6k*)urb->context;

	if (dev && urb->status == 0 && urb->actual_length > 0) {
		dbg("ir_rx_irq, length %d, buffer: %02X%02X %02X%02X %02X%02X %02X%02X",
		    urb->actual_length, dev->ir_rx_tbuffer[1], dev->ir_rx_tbuffer[0], dev->ir_rx_tbuffer[3],
		    dev->ir_rx_tbuffer[2], dev->ir_rx_tbuffer[5], dev->ir_rx_tbuffer[4], dev->ir_rx_tbuffer[7],
		    dev->ir_rx_tbuffer[6]);

		mi6k_ir_rx_store(dev, dev->ir_rx_tbuffer, urb->actual_length);
	}
}

static int mi6k_status(struct usb_mi6k *dev)
{
	/* Return MI6K_STATUS_* constants describing the device's state */
	unsigned char status;
	usb_control_msg(dev->udev, usb_rcvctrlpipe(dev->udev, 0),
			MI6K_CTRL_STATUS, 0x40 | USB_DIR_IN, 0, 0, &status, sizeof(status),
			REQUEST_TIMEOUT);
	return status;
}


/******************************************************************************/
/************************************************** Shared device functions ***/
/******************************************************************************/

static int mi6k_open(struct inode *inode, struct file *file)
{
	struct usb_mi6k *dev = NULL;
	struct mi6k_fd_private *prv = NULL;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);

	/* Prevent disconnects */
	down(&disconnect_sem);

	interface = usb_find_interface(&mi6k_driver, subminor);
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
	prv = kmalloc(sizeof(struct mi6k_fd_private), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		retval = -ENOMEM;
		goto exit;
	}

	/* save our object in the file's private structure */
	memset(prv, 0, sizeof(struct mi6k_fd_private));
	prv->dev = dev;
	file->private_data = prv;

exit:
	/* unlock this device */
	up (&dev->sem);

exit_no_device:
	up (&disconnect_sem);
	return retval;
}

static int mi6k_release(struct inode *inode, struct file *file)
{
	struct mi6k_fd_private *prv;
	struct usb_mi6k *dev;
	int retval = 0;

	prv = (struct mi6k_fd_private *) file->private_data;
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
		mi6k_delete(dev);
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


/******************************************************************************/
/************************************************** Main character device *****/
/******************************************************************************/

static ssize_t mi6k_dev_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	/* Pad unused bytes with zero, which the display will ignore */
	unsigned char tbuffer[] = {0, 0, 0, 0};
	struct mi6k_fd_private *prv;
	struct usb_mi6k *dev;
	ssize_t bytes_written = 0;
	int retval = 0;

	prv = (struct mi6k_fd_private *) file->private_data;
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

	bytes_written = min(sizeof(tbuffer), count);
	if (copy_from_user(tbuffer, buffer, bytes_written)) {
		retval = -EFAULT;
		goto exit;
	}

	/* Pack 4 bytes of the character stream into the packet's value and index parameters */
	mi6k_request(dev, MI6K_CTRL_VFD_WRITE,
		     tbuffer[0] |(((int)tbuffer[1]) << 8),
		     tbuffer[2] |(((int)tbuffer[3]) << 8));

	retval = bytes_written;

exit:
	/* unlock the device */
	up(&dev->sem);

	return retval;
}

static int mi6k_dev_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct mi6k_fd_private *prv;
	struct usb_mi6k *dev;
	int retval=0;
	struct mi6k_leds leds;

	prv = (struct mi6k_fd_private *) file->private_data;
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
		if (copy_from_user(&leds,(struct mi6k_leds*) arg, sizeof(leds))) {
			retval = -EFAULT;
			break;
		}
		mi6k_request(dev, MI6K_CTRL_LED_SET, leds.blue >> 6, leds.white >> 6);
		break;

	case MI6KIO_STATUS:
		/* Return MI6K_STATUS_* constants or'ed together */
		retval = mi6k_status(dev);
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


/******************************************************************************/
/************************************************** LIRC character device *****/
/******************************************************************************/

static ssize_t mi6k_lirc_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	DECLARE_WAITQUEUE(wait, current);
	struct mi6k_fd_private *prv;
	struct usb_mi6k *dev;
	int retval = 0;
	lirc_t value;
	int available;

	prv = (struct mi6k_fd_private *) file->private_data;
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

	/* If there's no data available yet, release our lock temporarily and
	 * block the process until there is data available.
	 */
	if (!mi6k_ir_rx_available(dev)) {
		add_wait_queue(&dev->ir_rx_wait, &wait);
		current->state = TASK_INTERRUPTIBLE;

		while (!mi6k_ir_rx_available(dev)) {

			if (file->f_flags & O_NONBLOCK) {
				retval = -EAGAIN;
				break;
			}
			if (signal_pending(current)) {
				retval = -ERESTARTSYS;
				break;
			}

			up(&dev->sem);
			schedule();
			down(&dev->sem);
		}

		current->state = TASK_RUNNING;
		remove_wait_queue(&dev->ir_rx_wait, &wait);
	}
	if (retval != 0)
		goto exit;

	/* Shuffle values from the ring buffer into the process' provided userspace buffer */
	available = mi6k_ir_rx_available(dev);
	while (count >= sizeof(value) && available > 0) {
		value = mi6k_ir_rx_pop(dev);
		if (copy_to_user(buffer, &value, sizeof(value))) {
			retval = -EFAULT;
			goto exit;
		}
		buffer += sizeof(value);
		retval += sizeof(value);
		count -= sizeof(value);
		available--;
	}

exit:
	/* unlock the device */
	up(&dev->sem);
	return retval;
}

static ssize_t mi6k_lirc_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	struct mi6k_fd_private *prv;
	struct usb_mi6k *dev;
	int retval = 0;
	lirc_t value = 0, pulse = 0;
	int pulse_flag = 1;

	prv = (struct mi6k_fd_private *) file->private_data;
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

	dbg("write count %d", count);

	while (count >= sizeof(lirc_t)) {
		/* Grab a value from userspace */
		if (copy_from_user(&value, buffer, sizeof(value))) {
			retval = -EFAULT;
			goto exit;
		}
		retval += sizeof(lirc_t);
		count -= sizeof(lirc_t);
		buffer += sizeof(lirc_t);

		if (pulse_flag) {
			/* We just got a pulse. Store it, but don't send
			 * anything until we get a corresponding space or we have no more data.
			 */
			pulse = value;
			pulse_flag = 0;
		}
		else {
			/* Send a pulse/space pair */
			mi6k_ir_tx_send(dev, pulse, value);
			pulse_flag = 1;
		}
	}

	if (!pulse_flag) {
		/* We ended our write on a pulse, send it with a space of zero */
		mi6k_ir_tx_send(dev, pulse, 0);
	}

	/* Flush our local buffer and the device's buffer */
	mi6k_ir_tx_flush(dev);

	/* Ignore any leftover bytes */
	retval += count;

exit:
	/* unlock the device */
	up(&dev->sem);

	return retval;
}

static int mi6k_lirc_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct mi6k_fd_private *prv;
	struct usb_mi6k *dev;
	int retval=0;
	unsigned long value;

	prv = (struct mi6k_fd_private *) file->private_data;
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

	case LIRC_GET_FEATURES:
		retval = put_user(LIRC_CAN_SEND_PULSE |
				  LIRC_CAN_REC_MODE2,
				  (unsigned long *) arg);
		break;

	case LIRC_GET_SEND_MODE:
		retval = put_user(LIRC_SEND2MODE(LIRC_CAN_SEND_PULSE),
				  (unsigned long *) arg);
		break;

	case LIRC_GET_REC_MODE:
		retval = put_user(LIRC_REC2MODE(LIRC_CAN_REC_MODE2),
				  (unsigned long *) arg);
		break;

	case LIRC_SET_SEND_MODE:
		retval = get_user(value,(unsigned long *) arg);
		if (retval != 0)
			break;

		/* only LIRC_MODE_PULSE supported */
		if (value != LIRC_MODE_PULSE)
			retval = -ENOSYS;
		break;

	case LIRC_SET_REC_MODE:
		retval = get_user(value,(unsigned long *) arg);
		if (retval != 0)
			break;

		/* only LIRC_MODE_MODE2 supported */
		if (value != LIRC_MODE_MODE2)
			retval = -ENOSYS;
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

static unsigned int mi6k_lirc_poll(struct file *file, poll_table *wait)
{
	struct mi6k_fd_private *prv;
	struct usb_mi6k *dev;

	prv = (struct mi6k_fd_private *) file->private_data;
	if (prv == NULL) {
		dbg("object is NULL");
		return -ENODEV;
	}
	dev = prv->dev;

	poll_wait(file, &dev->ir_rx_wait, wait);

	/* Was the device unplugged? */
	if (dev->udev == NULL)
		return POLLERR | POLLHUP;

	/* Data available in the rx ringbuffer? */
	if (mi6k_ir_rx_available(dev))
		return POLLIN | POLLRDNORM;

	return 0;
}


/******************************************************************************/
/************************************************** USB Housekeeping **********/
/******************************************************************************/

static inline void mi6k_delete(struct usb_mi6k *dev)
{
	if (dev->ir_rx_urb) {
		usb_unlink_urb(dev->ir_rx_urb);
		usb_free_urb(dev->ir_rx_urb);
	}
	if (dev->ir_rx_tbuffer) {
		usb_buffer_free(dev->udev, IR_URB_BUFFER_SIZE, dev->ir_rx_tbuffer, dev->ir_rx_dma);
	}
	kfree(dev);
}

static int mi6k_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct usb_mi6k *dev = NULL;
	struct usb_host_interface *host_interface = interface->cur_altsetting;
	int retval, i;

	/* See if the device offered us matches what we can accept */
	if ((udev->descriptor.idVendor != MI6K_VENDOR_ID) ||
	   (udev->descriptor.idProduct != MI6K_PRODUCT_ID)) {
		return -ENODEV;
	}

	/* allocate memory for our device state and intialize it */
	dev = kmalloc(sizeof(struct usb_mi6k), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		retval = -ENOMEM;
		goto error;
	}
	memset(dev, 0x00, sizeof(*dev));

	init_MUTEX(&dev->sem);
	dev->udev = udev;
	dev->interface = interface;
	init_waitqueue_head(&dev->ir_rx_wait);

	usb_set_intfdata(interface, dev);
	retval = usb_register_dev(interface, &mi6k_class);
	if (retval) {
		/* something prevented us from registering this driver */
		err ("Not able to get a minor for this device.");
		goto error;
	}

#if 0 /* FIXME */
	/* Initialize our LIRC-compatible character device */
	sprintf(name, "lirc%d", dev->minor);
	dev->lirc_devfs = devfs_register(usb_devfs_handle, name,
					 DEVFS_FL_DEFAULT, USB_MAJOR,
					 MI6K_MINOR_BASE + dev->minor*MINORS_PER_DEVICE + 1,
					 S_IFCHR | S_IRUSR | S_IWUSR |
					 S_IRGRP | S_IWGRP | S_IROTH,
					 &mi6k_lirc_fops, NULL);

	/* Automatically add a /dev/lirc symlink if this is the first device */
	if (dev->minor == 0) {
	  devfs_handle_t slave;
	  devfs_mk_symlink(NULL, "lirc", DEVFS_FL_DEFAULT, "usb/lirc0", &slave, NULL);
	  devfs_auto_unregister(dev->lirc_devfs, slave);
	}
#endif

	/* Allocate some DMA-friendly memory for URB transfers */
	dev->ir_rx_tbuffer = usb_buffer_alloc(udev, IR_URB_BUFFER_SIZE,
					      SLAB_ATOMIC, &dev->ir_rx_dma);
	if (!dev->ir_rx_tbuffer) {
		retval = -ENOMEM;
		goto error;
	}
	dev->ir_rx_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->ir_rx_urb) {
		retval = -ENODEV;
		goto error;
	}

	/* Begin our interrupt transfer polling for received IR data */
	usb_fill_int_urb(dev->ir_rx_urb, dev->udev,
			 usb_rcvintpipe(dev->udev, host_interface->endpoint[0].desc.bEndpointAddress),
			 dev->ir_rx_tbuffer, IR_URB_BUFFER_SIZE,
			 mi6k_ir_rx_irq, dev, 1);
	i = usb_submit_urb(dev->ir_rx_urb, GFP_KERNEL);
	if (i) {
		dbg("Error submitting URB: %d", i);
	}

	info("mi6k device now attached");
	return 0;

error:
	usb_set_intfdata(interface, NULL);
	mi6k_delete(dev);
	return retval;
}

static void mi6k_disconnect(struct usb_interface *interface)
{
	struct usb_mi6k *dev;
	int minor;

	/* prevent races with open() */
	down (&disconnect_sem);

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	down(&dev->sem);

	minor = dev->minor;
	usb_deregister_dev(interface, &mi6k_class);

#if 0 /* FIXME */
	/* remove our devfs node */
	devfs_unregister(dev->devfs);
	devfs_unregister(dev->lirc_devfs);
#endif

	/* if the device is not opened, then we clean up right now */
	if (!dev->open_count) {
		up(&dev->sem);
		mi6k_delete(dev);
	} else {
		dev->udev = NULL;
		up(&dev->sem);
	}

	up(&disconnect_sem);
	info("mi6k #%d now disconnected", minor);
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

	info(DRIVER_DESC " " DRIVER_VERSION " - " DRIVER_AUTHOR);
	return 0;
}

static void __exit usb_mi6k_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&mi6k_driver);
}

module_init(usb_mi6k_init);
module_exit(usb_mi6k_exit);

/* The End */
