/*
 * main.c - Entry point and main loop for the Unicone firmware
 *
 * Universal Controller Emulator project
 * Copyright (C) 2004 Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <stdio.h>
#include <string.h>

#include <usb_driver.h>
#include <uart_driver.h>
#include <util.h>

#include <unicone.h>


/* We give SDCC from 0xF800 to 0xFCFF as general-purpose xdata,
 * the rest is available for buffers. Start at 0xFD00. Note that
 * DMA buffers must be aligned on an 8-byte boundary.
 */
volatile xdata at 0xFD00 unsigned char ep1_out_x[64];


void main() {
  int c;

  uart_init();
  printf("\n---- Unicone firmware v%x.%02x ----\n",
	 UNICONE_VERSION >> 8, UNICONE_VERSION & 0xFF);

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

    MCR &= ~MCR_DTR;
    delay(100);
    MCR |= MCR_DTR;

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
