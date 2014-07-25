#include <tusb.h>
#include <stdio.h>
#include <util.h>
#include "dac.h"

void i2c_write_byte(unsigned char b, bit is_last) {
  I2CADR &= ~1;       /* Write */

  if (is_last)
    I2CSTA |= SWR;    /* Stop condition */
  else
    I2CSTA &= ~SWR;   /* No stop condition */

  I2CDAO = b;
  while ((I2CSTA & TXE) == 0) {
    /* Waiting for the transmission.. check for errors */
    if (I2CSTA & ERR) {
      I2CSTA |= ERR;
      printf("I2C Error\n");
      return;
    }
    watchdog_reset();
  }
}

unsigned char i2c_read_byte(bit is_last) {
  I2CADR |= 1;        /* Read */

  if (is_last)
    I2CSTA |= SRD;    /* Stop condition */
  else
    I2CSTA &= ~SRD;   /* No stop condition */
  I2CDAO = 0;         /* Dummy byte starts the read transfer */

  while ((I2CSTA & RXF) == 0) {
    /* Waiting for the transmission.. check for errors */
    if (I2CSTA & ERR) {
      I2CSTA |= ERR;
      printf("I2C Error\n");
      return 0;
    }
    watchdog_reset();
  }
  return I2CDAI;
}

void dac_write(unsigned char addr, unsigned short value) {
  /* Write a value to the temporary register of the given DAC */
  I2CADR = ((0x4C | (addr<<1)) << 1);  /* Write to the DAC */
  I2CSTA = ERR | S1_4 | TIE;           /* Clear error flag, 400khz mode, no stop condition */
  i2c_write_byte(0x00, 0);             /* Control byte, load DAC with data */
  i2c_write_byte(value >> 8, 0);
  i2c_write_byte(value & 0xFF, 1);
}

void dac_sync() {
  /* Load the last values set with dac_write() into all DACs simultaneously */
  I2CADR = (0x48 << 1);                /* Write to the DACs' broadcast address */
  I2CSTA = ERR | S1_4 | TIE;           /* Clear error flag, 400khz mode, no stop condition */
  i2c_write_byte(0x30, 0);             /* Control byte, load DAC with data */
  i2c_write_byte(0, 0);
  i2c_write_byte(0, 1);
}

/* The End */
