#include <usb.h>
#include <stdio.h>
#include <rwand_protocol.h>
#include <time.h>

struct rwand_status {
  unsigned short period;
  unsigned short phase;
  unsigned char edge_count;
  unsigned char mode;
  unsigned char flip_pending;
  unsigned char buttons;
};

usb_dev_handle *open_rwand(void) {
  struct usb_bus *busses;
  struct usb_bus *bus;
  struct usb_device *dev;

  /* Find the device */
  usb_init();
  usb_find_busses();
  usb_find_devices();
  busses = usb_get_busses();

  for (bus=busses; bus; bus=bus->next) {
    for (dev=bus->devices; dev; dev=dev->next) {
      if (dev->descriptor.idVendor == RWAND_VENDOR_ID &&
	  dev->descriptor.idProduct == RWAND_PRODUCT_ID) {
	return usb_open(dev);
      }
    }
  }
  printf("No device found\n");
  return NULL;
}

void control_write(usb_dev_handle *d, int request, int value, int index) {
  if (usb_control_msg(d, USB_TYPE_VENDOR, request,
		      value, index, NULL, 0, 500) < 0) {
    perror("usb_control_msg");
    exit(1);
  }
}

void control_read(usb_dev_handle *d, int request, unsigned char* buffer, int bytes) {
  if (usb_control_msg(d, USB_TYPE_VENDOR | USB_ENDPOINT_IN, request,
		      0, 0, buffer, bytes, 500) < 0) {
    perror("usb_control_msg");
    exit(1);
  }
}

unsigned char control_read_byte(usb_dev_handle *d, int request) {
  unsigned char b;
  control_read(d, request, &b, 1);
  return b;
}


void read_rwand_status(usb_dev_handle *d, struct rwand_status *status) {
  unsigned char packet[8];
  control_read(d, RWAND_CTRL_READ_STATUS, packet, sizeof(packet));
  status->period = packet[0] + (packet[1] << 8);
  status->phase = packet[2] + (packet[3] << 8);
  status->edge_count = packet[4];
  status->mode = packet[5];
  status->flip_pending = packet[6];
  status->buttons = packet[7];
}

void update_coil_driver(usb_dev_handle *d, struct rwand_status *status,
			float pulse_center, float pulse_width) {
  control_write(d, RWAND_CTRL_SET_COIL_PHASE,
		status->period * (pulse_center - pulse_width),
		status->period * (pulse_center + pulse_width));
}

void update_display_timing(usb_dev_handle *d, struct rwand_status *status,
			   float center, float width, float duty_cycle) {
  int col_and_gap_width = (status->period * (width/2)) / 80;
  int col_width = col_and_gap_width * duty_cycle;
  int gap_width = col_and_gap_width - col_width;
  int total_width = 80 * col_width + 79 * gap_width;
  int fwd_phase = status->period/2 * (center - width/2);
  int rev_phase = status->period - fwd_phase - total_width;
  static int fudge_factor = 0;

  control_write(d, RWAND_CTRL_SET_COLUMN_WIDTH, col_width, gap_width);
  control_write(d, RWAND_CTRL_SET_DISPLAY_PHASE, fwd_phase, rev_phase + fudge_factor);

  if (status->buttons & RWAND_BUTTON_UP)
    fudge_factor += 1;
  else if (status->buttons & RWAND_BUTTON_DOWN)
    fudge_factor -= 1;
  else
    return;
  printf("Fudge factor = %d\n", fudge_factor);
}

void unstall(usb_dev_handle *d) {
  unsigned period = -1;
  int unstall_edges = 0;
  struct rwand_status status, last_status;

  printf("Possibly stalled, trying to restart...\n");

  read_rwand_status(d, &last_status);
  status = last_status;

  while (unstall_edges < 40) {
    /* This part needs a lot of work...
     * We need to use our sole source of feedback (whether we get
     * angle sensor edges or not) to try to zero in on the resonant frequency.
     */
    if (unstall_edges == 0) {
      if (period > 53000)
	period = 47000;
      period += 500;
    }
    else {
      period -= 10;
    }

    printf("period = %d, edges = %d\n", period, unstall_edges);
    control_write(d, RWAND_CTRL_SET_PREDICTION, 0, period);
    update_coil_driver(d, &status, 0.25, 0.25);

    usleep(1000000);

    last_status = status;
    read_rwand_status(d, &status);
    unstall_edges += (status.edge_count - last_status.edge_count) & 0xFF;
  }
  printf("Non-stall verified\n");
}

