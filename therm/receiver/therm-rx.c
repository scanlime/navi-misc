/*
 * Firmware for the Therm receiver. This runs on a TUSB3410
 * microcontroller (modified Keyspan USA-19HS). A Laipac 315 MHz
 * ASK receiver is connected to our UART, and the I2C bus has a
 * local temperature sensor (TC74) and a MAX7300 driving the
 * front panel LEDs.
 *
 * This firmware handles most levels of the protocol. Raw bytes
 * received over the UART are converted into bits with a measure
 * of line noise, then packet framing is detected and verified.
 * Complete packets are buffered and sent back to the host.
 *
 * --Micah Dowty <micah@navi.cx>
 */

#include <stdio.h>
#include <string.h>
#include <usb_driver.h>
#include <i2c_driver.h>
#include <uart_driver.h>
#include <util.h>

/* We give SDCC from 0xF800 to 0xFCFF as general-purpose xdata,
 * the rest is available for buffers. Start at 0xFD00. Note that
 * DMA buffers must be aligned on an 8-byte boundary.
 */
volatile xdata at 0xFD00 unsigned char packet_buffer_1[64];
volatile xdata at 0xFD40 unsigned char packet_buffer_2[64];

static void max7300_write(unsigned char address, unsigned char data_byte)
{
  /* This is the I2C address of the LED controller */
  i2c_start(0x44);

  i2c_write_byte(address, 0);
  i2c_write_byte(data_byte, 1);
}


void main() {
  int c;

  uart_init();
  usb_init();

  /* Take the MAX7300 out of shutdown */
  max7300_write(0x04, 0x01);

  /* All LEDs off (active-low) */
  max7300_write(0x53, 0xFF);

  /* All pins are outputs */
  max7300_write(0x09, 0x55);
  max7300_write(0x0A, 0x55);
  max7300_write(0x0B, 0x55);
  max7300_write(0x0C, 0x55);
  max7300_write(0x0D, 0x55);
  max7300_write(0x0E, 0x55);
  max7300_write(0x0F, 0x55);

  while (1) {
    delay(2000);
    max7300_write(0x53, 0xFE - 0x02);
    delay(2000);
    max7300_write(0x53, 0xFE - 0x04);
    delay(2000);
    max7300_write(0x53, 0xFE - 0x08);
    delay(2000);
    max7300_write(0x53, 0xFE - 0x10);
  }


  while (1) {
    watchdog_reset();
    usb_poll();

  }
}


void usb_handle_vendor_request() {
  static unsigned char v = 0;
  v++;

  switch (usb_setup_buffer.bRequest) {


  }
}

/* The End */
