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

void io_tester() {
  while (1) {
    P3_0 = 1;
    P3_3 = 1;
    P3_4 = 0;
    delay(4000);
    I2CADR = 0x80;
    I2CDAO = 0x00;

    P3_0 = 1;
    P3_3 = 0;
    P3_4 = 1;
    delay(4000);
    I2CADR = 0x40;
    I2CDAO = 0x00;

    P3_0 = 0;
    P3_3 = 1;
    P3_4 = 1;
    delay(4000);
    I2CADR = 0x20;
    I2CDAO = 0x00;

    P3_0 = 1;
    P3_3 = 0;
    P3_4 = 1;
    delay(4000);
    I2CADR = 0x10;
    I2CDAO = 0x00;
  }
}

void main() {
  //io_tester();

  uart_init();
  puts("\n---- Startup ----");

  usb_init();
  puts("USB initialized");

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
