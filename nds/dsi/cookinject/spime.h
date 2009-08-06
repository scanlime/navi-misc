/*
 * Thin library for accessing the FPGA SPI memory emulator.
 *
 * The emulator is a separate project, at:
 *   http://svn.navi.cx/misc/trunk/nds/spi-mem-emulator/
 *
 */

#ifndef SPIME_H
#define SPIME_H

void spimeRead(uint32 addr, uint8 *data, uint32 count);
void spimeWrite(uint32 addr, uint8 *data, uint32 count);

#endif /* SPIME_H */
