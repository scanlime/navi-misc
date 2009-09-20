/*
 * Patch which dumps out the DSi's NVRAM. Writes data to 'nvram.bin'.
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
   REG_SPIDATA = FIRMWARE_READ;
   while (REG_SPICNT & SPI_BUSY);
   return REG_SPIDATA;
}

int
main()
{
   const int blockSize = 16;
   uint32_t buffer[IOH_PACKET_LEN/4];
   uint8_t *buf8 = (uint8_t*)buffer;
   int bufIndex, address;

   IOHook_Init();
   IOHook_FOpenW("nvram.bin");

   REG_SPICNT = SPI_BAUD_1MHz | SPI_DEVICE_FIRMWARE | SPI_CONTINUOUS | SPI_ENABLE;

   spiTransfer(FIRMWARE_READ);
   spiTransfer(0);
   spiTransfer(0);
   spiTransfer(0);

   for (address = 0; address < 0x20000;) {
      for (bufIndex = 0; bufIndex < blockSize; bufIndex++) {
         buf8[bufIndex] = spiTransfer(0);
         address++;
      }
      IOHook_FWrite(buffer, blockSize);
   }

   REG_SPICNT = 0;
   IOHook_Quit("Done!");
}
