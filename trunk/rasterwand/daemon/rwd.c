/*
 * Userspace Raster Wand driver
 *
 * This is a reimplementation of the rasterwand driver (rwand.ko) as a
 * userspace daemon. It automatically scans the USB bus for a rwand
 * device, identifies its version, and maintains a connection to that
 * device. It should support most of the features of the kernelspace
 * driver.
 *
 * This process accepts video frames on stdin, and writes button updates
 * (one update per line, whitespace-separated button tokens) to stdout.
 *
 * No support for tweaking timing parameters at runtime yet.
 *
 * Copyright(c) 2004-2007 Micah Dowty <micah@picogui.org>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <endian.h>
#include <linux/usbdevice_fs.h>
#include <rwand_protocol.h>


/******************************************************************************/
/******************************************************** Definitions *********/
/******************************************************************************/

#define NUM_OVERLAPPED_URBS  2    /* Number of URBs to keep submitted concurrently */

#define FILTER_SIZE          256  /* Increasing this will smooth out display jitter
                                   * at the expense of responding more slowly to
                                   * real changes
                                   */
#define PERIOD_TOLERANCE     20   /* Increasing this will reduce the frequency of
                                   * mall jumps in the display alignment at the increased
                                   * risk of having slightly incorrect timing.
                                   */

#define STABILIZER_EDGES     8    /* Number of edges to successfully exit stabilization */
#define STABILIZER_SECONDS   1    /* Time to unsuccessfully exit stabilization */

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define LE16_SWAP(x)  (x)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define LE16_SWAP(x)  ( (((x) >> 8) & 0xFF) | (((x) << 8) & 0xFF00) )
#endif

/* A simple averaging low-pass filter, O(1) */
struct filter {
    int buffer[FILTER_SIZE];    /* Circular buffer */
    int total;                  /* Total of all values currently in the buffer */
    int n_values;               /* Number of values currently in the buffer */
    int pointer;                /* Location to store the next new value in */
};

/* Timing calculated from the current status and settings */
struct rwand_timings {
    int    column_width;
    int    gap_width;
    int    fwd_phase;
    int    rev_phase;
    int    coil_begin;
    int    coil_end;
};

/* Startup timings */
struct rwand_startup {
    int  min_period;            /* Minimum period, in 2.66us units */
    int  max_period;            /* Maximum period, in 2.66us units */
    int  climb_rate;            /* Period climb rate, in 2.66us units per second */
    int  starting_edges;        /* Number of edges to successfully exit startup */
};

struct rwand_settings {
    int display_center;         /* The center of the display. 0 is full left,
				 * 0xFFFF is full right.
				 */
    int display_width;          /* The total width of the display, from 0 (nothing)
				 * to 0xFFFF (the entire wand sweep)
				 */
    int coil_center;            /* The center of the coil pulse. 0 is full left,
				 * 0x4000 is the center on the left-right pass,
				 * 0x8000 is full-right, 0xC000 is center on the
				 * right-left pass, and 0xFFFF is full left again.
				 */
    int coil_width;             /* The width of the coil pulse, from 0 (nothing) to
				 * 0xFFFF (the entire period)
				 */
    int duty_cycle;             /* The ratio of pixels to gaps. 0xFFFF has no gap,
				 * 0x0000 is all gap and no pixel.
				 */
    int fine_adjust;            /* Fine tuning for the front/back alignment */
    int num_columns;            /* The number of columns actually being displayed.
				 * This is set automatically on write().
				 */
};

/* Settings peculiar to each model of rasterwand clock */
struct model_intrinsics {
    int model;
    const char *name;
    struct rwand_settings settings;
    struct rwand_startup startup;
};

struct async_urb;
typedef void (async_urb_callback)(struct async_urb *async);

struct async_urb {
    struct usbdevfs_urb urb;
    async_urb_callback *callback;
    struct {
	struct {
	    __u8 bRequestType;
	    __u8 bRequest;
	    __u16 wValue;
	    __u16 wIndex;
	    __u16 wLength;
	} setup;
	union {
	    __u8 data[16];
	    struct {
		__u16 period;
		__u16 phase;
		__u8 edge_count;
		__u8 mode;
		__u8 flip_count;
		__u8 buttons;
	    } status;
	};
    };
};

