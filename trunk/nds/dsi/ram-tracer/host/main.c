#include "fastftdi.h"
#include <stdio.h>


int main(int argc, char **argv)
{
  FTDIDevice dev;
  int err;

  err = FTDIDevice_Open(&dev);
  if (err)
    return 1;

  err = FPGAConfig_LoadFile(&dev, argv[1]);
  if (err)
    return 1;

  FTDIDevice_SetMode(&dev, FTDI_INTERFACE_A,
		     FTDI_BITMODE_SYNC_FIFO, 0xFF,
		     1000000);

  if (1) {
    uint8_t buf[256];
    int i;
    
    for (i = 0; i < 256; i++)
      buf[i] = 0;

    i = sizeof buf;
    libusb_bulk_transfer(dev.handle, 2, buf, sizeof buf, &i, 1000);
  }

  FTDIDevice_Close(&dev);
  
  return 0;
}
