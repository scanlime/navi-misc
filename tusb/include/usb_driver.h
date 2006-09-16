/*
 * usb_driver.h - A relatively easy to use C interface to the USB
 *                hardware on the TUSB3210/TUSB3410 chips. This module
 *                implements many standard commands defined in the USB
 *                spec, and allows vendor-specific commands to be
 *                defined externally.
 *
 * Copyright (C) 2004-2005 Micah Dowty
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
#ifndef __USB_DRIVER_H
#define __USB_DRIVER_H

#include <tusb.h>

struct usb_ctrlrequest {
  unsigned char bRequestType;
  unsigned char bRequest;
  unsigned short wValue;
  unsigned short wIndex;
  unsigned short wLength;
};

struct usb_descriptor_header {
  unsigned char  bLength;
  unsigned char  bDescriptorType;
};

struct usb_device_descriptor {
  unsigned char  bLength;
  unsigned char  bDescriptorType;
  unsigned short bcdUSB;
  unsigned char  bDeviceClass;
  unsigned char  bDeviceSubClass;
  unsigned char  bDeviceProtocol;
  unsigned char  bMaxPacketSize0;
  unsigned short idVendor;
  unsigned short idProduct;
  unsigned short bcdDevice;
  unsigned char  iManufacturer;
  unsigned char  iProduct;
  unsigned char  iSerialNumber;
  unsigned char  bNumConfigurations;
};

struct usb_config_descriptor {
  unsigned char  bLength;
  unsigned char  bDescriptorType;
  unsigned short wTotalLength;
  unsigned char  bNumInterfaces;
  unsigned char  bConfigurationValue;
  unsigned char  iConfiguration;
  unsigned char  bmAttributes;
  unsigned char  MaxPower;
};

struct usb_interface_descriptor {
  unsigned char  bLength;
  unsigned char  bDescriptorType;
  unsigned char  bInterfaceNumber;
  unsigned char  bAlternateSetting;
  unsigned char  bNumEndpoints;
  unsigned char  bInterfaceClass;
  unsigned char  bInterfaceSubClass;
  unsigned char  bInterfaceProtocol;
  unsigned char  iInterface;
};

struct usb_endpoint_descriptor {
  unsigned char  bLength;
  unsigned char  bDescriptorType;
  unsigned char  bEndpointAddress;
  unsigned char  bmAttributes;
  unsigned short wMaxPacketSize;
  unsigned char  bInterval;
};

struct usb_descriptor_entry {
  unsigned char type;
  unsigned char index;
  unsigned char *buffer;
  int length;                    /* If this is 0 and type is USB_DT_STRING,
				  * the 'buffer' is treated as an ASCII string
				  * terminated with a NUL, rather than as a full
				  * USB string descriptor. This can't handle Unicode,
				  * but it makes strings infinitely easier to input.
				  */
  unsigned short language;       /* 0 for "don't care" */
};

/* The setup and EP0 buffers are in fixed locations in RAM */
volatile xdata at 0xFEF0 unsigned char usb_ep0out_buffer[8];
volatile xdata at 0xFEF8 unsigned char usb_ep0in_buffer[8];
volatile xdata at 0xFF00 struct usb_ctrlrequest usb_setup_buffer;

/*
 * Device and/or Interface Class codes
 */
#define USB_CLASS_PER_INTERFACE		0	/* for DeviceClass */
#define USB_CLASS_AUDIO			1
#define USB_CLASS_COMM			2
#define USB_CLASS_HID			3
#define USB_CLASS_PHYSICAL		5
#define USB_CLASS_STILL_IMAGE		6
#define USB_CLASS_PRINTER		7
#define USB_CLASS_MASS_STORAGE		8
#define USB_CLASS_HUB			9
#define USB_CLASS_CDC_DATA		0x0a
#define USB_CLASS_CSCID			0x0b	/* chip+ smart card */
#define USB_CLASS_CONTENT_SEC		0x0d	/* content security */
#define USB_CLASS_APP_SPEC		0xfe
#define USB_CLASS_VENDOR_SPEC		0xff

