#include <usb.h>
#include <stdio.h>
#include <math.h>

#define LASER_VENDOR_ID   0xE461
#define LASER_PRODUCT_ID  0x0006


usb_dev_handle *open_laser(void) {
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
      if (dev->descriptor.idVendor == LASER_VENDOR_ID &&
	  dev->descriptor.idProduct == LASER_PRODUCT_ID) {
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

void enqueue(usb_dev_handle *d, float x, float y, int duration, int coil_enable, int laser_enable) {
  int ix, iy;
  unsigned char buffer[64];

  ix = x * 0x8000 + 0x8000;
  iy = y * 0x8000 + 0x8000;
  if (ix < 0) ix = 0;
  if (ix > 0xFFFF) ix = 0xFFFF;
  if (iy < 0) iy = 0;
  if (iy > 0xFFFF) iy = 0xFFFF;

  if (duration < 1)
    duration = 1;
  if (duration > 255)
    duration = 0;

  buffer[0] = ix & 0xFF;
  buffer[1] = ix >> 8;
  buffer[2] = iy & 0xFF;
  buffer[3] = iy >> 8;

  for (ix=0; ix<64; ix++)
    buffer[ix] = rand();

  if (usb_bulk_write(d, 1, buffer, sizeof(buffer), 1000) < 0) {
    perror("usb_bulk_write");
    exit(1);
  }
}

void line(usb_dev_handle *d, float x1, float y1, float x2, float y2,
	  int steps, int step_duration, int laser_enable) {
  float alpha;
  for (alpha=0; alpha<=1; alpha+=1.0/steps)
    enqueue(d, x1 + (x2-x1)*alpha, y1 + (y2-y1)*alpha, step_duration, 1, laser_enable);
}

int main(int argc, char **argv) {
  usb_dev_handle *d;
  float theta;
  float x, y;
  int ix, iy, coil_enable, laser_enable;

  if (!(d = open_laser())) {
    printf("Error opening device\n");
    return 1;
  }

  //  control_write(d, LASER_CTRL_SET_TIMEBASE, 65000, 0);

  while (1) {
    const float r = 0.3;
    const int steps = 10;
    const int stepsize = 2;

    line(d, -r, -r,  r, -r, steps, stepsize, 1);
    line(d,  r, -r,  r, -r, 1, 1, 0);

    line(d,  r, -r,  r,  r, steps, stepsize, 1);
    line(d,  r,  r,  r,  r, 1, 1, 0);

    line(d,  r,  r, -r,  r, steps, stepsize, 1);
    line(d, -r,  r, -r,  r, 1, 1, 0);

    line(d, -r,  r, -r, -r, steps, stepsize, 1);
    line(d, -r, -r, -r, -r, 1, 1, 0);
  }

  return 0;
}
