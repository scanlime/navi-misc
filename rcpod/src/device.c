/*
 * device.c - Interfaces that deal with libusb directly, for opening and closing
 *            the rcpod and sending low-level commands to it.
 *
 * Remote Controlled PIC of Doom
 * Copyright (C) 2003 Micah Dowty <micah@picogui.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <usb.h>
#include <rcpod.h>
#include <rcpod_protocol.h>

#define RCPOD_TIMEOUT 500     /* Default USB timeout in milliseconds (1/2 second) */


/* Default error handler */
static void defaultErrorHandler(const char *function, int err, const char *message) {
  fprintf(stderr, "*** rcpod error in %s: %s\n", function, message);
  exit(1);
}

/* Current error handler, initially set to the default */
static rcpod_errorHandler *handleError = defaultErrorHandler;


/* Linked list of rcpod devices found by rcpod_FindDevices.
 * Note that the pointers in these device structures are still referring
 * to data allocated by libusb, so they should not be freed and they
 * may be invalid if usb_find_devices/usb_find_busses are called again.
 */
static struct usb_device *deviceList = NULL;


/************************************************** Low-level initialization */


void rcpod_Init(void) {
  /* Nothing to do here yet, it's just in the API to allow for future initialization needs... */
}


void rcpod_FindDevices(void) {
  struct usb_device *dev, *newdev;
  struct usb_bus *bus;

  /* Free any devices already in the list */
  while (deviceList) {
    dev = deviceList;
    deviceList = dev->next;
    free(dev);
  }

  /* Loop through all attached devices... */
  for (bus=usb_get_busses(); bus; bus=bus->next) {
    for (dev=bus->devices; dev; dev=dev->next) {

      /* Have we found an rcpod? */
      if (dev->descriptor.idVendor == RCPOD_VENDOR_ID &&
	  dev->descriptor.idProduct == RCPOD_PRODUCT_ID) {

	/* Make a shallow copy and add it to our deviceList */
	newdev = malloc(sizeof(struct usb_device));
	if (!newdev) {
	  handleError("rcpod_FindDevices", ENOMEM, strerror(ENOMEM));
	  return;
	}
	memcpy(newdev, dev, sizeof(struct usb_device));

	/* Prepend it, out of laziness :) */
	if (deviceList) {
	  newdev->next = deviceList;
	  newdev->prev = NULL;
	  deviceList->prev = newdev;
	  deviceList = newdev;
	}
	else {
	  /* First item inserted into an empty list */
	  newdev->next = NULL;
	  newdev->prev = NULL;
	  deviceList = newdev;
	}
      }
    }
  }
}


struct usb_device* rcpod_GetDevices(void) {
  return deviceList;
}


rcpod_dev* rcpod_Open(struct usb_device *usbdev) {
  rcpod_dev* rcpod;

  if (usbdev->descriptor.bcdDevice != RCPOD_PROTOCOL_VERSION) {
    static char errorBuffer[256];
    sprintf(errorBuffer, "Protocol version mismatch, device is version %d.%02x, host is version %d.%02x",
	    usbdev->descriptor.bcdDevice >> 8, usbdev->descriptor.bcdDevice & 0xFF,
	    RCPOD_PROTOCOL_VERSION >> 8, RCPOD_PROTOCOL_VERSION & 0xFF);
    handleError("rcpod_Open", 0, errorBuffer);
    return NULL;
  }

  rcpod = malloc(sizeof(rcpod_dev));
  if (!rcpod) {
    handleError("rcpod_GetDevices", ENOMEM, strerror(ENOMEM));
    return NULL;
  }

  rcpod->usbdevh = usb_open(usbdev);
  if (!rcpod->usbdevh) {
    /* libusb actually has access to a real error code, but from here
     * the best we can do is make one up...
     */
    handleError("usb_open", 0, "Error opening the USB device");
    return NULL;
  }

  /* Claim the first interface, we don't really care if this fails... */
  usb_claim_interface(rcpod->usbdevh, 0);

  return rcpod;
}


void rcpod_Close(rcpod_dev *rcpod) {
  usb_close(rcpod->usbdevh);
  free(rcpod);
}


rcpod_errorHandler *rcpod_SetErrorHandler(rcpod_errorHandler *handler) {
  rcpod_errorHandler *oldHandler = handleError;

  /* A NULL handler indicates the default */
  if (!handler)
    handler = defaultErrorHandler;

  handleError = handler;
  return oldHandler;
}


/************************************************** High-level initialization */


rcpod_dev* rcpod_InitSimple(void) {
  /* Init libusb */
  usb_init();
  usb_find_busses();
  usb_find_devices();

  /* Init librcpod */
  rcpod_Init();
  rcpod_FindDevices();

  /* Give up if there's no rcpod device */
  if (!deviceList) {
    handleError("rcpod_InitSimple", ENODEV, "No device found");
    return NULL;
  }

  /* Open the first one */
  return rcpod_Open(deviceList);
}


/************************************************** Low-level commands */


void rcpod_Poke(rcpod_dev* rcpod, int address, unsigned char data) {
  int retval;
  /* The address and data are sent in the wIndex and wValue parameters, respectively */
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR, RCPOD_CTRL_POKE,
			   data, address, NULL, 0, RCPOD_TIMEOUT);
  if (retval < 0)
    handleError("rcpod_Poke", -retval, strerror(-retval));
}


unsigned char rcpod_Peek(rcpod_dev* rcpod, int address) {
  int retval;
  unsigned char byte;
  /* Send the address in wIndex, expect a 1-byte response packet with the data */
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			   RCPOD_CTRL_PEEK, 0, address, (char*) &byte, 1, RCPOD_TIMEOUT);
  if (retval < 0) {
    handleError("rcpod_Peek", -retval, strerror(-retval));
    return 0;
  }
  return byte;
}


void rcpod_AnalogSampleAll(rcpod_dev* rcpod, unsigned char buffer[8]) {
  int retval;
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			   RCPOD_CTRL_ANALOG_ALL, 0, 0, (char*) buffer, 8, RCPOD_TIMEOUT);
  if (retval < 0)
    handleError("rcpod_AnalogSampleAll", -retval, strerror(-retval));
}

/* The End */
