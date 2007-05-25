/*
 * Userspace Raster Wand driver
 *
 * This is a reimplementation of the rasterwand driver (rwand.ko) as a
 * userspace daemon. It attaches to an rwand device specified on the
 * command line, identifies its version, and runs the state machine
 * and filters required to keep the device running.
 *
 * In the event of an error, including device disconnection, this
 * process will exit. It's the caller's responsibility to re-scan for
 * new rasterwand devices and start a new instance of this process.
 *
 * --------------------------------------------------------------------
 *
 * This process uses a simple line-based protocol on both stdin and
 * stdout. Each line is a sequence of space-separated tokens ending
 * in a newline. The first token identifies the command type.
 *
 * Commands received on stdin:
 *
 *   frame <data>
 *
 *      Set the contents of the framebuffer. This overwrites the
 *      existing framebuffer completely, and sets the display width if
 *      necessary. Framebuffer data is specified as a hexadecimal
 *      string, 2 ASCII bytes per framebuffer byte.
 *
 *      This command triggers a page flip. The controlling process
 *      should not send another frame until the 'frame_ack' command is
 *      sent in response. If another frame is sent first, the device
 *      could exhibit video tearing.
 *
 *   setting <setting> <value>
 *
 *      Change a setting, by name, to the specified integer value.
 *      Unknown settings are silently ignored. Supported settings:
 *
 *      display_center, display_width, coil_center, coil_width,
 *      duty_cycle, fine_adjust
 *
 *   power 1|0
 *
 *      Turn the device on or off.
 *
 * Responses sent via stdout:
 *
 *   frame_ack
 *
 *      The last 'frame' command is now visible on the device. We're
 *      now ready to accept another frame. Note that this response
 *      is actually sent any time the device completes a page flip,
 *      so 'frame' commands sent while a page flip is already in
 *      progress will not generate a corresponding frame_ack.
 *
 *   state <name>
 *
 *      The device's state machine switched states. Current state
 *      names are:
 *
 *      off, starting, stabilizing, running
 *
 *   buttons [<button> <button>...]
 *
 *      The state of the device's buttons has changed. This response
 *      includes a list of the zero or more buttons that are currently
 *      down. Button names are:
 *
 *      power, left, right, up, down, select
 *
 *   setting <setting> <value>
 *
 *      This reports the current value of a setting. One such command
 *      is sent at power-on for every supported setting, in order to
 *      report the default values for the detected rasterwand model.
 *
 *   model <name>
 *
 *      Reports the model name of a rasterwand we've successfully
 *      identified.
 *
 * --------------------------------------------------------------------
 *
 * Copyright(c) 2004-2007 Micah Dowty <micah@picogui.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include <errno.h>
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

union rwand_settings {
    struct {
	int display_center;     /* The center of the display. 0 is full left,
				 * 0xFFFF is full right.
				 */
	int display_width;      /* The total width of the display, from 0 (nothing)
				 * to 0xFFFF (the entire wand sweep)
				 */
	int coil_center;        /* The center of the coil pulse. 0 is full left,
				 * 0x4000 is the center on the left-right pass,
				 * 0x8000 is full-right, 0xC000 is center on the
				 * right-left pass, and 0xFFFF is full left again.
				 */
	int coil_width;         /* The width of the coil pulse, from 0 (nothing) to
				 * 0xFFFF (the entire period)
				 */
	int duty_cycle;         /* The ratio of pixels to gaps. 0xFFFF has no gap,
				 * 0x0000 is all gap and no pixel.
				 */
	int fine_adjust;        /* Fine tuning for the front/back alignment */
	int num_columns;        /* The number of columns actually being displayed.
				 * This is set automatically on write().
				 */
    } s;
    int items[1];
};

/* Settings peculiar to each model of rasterwand clock */
struct model_intrinsics {
    int model;
    const char *name;
    union rwand_settings settings;
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
    union rwand_settings settings;
    struct model_intrinsics *intrinsics;
} device;


/******************************************************************************/
/******************************************************** Data Tables *********/
/******************************************************************************/

/* These must be in the same order as the items in rwand_settings */
static const char *setting_names_table[] = {
    "display_center",
    "display_width",
    "coil_center",
    "coil_width",
    "duty_cycle",
    "fine_adjust",
    NULL,
};

