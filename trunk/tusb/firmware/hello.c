#include <tusb.h>
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

void main() {
  EA = 0;     /* Global interrupt disable */

  uart_init();
  puts("\n---- Startup ----");

  usb_init();
  puts("USB initialized");

  /* Set up extra endpoints */
  OEPCNF_1 = UBME;
  OEPBBAX_1 = (0xFD00 >> 3) & 0xFF;
  OEPSIZXY_1 = sizeof(ep1_out_x);
  OEPBCTX_1 = 0;

  while (1) {
    watchdog_reset();
    usb_poll();

    c = OEPBCTX_1;
    if (c) {
      c &= ~0x80;
      printf("Received %d bytes on EP1 OUT: %02X %02X %02X %02x...\n", c,
	     ep1_out_x[0], ep1_out_x[1], ep1_out_x[2], ep1_out_x[3]);
      OEPBCTX_1 = 0;
    }
  }
}


void usb_handle_vendor_request() {
  static unsigned char v = 0;
  v++;

  printf("Vendor request %d\n", usb_setup_buffer.bRequest);

  switch (usb_setup_buffer.bRequest) {

    /* Experimental EP0 OUT */
  case 42:
    /* ACK the setup packet */
    OEPBCNT_0 = 0;

    /* Leave setup */
    USBCTL &= ~SIR;
    USBSTA = SETUP;

    /* Wait for our OUT data to arrive */
    printf("Waiting for OUT data\n");
    while ((OEPBCNT_0 & 0x80) == 0)
      watchdog_reset();
    printf("Received data: '%s'\n", usb_ep0out_buffer);
    break;

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
