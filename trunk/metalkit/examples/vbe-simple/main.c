/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * Metalkit example: A simplistic VESA BIOS graphics example.
 */

#include "vbe.h"
#include "console_vga.h"

int
main(void)
{
   ConsoleVGA_Init();
   VBE_InitSimple(800, 600, 16);

   uint16 *fb = gVBE.current.info.linearAddress;
   int i;

   for (i = 0; i < (800 * 600); i++) {
      fb[i] = i;
   }

   while (1) {
      for (i = 0; i < (800 * 600); i++) {
	 fb[i] = ~fb[i];
      }
   }

   return 0;
}
