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


void main() {

  usb_init();

  while (1) {
    P3_0 = 1;
    P3_0 = 0;
  }

}

void usb_handle_vendor_request() {
}

/* The End */
