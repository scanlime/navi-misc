/* Flashes an LED connected to RC2 (pin 13).
 * An example for GPIOs and pin descriptors.
 */

#include <rcpod.h>
#include <unistd.h>

int main() {
  rcpod_dev *rcpod = rcpod_InitSimple();
  rcpod_pin led;

  /* Define the LED pin once, then demonstrate pin descriptor manipulation
   * to turn its port into an output and toggle it on and off.
   */
  led = RCPOD_PIN_RC2;

  /* Make the pin an output */
  rcpod_GpioAssert(rcpod, RCPOD_OUTPUT(led));

  while (1) {
    /* In every iteration, negate the LED's current value and assert the new pin descriptor */
    led = RCPOD_NEGATE(led);
    rcpod_GpioAssert(rcpod, led);
    usleep(100000);
  }

  return 0;
}
