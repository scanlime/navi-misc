#include <usb.h>
#include <stdio.h>
#include <SDL.h>
#include "../include/efs_protocol.h"

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

int tune_phase(usb_dev_handle *d) {
  /* After a proper period has been chosen, this searches
   * for the first local maximum in the output as the phase is varied.
   * This will be at 90 degrees on the sine wave.
   */
  int i, max_i;
  float reading, max_value;
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
   */
  i = 255;
  max_i = 255;
  max_value = 0;
  while (i > 0) {
    set_param_byte(d, EFS_PARAM_PHASE, i);
    reading = take_average_reading(d, samples);
    if (reading > max_value) {
      max_value = reading;
      max_i = i;
    }
    else {
      /* If we've fallen below the extreme by at least 'epsilon', stop searching */
      if (reading < (max_value - epsilon))
	break;
    }
    i--;
  }
  return max_i;
}

int main(int argc, char **argv) {
  struct {
    int port_xor;
    int adcon;
    int period;
    int phase;
    float base;
  } channels[] = {
    {0x03, 0x81, 0, 0},
    {0x0C, 0x81, 0, 0},
    {0x30, 0x81, 0, 0},
    {0xC0, 0x81, 0, 0},
    {0x03, 0x89, 0, 0},
    {0x0C, 0x89, 0, 0},
    {0x30, 0x89, 0, 0},
    {0xC0, 0x89, 0, 0},
  };

  SDL_Surface *screen;
  SDL_Event event;
  SDL_Rect r;
  int i;
  float x;
  usb_dev_handle *d = open_efs();
  if (!d)
    return 1;

  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(640, 480, 0, 0);

  set_param_byte(d, EFS_PARAM_NUM_HALF_PERIODS, 30);
  set_param_byte(d, EFS_PARAM_LC_TRIS_INIT,     0x00);
  set_param_byte(d, EFS_PARAM_LC_PORT_INIT,     0x55);

  for (i=0; i<8; i++) {
    fprintf(stderr, "Optimizing channel %d...\n", i);
    set_param_byte(d, EFS_PARAM_LC_PORT_XOR, channels[i].port_xor);
    set_param_byte(d, EFS_PARAM_ADCON_INIT,  channels[i].adcon);
    channels[i].period = tune_period(d);
    set_param_byte(d, EFS_PARAM_PERIOD,      channels[i].period);
    channels[i].phase  = tune_phase(d);
    set_param_byte(d, EFS_PARAM_PHASE,       channels[i].phase);
    channels[i].base = take_average_reading(d, 30);
  }

  fprintf(stderr, "Running\n");

  while (1) {
    SDL_FillRect(screen, NULL, 0x000000);

    for (i=0; i<8; i++) {
      set_param_byte(d, EFS_PARAM_LC_PORT_XOR, channels[i].port_xor);
      set_param_byte(d, EFS_PARAM_ADCON_INIT,  channels[i].adcon);
      set_param_byte(d, EFS_PARAM_PERIOD,      channels[i].period);
      set_param_byte(d, EFS_PARAM_PHASE,       channels[i].phase);

      x = take_reading(d);
      r.x = i * 40;
      r.y = screen->h * (0.5 + (x - channels[i].base) * 0.01);
      r.w = 30;
      r.h = screen->h - r.y;
      SDL_FillRect(screen, &r, 0x8080FF);
    }

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
	SDL_Quit();
	return 0;
      }
    }
    SDL_Flip(screen);
  }
}
