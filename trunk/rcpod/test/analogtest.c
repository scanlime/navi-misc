/* Read all A/D converters, print their values to stdout */

#include <rcpod.h>
#include <stdio.h>

int main() {
  unsigned char buffer[8];
  int channel;
  rcpod_dev *rcpod = rcpod_InitSimple();

  while (1) {
    rcpod_AnalogReadAll(rcpod, buffer);

    for (channel=0; channel < sizeof(buffer); channel++)
      printf("%d ", buffer[channel]);
    printf("\n");
  }

  return 0;
}
