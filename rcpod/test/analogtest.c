/* Read all A/D converters, print their values to stdout */

#include <rcpod.h>
#include <stdio.h>

int main() {
  unsigned char buffer[8];
  int i;
  rcpod_dev *rcpod = rcpod_InitSimple();

  while (1) {
    rcpod_AnalogReadAll(rcpod, buffer);
    for (i=0; i < sizeof(buffer); i++)
      printf("%d ", buffer[i]);
    printf("\n");
  }

  return 0;
}
