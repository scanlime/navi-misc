#include <usb.h>
#include <unicone.h>
#include "device.h"

int main()
{
  struct unicone_device* dev;
  int brightness;

  unicone_usb_init();
  dev = unicone_device_new();
  if (!dev)
    return 1;

  brightness = 0;
  while (1) {
    brightness += 128;

    printf("Brightness = 0x%04X\n", brightness);
    usb_control_msg(dev->usbdev, USB_TYPE_VENDOR, UNICONE_REQ_LED_BRIGHTNESS,
		    brightness, 0, NULL, 0, 1000);
  }

  return 0;
}

