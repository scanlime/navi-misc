/* Read all A/D converters, print their values to stdout */

#include <rcpod.h>
#include <stdio.h>

int main() {
  unsigned char buffer[8];
  int i;
  rcpod_dev *rcpod = rcpod_InitSimple();

  while (1) {
    /* Use CmdAnalogAll */
    rcpod_CmdAnalogAll(rcpod, buffer);
    for (i=0; i < sizeof(buffer); i++)
      printf("%d ", buffer[i]);

#if 0
    printf("\t\t");

    /* Sample it ourselves */
    for (i=0; i < sizeof(buffer); i++) {
      rcpod_CmdPoke(rcpod, RCPOD_REG_ADCON0, 0x81 | (i << 3));   /* Select channel */
      rcpod_CmdPoke(rcpod, RCPOD_REG_ADCON0, 0x85 | (i << 3));   /* Start conversion */
      printf("%d ", rcpod_CmdPeek(rcpod, RCPOD_REG_ADRES));
    }
#endif

    printf("\n");
  }

  return 0;
}
