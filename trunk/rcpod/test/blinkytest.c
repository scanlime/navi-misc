/* Toggles all of PORTC on and off, blinking an LED connected to it */

#include <rcpod.h>
#include <unistd.h>

int main() {
  rcpod_dev *rcpod = rcpod_InitSimple();

  /* Make PORTC all outputs */
  rcpod_Poke(rcpod, RCPOD_REG_TRISC, 0x00);

  /* Flash PORTC */
  while (1) {
    rcpod_Poke(rcpod, RCPOD_REG_PORTC, 0xFF);
    usleep(50000);
    rcpod_Poke(rcpod, RCPOD_REG_PORTC, 0x00);
    usleep(50000);
  }

  return 0;
}
