#include <usb.h>
#include <stdio.h>
#include <laser_protocol.h>
#include <math.h>


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

  ix = x * 0x200 + 0x200;
  iy = y * 0x200 + 0x200;
  if (ix < 0) ix = 0;
  if (ix > 0x3FF) ix = 0x3FF;
  if (iy < 0) iy = 0;
  if (iy > 0x3FF) iy = 0x3FF;

  if (duration < 1)
    duration = 1;
  if (duration > 255)
    duration = 0;

  control_write(d, LASER_CTRL_ENQUEUE,
		(((iy >> 2) & 0xFF) << 8) | ((ix >> 2) & 0xFF),
		(coil_enable ? 0x8000 : 0) |
		(laser_enable ? 0x4000 : 0) |
		((iy & 0x03) << 12) |
		((ix & 0x03) << 8) |
		duration);
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

  control_write(d, LASER_CTRL_SET_TIMEBASE, 55000, 0);

  while (1) {
    int l = 1;
    for (theta=0; theta < M_PI; theta += 0.2) {
      const float r = 0.2;
      const int s = 2;

      line(d, -r, -r,  r, -r, s, 10, 1);
      line(d,  r, -r,  r, -r, 1, 1, 0);

      line(d,  r, -r,  r,  r, s, 10, 1);
      line(d,  r,  r,  r,  r, 1, 1, 0);

      line(d,  r,  r, -r,  r, s, 10, 1);
      line(d, -r,  r, -r,  r, 1, 1, 0);

      line(d, -r,  r, -r, -r, s, 10, 1);
      line(d, -r, -r, -r, -r, 1, 1, 0);
    }
  }

  return 0;
}
