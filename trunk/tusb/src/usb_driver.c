/*
 * usb_driver.c - A relatively easy to use C interface to the USB
 *                hardware on the TUSB3210/TUSB3410 chips. This module
 *                implements many standard commands defined in the USB
 *                spec, and allows vendor-specific commands to be
 *                defined externally.
 *
 * Copyright (C) 2004  Micah Dowty
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

#include <tusb.h>
#include <string.h>
#include "util.h"
#include "usb_driver.h"

static void usb_handle_setup();
static void usb_handle_standard_request();
static void usb_handle_descriptor_request();
static void usb_leave_setup();
static void usb_write_ep0_string_descriptor(unsigned char *string);
static void usb_wait_for_ep0_in();
static void usb_ack_ep0_in();


static void usb_handle_descriptor_request() {
  /* Search for the requested descriptor and send it
   */
  int i;
  unsigned char type = usb_setup_buffer.wValue >> 8;
  unsigned char index = (unsigned char) usb_setup_buffer.wValue;
  unsigned short language = usb_setup_buffer.wIndex;

  for (i=0; usb_descriptors[i].type; i++) {
    if (usb_descriptors[i].type != type)
      continue;
    if (usb_descriptors[i].index != index)
      continue;
    if (usb_descriptors[i].language != 0 &&      /* language==0 is "don't care" */
	usb_descriptors[i].language != language)
      continue;

    if (usb_descriptors[i].type == USB_DT_STRING && usb_descriptors[i].length == 0) {
      /* Send an ASCII nul-terminated string, converted on the fly to
       * a USB descriptor with 16-bit wide characters. This makes the
       * strings far more convenient to enter and store.
       */
      usb_write_ep0_string_descriptor(usb_descriptors[i].buffer);
    }
    else {
      /* Send a generic block of RAM verbatim */
      usb_write_ep0_buffer(usb_descriptors[i].buffer, usb_descriptors[i].length);
    }
    return;
  }
}

static void usb_handle_standard_request() {
  switch (usb_setup_buffer.bRequest) {

  case USB_REQ_SET_ADDRESS:
    /* Set the device address and send a 0-length response */
    FUNADR = usb_setup_buffer.wValue;
    usb_write_ack();
    break;

  case USB_REQ_GET_DESCRIPTOR:
    /* Send back one of our descriptors from the table */
    usb_handle_descriptor_request();
    break;

  case USB_REQ_SET_CONFIGURATION:
  case USB_REQ_SET_INTERFACE:
    /* We only have one configuration/interface, so nothing to do */
    usb_write_ack();
    break;

  }
}

void usb_write_ack() {
  /* Send a response to a dataless request */
  OEPCNFG_0 |= STALL;
  IEPBCNT_0 = 0;
}

static void usb_leave_setup() {
  /* Leave setup if we're in it */
  USBCTL &= ~SIR;
  USBSTA = SETUP;
}

static void usb_wait_for_ep0_in() {
  /* The host can't read our buffer if we're still in setup */
  usb_leave_setup();

  /* Wait for the host to read our buffer */
  while ((IEPBCNT_0 & 0x80) == 0)
    watchdog_reset();
}

static void usb_ack_ep0_in() {
  /* ACK the status stage */
  OEPBCNT_0 = 0;
}

void usb_write_ep0_buffer(unsigned char *buffer, int length) {
  /* Sent a response to a request expecting EP0 IN data */
  int packet_length;
  bit explicit_stop;

  if (length >= usb_setup_buffer.wLength) {
    /* Never send more than the host wants */
    explicit_stop = 0;
    length = usb_setup_buffer.wLength;
  }
  else {
    /* The host is asking for more than we have, we need
     * to make sure we explicitly terminate the transaction.
     */
    explicit_stop = 1;
  }

  /* Always send at least one packet, even if our length is zero */
  do {
    /* Send up to 8 bytes at a time */
    if (length > 8)
      packet_length = 8;
    else
      packet_length = length;

    memcpy(usb_ep0in_buffer, buffer, packet_length);
    IEPBCNT_0 = packet_length;
    usb_wait_for_ep0_in();

    buffer += packet_length;
    length -= packet_length;
  } while (length > 0);

  /* If our last packet wasn't smaller than the maximum packet length
   * and they asked for more data than we've sent, we need to send
   * a short packet to explicitly terminate the transction.
   */
  if (explicit_stop && packet_length == 8) {
    IEPBCNT_0 = 0;
    usb_wait_for_ep0_in();
  }

  usb_ack_ep0_in();
}

