#include <stdio.h>
#include <string.h>
#include "usb_driver.h"
#include "uart_driver.h"
#include "util.h"

/* We give SDCC from 0xF800 to 0xFCFF as general-purpose xdata,
 * the rest is available for buffers. Start at 0xFD00.
 */
volatile xdata at 0xFD00 unsigned char ep1_out_x[64];
unsigned char c;

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

signed char therm_read(unsigned char addr) {
  I2CADR = ((0x48 | addr) << 1);    /* Write to the therm */
  I2CSTA = ERR;                     /* Clear error flag, 100khz mode, no stop condition */
  i2c_write_byte(0x01, 0);          /* Write to CONFIG */
  i2c_write_byte(0x00, 1);
  i2c_write_byte(0x00, 1);          /* Start reading temperatures */
  return i2c_read_byte(1);
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

void isr() interrupt 1 using 1 {
}

int rand() {
  static int s = 1234567;
  s ^= 0xFFFFFFFF;
  s ^= s << 1;
  s ^= s << 3;
  s ^= s << 9;
  return s;
}

void main() {
  uart_init();
  puts("\n---- Startup ----");

  usb_init();
  puts("USB initialized");

  EX0 = 1;
  ET0 = 0;
  EX1 = 0;
  ET1 = 0;
  ES  = 0;
  EA  = 1;

  LED = 0;

  while (1) {
    dac_write(0, rand()); dac_write(1, rand());
    dac_sync();
  }

  /* Set up the first EP1 OUT transfer */
  usb_dma_unstall(EDB_OEP1);
  usb_dma_setup(EDB_OEP1, ep1_out_x, sizeof(ep1_out_x));

  while (1) {
    watchdog_reset();
    usb_poll();

    c = usb_dma_status(EDB_OEP1);
    if (c) {
      printf("Received %d bytes on EP1 OUT: %02X %02X %02X %02x...\n", c,
	     ep1_out_x[0], ep1_out_x[1], ep1_out_x[2], ep1_out_x[3]);
      usb_dma_setup(EDB_OEP1, ep1_out_x, sizeof(ep1_out_x));
    }
  }
}


void usb_handle_vendor_request() {
  static unsigned char v = 0;
  v++;

  printf("Vendor request %d\n", usb_setup_buffer.bRequest);

  switch (usb_setup_buffer.bRequest) {

    /* EP0 IN test */
  case 123:
    printf("Sending %d\n", v);
    usb_write_ep0_buffer(&v, 1);
    break;

    /* LED test, uses wValue but no data stage */
  case 13:
    if (usb_setup_buffer.wValue)
      LED = 0;
    else
      LED = 1;
    usb_write_ack();
    break;

  }
}

/* The End */
