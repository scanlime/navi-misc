#include <usb.h>
#include <stdio.h>
#include <rwand_protocol.h>
#include <time.h>

struct prediction_status {
  unsigned short period;
  unsigned short phase;
  unsigned char edge_count;
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


void read_prediction_status(usb_dev_handle *d, struct prediction_status *status) {
  unsigned char packet[5];
  control_read(d, RWAND_CTRL_READ_PREDICTION, packet, sizeof(packet));
  status->period = packet[0] + (packet[1] << 8);
  status->phase = packet[2] + (packet[3] << 8);
  status->edge_count = packet[4];
}

void update_coil_driver(usb_dev_handle *d, struct prediction_status *status,
			float pulse_center, float pulse_width) {
  control_write(d, RWAND_CTRL_SET_COIL_PHASE,
		status->period * (pulse_center - pulse_width),
		status->period * (pulse_center + pulse_width));
}

void update_display_timing(usb_dev_handle *d, struct prediction_status *status,
			   float center, float width) {
  int col_width = (status->period * (width/2)) / 80;

  control_write(d, RWAND_CTRL_SET_COLUMN_WIDTH, col_width, 0);
  control_write(d, RWAND_CTRL_SET_DISPLAY_PHASE,
		status->period * 0.5 * (center - width/2),
		status->period * 0.5 * (1 + center - width/2) - col_width*0.96);
}

void unstall(usb_dev_handle *d) {
  unsigned period = -1;
  int unstall_edges = 0;
  struct prediction_status predicted, last_predicted;

  printf("Possibly stalled, trying to restart...\n");

  read_prediction_status(d, &last_predicted);
  predicted = last_predicted;

  while (unstall_edges < 100) {
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
    update_coil_driver(d, &predicted, 0.25, 0.25);

    usleep(1000000);

    last_predicted = predicted;
    read_prediction_status(d, &predicted);
    unstall_edges += (predicted.edge_count - last_predicted.edge_count) & 0xFF;
  }
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
  for (i=0; i<80; i+=3) {
    if (i+3 >= 80)
      i = 77;
    control_write(d, RWAND_CTRL_RANDOM_WRITE3, i | (columns[i] << 8),
		  columns[i+1] | (columns[i+2] << 8));
  }
  control_write(d, RWAND_CTRL_FLIP, 0,0);
}

int main(int argc, char **argv) {
  usb_dev_handle *d;
  struct prediction_status predicted, last_predicted;
  int last_period = 0;
  time_t last_edge_timestamp;
  unsigned char frame[80];

  if (!(d = open_rwand()))
    return 1;

  read_image(frame, "test-image.pgm");
  refresh_display(d, frame);

  /* start out stalled, so we can verify our current status */
  last_edge_timestamp = 0;
  read_prediction_status(d, &predicted);

  while (1) {
    /* Read the current period prediction, calculate the frequency.
     * (the period is in units of 16 CPU cycles on a 6 MIPS processor)
     */
    last_predicted = predicted;
    read_prediction_status(d, &predicted);
    printf("%.02f Hz    Buttons: 0x%02X\n",
	   1/(predicted.period * 16 / 6000000.0),
	   control_read_byte(d, RWAND_CTRL_CHECK_BUTTONS));

    /* Have we had any synchronization edges this time? */
    if (predicted.edge_count != last_predicted.edge_count)
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
    update_coil_driver(d, &predicted, 0.25, 0.2);

    /* Update display phase and column width */
    update_display_timing(d, &predicted, 0.5, 0.75);

    /* If our last page flip has finished, write another frame */
    if (!control_read_byte(d, RWAND_CTRL_CHECK_FLIP)) {
      static float t = 0;
      int i, y;
      for (i=0; i<80; i++) {
	y = sin(t + i * 0.2) * 3.99 + 4;
	frame[i] = 1<<y;
      }
      t+=0.4;
      refresh_display(d, frame);
      control_write(d, RWAND_CTRL_FLIP, 0, 0);
    }
  }

  return 0;
}
