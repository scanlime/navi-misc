#include <linux/config.h>
#include <linux/module.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/semaphore.h>
#include <linux/prefetch.h>
#include <asm/io.h>
#include <linux/smp_lock.h>
#include <linux/devfs_fs_kernel.h>
#include <rwand_dev.h>

#define DRIVER_VERSION "v0.1"

static struct usb_device_id rwand_table [] = {
    { USB_DEVICE( RWAND_VENDOR_ID, RWAND_PRODUCT_ID ) },
    { }
};

MODULE_DEVICE_TABLE( usb, rwand_table );
MODULE_DESCRIPTION( "Rasterwand USB kernel module" );
MODULE_LICENSE( "GPL" );

struct rwand_timings {
    int column_width;
    int gap_width;
    int fwd_phase;
    int rev_phase;
    int coil_begin;
    int coil_end;
};

struct model_intrinsics {
    int	model;
    const char * name;
    int coil_center;
    int coil_width;
    int fine_adjust;
    struct rwand_startup startup;
};

#define FILTER_SIZE 	256
#define PERIOD_TOLERANCE 20
#define STARTING_EDGES 20
#define STABILIZER_EDGES 8
#define STABILIZER_TIME HZ

struct filter {
    int buffer[FILTER_SIZE];
    int total;
    int n_values;
    int pointer;
};
    
#define STATE_OFF	0
#define STATE_STARTING	1
#define STATE_STABILIZING 2
#define STATE_RUNNING	3

struct usb_rwand {
    struct usb_device *dev;
    unsigned char iface;
    devfs_handle_t devfs;
    wait_queue_head_t wq;
    struct semaphore lock;
    int user;
    int removed;
    unsigned char minor;
    struct urb *irq;
    void *irq_data;
    dma_addr_t irq_dma;
    struct usb_ctrlrequest status_request;
    struct rwand_settings settings;
    int settings_dirty;
    struct model_intrinsics *intrinsics;
    struct rwand_startup startup;
    unsigned char fb[RWAND_FB_BYTES+4];
    struct rwand_status status;
    int state;
    struct filter period_filter;
    int filtered_period;
    int edge_count;
    unsigned long state_timer;
    wait_queue_head_t page_flip_waitq;
    int page_flip_pending;
    int open_count;
    struct usb_endpoint_descriptor *endpoint;
};

static struct usb_rwand *minor_table[1];

struct rwand_fd_private {
    struct usb_rwand *dev;
};

static DECLARE_MUTEX( disconnect_sem );
static DECLARE_MUTEX( minor_table_mutex );

extern devfs_handle_t usb_devfs_handle;

static void	rwand_status_irq	(struct urb *urb);
static void	*rwand_probe		(struct usb_device *dev, unsigned int ifnum, const struct usb_device_id *id);
static void	rwand_disconnect	(struct usb_device *dev, void *ptr);
static inline void usb_rwand_remove_disconnected( struct usb_rwand *rwand );
static void rwand_reset_settings( struct usb_rwand *dev );
static void rwand_delete( struct usb_rwand *rwand );
static void rwand_enter_state_starting( struct usb_rwand *dev );
static void rwand_enter_state_stabilizing  (struct usb_rwand *dev);
static void    rwand_enter_state_running      (struct usb_rwand *dev);
static void rwand_enter_state_off( struct usb_rwand *dev );
static int rwand_read_byte( struct usb_rwand *dev, unsigned short request, unsigned short wValue, unsigned short wIndex );
static void rwand_set_model( struct usb_rwand *dev );
static void rwand_process_status( struct usb_rwand *dev, const unsigned char *packet );
static void rwand_decode_status( const unsigned char *in, struct rwand_status *out );
static void rwand_update_state_off(struct usb_rwand *dev, struct rwand_status *new_status);
static void rwand_update_state_starting(struct usb_rwand *dev, struct rwand_status *new_status);
static void rwand_update_state_stabilizing(struct usb_rwand *dev, struct rwand_status *new_status);
static void rwand_update_state_running(struct usb_rwand *dev, struct rwand_status *new_status);
static void rwand_ensure_mode(struct usb_rwand *dev, struct rwand_status *new_status, int mode);
static int filter_push(struct filter *filter, int new_value);
static void filter_reset(struct filter *filter);
static unsigned char reverse_bits(unsigned char b);
static void rwand_calc_timings(struct rwand_settings *settings, int period, struct rwand_timings *timings);
static void rwand_nb_request(struct usb_rwand *dev, unsigned short request, unsigned short wValue, unsigned short wIndex);
static int rwand_open(struct inode *inode, struct file *file);
static int rwand_release(struct inode *inode, struct file *file);
static int rwand_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t rwand_write(struct file *file, const char *buffer, size_t count, loff_t *ppos);
static int rwand_wait_for_fb(struct usb_rwand *dev);
static void rwand_update_fb(struct usb_rwand *dev, int bytes, unsigned char *fb);
static void rwand_request_flip(struct usb_rwand *dev);

