/* Toggles all of PORTC on and off, blinking an LED connected to it */

#include <rcpod.h>
#include <unistd.h>

int main() {
  rcpod_dev *rcpod = rcpod_InitSimple();

  /* Make PORTC all outputs */
  rcpod_Poke(rcpod, 0x87, 0x00);

  /* Flash PORTC */
  while (1) {
    rcpod_Poke(rcpod, 0x07, 0xFF);
    usleep(50000);
    rcpod_Poke(rcpod, 0x07, 0x00);
    usleep(50000);
  }

  return 0;
}
