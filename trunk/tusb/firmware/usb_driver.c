#include <tusb.h>
#include <stdio.h>
#include <string.h>
#include "usb_driver.h"
#include "util.h"

static void usb_handle_setup();
static void usb_handle_standard_request();
static void usb_unhandled_request();
static void usb_handle_descriptor_request();


const static struct usb_device_descriptor dev_descript = {
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
  /* iManufacturer      */  1,
  /* iProduct           */  2,
  /* iSerialNumber      */  0,
  /* bNumConfigurations */  1,
};

const static struct {
  struct usb_config_descriptor config;
  struct usb_interface_descriptor if0;
  struct usb_endpoint_descriptor if0_ep0;
} config_descript = {
  /* Configuration */
  {
    /* bLength             */  sizeof(struct usb_config_descriptor),
    /* bDescriptorType     */  USB_DT_CONFIG,
    /* wTotalLength        */  sizeof(config_descript),
    /* bNumInterfaces      */  1,
    /* bConfigurationValue */  1,
    /* iConfiguration      */  0,
    /* bmAttributes        */  0x80,
    /* MaxPower            */  50,
  },
  /* First interface */
  {
    /* bLength             */  sizeof(struct usb_interface_descriptor),
    /* bDescriptorType     */  USB_DT_INTERFACE,
    /* bInterfaceNumber    */  0,
    /* bAlternateSetting   */  0,
    /* bNumEndpoints       */  1,
    /* bInterfaceClass     */  USB_CLASS_VENDOR_SPEC,
    /* bInterfaceSubClass  */  USB_CLASS_VENDOR_SPEC,
    /* bInterfaceProtocol  */  USB_CLASS_VENDOR_SPEC,
    /* iInterface          */  0,
  },
  /* First interface's first endpoint */
  {
    /* bLength             */  sizeof(struct usb_endpoint_descriptor),
    /* bDescriptorType     */  USB_DT_ENDPOINT,
    /* bEndpointAddress    */  1 | USB_DIR_OUT,
    /* bmAttributes        */  USB_ENDPOINT_XFER_BULK,
    /* wMaxPacketSize      */  64,
    /* bInterval           */  0,
  },
};

const static unsigned char str_lang_table[] = {
  sizeof(str_lang_table), USB_DT_STRING,
  0x09, 0x04, /* English */
};

const static unsigned char str_manufacturer[] = {
  sizeof(str_manufacturer), USB_DT_STRING,
  'M',0, 'i',0, 'c',0, 'a',0, 'h',0,
};

const static unsigned char str_product[] = {
  sizeof(str_product), USB_DT_STRING,
  'F',0, 'o',0, 'o',0,
};

const static struct usb_descriptor_entry descriptor_table[] = {
  {USB_DT_DEVICE, 0, 0, &dev_descript,    sizeof(dev_descript)},
  {USB_DT_CONFIG, 0, 0, &config_descript, sizeof(config_descript)},
  {USB_DT_STRING, 0, 0, str_lang_table,   sizeof(str_lang_table)},
  {USB_DT_STRING, 1, 0, str_manufacturer, sizeof(str_manufacturer)},
  {USB_DT_STRING, 2, 0, str_product,      sizeof(str_product)},
  {0},
};

static void usb_handle_descriptor_request() {
  /* Search for the requested descriptor and send it
   */
  struct usb_descriptor_entry *entry = descriptor_table;
  unsigned char type = usb_setup_buffer.wValue >> 8;
  unsigned char index = (unsigned char) usb_setup_buffer.wValue;
  unsigned short language = usb_setup_buffer.wIndex;

  while (entry->type) {
    if (entry->type == type && entry->index == index && entry->language == language) {
      /* Found it, send the descriptor */
      usb_write_ep0_buffer(entry->buffer, entry->length);
      return;
    }
    entry++;
  }

  printf("Descriptor unavailable, ignoring\n");
}

static void usb_handle_standard_request() {
  switch (usb_setup_buffer.bRequest) {

  case USB_REQ_SET_ADDRESS:
    /* Set the device address and send a 0-length response */
    printf("Setting address to %d\n", usb_setup_buffer.wValue);
    FUNADR = usb_setup_buffer.wValue;
    OEPCNFG_0 |= STALL;
    IEPBCNT_0 = 0;
    break;

  case USB_REQ_GET_DESCRIPTOR:
    printf("Descriptor 0x%04X requested\n", usb_setup_buffer.wValue);
    usb_handle_descriptor_request();
    break;

  default:
    usb_unhandled_request();
  }
}

void usb_write_ep0_buffer(void *buffer, int length) {
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

    /* Put some data in the EP0 IN buffer */
    memcpy(usb_ep0in_buffer, buffer, packet_length);
    IEPBCNT_0 = packet_length;

    /* Leave setup if we're in it */
    USBCTL &= ~SIR;
    USBSTA = SETUP;

    /* Wait for the host to read our buffer */
    while ((IEPBCNT_0 & 0x80) == 0)
      watchdog_reset();

    buffer += packet_length;
    length -= packet_length;
  } while (length > 0);

  /* ACK the status stage */
  OEPBCNT_0 = 0;
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
  delay(60000);     /* Wait a bit and reconnect */
  USBCTL |= CONT;
}

/* The End */
