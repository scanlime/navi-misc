#include "fastftdi.h"
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

FILE *outputFile;
bool exitRequested;


static bool
detectOverrun(uint8_t *buffer, int length)
{
   while (length) {
      if (*buffer != 0xFF)
         return false;
      buffer++;
      length--;
   }
   return true;
}


static int
readCallback(uint8_t *buffer, int length, FTDIProgressInfo *progress, void *userdata)
{
   if (length) {
      /*
       * We don't actually parse the received data, but do check for
       * buffer overflows.  If the hardware buffer overruns, the FPGA
       * will start sending 0xFF bytes as fast as it can.
       */
      if (detectOverrun(buffer, length)) {
         fprintf(stderr,
                 "\n\n"
                 "*** Hardware buffer overrun! ***\n"
                 "The USB bus or PC can't keep up with the incoming "
                 "data. Capture has been aborted.\n"
                 "\n");
         return 1;
      }

      if (fwrite(buffer, length, 1, outputFile) != 1) {
         perror("Write error");
         return 1;
      }
   }

   if (progress)
      fprintf(stderr, "  %3d:%02d [ %9.3f MB captured ] %7.1f kB/s current, "
              "%7.1f kB/s average\r",
              (int)progress->totalTime / 60, (int)progress->totalTime % 60,
              progress->current.totalBytes / (1024.0 * 1024.0),
              progress->currentRate / 1024.0,
              progress->totalRate / 1024.0);

   return exitRequested ? 1 : 0;
}


static void
sigintHandler(int signum)
{
   exitRequested = true;
}


int main(int argc, char **argv)
{
  FTDIDevice dev;
  int err;

  err = FTDIDevice_Open(&dev);
  if (err) {
     fprintf(stderr, "USB: Error opening device\n");
     return 1;
  }

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

  signal(SIGINT, sigintHandler);

  err = FTDIDevice_ReadStream(&dev, FTDI_INTERFACE_A, readCallback, NULL, 64, 16);
  if (err < 0 && !exitRequested)
     return 1;

  FTDIDevice_Close(&dev);
  fclose(outputFile);

  fprintf(stderr, "\nCapture ended.\n");

  return 0;
}
