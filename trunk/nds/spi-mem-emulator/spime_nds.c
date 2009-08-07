/*
 * Thin library for accessing the FPGA SPI memory
 * emulator from the Nintendo DS.
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <nds.h>
#include "spime_nds.h"

/*
 * Using the naming conventions from http://nocash.emubase.de/gbatek.htm
 */

#define REG_AUXSPICNT  (*(vu16*)0x40001A0)
#define REG_AUXSPIDATA (*(vu16*)0x40001A2)

#define SPICNT_HOLD_CS    (1 << 6)
#define SPICNT_BUSY       (1 << 7)
#define SPICNT_SPI_MODE   (1 << 13)
#define SPICNT_SLOT_EN    (1 << 15)

/*
 * 20-bit memory read/write commands, special for our emulator.
 */

#define SPIME_CMD_WRITE20   0xA00000
#define SPIME_CMD_READ20    0xB00000


static inline void
spiWait()
{
   while (REG_AUXSPICNT & SPICNT_BUSY);
}

static inline void
spiInit()
{
   sysSetCardOwner(BUS_OWNER_ARM9);
}

static inline void
spiBeginTransfer()
{
   REG_AUXSPICNT = SPICNT_HOLD_CS | SPICNT_SPI_MODE | SPICNT_SLOT_EN;
}

static inline void
spiLastByte()
{
   REG_AUXSPICNT = SPICNT_SPI_MODE | SPICNT_SLOT_EN;
}

static inline void
spiEndTransfer()
{
   REG_AUXSPICNT = 0;
}

static void
spimeSendCommand(uint32 cmd, uint32 addr)
{
   addr = (addr & 0xFFFFF) | cmd;

   REG_AUXSPIDATA = (uint8)(addr >> 16);
   spiWait();
   REG_AUXSPIDATA = (uint8)(addr >> 8);
   spiWait();
   REG_AUXSPIDATA = (uint8)addr;
   spiWait();
}

void
spimeRead(uint32 addr, uint8 *data, uint32 count)
{
   spiInit();
   spiBeginTransfer();
   spimeSendCommand(SPIME_CMD_READ20, addr);

   while (count--) {

      if (!count)
         spiLastByte();

      REG_AUXSPIDATA = 0;
      spiWait();
      *(data++) = REG_AUXSPIDATA;
   }

   spiEndTransfer();
}

void
spimeWrite(uint32 addr, uint8 *data, uint32 count)
{
   spiInit();
   spiBeginTransfer();
   spimeSendCommand(SPIME_CMD_WRITE20, addr);

   while (count--) {

      if (!count)
         spiLastByte();

      REG_AUXSPIDATA = *(data++);
      spiWait();
   }

   spiEndTransfer();
}
