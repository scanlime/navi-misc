#include <usb.h>
#include <math.h>
#include <unicone.h>
#include "device.h"

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
    theta += 0.1;

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