#define REQUEST_TIMEOUT (HZ/10)
#define STATUS_PACKET_SIZE 8

#ifdef VERTICAL_FLIP
#define CONVERT_FB_BYTE(b)   (reverse_bits(b))
#else
#define CONVERT_FB_BYTE(b)   (b)
#endif


static struct file_operations rwand_dev_fops = {
    .owner	= THIS_MODULE,
    .ioctl	= rwand_ioctl,
    .open	= rwand_open,
    .release	= rwand_release,
    .write	= rwand_write,
};

static struct usb_driver rwand_driver = {
    .name	= "rwand",
    .probe	= rwand_probe,
    .disconnect	= rwand_disconnect,
    .id_table	= rwand_table,
    .minor	= RWAND_MINOR_BASE,
    .fops	= &rwand_dev_fops,
};

static struct model_intrinsics model_table[] = {
    {
	.model		= 1,
	.name		= "Original Ravinia",
	.coil_center	= 0x4000,
	.coil_width	= 0x7000,
	.fine_adjust	= -125,
	.startup	= {
	    .min_period = 44726.922,
	    .max_period = 44274.759,
	    .climb_rate = 65.502,
	},
    },
    {
	.model		= 2,
	.name		= "Fascinations XP3",
	.coil_center	= 0x4000,
	.coil_width	= 0x6200,
	.fine_adjust	= -80,
	.startup	= {
	    .min_period	= 45000,
	    .max_period = 50000,
	    .climb_rate = 700,
	},
    },
    { }
};

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

static void rwand_update_state_off(struct usb_rwand *dev, struct rwand_status *new_status)
{
	/* Everything off */
	rwand_ensure_mode(dev, new_status, 0);
}

static void rwand_update_state_starting(struct usb_rwand *dev, struct rwand_status *new_status)
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

	if (dev->edge_count > STARTING_EDGES)
		rwand_enter_state_stabilizing(dev);
	
}

static void rwand_update_state_stabilizing(struct usb_rwand *dev, struct rwand_status *new_status)
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

static void rwand_update_state_running(struct usb_rwand *dev, struct rwand_status *new_status)
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
static void rwand_ensure_mode(struct usb_rwand *dev, struct rwand_status *new_status, int mode)
{
	if (new_status->mode != mode)
		rwand_nb_request(dev, RWAND_CTRL_SET_MODES, mode, 0);
}

/* Completion handler for anonymous nonblocking requests. prints out
 * errors, but normally just frees the URB and goes on. Frees the
 * additional data in urb->context if not NULL.
 */
static void rwand_nb_irq(struct urb *urb)
{
	if (urb->status < 0)
		err("Bad URB status/size: status=%d, length=%d", urb->status, urb->actual_length);
	if (urb->context) {
		kfree(urb->context);
		urb->context = NULL;
	}
	usb_free_urb(urb);
}


static void rwand_status_irq( struct urb *urb )
{
    struct usb_rwand *dev = (struct usb_rwand*)urb->context;

    switch (urb->status) {
    case -ECONNRESET:
	return;
    case -ENOENT:
	err("ENOENT in rwand_irq");
	return;
    case -ESHUTDOWN:
	err("ESHUTDOWN in rwand_irq");
	return;
    }

    if( urb->status != 0 || urb->actual_length != STATUS_PACKET_SIZE) {
	err("Bad URB status/size: status=%d, length=%d", urb->status, urb->actual_length);
	return;
    }

    rwand_process_status( dev, dev->irq_data );

    usb_fill_control_urb( dev->irq, dev->dev, usb_rcvctrlpipe( dev->dev, 0 ),
	(unsigned char*) &dev->status_request,
	dev->irq_data, STATUS_PACKET_SIZE, rwand_status_irq, dev);

    int retval;
    retval = usb_submit_urb(dev->irq);
    if (retval) {
    	info("Error in rwand_nb_request, retval=%d", retval);
    }
    return;
}

