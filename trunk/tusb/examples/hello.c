#include <stdio.h>
#include <string.h>
#include "usb_driver.h"
#include "uart_driver.h"
#include "util.h"

/* We give SDCC from 0xF800 to 0xFCFF as general-purpose xdata,
 * the rest is available for buffers. Start at 0xFD00. Note that
 * DMA buffers must be aligned on an 8-byte boundary.
 */
volatile xdata at 0xFD00 unsigned char ep1_out_x[64];


void main() {
  int c;

  /* After initializing the TUSB3410's built-in UART, we
   * can treat it like stdout and use printf and puts
   */
  uart_init();
  puts("\n---- Startup ----");

  /* This connects to the USB bus and initializes EP0 */
  usb_init();
  puts("USB initialized");

  /* Turn on the (active-low) LED */
  LED = 0;

  /* Set up the first EP1 OUT transfer */
  usb_dma_unstall(EDB_OEP1);
  usb_dma_setup(EDB_OEP1, ep1_out_x, sizeof(ep1_out_x));

  while (1) {
    watchdog_reset();
    usb_poll();

    /* Poll for completed DMA transfers from the EP1 OUT endpoint */
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
