#include <tusb.h>
#include <stdio.h>
#include <string.h>
#include "usb_driver.h"
#include "uart_driver.h"
#include "util.h"


void main() {
  EA = 0;     /* Global interrupt disable */

  uart_init();
  puts("\n---- Startup ----");

  usb_init();
  puts("USB initialized");

  while (1) {
    watchdog_reset();
    usb_poll();
  }
}


void usb_handle_vendor_request() {
  static unsigned char v = 0;
  v++;

  printf("Vendor request %d\n", usb_setup_buffer.bRequest);

  switch (usb_setup_buffer.bRequest) {

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

  case 123:
    printf("Sending %d\n", v);
    usb_write_ep0_buffer(&v, 1);
    break;

  }
}

/* The End */
