/*
 * Demonstrate the LED brightness control device
 */

#include <usb.h>
#include <sys/time.h>
#include <math.h>
#include <unicone.h>
#include "device.h"
#include "progress.h"

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
  struct progress_reporter* progress = progress_reporter_console_new();
  float theta, brightness;
  int i_brightness;
  unsigned char led_data[2];

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

  theta = 0;
  while (1) {
    theta += delta_time() * 3.0;
    theta = fmod(theta, M_PI * 2);

    brightness = sin(theta) * 0.4 + 0.5;
    brightness = pow(brightness, 1.8);
    i_brightness = (int)(brightness * 0xFFFF + 0.5);

    led_data[0] = i_brightness >> 8;
    led_data[1] = i_brightness & 0xFF;

    if (unicone_device_i2c_write(dev, UNICONE_I2C_LED, led_data, sizeof(led_data)) < 0) {
      perror("unicone_device_i2c_write");
      return 1;
    }
  }

  return 0;
}

