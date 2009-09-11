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
  if (err) {
    fprintf(stderr, "FPGA: err=%d\n", err);
  }
  return 0;


  //  FTDIDevice_SetBitmode(&dev, FTDI_BITMODE_SYNC_FIFO);

  FTDIDevice_SetMode(&dev, FTDI_INTERFACE_A,
		     FTDI_BITMODE_SYNC_FIFO, 0xFF,
		     1000000);
  
  /*
  FTDIDevice_SetMode(&dev, FTDI_INTERFACE_A,
		     FTDI_BITMODE_BITBANG, 0xFF,
		     1000000);
  */

  while (1) {
    uint8_t buf[256];
    int i;
    
    for (i = 0; i < 256; i++)
      buf[i] = 0;

    i = sizeof buf;
    if (libusb_bulk_transfer(dev.handle, 2, buf, sizeof buf, &i, 1000))
      break;
  }

  FTDIDevice_Close(&dev);
  
  return 0;
}
