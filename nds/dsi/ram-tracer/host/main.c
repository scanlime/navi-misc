#include "fastftdi.h"
#include <stdio.h>


FILE *outputFile;


static int
readCallback(uint8_t *buffer, int length, FTDIProgressInfo *progress, void *userdata)
{
   if (length)
      fwrite(buffer, length, 1, outputFile);

   if (progress)
      fprintf(stderr, "  %3d:%02d [ %9.3f MB captured ] %7.1f kB/s current, "
              "%7.1f kB/s average\r",
              (int)progress->totalTime / 60, (int)progress->totalTime % 60,
              progress->current.totalBytes / (1024.0 * 1024.0),
              progress->currentRate / 1024.0,
              progress->totalRate / 1024.0);

   return 0;
}


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

  outputFile = fopen(argv[2], "wb");
  if (!outputFile) {
     perror("opening output file");
     return 1;
  }

  err = FTDIDevice_SetMode(&dev, FTDI_INTERFACE_A,
                           FTDI_BITMODE_SYNC_FIFO, 0xFF, 0);
  if (err)
    return 1;

  err = FTDIDevice_ReadStream(&dev, FTDI_INTERFACE_A, readCallback, NULL, 64, 16);
  if (err < 0)
     return 1;

  FTDIDevice_Close(&dev);
  fclose(outputFile);

  return 0;
}
