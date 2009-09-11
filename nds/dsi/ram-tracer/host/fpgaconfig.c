/*
 * fpgaconfig.c - FPGA configuration for a Spartan 3E in Slave Parallel mode
 *                over an FT232H interface.
 *
 *   Pin assignment:
 *
 *    FT2232H     FPGA
 *    --------------------
 *    AD[7:0]     D[7:0]
 *    AC1/WRSTB#  CCLK
 *    BD0*        CSI
 *    BD1*        RDWR
 *    BD2*        PROG
 *    BD3*        DONE
 *
 *    * = Series ~1K resistor
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

#include "fpgaconfig.h"
#include <unistd.h>
#include <stdio.h>

#define CONFIG_BIT_RATE  100000   // Pretty much arbitrary...
#define PORTB_CSI_BIT    (1 << 0)
#define PORTB_RDWR_BIT   (1 << 1)
#define PORTB_PROG_BIT   (1 << 2)
#define PORTB_DONE_BIT   (1 << 3)


int
FPGAConfig_LoadFile(FTDIDevice *dev, const char *filename)
{
  int err = 0;
  uint8_t byte;
  FILE *file;

  /*
   * Make sure we can read the bitstream file...
   */

  file = fopen(filename, "rb");
  if (!file) {
    perror("FPGA: Error opening bitstream");
    return -1;
  }

  /*
   * Initialize the FTDI chip using both interfaces as bit-bang.
   * Interface A is a byte-wide parallel port for config data, and
   * interface B is GPIO for the control signals.
   */

  err = FTDIDevice_SetMode(dev, FTDI_INTERFACE_A,
			   FTDI_BITMODE_BITBANG, 0xFF,
			   CONFIG_BIT_RATE);
  if (err)
    goto done;

  err = FTDIDevice_SetMode(dev, FTDI_INTERFACE_B,
			   FTDI_BITMODE_BITBANG,
			   PORTB_CSI_BIT | PORTB_RDWR_BIT | PORTB_PROG_BIT,
			   CONFIG_BIT_RATE);
  if (err)
    goto done;

  /*
   * Begin configuration: Pulse PROG low.
   */

  fprintf(stderr, "FPGA: resetting...\n");

  err = FTDIDevice_WriteByteSync(dev, FTDI_INTERFACE_B,
				 PORTB_CSI_BIT | PORTB_RDWR_BIT | PORTB_PROG_BIT);
  if (err)
    goto done;

  err = FTDIDevice_WriteByteSync(dev, FTDI_INTERFACE_B,
				 PORTB_CSI_BIT | PORTB_RDWR_BIT);
  if (err)
    goto done;

  // Wait for the FPGA to initialize
  usleep(10000);

  // Into programming mode (CSI/RDWR low, PROG high)
  err = FTDIDevice_WriteByteSync(dev, FTDI_INTERFACE_B, PORTB_PROG_BIT);
  if (err)
    goto done;

  fprintf(stderr, "FPGA: sending configuration bitstream\n");

  // Make sure DONE is low now, for sanity.
  err = FTDIDevice_ReadByteSync(dev, FTDI_INTERFACE_B, &byte);
  if (err)
    goto done;
  if (byte & PORTB_DONE_BIT) {
    fprintf(stderr, "FPGA: DONE pin stuck high? (GPIO=%02x)\n", byte);
    //err = -1;
    //goto done;
  }

  /*
   * Send the bitstream from disk
   */

  while (!feof(file)) {
    uint8_t buffer[4096];
    size_t len = fread(buffer, 1, sizeof buffer, file);

    if (len > 0) {
      err = FTDIDevice_WriteSync(dev, FTDI_INTERFACE_A, buffer, len);
      if (err)
	goto done;
    }
  }

  /*
   * Did configuration succeed? Check the DONE pin.
   */

  err = FTDIDevice_ReadByteSync(dev, FTDI_INTERFACE_B, &byte);
  if (err)
    goto done;
  if (!(byte & PORTB_DONE_BIT)) {
    fprintf(stderr, "FPGA: Configuration error!\n");
    err = -1;
    goto done;
  }

  fprintf(stderr, "FPGA: configured\n");

 done:
  fclose(file);
  return err;
}