void read_image(unsigned char *columns, const char *filename) {
  /* Read in an 80x8 image in PGM format, store in the columns array
   * This is for a crappy little test program, so it ignores the first 4 lines
   * rather than reading the header properly.
   */
  FILE *f = fopen(filename, "rb");
  char line[256];
  int x, y;

  if (!f) {
    printf("Can't open %s\n", filename);
    exit(1);
  }
  fgets(line, 255, f);
  fgets(line, 255, f);
  fgets(line, 255, f);
  fgets(line, 255, f);

  memset(columns, 0, 80);
  for (y=0; y<8; y++)
    for (x=0; x<80; x++)
      if (fgetc(f) < 0x80)
	columns[x] |= 1 << y;
  fclose(f);
}

void refresh_display(usb_dev_handle *d, unsigned char *columns) {
  /* Write out an 80x8 frame, given an 80-byte column array */
  int i, b;
  for (i=0; i<30; i+=3) {
    if (i+3 >= 80)
      i = 77;
    control_write(d, RWAND_CTRL_RANDOM_WRITE3, i | (columns[i] << 8),
		  columns[i+1] | (columns[i+2] << 8));
  }
  control_write(d, RWAND_CTRL_FLIP, 0,0);
}

int main(int argc, char **argv) {
  usb_dev_handle *d;
  struct rwand_status status, last_status;
  time_t last_edge_timestamp;
  unsigned char frame[80];

  if (!(d = open_rwand()))
    return 1;

  /* start out stalled, so we can verify our current status */
  last_edge_timestamp = 0;
  read_rwand_status(d, &status);

  while (status.buttons & RWAND_BUTTON_POWER) {
    /* Read the current period prediction, calculate the frequency.
     * (the period is in units of 16 CPU cycles on a 6 MIPS processor)
     */
    last_status = status;
    read_rwand_status(d, &status);

    /*
      printf("%.02f Hz    Buttons: 0x%02X\n",
      1/(status.period * 16 / 6000000.0),
      status.buttons);
    */

    /* Have we had any synchronization edges this time? */
    if (status.edge_count != last_status.edge_count)
      last_edge_timestamp = time(NULL);

    /* If it's been a while since we've seen a sync edge, conclude we're stalled */
    if (time(NULL) > last_edge_timestamp + 1) {
      control_write(d, RWAND_CTRL_SET_MODES,
		    RWAND_MODE_ENABLE_SYNC |
		    RWAND_MODE_ENABLE_COIL, 0);
      unstall(d);
      last_edge_timestamp = time(NULL);
      control_write(d, RWAND_CTRL_SET_MODES,
		    RWAND_MODE_ENABLE_SYNC |
		    RWAND_MODE_ENABLE_COIL |
		    RWAND_MODE_ENABLE_DISPLAY, 0);
    }

    /* update coil driver phase */
    update_coil_driver(d, &status, 0.25, 0.2);

    /* Update display phase and column width */
    update_display_timing(d, &status, 0.5, 0.75, 0.75);

    /* Was the square just now pushed? */
    if ((status.buttons & RWAND_BUTTON_SQUARE) && !(last_status.buttons & RWAND_BUTTON_SQUARE)) {
      read_image(frame, "test-image.pgm");
      refresh_display(d, frame);
      control_write(d, RWAND_CTRL_FLIP, 0, 0);
    }
    else if (!(status.buttons & RWAND_BUTTON_SQUARE)) {
      /* If our last page flip has finished, write another frame */
      if (!status.flip_pending) {
	static float t = 0;
	int i, y;
	for (i=0; i<80; i++) {
	  y = sin(t + i * 0.2) * 3.99 + 4;
	  frame[i] = 1<<y;
	}

	if (status.buttons & RWAND_BUTTON_LEFT)
	  t-=0.4;
	else if (status.buttons & RWAND_BUTTON_RIGHT)
	  t+=0.4;

	refresh_display(d, frame);
	control_write(d, RWAND_CTRL_FLIP, 0, 0);
      }
    }
  }

  /* Turn it off */
  control_write(d, RWAND_CTRL_SET_MODES, 0, 0);
  return 0;
}
