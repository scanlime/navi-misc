/*
 * Thin library for accessing the FPGA SPI memory emulator.
 *
 * The emulator is a separate project, at:
 *   http://svn.navi.cx/misc/trunk/nds/spi-mem-emulator/
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

#include <nds.h>
#include "spime.h"

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
