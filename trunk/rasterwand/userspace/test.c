#include <usb.h>
#include <stdio.h>
#include <rwand_protocol.h>
#include <time.h>

struct rwand_status {
  unsigned short period;
  unsigned short phase;
  unsigned char edge_count;
  unsigned char mode;
  unsigned char flip_count;
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
  status->flip_count = packet[6];
  status->buttons = packet[7];
}

void update_coil_driver(usb_dev_handle *d, struct rwand_status *status,
			float pulse_center, float pulse_width) {
  control_write(d, RWAND_CTRL_SET_COIL_PHASE,
		status->period * (pulse_center - pulse_width),
		status->period * (pulse_center + pulse_width));
}

void update_display_timing(usb_dev_handle *d, struct rwand_status *status,
			   float center, float width) {
  const int num_columns = 60;
  static float duty_cycle = 0.5;
  int col_and_gap_width = (status->period * (width/2)) / num_columns;
  int col_width = col_and_gap_width * duty_cycle;
  int gap_width = col_and_gap_width - col_width;
  int total_width = (num_columns+1) * col_width + num_columns * gap_width;
  int fwd_phase = status->period/2 * (center - width/2);
  int rev_phase = status->period - fwd_phase - total_width;
  static int fudge_factor = 0;

  control_write(d, RWAND_CTRL_SET_NUM_COLUMNS, num_columns, 0);
  control_write(d, RWAND_CTRL_SET_COLUMN_WIDTH, col_width, gap_width);
  control_write(d, RWAND_CTRL_SET_DISPLAY_PHASE, fwd_phase, rev_phase + fudge_factor);

  if (status->buttons & RWAND_BUTTON_SQUARE) {
    if (status->buttons & RWAND_BUTTON_UP)
      duty_cycle += 0.02;
    else if (status->buttons & RWAND_BUTTON_DOWN)
      duty_cycle -= 0.02;
    else
      return;
    if (duty_cycle > 1)
      duty_cycle = 1;
    if (duty_cycle < 0)
      duty_cycle = 0;
  }
  else {
    if (status->buttons & RWAND_BUTTON_UP)
      fudge_factor += 1;
    else if (status->buttons & RWAND_BUTTON_DOWN)
      fudge_factor -= 1;
    else
      return;
  }

  printf("col_width=%d  gap_width=%d  fudge_factor=%d\n", col_width, gap_width, fudge_factor);
}

void unstall(usb_dev_handle *d) {
  unsigned period = -1;
  int unstall_edges = 0;
  struct rwand_status status, last_status;

  printf("Possibly stalled, trying to restart...\n");

  read_rwand_status(d, &last_status);
  status = last_status;

  while (unstall_edges < 40) {
    if (period > 50000)
      period = 45000;
    period += 5;

    printf("period = %d, edges = %d\n", period, unstall_edges);
    control_write(d, RWAND_CTRL_SET_PERIOD, period, 0);
    update_coil_driver(d, &status, 0.25, 0.25);

    usleep(10000);

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

void rwand_write12(usb_dev_handle *d, unsigned char *buffer) {
  unsigned char pkt_data[9];

  /* This is nuts, and I have no idea why yet */
  pkt_data[8] = buffer[4];
  memcpy(pkt_data+1, buffer+5, 7);

  if (usb_control_msg(d, USB_TYPE_VENDOR | USB_ENDPOINT_OUT, RWAND_CTRL_SEQ_WRITE12,
		      buffer[0] | (buffer[1] << 8),
		      buffer[2] | (buffer[3] << 8),
		      pkt_data, 9, 100) < 0) {
    perror("usb_control_msg");
  }
}


void refresh_display(usb_dev_handle *d, unsigned char *columns) {
  /* Write out an 80x8 frame, given an 80-byte column array */
  int i;

  for (i=0; i<5; i++)
    rwand_write12(d, columns+(i*12));

  control_write(d, RWAND_CTRL_FLIP, 0,0);
}

int main(int argc, char **argv) {
  usb_dev_handle *d;
  struct rwand_status status, last_status;
  unsigned char frame[80];
  int flip_pending = 0;

  if (!(d = open_rwand()))
    return 1;

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

    /* The STALL_DETECT bit will turn off the coil and display automatically
     * if it's stalled. Run the unstall algorithm.
     */
    if (!(status.mode & RWAND_MODE_ENABLE_DISPLAY)) {
      control_write(d, RWAND_CTRL_SET_MODES, RWAND_MODE_ENABLE_COIL, 0);
      unstall(d);
      control_write(d, RWAND_CTRL_SET_MODES,
		    RWAND_MODE_ENABLE_SYNC |
		    RWAND_MODE_ENABLE_COIL |
		    RWAND_MODE_STALL_DETECT |
		    RWAND_MODE_ENABLE_DISPLAY, 0);
    }

    /* update coil driver phase */
    update_coil_driver(d, &status, 0.25, 0.2);

    /* Update display phase and column width */
    update_display_timing(d, &status, 0.5, 0.3);

    if (status.flip_count != last_status.flip_count)
      flip_pending = 0;

    /* Was the square just now pushed? */
    if ((status.buttons & RWAND_BUTTON_SQUARE) && !(last_status.buttons & RWAND_BUTTON_SQUARE)) {
      read_image(frame, "test-image.pgm");
      refresh_display(d, frame);
      control_write(d, RWAND_CTRL_FLIP, 0, 0);
      flip_pending = 1;
    }
    else if (!(status.buttons & RWAND_BUTTON_SQUARE)) {
      /* If our last page flip has finished, write another frame */
      if (!flip_pending) {
	static float t = 0;
	int i, y;
	for (i=0; i<80; i++) {
	  y = sin(t + i * 0.2) * 3.99 + 4;
	  frame[i] = 1<<y;
	}

	if (status.buttons & RWAND_BUTTON_LEFT)
	  t-=0.7;
	else if (status.buttons & RWAND_BUTTON_RIGHT)
	  t+=0.7;

	refresh_display(d, frame);
	control_write(d, RWAND_CTRL_FLIP, 0, 0);
	flip_pending = 1;
      }
    }
  }

  /* Turn it off */
  control_write(d, RWAND_CTRL_SET_MODES, 0, 0);
  return 0;
}