static void usb_write_ep0_string_descriptor(unsigned char *string) {
  /* A version of usb_write_ep0_buffer() that converts an
   * ASCII NUL-terminated string to a USB string descriptor on the fly.
   */
  int remaining_length = usb_setup_buffer.wLength;
  int total_length = 2 + 2*strlen(string);
  int packet_length;
  xdata unsigned char *dest;
  bit explicit_stop;

  if (remaining_length > total_length) {
    /* The host is asking for more than we have, we need
     * to make sure we explicitly terminate the transaction.
     */
    explicit_stop = 1;
    remaining_length = total_length;
  }
  else {
    explicit_stop = 0;
  }

  /* The first packet includes a header */
  dest = usb_ep0in_buffer;
  *(dest++) = total_length;
  *(dest++) = USB_DT_STRING;
  packet_length = 2;
  remaining_length -= 2;

  while (1) {
    /* Add more bytes to this packet while we can */
    while (remaining_length > 0 && packet_length < 8) {
      *(dest++) = *(string++);
      *(dest++) = 0;
      packet_length += 2;
      remaining_length -= 2;
    }

    IEPBCNT_0 = packet_length;
    usb_wait_for_ep0_in();
    dest = usb_ep0in_buffer;

    if (remaining_length <= 0)
      break;

    packet_length = 0;
  }

  /* If our last packet wasn't smaller than the maximum packet length
   * and they asked for more data than we've sent, we need to send
   * a short packet to explicitly terminate the transction.
   */
  if (explicit_stop && packet_length == 8) {
    IEPBCNT_0 = 0;
    usb_wait_for_ep0_in();
  }

  usb_ack_ep0_in();
}

static void usb_handle_setup() {
  /* Indicate that we're busy with a setup packet */
  USBCTL |= SIR;

  /* Set the direction bit */
  if (usb_setup_buffer.bRequestType & USB_DIR_IN)
    USBCTL |= DIR;
  else
    USBCTL &= ~DIR;

  /* Look at the type field to start finding a request handler */
  switch (usb_setup_buffer.bRequestType & USB_TYPE_MASK) {

  case USB_TYPE_STANDARD:
    usb_handle_standard_request();
    break;

  case USB_TYPE_VENDOR:
    usb_handle_vendor_request();
    break;

  }
  usb_leave_setup();
}

void usb_poll() {
  /* Look for SETUP packets, ignore function resets
   * and suspend/resume requests for now.
   */
  if (USBSTA & SETUP)
    usb_handle_setup();
}

void usb_init() {
  FUNADR = 0;       /* We haven't been assigned an address yet */
  USBMSK = 0;       /* No USB interrupts */

  /* Set up endpoint 0. In and out both start out stalled. */
  IEPCNFG_0 = STALL | UBME;
  OEPCNFG_0 = STALL | UBME;

  USBCTL &= ~CONT;  /* Disconnect from USB */
  USBSTA = 0xFF;    /* Clear USB status bits */
  delay(1000);      /* Wait a bit and reconnect */
  USBCTL |= CONT;
}

void usb_dma_setup(unsigned char ep, xdata unsigned char *buffer, unsigned char buffer_size) {
  EDB[ep].x_base = (((unsigned short)buffer) >> 3) & 0xFF;
  EDB[ep].buffer_size = buffer_size;
  EDB[ep].x_count = 0;
}

void usb_dma_stall(unsigned char ep) {
  EDB[ep].config = UBME | STALL;
}

void usb_dma_unstall(unsigned char ep) {
  EDB[ep].config = UBME;
}

int usb_dma_status(unsigned char ep) {
  unsigned char c = EDB[ep].x_count;
  if (c & 0x80)
    return c & 0x7F;
  else
    return 0;
}

/* The End */
