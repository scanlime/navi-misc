/* Display a hex dump of the rcpod's entire address space */

#include <rcpod.h>
#include <stdio.h>
#include <ctype.h>

int main() {
  int lineAddr, i;
  unsigned char lineBuffer[16];
  rcpod_dev *rcpod = rcpod_InitSimpleWithoutReset();

  /* Simple hex dump, hardcoded to 16 bytes per line
   * and without support for partial lines.
   */
  for (lineAddr=0; lineAddr < RCPOD_MEM_SIZE; lineAddr+=16) {
    printf("%04X:  ", lineAddr);

    /* Fill the line buffer */
    rcpod_CmdPeek8(rcpod, lineAddr, lineBuffer);
    rcpod_CmdPeek8(rcpod, lineAddr+8, lineBuffer+8);

    /* Hex values for this line */
    for (i=0; i<16; i++)
      printf("%02X ", lineBuffer[i]);

    printf(" ");

    /* ASCII values for this line */
    for (i=0; i<16; i++)
      printf("%c", isprint(lineBuffer[i]) ? lineBuffer[i] : '.');

    printf("\n");
  }

  return 0;
}
