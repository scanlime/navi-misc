#include <tusb.h>
#include <stdio.h>
#include <string.h>
#include "usb_driver.h"
#include "util.h"

static void usb_handle_setup();
static void usb_handle_standard_request();
static void usb_unhandled_request();


static struct usb_device_descriptor dev_descript = {
  /* bLength            */  sizeof(struct usb_device_descriptor),
  /* bDescriptorType    */  USB_DT_DEVICE,
  /* bcdUSB             */  0x0110,
  /* bDeviceClass       */  USB_CLASS_PER_INTERFACE,
  /* bDeviceSubClass    */  0,
  /* bDeviceProtocol    */  0,
  /* bMaxPacketSize0    */  8,
  /* idVendor           */  0xE461,
  /* idProduct          */  0x0007,
  /* bcdDevice          */  0x0100,
  /* iManufacturer      */  0,
  /* iProduct           */  0,
  /* iSerialNumber      */  0,
  /* bNumConfigurations */  1,
};

static void usb_handle_standard_request() {
  static int offset=0;
  int len;

  switch (usb_setup_buffer.bRequest) {

  case USB_REQ_SET_ADDRESS:
    /* Set the device address and send a 0-length response */
    //OEPCNFG_0 |= STALL;
    FUNADR = usb_setup_buffer.wValue;
    printf("Setting address to %d\n", FUNADR);
    IEPBCNT_0 = 0;
    break;

  case USB_REQ_GET_DESCRIPTOR:
    printf("Sending descriptor: value=0x%04X, length=%d\n", usb_setup_buffer.wValue, usb_setup_buffer.wLength);

    /* Acknowledge the status stage */
    OEPCNFG_0 |= STALL;

    len = sizeof(dev_descript) - offset;
    if (len > usb_setup_buffer.wLength)
      len = usb_setup_buffer.wLength;
    printf("   sending back %d bytes starting at %d\n", len, offset);
    memcpy(usb_ep0in_buffer, ((unsigned char *)&dev_descript)+offset, len);
    offset += usb_setup_buffer.wLength;

    printf("0x%02X\n", IEPCNFG_0);

    IEPBCNT_0 = len;
    break;

  default:
    usb_unhandled_request();
  }
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
  delay(60000);     /* Wait a bit and reconnect */
  USBCTL |= CONT;
}

/* The End */