static struct {
    int fd;
    int input_fd;

    enum {
	STATE_OFF,
	STATE_STARTING,
	STATE_STABILIZING,
	STATE_RUNNING,
    } state, last_state;

    unsigned int modes;
    unsigned int edge_count;
    int filtered_period;
    int settings_dirty;
    int flip_pending;
    int power;

    struct timeval state_timer;
    struct async_urb last_status_urb;
    struct filter filter;
    struct rwand_settings settings;
    struct model_intrinsics *intrinsics;
} device;


/******************************************************************************/
/******************************************************** Data Tables *********/
/******************************************************************************/

static struct model_intrinsics model_table[] = {
    {
	.model          = 1,
	.name           = "Original Ravinia",
	.settings       = {
	    .display_center = 0x8000,
	    .display_width  = 0x7C00,
	    .coil_center    = 0x4000,
	    .coil_width     = 0x7000,
	    .duty_cycle     = 0xA000,
	    .fine_adjust    = -185,
	    .num_columns    = 80,
	},
	.startup        = {
	    .min_period     = 45000,
	    .max_period     = 50000,
	    .climb_rate     = 700,
	    .starting_edges = 20,
	},
    },
    {
	.model          = 2,
	.name           = "Fascinations XP3",
	.settings       = {
	    .display_center = 0x8000,
	    .display_width  = 0x7C00,
	    .coil_center    = 0x4000,
	    .coil_width     = 0x7000,
	    .duty_cycle     = 0xA000,
	    .fine_adjust    = -80,
	    .num_columns    = 80,
	},
	.startup        = {
	    .min_period     = 42316,
	    .max_period     = 42818,
	    .climb_rate     = 4662,
	    .starting_edges = 100,
	},
    },
    { }
};


/******************************************************************************/
/******************************************************** Filter **************/
/******************************************************************************/

/* Add a new value to the filter, returning the filter's current value */
static int
filter_push(struct filter *filter, int new_value)
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

static void
filter_reset(struct filter *filter)
{
    filter->total = 0;
    filter->n_values = 0;
    filter->pointer = 0;
}


/******************************************************************************/
/******************************************************** Async URB Layer *****/
/******************************************************************************/

/*
 * Asynchronously submit a URB. Its callback will be invoked later.
 */
static void
async_urb_submit(struct async_urb *async)
{
    if (ioctl(device.fd, USBDEVFS_SUBMITURB, async) < 0) {
	perror("USBDEVFS_SUBMITURB");
    }
}

/*
 * Allocate and zero a new URB
 */
static struct async_urb *
async_urb_new()
{
    struct async_urb *async = calloc(1, sizeof *async);
    if (!async) {
	perror("malloc");
	exit(1);
    }
    return async;
}

/*
 * Allocate a new Control URB
 */
static struct async_urb *
async_urb_new_control(__u8 bRequestType, __u8 bRequest, __u16 wLength,
                      async_urb_callback *callback)
{
    struct async_urb *async = async_urb_new();

    async->urb.type = USBDEVFS_URB_TYPE_CONTROL;
    async->urb.endpoint = 0;
    async->urb.buffer_length = sizeof async->setup + wLength;
    async->urb.buffer = &async->setup;
    async->setup.bRequestType = bRequestType;
    async->setup.bRequest = bRequest;
    async->setup.wLength = LE16_SWAP(wLength);
    async->callback = callback;

    return async;
}

/*
 * Perform an asynchronous control write, with no completion notification.
 * All asynchronous control writes will be queued and performed in FIFO order.
 */
static void
control_write_async(__u8 bRequest, __u16 wValue, __u16 wIndex,
                    __u16 wLength, const char *data)
{
    struct async_urb *urb = async_urb_new_control(0x40, bRequest, wLength,
						  (async_urb_callback*) free);
    urb->setup.wValue = LE16_SWAP(wValue);
    urb->setup.wIndex = LE16_SWAP(wIndex);
    memcpy(urb->data, data, wLength);
    async_urb_submit(urb);
}


