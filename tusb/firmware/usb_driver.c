#include <tusb.h>
#include <stdio.h>
#include <string.h>
#include "usb_driver.h"
#include "util.h"

static void usb_handle_setup();
static void usb_handle_standard_request();
static void usb_unhandled_request();
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
    printf("Setting address to %d\n", usb_setup_buffer.wValue);
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

  default:
    usb_unhandled_request();
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

  /* Never send more than the host wants */
  if (length > usb_setup_buffer.wLength)
    length = usb_setup_buffer.wLength;

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

  if (remaining_length > total_length)
    remaining_length = total_length;

  /* The first packet includes a header */
  dest = usb_ep0in_buffer;
  *(dest++) = total_length;
  *(dest++) = USB_DT_STRING;
  packet_length = 2;
  remaining_length -= 2;

  do {
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
    packet_length = 0;
  } while (remaining_length > 0);

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

  default:
    usb_unhandled_request();
  }

  /* Yay, we're done */
  USBCTL &= ~SIR;
}

static void usb_unhandled_request() {
  printf("Unhandled request: request_type=%d request=%d value=%d index=%d length=%d\n",
	 usb_setup_buffer.bRequestType,
	 usb_setup_buffer.bRequest,
	 usb_setup_buffer.wValue,
	 usb_setup_buffer.wIndex,
	 usb_setup_buffer.wLength);
}

void usb_poll() {
  /* Look for SETUP packets, ignore function resets
   * and suspend/resume requests for now.
   */
  if (USBSTA & SETUP) {
    usb_handle_setup();
    USBSTA = SETUP;
  }
}

void usb_init() {
  FUNADR = 0;       /* We haven't been assigned an address yet */

  /* Set up endpoint 0. In and out both start out stalled. */
  IEPCNFG_0 = STALL | UBME;
  OEPCNFG_0 = STALL | UBME;

  USBCTL &= ~CONT;  /* Disconnect from USB */
  USBSTA = 0xFF;    /* Clear USB status bits */
  delay(1000);     /* Wait a bit and reconnect */
  USBCTL |= CONT;
}

/* The End */
