/*
 * Patch which writes to the DSi's NVRAM. Overwrites the whole
 * flash chip with 128k of data from 'nvram.bin'.
 * --Micah Dowty <micah@navi.cx>
 */

#define ARM7
#include <nds/arm7/serial.h>
#include "iohook.h"

asm(".pushsection .text.launcher_arm7_entry \n"
    "  b main \n"
    ".popsection \n");

static uint8_t
spiTransfer(uint8_t c)
{
   while (REG_SPICNT & SPI_BUSY);
   REG_SPIDATA = c;
   while (REG_SPICNT & SPI_BUSY);
   return REG_SPIDATA;
}

static void
spiAddress(uint32_t addr)
{
   spiTransfer(addr >> 16);
   spiTransfer(addr >> 8);
   spiTransfer(addr);
}

static void
spiBegin(void)
{
   REG_SPICNT = SPI_BAUD_1MHz | SPI_DEVICE_FIRMWARE | SPI_CONTINUOUS | SPI_ENABLE;
}

static void
spiEnd(void)
{
   REG_SPICNT = 0;
}

static uint8_t
spiReadByte(uint8_t cmd)
{
   uint8_t sr;
   spiBegin();
   spiTransfer(cmd);
   sr = spiTransfer(0);
   spiEnd();
   return sr;
}

static void
spiWriteEnable(void)
{
   // Set write enable
   spiBegin();
   spiTransfer(FIRMWARE_WREN);
   spiEnd();

   // Check to make sure write enable is set
   if (spiReadByte(FIRMWARE_RDSR) != 2) {
      IOHook_Quit("Write enable failed!");
   }
}

static void
spiWriteWait(void)
{
   // Poll for WIP==0
   while (spiReadByte(FIRMWARE_RDSR) & 1);
}

int
main()
{
   int address;

   IOHook_Init();
   IOHook_FOpenR("nvram.bin");

   // Check manufacturer ID
   if (spiReadByte(FIRMWARE_RDID) != 0x20) {
      IOHook_Quit("Bad JEDEC ID");
   }

   // Program one page (256 bytes) at a time.
   for (address = 0; address < 0x20000; address += 0x100) {
      const int pageSize = 256;
      uint32_t pageBuf[pageSize/4 + IOH_PAD32];
      uint8_t *bytes = (uint8_t*)pageBuf;
      int i;

      IOHook_FSeek(address);
      IOHook_FRead(pageBuf, pageSize);

      // Program the page using Page Write mode

      IOHook_LogStr("Programming page");
      IOHook_LogHex(&address, sizeof address);

      spiWriteWait();
      spiWriteEnable();
      spiBegin();
      spiTransfer(0x0A);
      spiAddress(address);
      for (i = 0; i < pageSize; i++)
         spiTransfer(bytes[i]);
      spiEnd();
      spiWriteWait();

      // Verify the page

      IOHook_LogStr("verifying...");

      spiBegin();
      spiTransfer(0x03);
      spiAddress(address);
      for (i = 0; i < pageSize; i++)
         if (spiTransfer(0) != bytes[i]) {
            IOHook_LogHex(&i, sizeof i);
            IOHook_Quit("Verify error");
         }
      spiEnd();
   }

   IOHook_Quit("Done!");
}
