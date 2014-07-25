#include <stdio.h>
#include <usb_driver.h>
#include <uart_driver.h>
#include <util.h>
#include "dac.h"

/* We give SDCC from 0xF800 to 0xFCFF as general-purpose xdata,
 * the rest is available for buffers. Start at 0xFD00. Note that
 * DMA buffers must be aligned on an 8-byte boundary.
 */
volatile xdata at 0xFD00 unsigned short ep1_out_x[32];


void main() {
  int c;
  unsigned short *p;

  uart_init();
  puts("\n---- Startup ----");

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
      p = ep1_out_x;
      while (c >= 4) {
	dac_write(0, p[0]);
	dac_write(1, p[1]);
	dac_sync();
	c -= 2;
	p += 2;
      }

      usb_dma_setup(EDB_OEP1, ep1_out_x, sizeof(ep1_out_x));
    }
  }
}


void usb_handle_vendor_request() {
  printf("Vendor request %d\n", usb_setup_buffer.bRequest);

  switch (usb_setup_buffer.bRequest) {


  }
}

/* The End */
