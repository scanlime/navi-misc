#include <usb.h>
#include <sys/time.h>
#include <math.h>
#include <unicone.h>
#include "device.h"

float delta_time() {
  /* Get the time, in seconds, since the last call to this function */

  static struct timeval then;
  static int then_valid = 0;
  struct timeval now;
  float delta;

  gettimeofday(&now, NULL);

  if (then_valid)
    delta = (now.tv_sec - then.tv_sec) +
            (now.tv_usec - then.tv_usec) / 1000000.0;
  else
    delta = 0;

  then = now;
  then_valid = 1;
  return delta;
}


int main()
{
  struct unicone_device* dev;
  float theta, brightness;

  unicone_usb_init();
  dev = unicone_device_new();
  if (!dev)
    return 1;

  theta = 0;
  while (1) {
    theta += delta_time() * 3.0;

    brightness = sin(theta) * 0.4 + 0.5;
    brightness = pow(brightness, 1.8);

    if (usb_control_msg(dev->usbdev, USB_TYPE_VENDOR, UNICONE_REQ_LED_BRIGHTNESS,
			(int)(brightness * 0xFFFF + 0.5), 0, NULL, 0, 1000) < 0) {
      perror("usb_control_msg");
      return 1;
    }
  }

  return 0;
}

