/*
 * main.c - Firmware for the Therm receiver. This runs on a TUSB3410
 *          microcontroller (modified Keyspan USA-19HS). A Laipac 315 MHz
 *          ASK receiver is connected to our UART, and the I2C bus has a
 *          local temperature sensor (TC74) and a MAX7300 driving the
 *          front panel LEDs.
 *
 *          This firmware handles most levels of the protocol. Raw bytes
 *          received over the UART are converted into bits with a measure
 *          of line noise, then packet framing is detected and verified.
 *          Complete packets are buffered and sent back to the host.
 *
 * Wireless therm system
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

#include <usb_driver.h>
#include <util.h>
#include <therm-rx-protocol.h>
#include "therm-rx.h"

/* We give SDCC from 0xF800 to 0xFCFF as general-purpose xdata,
 * the rest is available for buffers. Start at 0xFD00. Note that
 * DMA buffers must be aligned on an 8-byte boundary.
 */
volatile xdata at 0xFD00 unsigned char packet_buffer_1[64];
volatile xdata at 0xFD40 unsigned char packet_buffer_2[64];

#define PACKET_ENDPOINT EDB_IEP1


void main() {
  int c;
  unsigned char current_buffer = 0;

  leds_init();
  tc74_init();
  receiver_init();

  USBCTL &= ~CONT;   /* Disconnect then reconnect to USB */
  delay(10000);
  usb_init();
  usb_dma_unstall(PACKET_ENDPOINT);

  receiver_set_buffer(packet_buffer_1, sizeof(packet_buffer_1));
  current_buffer = 0;

  while (1) {
    watchdog_reset();
    usb_poll();
    leds_update();

    /* If we've received a packet, start sending that one back and swap buffers */
    c = receiver_poll();
    if (c) {
      leds_queue_spinner(1);
      usb_dma_setup(PACKET_ENDPOINT, current_buffer ? packet_buffer_2 : packet_buffer_1, c);
      current_buffer = !current_buffer;
      receiver_set_buffer(current_buffer ? packet_buffer_2 : packet_buffer_1, sizeof(packet_buffer_1));
    }
  }
}


void usb_handle_vendor_request() {
  static unsigned char v = 0;
  unsigned char retval[8];
  v++;

  switch (usb_setup_buffer.bRequest) {

  case THERMRX_REQ_REBOOT:
    /* Let the watchdog reboot us */
    usb_write_ack();
    while (1);

  case THERMRX_REQ_LOCAL_TEMP:
    retval[0] = tc74_read();
    usb_write_ep0_buffer(retval, 1);
    break;

  }
}

/* The End */
