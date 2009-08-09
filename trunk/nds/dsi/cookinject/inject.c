/*
 * Payload for the 'cookinject' exploit.
 *
 * XXX: Work in progress. This isn't currently designed to do anything
 *      specific, it's just a playground for reverse-engineering the DSi.
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

extern void fifo_tx_trampoline(void);
extern void arm7_trampoline(void);

static void setupLogo();
static void fifoTX(u32 word);


void main()
{
   u32 old_ime = REG_IME;
   u32 old_ie = REG_IE;
   REG_IME = 0;
   REG_IE = 0;

   setupLogo();

   /*
    * Overwrite the player name from the in-memory saved game.
    * This prevents the game from crashing when it tries to access this in the future.
    */
   MAINRAM8[0x1798c0] = '\0';

   /* Hook the ARM9 FIFO transmit code */
   memcpy((void*)0x20D4a90, &fifo_tx_trampoline, 8);

   /*
    * Hook the ARM7 processor's main loop, via a call it makes into main RAM.
    *
    * XXX: This page of memory appears to be protected normally. Just reading
    *      from it will crash the ARM9!
    *
    * The protection range appears to start at 0x2F80000. In other words,
    * the last megabyte of the 16MB system RAM is not normally accessible.
    *
    * Normal memory protection settings on the DSi, according to the
    * CP15 registers.  The DSi seems to only use privileged mode, so
    * there are no permissions for user mode.
    *
    * Flag legend:
    *   UC = Uncached, no data/instruction read cache
    *   UB = Unbuffered, no data write buffer
    *   NR = No read or write access
    *   NX = Cannot execute from
    *   RO = Read-only
    *
    * Region  Addrs                     Special Flags     Description
    * -----------------------------------------------------------------------
    *
    *   0     0x04000000 - 0x05FFFFFF   UC UB             Memory-mapped I/O
    *   1     0x02000000 - 0x02FFFFFF                     System RAM (16MB)
    *   2     0x02F80000 - 0x02FBFFFF   UC UB NR          Protected memory
    *   3     0x08000000 - 0x09FFFFFF                     Unknown
    *   4     0x027C0000 - 0x027C1FFF   UC UB    NX       Protected memory
    *   5     0x01000000 - 0x017FFFFF   UC UB             Unknown
    *   6     0xFFFF0000 - 0xFFFF3FFF      UB RO          Unknown
    *   7     0x02FFC000 - 0x02FFDFFF   UC UB    NX       Protected memory
    *
    * XXX: not sure the size on region (2) makes sense?
    *
    * The protected memory in region (2) is ARM7 code, normally
    * unaccessable to the ARM9. Looks like this is lesser-used ARM7
    * code, like the Wifi stack and SPI support.
    *
    * Looks like there are still only 8 protection registers in CP15.
    */

   /*
    * Unprotect all memory regions.
    */
   asm volatile ("mcr p15, 0, %0, c5, c0, 0" :: "r" (0x0000FFFF));
   asm volatile ("mcr p15, 0, %0, c5, c0, 1" :: "r" (0x0000FFFF));
   asm volatile ("mcr p15, 0, %0, c5, c0, 2" :: "r" (0x33333333));
   asm volatile ("mcr p15, 0, %0, c5, c0, 3" :: "r" (0x33333333));

   /*
    * There is a function at 0x2FE0370 which is part of the SPI
    * support in the ARM7. Hook it. The hook will be invoked
    * when the game tries to load or save.
    */
   memcpy((void*)0x2fe0370, &arm7_trampoline, 8);

   /*
    * XXX: Trying to trigger the hook immediately, but this doesn't work yet.
    */
#if 0
   fifoTX(0x22B);
   fifoTX(0x26B);
   fifoTX(0x1AB);
   REG_IPC_SYNC = (REG_IPC_SYNC & 0xF) << 8;
#endif

   /* Give SPI over to the ARM7 */
   sysSetCardOwner(BUS_OWNER_ARM7);

   REG_IE = old_ie;
   REG_IME = old_ime;
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


#define ARM7
#include <nds/arm7/serial.h>

int writePowerManagement(int reg, int command) {
   // Write the register / access mode (bit 7 sets access mode)
  while (REG_SPICNT & SPI_BUSY);
  REG_SPICNT = SPI_ENABLE | SPI_BAUD_1MHZ | SPI_BYTE_MODE |
     SPI_CONTINUOUS | SPI_DEVICE_POWER;
  REG_SPIDATA = reg;

  // Write the command / start a read
  while (REG_SPICNT & SPI_BUSY);
  REG_SPICNT = SPI_ENABLE | SPI_BAUD_1MHZ | SPI_BYTE_MODE | SPI_DEVICE_POWER;
  REG_SPIDATA = command;

  // Read the result
  while (REG_SPICNT & SPI_BUSY);
  return REG_SPIDATA & 0xFF;
}


void arm7_hook()
{
   /*
    * Stack dump
    */
#if 0
   u32 sp;
   asm ("mov %0, sp" : "=r" (sp));
   spimeWrite(0, &sp, 4);
   spimeWrite(0x10, sp, 0x1000);
#endif


   writePowerManagement(0, 0xFF);
}

u32 fifo_tx_hook(u32 word)
{
   /*
    * Low nybble appears to be major opcode:
    *
    *   5: Camera
    *   6: ??? Occurrs very frequently, might be touchscreen?
    *   7: Sound
    *   8: ??? Doesn't seem to be important...
    *   B: SPI EEPROM
    *
    * Top byte might be flags? Bit 31 looks like it could be transfer
    * direction.
    */

   flash_line(word & 0x7F);

   /*
    * Buffer FIFO words to system memory first
    */

#if 0
   static struct {
      u32 data[0x4000];
      u32 count;
   } buf;

   if (buf.count < sizeof buf.data / sizeof buf.data[0]) {
      buf.data[buf.count++] = word;
   } else {
      spimeWrite(0x10000, (void*)buf.data, buf.count << 2);
   }
#endif

#if 0
   if ((word & 0xFF) == 0x55) {
      /*
       * Camera command
       */

      /* This breaks the camera */
      word ^= 0x100000;
   }
#endif

   return word;
}

static void fifoTX(u32 word)
{
   while (REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL);
   REG_IPC_FIFO_TX = word;
}

