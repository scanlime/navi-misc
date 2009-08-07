/*
 * Thin library for accessing the FPGA SPI memory
 * emulator from the Nintendo DS.
 */

#ifndef SPIME_H
#define SPIME_H

void spimeRead(uint32 addr, uint8 *data, uint32 count);
void spimeWrite(uint32 addr, uint8 *data, uint32 count);

#endif /* SPIME_H */