/* Send a generic nonblocking control request, without completion notification.
 * This is safe for use in interrupt context.
 */
static void rwand_nb_request(struct usb_rwand *dev, unsigned short request,
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

	urb = usb_alloc_urb(0);
	if (!urb) {
		kfree(dr);
		err("Out of memory in rwand_nb_request");
		return;
	}

	usb_fill_control_urb(urb, dev->dev, usb_sndctrlpipe(dev->dev, 0),
			     (unsigned char*)dr, NULL, 0,
			     rwand_nb_irq, dr);

	retval = usb_submit_urb(urb);
	if (retval) {
		info("Error in rwand_nb_request, retval=%d", retval);
	}
}

static void rwand_decode_status( const unsigned char *in, struct rwand_status *out )
{
    out->period = in[0] + (in[1] << 8);
    out->phase = in[2] + (in[3] << 8);
    out->edge_count = in[4];
    out->mode = in[5];
    out->flip_count = in[6];
}

static void rwand_process_status( struct usb_rwand *dev, const unsigned char *packet )
{
    struct rwand_status new_status;
    int power = 1;
    rwand_decode_status(packet, &new_status);
    
    if (new_status.flip_count != dev->status.flip_count) {
	dev->page_flip_pending = 0;
	wake_up_interruptible( &dev->wq );
    }
    
    dev->edge_count += (new_status.edge_count - dev->status.edge_count) & 0xFF;
    

    /* Is the display supposed to be on now? */
/*
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
*/
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

static void rwand_delete( struct usb_rwand *rwand )
{
    minor_table[rwand->minor] = NULL;
    if( rwand->irq ) {
	usb_unlink_urb( rwand->irq );
	usb_free_urb( rwand->irq );
    }
    if( rwand->irq_data ) {
	kfree( rwand->irq_data );
    }
    kfree( rwand );
}

static void rwand_enter_state_off( struct usb_rwand *dev )
{
    dev->state = STATE_OFF;
}

static void rwand_enter_state_starting( struct usb_rwand *dev )
{
    dev->state = STATE_STARTING;
    dev->edge_count = 0;
    dev->state_timer = jiffies;
}

static void    rwand_enter_state_stabilizing  (struct usb_rwand *dev)
{
    dev->state = STATE_STABILIZING;
    dev->edge_count = 0;
    dev->state_timer = jiffies + STABILIZER_TIME;
}

static void    rwand_enter_state_running      (struct usb_rwand *dev)
{
	dev->state = STATE_RUNNING;
}

static void rwand_set_model( struct usb_rwand *dev )
{
    int model;
    struct model_intrinsics *intrinsics;
    
    model = rwand_read_byte( dev, RWAND_CTRL_GET_HW_MODEL, 0, 0);
    
    for( intrinsics = model_table; intrinsics->model; intrinsics++ ) {
	if( intrinsics->model == model) {
	    info("Found model %d hardware", model );
	    dev->intrinsics = intrinsics;
	    return;
	}
    }
    
    dev->intrinsics = model_table;
    err("Found unknown hardware model %d", model);
}

static int rwand_read_byte( struct usb_rwand *dev, unsigned short request,
    unsigned short wValue, unsigned short wIndex )
{
    int retval;
    unsigned char byte;
    retval = usb_control_msg( dev->dev, usb_rcvctrlpipe( dev->dev, 0),
	request, USB_TYPE_VENDOR | USB_DIR_IN,
	wValue, wIndex, &byte, 1, REQUEST_TIMEOUT);

    if(retval!=1) {
	err("Error in rwand_request, retval=%d", retval);
    }
    return byte;
}

static void rwand_reset_settings( struct usb_rwand *dev )
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

static void *rwand_probe (struct usb_device *dev, unsigned int ifnum,
    const struct usb_device_id *id)
{
    struct usb_interface_descriptor *interface;
    struct usb_interface *iface;
    struct usb_rwand *rwand;
    int retval;
    int minor;
    
    interface = &dev->actconfig->interface[ifnum].altsetting[0];
    
    if (dev->descriptor.idVendor != RWAND_VENDOR_ID ||
	dev->descriptor.idProduct != RWAND_PRODUCT_ID) {
	return NULL;
    }
    
    info( "Found a rasterwand!" );
    
    down(&minor_table_mutex);
    for( minor = 0; minor < 1; ++minor) {
	if( minor_table[minor] == NULL)
	    break;
    }
    
    /* Allocate memory for our device state and initialize it */

    rwand = kmalloc(sizeof(*rwand), GFP_KERNEL);
    if (rwand == NULL) {
	err("Out of memory");
	return NULL;
    }
    memset(rwand, 0, sizeof(*rwand));
    minor_table[minor] = rwand;
    iface = &dev->actconfig->interface[ifnum];
    interface = &iface->altsetting[iface->act_altsetting];
    rwand->endpoint = &interface->endpoint[0];
    
    rwand->dev = dev;
    rwand->iface = interface->bInterfaceNumber;
    rwand->endpoint = interface->endpoint;
    
    init_MUTEX(&rwand->lock);
    init_waitqueue_head(&rwand->wq);
    rwand->minor = minor;
    
    rwand_set_model( rwand );
    rwand_reset_settings( rwand );
    rwand_enter_state_off( rwand );
    
    rwand->status_request.bRequestType = USB_TYPE_VENDOR;
    rwand->status_request.bRequest = RWAND_CTRL_READ_STATUS;
    
//    rwand->irq_data = usb_buffer_alloc( dev, STATUS_PACKET_SIZE, SLAB_ATOMIC, &rwand->irq_dma);
//    if (!rwand->irq_data) {
//	retval = -ENOMEM;
//	goto error;
//    }

    rwand->irq_data = kmalloc(STATUS_PACKET_SIZE, SLAB_ATOMIC);    
    if( rwand->irq_data == NULL ) {
	err("Out of memory");
	return NULL;
    }

    char name[10];
    sprintf( name, "rwand%d", rwand->minor );
    rwand->devfs = devfs_register
	(usb_devfs_handle, name,
	DEVFS_FL_DEFAULT, USB_MAJOR,
	RWAND_MINOR_BASE + rwand->minor,
	S_IFCHR | S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP |S_IWOTH|S_IROTH,
	&rwand_dev_fops, NULL );
    
    rwand->irq = usb_alloc_urb( 0 );
    if( !rwand->irq ) {
	retval = -ENODEV;
	goto error;
    }
    
    usb_fill_control_urb( rwand->irq, rwand->dev, usb_rcvctrlpipe( dev, 0 ),
	(unsigned char*) &rwand->status_request,
	rwand->irq_data, STATUS_PACKET_SIZE, rwand_status_irq, rwand);
	
    usb_submit_urb( rwand->irq );

    info("Raster Wand device now attached" );
    up (&minor_table_mutex);
    
    return rwand;

error:
    rwand_delete(rwand);
    return NULL;
}

/******************************************************************************/
/******************************************************* Character device *****/
/******************************************************************************/

static int rwand_open(struct inode *inode, struct file *file)
{
	struct usb_rwand *dev = NULL;
	struct rwand_fd_private *prv = NULL;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = MINOR(inode->i_rdev) - RWAND_MINOR_BASE;

	/* Prevent disconnects */
	down(&disconnect_sem);

	dev = minor_table[subminor];
	if (!dev) {
		retval = -ENODEV;
		goto exit_no_device;
	}

	/* lock this device */
	down (&dev->lock);

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
	up (&dev->lock);

exit_no_device:
	up (&disconnect_sem);
	return retval;
}

static int rwand_release(struct inode *inode, struct file *file)
{
	struct rwand_fd_private *prv;
	struct usb_rwand *dev;
	int retval = 0;

	prv =(struct rwand_fd_private *)file->private_data;
	if (prv == NULL) {
		dbg("object is NULL");
		return -ENODEV;
	}
	dev = prv->dev;

	/* lock our device */
	down(&dev->lock);

	if (dev->open_count <= 0) {
		dbg("device not opened");
		retval = -ENODEV;
		goto exit_not_opened;
	}

	if (dev->dev == NULL) {
		/* the device was unplugged before the file was released */
		up(&dev->lock);
		rwand_delete(dev);
		return 0;
	}

	/* decrement our usage count for the device */
	--dev->open_count;

	/* Free this device's private data */
	kfree(prv);
	file->private_data = NULL;

exit_not_opened:
	up(&dev->lock);
	return retval;
}


static int rwand_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct rwand_fd_private *prv;
	struct usb_rwand *dev;
	int retval = 0;

	prv =(struct rwand_fd_private *)file->private_data;
	if (prv == NULL) {
		dbg("object is NULL");
		return -ENODEV;
	}
	dev = prv->dev;

	/* lock this object */
	down(&dev->lock);

	/* verify that the device wasn't unplugged */
	if (dev->dev == NULL) {
		up(&dev->lock);
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
	up(&dev->lock);

	return retval;
}


static ssize_t rwand_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	struct rwand_fd_private *prv;
	struct usb_rwand *dev;
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
	down(&dev->lock);

	/* verify that the device wasn't unplugged */
	if (dev->dev == NULL) {
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
	up(&dev->lock);

	return retval;
}

/* Wait for any pending page flips to finish. This can return an error
 * if the device is disconnected or we're interrupted while waiting.
 * This should be called while holding the device lock.
 */
static int rwand_wait_for_fb(struct usb_rwand *dev)
{
	DECLARE_WAITQUEUE(wait, current);
	int retval = 0;

	if (dev->page_flip_pending && (dev->status.mode & RWAND_MODE_ENABLE_DISPLAY)) {
		add_wait_queue(&dev->wq, &wait);
		current->state = TASK_INTERRUPTIBLE;

		while (dev->page_flip_pending && (dev->status.mode & RWAND_MODE_ENABLE_DISPLAY)) {
			if (signal_pending(current)) {
				retval = -ERESTARTSYS;
				break;
			}

			up(&dev->lock);
			schedule();
			down(&dev->lock);
		}

		current->state = TASK_RUNNING;
		remove_wait_queue(&dev->wq, &wait);
	}

	return retval;
}


/* Change the device's framebuffer size and/or contents to match the one given.
 * This should be called while holding the device lock.
 */
static void rwand_update_fb(struct usb_rwand *dev, int bytes, unsigned char *fb)
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
		retval = usb_control_msg(dev->dev, usb_sndctrlpipe(dev->dev, 0),
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
static void rwand_request_flip(struct usb_rwand *dev)
{
	dev->page_flip_pending = 1;
	rwand_nb_request(dev, RWAND_CTRL_FLIP, 0, 0);
}

static void rwand_disconnect (struct usb_device *dev, void *ptr)
{
    struct usb_rwand *rwand = (struct usb_rwand *) ptr;
    int minor;

    /* prevent races with open() */
    down (&disconnect_sem);
    
//    dev = usb_get_intfdata (interface);
//    usb_set_intfdata (interface, NULL);

    down (&rwand->lock);

    minor = rwand->minor;
//    input_unregister_device(&dev->input);
//    usb_deregister_dev(interface, &rwand_class);

    /* if the device is not opened, then we clean up right now */
    if (!rwand->open_count) {
    	up(&rwand->lock);
    	rwand_delete(rwand);
    } else {
    	rwand->dev = NULL;
    	up(&rwand->lock);
    }

    up(&disconnect_sem);
    devfs_unregister( rwand->devfs );
    info("rwand #%d now disconnected", minor);
}
/*
static inline void usb_rwand_remove_disconnected( struct usb_rwand *rwand )
{
    rwand->dev = NULL;
    wake_up_interruptible( &rwand->wq );
    if( rwand->irq_data != NULL )
	kfree( rwand->irq_data );    
    kfree(rwand);
}
*/
static int __init rwand_init( void )
{
    int result = 0;
    
    result = usb_register( &rwand_driver );
    if( result < 0 )
    {
	err( "usb_register failed with errno: %d", result );
    }
    else
    {
	info( "Rasterwand driver version " DRIVER_VERSION " loaded." );
    }
    
    return result;
}

static void __exit rwand_exit( void )
{
    usb_deregister( &rwand_driver );
}

module_init( rwand_init );
module_exit( rwand_exit );