/******************************************************************************/
/*************************************************** Low-level Device Layer ***/
/******************************************************************************/

/* Send a blocking control request, read one byte of data back */
static int
rwand_read_byte(unsigned short request, unsigned short wValue, unsigned short wIndex)
{
    struct usbdevfs_ctrltransfer xfer;
    unsigned char byte;

    xfer.bRequestType = 0xC0;
    xfer.bRequest = request;
    xfer.wValue = wValue;
    xfer.wIndex = wIndex;
    xfer.wLength = sizeof byte;
    xfer.timeout = 1000;
    xfer.data = &byte;

    if (ioctl(device.fd, USBDEVFS_CONTROL, &xfer) < 0) {
	perror("USBDEVFS_CONTROL");
    }

    return byte;
}

/* Change modes if necessary */
static void
rwand_set_modes(int modes)
{
    if (modes != device.modes) {
	device.modes = modes;
	control_write_async(RWAND_CTRL_SET_MODES, modes, 0, 0, NULL);
    }
}

/* Calculate all the fun little timing parameters needed by the hardware */
static void
rwand_calc_timings(struct rwand_settings *settings,
		   int period,
                   struct rwand_timings *timings)
{
    int col_and_gap_width, total_width;

    /* The coil driver just needs to have its relative timings
     * multiplied by our predictor's current period. This is fixed
     * point math with 16 digits to the right of the binary point.
     */
    timings->coil_begin = (period * (settings->coil_center -
				     settings->coil_width/2)) >> 16;
    timings->coil_end   = (period * (settings->coil_center +
				     settings->coil_width/2)) >> 16;

