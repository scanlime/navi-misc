/* Flashes LEDs connected to RC1 and RC2
 * (such as the LEDs installed on the rcpod-485 board)
 *
 * An example for GPIOs and pin descriptors.
 */

#include <rcpod.h>
#include <unistd.h>

int main() {
  rcpod_dev *rcpod = rcpod_InitSimple();
  rcpod_pin leds[2];

  /* Define the LED pin once, then demonstrate pin descriptor manipulation
   * to turn its port into an output and toggle it on and off.
   */
  leds[0] = RCPOD_PIN_RC1;
  leds[1] = RCPOD_NEGATE(RCPOD_PIN_RC2);

  /* Make the pins outputs */
  rcpod_GpioAssert(rcpod, RCPOD_OUTPUT(leds[0]));
  rcpod_GpioAssert(rcpod, RCPOD_OUTPUT(leds[1]));

  while (1) {
    /* In every iteration, negate the LED's current value and assert the new pin descriptors */
    leds[0] = RCPOD_NEGATE(leds[0]);
    leds[1] = RCPOD_NEGATE(leds[1]);

    /* Multiple pin descriptors can be asserted at once, up to 4x faster */
    rcpod_GpioAssertBuffer(rcpod, leds, 2);

    usleep(200000);
  }

  return 0;
}