static struct model_intrinsics model_table[] = {
    {
	.model          = 1,
	.name           = "ravinia",
	.settings       = {
	    .s          = {
		.display_center = 0x8000,
		.display_width  = 0x7C00,
		.coil_center    = 0x4000,
		.coil_width     = 0x7000,
		.duty_cycle     = 0xA000,
		.fine_adjust    = -185,
		.num_columns    = 80,
	    },
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
	.name           = "fascinations_xp3",
	.settings       = {
	    .s          = {
		.display_center = 0x8000,
		.display_width  = 0x7C00,
		.coil_center    = 0x4000,
		.coil_width     = 0x6400,
		.duty_cycle     = 0xA000,
		.fine_adjust    = -80,
		.num_columns    = 80,
	    },
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
	exit(1);
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
	exit(1);
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
rwand_calc_timings(union rwand_settings *settings,
		   int period,
                   struct rwand_timings *timings)
{
    int col_and_gap_width, total_width;

    /* The coil driver just needs to have its relative timings
     * multiplied by our predictor's current period. This is fixed
     * point math with 16 digits to the right of the binary point.
     */
    timings->coil_begin = (period * (settings->s.coil_center -
				     settings->s.coil_width/2)) >> 16;
    timings->coil_end   = (period * (settings->s.coil_center +
				     settings->s.coil_width/2)) >> 16;

    if (settings->s.num_columns > 0) {
	/* Now calculate the display timings. We start out with the precise
	 * width of our columns, so that the width of the whole display
	 * can be calculated accurately.
	 */
	col_and_gap_width = (period / settings->s.num_columns *
			     settings->s.display_width) >> 17;
	timings->column_width = (col_and_gap_width * settings->s.duty_cycle) >> 16;
	timings->gap_width = col_and_gap_width - timings->column_width;
	total_width =
	    (settings->s.num_columns) * timings->column_width +
	    (settings->s.num_columns-1) * timings->gap_width;


	/* Now that we know the true width of the display, we can calculate the
	 * two phase timings. These indicate when it starts the forward scan and the
	 * backward scan, relative to the left position. The alignment between
	 * the forward and backward scans should be calculated correctly, but it
	 * can be tweaked using settings->s.fine_adjust. This value is set per-model
	 * to account for latency in the interruption sensor and LED drive hardware.
	 */
	timings->fwd_phase = ((period * settings->s.display_center) >> 17) - total_width/2;
	timings->rev_phase = period - timings->fwd_phase -
	    total_width + settings->s.fine_adjust;
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
    if (width != device.settings.s.num_columns) {
	device.settings.s.num_columns = width;
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
			    device.settings.s.num_columns, 0, 0, NULL);
    }
}


/******************************************************************************/
/*********************************************** High-level Device Layer ******/
/******************************************************************************/

/*
 * Convert a hex digit to an integer. Returns 0 if the character is
 * not a valid hex digit.
 */
int
hexdigit_to_int(char digit)
{
    if (digit >= '0' && digit <= '9') {
	return digit - '0';
    }
    if (digit >= 'a' && digit <= 'f') {
	return digit - 'a' + 10;
    }
    if (digit >= 'A' && digit <= 'F') {
	return digit - 'A' + 10;
    }
    return 0;
}

/*
 * Convert a NUL-terminated hexadecimal string to binary, in-place.
 * Returns the length of the converted data.
 *
 * Every input byte is converted to an output nybble, no matter what.
 * Whitespace is not ignored. Characters that aren't valid hex digits
 * are treated as zero. This includes the NUL terminator, in the case
 * that the input string is an odd number of characters long.
 */
size_t
hex_to_bin_inplace(unsigned char *data)
{
    unsigned char *in = data, *out = data;

    while (*in) {
	*out = (hexdigit_to_int(in[0]) << 4) | hexdigit_to_int(in[1]);
	out++;
	if (!in[1]) {
	    break;
	}
	in += 2;
    }
    
    return out - data;
}


/*
 * Process a complete line received over stdin. (Not including the newline
 * character itself). Bogus commands are ignored.
 */
void
handle_command(char *line)
{
    const char *delim = " \t";
    char *sptr;
    char *command = strtok_r(line, delim, &sptr);
    if (!command)
	return;

    if (strcmp(command, "frame") == 0) {
	unsigned char *data = (unsigned char *) strtok_r(NULL, delim, &sptr);
	rwand_write_frame(data, hex_to_bin_inplace(data));
	return;
    }

    if (strcmp(command, "setting") == 0) {
	char *name = strtok_r(NULL, delim, &sptr);
	char *value = strtok_r(NULL, delim, &sptr);
	if (name && value) {
	    int i;
	    for (i=0; setting_names_table[i]; i++) {
		if (strcmp(setting_names_table[i], name) == 0) {
		    device.settings.items[i] = atoi(value);
		    device.settings_dirty = 1;
		    break;
		}
	    }
	}
	return;
    }

    if (strcmp(command, "power") == 0) {
	char *value = strtok_r(NULL, delim, &sptr);
	if (value) {
	    device.power = atoi(value);
	}
	return;
    }
}


/*
 * Completion callback for status URBs. This drives the main control
 * loop for the device. We always re-submit the status URB, in order
 * to keep the loop going.
 */
void
status_urb_complete(struct async_urb *async)
{
    /* Report button changes */
    if (async->status.buttons != device.last_status_urb.status.buttons) {
	printf("buttons");
	if (async->status.buttons & RWAND_BUTTON_POWER)  printf(" power");
	if (async->status.buttons & RWAND_BUTTON_SQUARE) printf(" select");
	if (async->status.buttons & RWAND_BUTTON_RIGHT)  printf(" right");
	if (async->status.buttons & RWAND_BUTTON_LEFT)   printf(" left");
	if (async->status.buttons & RWAND_BUTTON_UP)     printf(" up");
	if (async->status.buttons & RWAND_BUTTON_DOWN)   printf(" down");
	printf("\n");
    }

    /* Report changes in device state */
    if (device.state != device.last_state) {
	printf("state");
	if (device.state == STATE_OFF)                   printf(" off");
	if (device.state == STATE_STARTING)              printf(" starting");
	if (device.state == STATE_STABILIZING)           printf(" stabilizing");
	if (device.state == STATE_RUNNING)               printf(" running");
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
    if (device.flip_pending &&
	async->status.flip_count != device.last_status_urb.status.flip_count) {
	printf("frame_ack\n");
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
	    printf("model %s\n", intrinsics->name);
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
 * initializes its intrinsics, and outputs information about the
 * model and initial settings.
 *
 * This initializes the hardware by clearing all mode flags and
 * sending a minimal framebuffer, and it starts polling for status
 * changes.
 */
void
rwand_init()
{
    int i;
    unsigned char frame[4] = { 0 };

    filter_reset(&device.filter);
    device.power = 1;
    rwand_set_model();

    device.settings = device.intrinsics->settings;

    for (i=0; setting_names_table[i]; i++) {
	printf("setting %s %d\n",
	       setting_names_table[i],
	       device.settings.items[i]);
    }

    control_write_async(RWAND_CTRL_SET_MODES, 0, 0, 0, NULL);
    rwand_write_frame(frame, 1);
    device.flip_pending = 0;

    for (i=0; i<NUM_OVERLAPPED_URBS; i++) {
	async_urb_submit(async_urb_new_control(0xc0, RWAND_CTRL_READ_STATUS, 8,
					       status_urb_complete));
    }
}

/*
 * Open the device, initialize it, then run the main loop. The main
 * loop responds to input from two sources:
 *
 *  - On URB completion, invoke the URB's callback
 *
 *  - On receiving input from stdin, buffer it into
 *    complete lines and send those lines to handle_command.
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

   /* Make stdin nonblocking */
   if (fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK | fcntl(STDIN_FILENO, F_GETFL)) < 0) {
       perror("fcntl");
       return 1;
   }

   while (1) {
       fd_set read_fds, write_fds;
       
       /*
	* Wait for either a completed URB or data on stdin. Note that URB
	* completion counts as 'write' as far as select() and poll() are
	* concerned.
	*/
       FD_ZERO(&read_fds);
       FD_ZERO(&write_fds);
       FD_SET(STDIN_FILENO, &read_fds);
       FD_SET(device.fd, &write_fds);
       if (select(device.fd + 1, &read_fds, &write_fds, NULL, NULL) < 0) {
	   perror("select");
	   return 1;
       }

       /*
	* Read and handle all pending URBs
	*/
       if (FD_ISSET(device.fd, &write_fds)) {
	   while (1) {
	       struct async_urb *async = NULL;
	       if (ioctl(device.fd, USBDEVFS_REAPURBNDELAY, &async) < 0) {
		   if (errno == EAGAIN) {
		       break;
		   }
		   perror("USBDEVFS_REAPURBNDELAY");
		   return 1;
	       }
	       async->callback(async);
	   };
       }

       /*
	* Read from stdin and reassemble complete lines without blocking.
	*/
       if (FD_ISSET(STDIN_FILENO, &read_fds)) {
	   static char read_buffer[1024];
	   static size_t bytes_read;

	   while (1) {
	       char *newline;
	       int retval = read(STDIN_FILENO,
				 read_buffer + bytes_read,
				 sizeof read_buffer - bytes_read);

	       if (retval <= 0) {
		   if (errno == EAGAIN) {
		       break;
		   }
		   perror("read");		   
		   return 1;
	       }

	       bytes_read += retval;
	       newline = memchr(read_buffer, '\n', bytes_read);
	       if (newline) {
		   size_t line_size = newline - read_buffer + 1;
		   *newline = '\0';
		   handle_command(read_buffer);
		   bytes_read -= line_size;
		   memmove(read_buffer, newline+1, bytes_read);
	       } else if (bytes_read == sizeof read_buffer) {
		   /* Line too long, discard it */
		   bytes_read = 0;
	       }
	   }
       }
   }
}
