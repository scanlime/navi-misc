/*
 * hello.c - A simple example program for the TUSB3210
 *
 * --Micah Dowty <micah@navi.cx>
 */

#include "usb_driver.h"
#include "util.h"

/* We give SDCC from 0xF800 to 0xFCFF as general-purpose xdata,
 * the rest is available for buffers. Start at 0xFD00. Note that
 * DMA buffers must be aligned on an 8-byte boundary.
 */
volatile xdata at 0xFD00 unsigned char ep1_out_x[64];

void idler() {
  /* Debug code. Look at P3.0 on an oscilloscope to
   * measure main loop period, to get an idea of max
   * latency for things serviced from here.
   */
  P3_0 = !P3_0;

  watchdog_reset();
}

void main() {
  usb_init();
  usb_set_idle_handler(idler);

  while (1) {
    idler();
    usb_poll();
  }
}

void usb_handle_vendor_request() {
}

/* The End */
