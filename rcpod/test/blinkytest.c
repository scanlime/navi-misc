#include <usb.h>

int rcpod_poke(usb_dev_handle *rcpod, int address, int data);
usb_dev_handle *rcpod_open(void);

int main() {
  usb_dev_handle *rcpod;
  int r;

  rcpod = rcpod_open();
  if (!rcpod) {
    printf("No rcpod device found\n");
    return 1;
  }

  /* Make PORTC all outputs */
  r = rcpod_poke(rcpod, 0x87, 0x00);
  if (r < 0) {
    perror("poke");
    return 1;
  }

  /* Flash PORTC */
  while (1) {
    r = rcpod_poke(rcpod, 0x07, 0xFF);
    if (r < 0) {
      perror("poke");
      return 1;
    }

    usleep(50000);

    r = rcpod_poke(rcpod, 0x07, 0x00);
    if (r < 0) {
      perror("poke");
      return 1;
    }

    usleep(50000);
  }

  return 0;
}
