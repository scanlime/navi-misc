/* Flashes an LED connected to RC2 (pin 13) */

#include <rcpod.h>
#include <unistd.h>

int main() {
  rcpod_dev *rcpod = rcpod_InitSimple();
  rcpod_pin pins[] = {0,0,0,0};

  printf("%d\n", RCPOD_PIN(0, 1, 3, 2));

  printf("%d\n", rcpod_CmdGpioRead(rcpod, RCPOD_PIN(1, 1, 3, 2)));

  /* Make the pin an output */
  pins[0] = RCPOD_PIN(0, 1, 3, 2);
  rcpod_CmdGpioAssert(rcpod, pins);

  printf("%d\n", rcpod_CmdGpioRead(rcpod, RCPOD_PIN(1, 1, 3, 2)));

  while (1) {
    pins[0] = RCPOD_PIN(0, 0, 3, 2);
    rcpod_CmdGpioAssert(rcpod, pins);
    usleep(50000);

    pins[0] = RCPOD_PIN(1, 0, 3, 2);
    rcpod_CmdGpioAssert(rcpod, pins);
    usleep(50000);
  }

  return 0;
}
