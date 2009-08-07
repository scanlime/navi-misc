/*
 * Payload for the 'cookinject' exploit.
 *
 * This code is loaded from SPI memory by the bootstrapper in
 * 'exploit.s'.  Our main() is called. When it returns, we return
 * control to the game.
 *
 * Copyright (C) 2009 Micah Dowty <micah@navi.cx>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <string.h>
#include <nds.h>
#include "spime_nds.h"
#include "logo.h"

#define  DTCM             ((u8*)0x027c0000)
#define  ITCM             ((u8*)0x01ff8000)
#define  DTCM32(offset)   (*(u32*)(DTCM + (offset)))
#define  ITCM32(offset)   (*(u32*)(ITCM + (offset)))

extern void isr_trampoline(void);
extern void fifo_tx_trampoline(void);
extern uint32 isr_original;

static void setupLogo();


void main()
{
   setupLogo();

   /*
    * Overwrite the player name from the in-memory saved game.
    * This prevents the game from crashing when it tries to access this in the future.
    */
   MAINRAM8[0x1798c0] = '\0';

   /* Redirect the main interrupt vector through a trampoline that calls isr_hook() */
   isr_original = DTCM32(0x3FFC);
   DTCM32(0x3FFC) = (uint32) isr_trampoline;

   /* Hook the ARM9 FIFO transmit code */
   memcpy((void*)0x20D4a90, &fifo_tx_trampoline, 8);
}


void setupLogo()
{
   /*
    * The game already drew a background image on the top screen.
    * Replace it with our logo. It hasn't drawn the bottom screen
    * yet, so we can't do much about that right now.
    */
   decompress(logoPal, BG_PALETTE, RLEVram);
   decompress(logoBitmap, BG_GFX, RLEVram);

   /*
    * After we return, the game is going to try and display a
    * "Welcome" message on the screen. That message is mostly
    * garbage at this point, since it contained our player name
    * from the save file- and we'd rather it not scribble all
    * over our pretty logo. So stub out the beginning of the
    * buffer that holds the welcome message.
    */
   MAINRAM8[0x20c310] = '\0';

   /*
    * Make the logo visible immediately. We don't have to do this,
    * since the game will fade both screens in.  but it helps with
    * debugging.
    */
   REG_MASTER_BRIGHT = 0;
}

void flash_line(int y)
{
   static vu16 c[192];
   vu16 *a = BG_GFX_SUB + 128 * y;
   vu16 *b = a + 128;
   vu16 x, *p;

   x = ++c[y];
   for (p = a; p < b; p++) {
      *p = x;
   }
}

static vblank_hook()
{
   //   flash_line(0);
}

void isr_hook()
{
   if (REG_IF & 1) {
      vblank_hook();
   }
}

u32 fifo_tx_hook(u32 word)
{
   u8 cmd = word >> 24;
   flash_line(cmd & 0x7F);

   /*
    * Buffer FIFO words to system memory first
    */

#if 0
   static struct {
      u32 data[1024];
      u32 count;
   } buf;

   if (buf.count < 1024) {
      buf.data[buf.count++] = word;
   }
#endif

   /*
    * Flush when we get to a 'safe' command
    */

   if (word == 0x060000c0) {
      static u32 addr = 0x10000;

      spimeWrite(addr, &addr, 4);
      addr += 4;

      /*
      spimeWrite(addr, (void*)buf.data, buf.count << 2);
      addr += buf.count << 2;
      buf.count = 0;
      */
   }

   return word;
}
