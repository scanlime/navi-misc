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

void unstall(usb_dev_handle *d) {
  unsigned period = -1;
  int unstall_edges = 0;
  struct prediction_status predicted, last_predicted;

  printf("Stalled. Trying to restart...\n");

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

int main(int argc, char **argv) {
  usb_dev_handle *d;
  struct prediction_status predicted, last_predicted;
  int last_period = 0;
  time_t last_edge_timestamp = time(NULL);
  memset(&predicted, 0, sizeof(predicted));

  if (!(d = open_rwand()))
    return 1;

  control_write(d, RWAND_CTRL_SET_MODES,
		RWAND_MODE_ENABLE_SYNC |
		RWAND_MODE_ENABLE_COIL |
		//RWAND_MODE_COIL_DEBUG |
		//RWAND_MODE_ENABLE_DISPLAY |
		0,
		0);

  while (1) {
    /* Read the current period prediction, calculate the frequency.
     * (the period is in units of 16 CPU cycles on a 6 MIPS processor)
     */
    last_predicted = predicted;
    read_prediction_status(d, &predicted);
    printf("Wand period: %d (%.02f Hz)\n", predicted.period,
	   1/(predicted.period * 16 / 6000000.0));

    /* Have we had any synchronization edges this time? */
    if (predicted.edge_count != last_predicted.edge_count)
      last_edge_timestamp = time(NULL);

    /* If it's been a while since we've seen a sync edge, conclude we're stalled */
    if (time(NULL) > last_edge_timestamp + 1) {
      unstall(d);
      last_edge_timestamp = time(NULL);
    }

    update_coil_driver(d, &predicted, 0.25, 0.2);

    control_write(d, RWAND_CTRL_SET_DISPLAY_PHASE, 100, 0x4FFF);
    control_write(d, RWAND_CTRL_SET_COLUMN_WIDTH, 100, 0);
    control_write(d, RWAND_CTRL_FLIP, 0,0);

    usleep(50000);
  }

  return 0;
}