/*
 * USB types
 */
#define USB_TYPE_MASK			(0x03 << 5)
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)

/*
 * USB recipients
 */
#define USB_RECIP_MASK			0x1f
#define USB_RECIP_DEVICE		0x00
#define USB_RECIP_INTERFACE		0x01
#define USB_RECIP_ENDPOINT		0x02
#define USB_RECIP_OTHER			0x03

/*
 * USB directions
 */
#define USB_DIR_OUT			0		/* to device */
#define USB_DIR_IN			0x80		/* to host */

/*
 * Descriptor types
 */
#define USB_DT_DEVICE			0x01
#define USB_DT_CONFIG			0x02
#define USB_DT_STRING			0x03
#define USB_DT_INTERFACE		0x04
#define USB_DT_ENDPOINT			0x05

#define USB_DT_HID			(USB_TYPE_CLASS | 0x01)
#define USB_DT_REPORT			(USB_TYPE_CLASS | 0x02)
#define USB_DT_PHYSICAL			(USB_TYPE_CLASS | 0x03)
#define USB_DT_HUB			(USB_TYPE_CLASS | 0x09)

/*
 * Descriptor sizes per descriptor type
 */
#define USB_DT_DEVICE_SIZE		18
#define USB_DT_CONFIG_SIZE		9
#define USB_DT_INTERFACE_SIZE		9
#define USB_DT_ENDPOINT_SIZE		7
#define USB_DT_ENDPOINT_AUDIO_SIZE	9	/* Audio extension */
#define USB_DT_HUB_NONVAR_SIZE		7
#define USB_DT_HID_SIZE			9

/*
 * Endpoints
 */
#define USB_ENDPOINT_NUMBER_MASK	0x0f	/* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK		0x80

#define USB_ENDPOINT_XFERTYPE_MASK	0x03	/* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL	0
#define USB_ENDPOINT_XFER_ISOC		1
#define USB_ENDPOINT_XFER_BULK		2
#define USB_ENDPOINT_XFER_INT		3

/*
 * Standard requests
 */
#define USB_REQ_GET_STATUS		0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE		0x03
#define USB_REQ_SET_ADDRESS		0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE		0x0A
#define USB_REQ_SET_INTERFACE		0x0B
#define USB_REQ_SYNCH_FRAME		0x0C

/*
 * HID requests
 */
#define USB_REQ_GET_REPORT		0x01
#define USB_REQ_GET_IDLE		0x02
#define USB_REQ_GET_PROTOCOL		0x03
#define USB_REQ_SET_REPORT		0x09
#define USB_REQ_SET_IDLE		0x0A
#define USB_REQ_SET_PROTOCOL		0x0B

/* Main entry points */
void usb_init();
void usb_poll();

/* Application-defined */
void usb_handle_vendor_request();
void usb_set_idle_handler(void (*callback)());
extern const struct usb_descriptor_entry usb_descriptors[];

/* Request reply functions (blocking) */
void usb_write_ep0_buffer(unsigned char *buffer, int length);
void usb_write_ack();

/* Setup and polling of DMA transfers to/from EP1 through EP3.
 * The 'ep' parameter must be an EDB_* constant from tusb.h
 * specifying an endpoint index.
 *
 * Buffer addresses must be in external RAM, aligned on an
 * 8-byte boundary.
 *
 * usb_dma_status returns 0 if nothing has happened, nonzero
 * if a transfer has completed. For OUT endpoints, this will
 * always be the number of bytes received.
 *
 * usb_dma_setup needs to be called to start or continue a transfer.
 * usb_dma_unstall should only be called before the first usb_dma_setup
 * normally.
 */
void usb_dma_setup(unsigned char ep, xdata unsigned char *buffer, unsigned char buffer_size);
void usb_dma_stall(unsigned char ep);
void usb_dma_unstall(unsigned char ep);
int  usb_dma_status(unsigned char ep);

#endif /* __USB_DRIVER_H */

/* The End */
