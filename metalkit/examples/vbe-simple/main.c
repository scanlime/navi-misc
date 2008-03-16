/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * Metalkit example: A simplistic VESA BIOS graphics example.
 */

#include "vbe.h"

int
main(void)
{
   VBE_InitSimple(800, 600, 16);
   uint16 *fb = gVBE.current.info.linearAddress;
   int i;

   for (i = 0; i < (800 * 600); i++) {
      fb[i] = i;
   }

   return 0;
}
