#include <usb.h>
#include <math.h>
#include <stdio.h>
#include "../include/efs_protocol.h"

struct phase_info {
  int position;
  float average;
};

usb_dev_handle *open_efs(void) {
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
      if (dev->descriptor.idVendor == EFS_VENDOR_ID &&
	  dev->descriptor.idProduct == EFS_PRODUCT_ID) {
	return usb_open(dev);
      }
    }
  }
  printf("No device found\n");
  return NULL;
}

void set_param_byte(usb_dev_handle *d, int address, int value) {
  if (usb_control_msg(d, USB_TYPE_VENDOR, EFS_CTRL_SET_PARAM_BYTE,
		      value, address, NULL, 0, 500) < 0)
    perror("usb_control_msg");
}

int take_reading(usb_dev_handle *d) {
  unsigned char c;
  if (usb_control_msg(d, USB_TYPE_VENDOR | USB_ENDPOINT_IN, EFS_CTRL_TAKE_READING,
		      0, 0, &c, 1, 500) < 0)
    perror("usb_control_msg");
  return c;
}

float take_average_reading(usb_dev_handle *d, int samples) {
  /* Take 'samples' readings, and average them to
   * yield a floating point value.
   */
  int accumulator = 0;
  int i;
  for (i=0; i<samples; i++)
    accumulator += take_reading(d);
  return ((float)accumulator) / samples;
}

int tune_period(usb_dev_handle *d) {
  /* Search for the resonant period of the LC tank(s), using
   * current transmit and receive parameters.
   */
  int i, j;
  int max_value, max_i, reading;

  fprintf(stderr, "Finding the transmitters' resonant frequency...\n");
  i = -1;
  max_value = -1;
  for (i=215; i<235; i++) {
    set_param_byte(d, EFS_PARAM_PERIOD, i);

    /* At each period value, sample several phases */
    for (j=192; j<256; j+=8) {
      set_param_byte(d, EFS_PARAM_PHASE, j);
      reading = take_reading(d);
      if (reading > max_value) {
	max_value = reading;
	max_i = i;
      }
    }
  }
  return max_i;
}

void tune_phases(usb_dev_handle *d, struct phase_info *p) {
  /* After a proper period has been chosen, this searches
   * for local minima in the output as the phase is varied,
   * and generates a set of phase values for 90, 180, 270, and
   * 360 degrees. 0 degrees can't be measured because it occurs
   * before A/D sampling can begin. For each measured phase,
   * a phase_info structure is filled in with its EFS_PARAM_PHASE
   * value and it's average reading. This is used later to account
   * for the LC tank quenching after the square wave has been removed.
   */
  int i, extreme_i, cycle_period;
  float reading, extreme_value;
  const int samples = 15;
  const float epsilon = 7;

  fprintf(stderr, "Tuning receiver phase...\n");

  /* Start at 255. Note that EFS_PARAM_PHASE is a delay table
   * index, so smaller values generate longer delays. 255 is as
   * close as we can get to the instant the last state transition
   * on the transmitter output was made.
   *
   * Currently this assumes that we start out with a positive slope.
   * This is true for the transmitter parameters below. The slope
   * depends on several things, including whether EFS_PARAM_NUM_HALF_PERIODS
   * is even or odd, and the initial polarity given in EFS_PARAM_LC_PORT_INIT.
   *
   * Start by searching for the first local maximum. This is our
   * 90 degree mark.
   */
  i = 255;
  extreme_i = 255;
  extreme_value = 0;
  while (i > 0) {
    set_param_byte(d, EFS_PARAM_PHASE, i);
    reading = take_average_reading(d, samples);
    if (reading > extreme_value) {
      extreme_value = reading;
      extreme_i = i;
    }
    else {
      /* If we've fallen below the extreme by at least 'epsilon', stop searching */
      if (reading < (extreme_value - epsilon))
	break;
    }
    i--;
  }
  p[0].position = extreme_i;
  p[0].average = extreme_value;
  fprintf(stderr, " * found 90 degrees at %d, reading %f\n", extreme_i, extreme_value);

  /* Now, starting where we left off, look for the next local minumum.
   * That will be our 270 degree mark
   */
  i = extreme_i;
  extreme_value = 255;
  while (i > 0) {
    set_param_byte(d, EFS_PARAM_PHASE, i);
    reading = take_average_reading(d, samples);
    if (reading < extreme_value) {
      extreme_value = reading;
      extreme_i = i;
    }
    else {
      /* If we've risen above the extreme by at least 'epsilon', stop searching */
      if (reading > (extreme_value + epsilon))
	break;
    }
    i--;
  }
  p[2].position = extreme_i;
  p[2].average = extreme_value;
  fprintf(stderr, " * found 270 degrees at %d, reading %f\n", extreme_i, extreme_value);

  /* Now that we know the 90 and 270 degree locations, we can assume the frequency
   * is constant and infer the location of the 180 and 360 degree points. We do still have
   * to measure their average reading however.
   *
   * One side effect of this calculation is that we now know the period of our waveform
   * in CPU cycles (1/6 microseconds) and that's easy to convert to kilohertz.
   */
  cycle_period = (p[0].position - p[2].position) * 2;
  fprintf(stderr, " * assuming frequency of %.02f kHz\n", 6000.0 / cycle_period);

  p[1].position = p[0].position - cycle_period / 4;
  set_param_byte(d, EFS_PARAM_PHASE, p[1].position);
  p[1].average = take_average_reading(d, samples);
  fprintf(stderr, " * inferred 180 degrees at %d, reading %f\n", p[1].position, p[1].average);

  p[3].position = p[2].position - cycle_period / 4;
  set_param_byte(d, EFS_PARAM_PHASE, p[3].position);
  p[3].average = take_average_reading(d, samples);
  fprintf(stderr, " * inferred 360 degrees at %d, reading %f\n", p[3].position, p[3].average);
}

float demodulate(usb_dev_handle *d, struct phase_info *p) {
  /* Given the phase information found by tune_phases,
   * take four samples and calculate the amplitude at our
   * transmit frequency.
   */
  float readings[4];
  int i;
  float s, c;

  /* Calculate the approximate gain in the signal after 180 degrees phase difference.
   * This is used to counteract the LC oscillator's quenching during demodulation.
   */
  const float gain_180 = (p[2].average - p[3].average) / (p[1].average - p[0].average);

  for (i=0; i<4; i++) {
    set_param_byte(d, EFS_PARAM_PHASE, p[i].position);
    readings[i] = take_average_reading(d, 5);
  }

  s = readings[0] - readings[2] * gain_180;
  c = readings[1] - readings[3] * gain_180;

  return sqrt(s*s + c*c);
}

int main(int argc, char **argv) {
  usb_dev_handle *d = open_efs();
  struct phase_info p[4];
  if (!d)
    return 1;

  set_param_byte(d, EFS_PARAM_LC_PORT_XOR,      0x30);
  set_param_byte(d, EFS_PARAM_ADCON_INIT,       0x81);
  set_param_byte(d, EFS_PARAM_NUM_HALF_PERIODS, 30);
  set_param_byte(d, EFS_PARAM_LC_TRIS_INIT,     0x00);
  set_param_byte(d, EFS_PARAM_LC_PORT_INIT,     0x55);
  set_param_byte(d, EFS_PARAM_PERIOD,           tune_period(d));
  tune_phases(d, p);

  fprintf(stderr, "Demodulating...\n");
  while (1) {
    float x = demodulate(d, p);
    int i;
    for (i=0; i<x; i++)
      printf("*");
    printf("\n");
  }

  return 0;
}