    if (settings->num_columns > 0) {
	/* Now calculate the display timings. We start out with the precise
	 * width of our columns, so that the width of the whole display
	 * can be calculated accurately.
	 */
	col_and_gap_width = (period / settings->num_columns *
			     settings->display_width) >> 17;
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
	timings->rev_phase = period - timings->fwd_phase -
	    total_width + settings->fine_adjust;
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

/*
 * Asynchronously write a frame to the device.
 * Frame width should be a multiple of 4.
 */
static void
rwand_write_frame(unsigned char *data, int width)
{
    if (width != device.settings.num_columns) {
	device.settings.num_columns = width;
	device.settings_dirty = 1;
    }

    while (width > 0) {
	control_write_async(RWAND_CTRL_SEQ_WRITE4,
			    data[0] | (data[1] << 8),
			    data[2] | (data[3] << 8),
			    0, NULL);
	width -= 4;
	data += 4;
    }

    control_write_async(RWAND_CTRL_FLIP, 0, 0, 0, NULL);
    device.flip_pending = 1;
}


/******************************************************************************/
/******************************************************** State Handlers ******/
/******************************************************************************/

static void
rwand_enter_state_off()
{
    device.state = STATE_OFF;
}

static void
rwand_enter_state_starting()
{
    device.state = STATE_STARTING;
    device.edge_count = 0;
    gettimeofday(&device.state_timer, NULL);
}

static void
rwand_enter_state_stabilizing()
{
    device.state = STATE_STABILIZING;
    device.edge_count = 0;

    /* Set the stabilization deadline */
    gettimeofday(&device.state_timer, NULL);
    device.state_timer.tv_sec += STABILIZER_SECONDS;
}

static void
rwand_enter_state_running()
{
    device.state = STATE_RUNNING;
}

static void
rwand_update_state_off(struct async_urb *status_urb)
{
    rwand_set_modes(0);
}

static void
rwand_update_state_starting(struct async_urb *status_urb)
{
    int new_period;
    struct rwand_timings timings;
    struct rwand_startup *startup = &device.intrinsics->startup;
    struct timeval now;

    rwand_set_modes(RWAND_MODE_ENABLE_COIL);

    /* Cycle through different frequencies trying to get our wand to start up */
    new_period = status_urb->status.period;

    if (new_period > startup->max_period ||
	new_period < startup->min_period) {
	if (startup->climb_rate >= 0)
	    new_period = startup->min_period;
	else
	    new_period = startup->max_period;
    }

    gettimeofday(&now, NULL);
    new_period += (now.tv_sec - device.state_timer.tv_sec) * startup->climb_rate +
	(now.tv_usec - device.state_timer.tv_usec) * startup->climb_rate / 1000000;
    device.state_timer = now;

    /* Set the new period and coil phase */
    rwand_calc_timings(&device.settings, new_period, &timings);
    control_write_async(RWAND_CTRL_SET_PERIOD,
			new_period, 0, 0, NULL);
    control_write_async(RWAND_CTRL_SET_COIL_PHASE,
			timings.coil_begin, timings.coil_end, 0, NULL);

    if (device.edge_count > startup->starting_edges) {
	rwand_enter_state_stabilizing();
    }
}

static void
rwand_update_state_stabilizing(struct async_urb *status_urb)
{
    struct timeval now;

    rwand_set_modes(RWAND_MODE_ENABLE_SYNC | RWAND_MODE_ENABLE_COIL);

    if (device.edge_count > STABILIZER_EDGES) {
	/* Successful exit */
	rwand_enter_state_running();
    } else {
	gettimeofday(&now, NULL);
	if (device.state_timer.tv_sec < now.tv_sec ||
	    (device.state_timer.tv_sec == now.tv_sec &&
	     device.state_timer.tv_usec < now.tv_usec)) {
	    
	    /* Unsuccessful exit */
	    rwand_enter_state_starting();
	}
    }
}

static void
rwand_update_state_running(struct async_urb *status_urb)
{
    int new_filtered_period;

    /* We want the works */
    rwand_set_modes(RWAND_MODE_STALL_DETECT |
		    RWAND_MODE_ENABLE_SYNC |
		    RWAND_MODE_ENABLE_COIL |
		    RWAND_MODE_ENABLE_DISPLAY);

    /* If our display just turned off, the firmware detected a stall. Go back
     * to the 'starting' state and give up this.
     */
    if ((device.last_status_urb.status.mode & RWAND_MODE_ENABLE_DISPLAY) &&
	!(status_urb->status.mode & RWAND_MODE_ENABLE_DISPLAY)) {
	rwand_enter_state_starting();
	return;
    }

    /* If our mode has changed, both force a timing update and reset the period filter */
    if (device.last_status_urb.status.mode != status_urb->status.mode) {
	device.settings_dirty = 1;
	filter_reset(&device.filter);
    }

    new_filtered_period = filter_push(&device.filter,
				      LE16_SWAP(status_urb->status.period));

    if (device.settings_dirty ||
	abs(new_filtered_period - device.filtered_period) > PERIOD_TOLERANCE) {

	struct rwand_timings timings;
	device.settings_dirty = 0;

	rwand_calc_timings(&device.settings, new_filtered_period, &timings);
	device.filtered_period = new_filtered_period;

	control_write_async(RWAND_CTRL_SET_COIL_PHASE,
			    timings.coil_begin, timings.coil_end, 0, NULL);
	control_write_async(RWAND_CTRL_SET_COLUMN_WIDTH,
			    timings.column_width, timings.gap_width, 0, NULL);
	control_write_async(RWAND_CTRL_SET_DISPLAY_PHASE,
			    timings.fwd_phase, timings.rev_phase, 0, NULL);
	control_write_async(RWAND_CTRL_SET_NUM_COLUMNS,
			    device.settings.num_columns, 0, 0, NULL);
    }
}


/******************************************************************************/
/*********************************************** High-level Device Layer ******/
/******************************************************************************/

/*
 * Completion callback for status URBs. This drives the main control
 * loop for the device. We always re-submit the status URB, in order
 * to keep the loop going.
 */
void
status_urb_complete(struct async_urb *async)
{
    /* Report changes in button/device state */
    if (async->status.buttons != device.last_status_urb.status.buttons ||
	device.state != device.last_state) {
	if (device.state == STATE_OFF)                   printf("off ");
	if (device.state == STATE_STARTING)              printf("starting ");
	if (device.state == STATE_STABILIZING)           printf("stabilizing ");
	if (device.state == STATE_RUNNING)               printf("running ");
	if (async->status.buttons & RWAND_BUTTON_SQUARE) printf("select ");
	if (async->status.buttons & RWAND_BUTTON_RIGHT)  printf("right ");
	if (async->status.buttons & RWAND_BUTTON_LEFT)   printf("left ");
	if (async->status.buttons & RWAND_BUTTON_UP)     printf("up ");
	if (async->status.buttons & RWAND_BUTTON_DOWN)   printf("down ");
	printf("\n");

	device.last_state = device.state;
    }

    /*
     * Update our local edge counter
     */
    device.edge_count += async->status.edge_count - device.last_status_urb.status.edge_count;

    /*
     * If the page flip counter has incremented, clear our flip pending
     * flag to let us start writing a new frame.
     */
    if (async->status.flip_count != device.last_status_urb.status.flip_count) {
	device.flip_pending = 0;
    }

    /*
     * Is the display supposed to be on now?
     */
    if (device.power) {
	if (device.state == STATE_OFF) {
	    rwand_enter_state_starting();
	}
    } else {
	if (device.state != STATE_OFF) {
	    rwand_enter_state_off();
	}
    }

    /* Now just go to a state-specific handler */
    switch (device.state) {
    case STATE_OFF:
	rwand_update_state_off(async);
	break;
    case STATE_STARTING:
	rwand_update_state_starting(async);
	break;
    case STATE_STABILIZING:
	rwand_update_state_stabilizing(async);
	break;
    case STATE_RUNNING:
	rwand_update_state_running(async);
	break;
    }

    device.last_status_urb = *async;
    async_urb_submit(async);
}

/* Look up the device's model and find it in our table */
static void
rwand_set_model()
{
    int model;
    struct model_intrinsics *intrinsics;

    model = rwand_read_byte(RWAND_CTRL_GET_HW_MODEL, 0, 0);

    for (intrinsics=model_table; intrinsics->model; intrinsics++) {
	if (intrinsics->model == model) {
	    /* Found it */
	    device.intrinsics = intrinsics;
	    return;
	}
    }
    
    /*
     * Oops, can't find it. Generate an error, and use the first
     * model in the table whether it's right or not.
     */
    device.intrinsics = model_table;
    fprintf(stderr, "Warning: Found unknown hardware model %d\n", model);
}

/*
 * Initialize the device by resetting its mode and submitting all
 * initial status requests. This also probes the device model and
 * initializes its intrinsics.
 */
void
rwand_init()
{
    int i;

    filter_reset(&device.filter);
    device.power = 1;
    rwand_set_model();
    device.settings = device.intrinsics->settings;

    control_write_async(RWAND_CTRL_SET_MODES, 0, 0, 0, NULL);

    for (i=0; i<NUM_OVERLAPPED_URBS; i++) {
	async_urb_submit(async_urb_new_control(0xc0, RWAND_CTRL_READ_STATUS, 8,
					       status_urb_complete));
    }
}

/*
 * Open the device, initialize it, then run the main loop. This waits
 * for completed URBs and invokes the proper callbacks.
 */
int
main(int argc, char **argv)
{
   if (argc != 2) {
      fprintf(stderr, "usage: %s /proc/bus/usb/<BUS>/<ADDR>\n", argv[0]);
      return 1;
   }

   /*
    * Open the USB device
    */
   device.fd = open(argv[1], O_RDWR);
   if (device.fd < 0) {
      perror("open");
      return 1;
   }

   rwand_init();

   while (1) {
      struct async_urb *async = NULL;
      if (ioctl(device.fd, USBDEVFS_REAPURB, &async) < 0) {
         perror("USBDEVFS_REAPURB");
      }
      if (async) {
         async->callback(async);
      }
   }
}
