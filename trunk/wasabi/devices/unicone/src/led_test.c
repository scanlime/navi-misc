/*
 * Demonstrate LED brightness control
 */

#include <unicone.h>
#include "unicone_device.h"
#include "progress.h"

int main()
{
  struct unicone_device* dev;
  struct progress_reporter* progress = progress_reporter_console_new();

  unicone_usb_init();
  dev = unicone_device_new();
  if (!dev) {
    printf("Can't open unicone device\n");
    return 1;
  }

  if (unicone_device_configure(dev, "firmware.bin", "fpga/test/test.bit", progress) < 0) {
    printf("Error configuring unicone device\n");
    return 1;
  }

  while (1) {
    unicone_device_set_led(dev, 1, 0.015);
    sleep(1);
  }

  return 0;
}

