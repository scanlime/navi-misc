/*
 * device.c - Initialization, device opening/closing, and error handling
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
#include <rcpod_private.h>


/* Default error handler */
void rcpod_DefaultErrorHandler(const char *function, int err, const char *message) {
  fprintf(stderr, "*** rcpod error in %s: %s\n", function, message);
  exit(1);
}

/* Current error handler, initially set to the default */
rcpod_errorHandler *rcpod_HandleError = rcpod_DefaultErrorHandler;


/* Linked list of rcpod devices found by rcpod_FindDevices.
 * Note that the pointers in these device structures are still referring
 * to data allocated by libusb, so they should not be freed and they
 * may be invalid if usb_find_devices/usb_find_busses are called again.
 */
static struct usb_device *deviceList_head = NULL;
static struct usb_device *deviceList_tail = NULL;


void rcpod_Init(void) {
  /* Nothing to do here yet, it's just in the API to allow for future initialization needs... */
}


void rcpod_FindDevices(void) {
  struct usb_device *dev, *newdev;
  struct usb_bus *bus;

  /* Free any devices already in the list */
  while (deviceList_head) {
    dev = deviceList_head;
    deviceList_head = dev->next;
    free(dev);
  }
  deviceList_tail = NULL;

  /* Loop through all attached devices... */
  for (bus=usb_get_busses(); bus; bus=bus->next) {
    for (dev=bus->devices; dev; dev=dev->next) {

      /* Have we found an rcpod? */
      if (dev->descriptor.idVendor == RCPOD_VENDOR_ID &&
	  dev->descriptor.idProduct == RCPOD_PRODUCT_ID) {

	/* Make a shallow copy and add it to our deviceList */
	newdev = malloc(sizeof(struct usb_device));
	if (!newdev) {
	  rcpod_HandleError("rcpod_FindDevices", ENOMEM, strerror(ENOMEM));
	  return;
	}
	memcpy(newdev, dev, sizeof(struct usb_device));

	/* Append it to the list */
	if (deviceList_tail) {
	  newdev->prev = deviceList_tail;
	  newdev->next = NULL;
	  deviceList_tail->next = newdev;
	  deviceList_tail = newdev;
	}
	else {
	  /* First item inserted into an empty list */
	  newdev->next = NULL;
	  newdev->prev = NULL;
	  deviceList_head = newdev;
	  deviceList_tail = newdev;
	}
      }
    }
  }
}


struct usb_device* rcpod_GetDevices(void) {
  return deviceList_head;
}


rcpod_dev* rcpod_Open(struct usb_device *usbdev) {
  rcpod_dev* rcpod;

  if (usbdev->descriptor.bcdDevice != RCPOD_PROTOCOL_VERSION) {
    /* We had a version mismatch. Both version numbers are stored as BCD integers
     * with two digits to the right of the decimal point, decode them for an error message.
     */
    char errorBuffer[256];
    sprintf(errorBuffer, "Protocol version mismatch, device is version %x.%02x, host is version %x.%02x",
	    usbdev->descriptor.bcdDevice >> 8, usbdev->descriptor.bcdDevice & 0xFF,
	    RCPOD_PROTOCOL_VERSION >> 8, RCPOD_PROTOCOL_VERSION & 0xFF);
    rcpod_HandleError("rcpod_Open", 0, errorBuffer);
    return NULL;
  }

  rcpod = malloc(sizeof(rcpod_dev));
  if (!rcpod) {
    rcpod_HandleError("rcpod_GetDevices", ENOMEM, strerror(ENOMEM));
    return NULL;
  }

  rcpod->usbdevh = usb_open(usbdev);
  if (!rcpod->usbdevh) {
    /* libusb actually has access to a real error code, but from here
     * the best we can do is make one up...
     */
    rcpod_HandleError("usb_open", 0, "Error opening the USB device");
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
  rcpod_errorHandler *oldHandler = rcpod_HandleError;

  /* A NULL handler indicates the default */
  if (!handler)
    handler = rcpod_DefaultErrorHandler;

  rcpod_HandleError = handler;
  return oldHandler;
}


void rcpod_Reset(rcpod_dev *rcpod) {
  /* This resets relevant registers to their power-on defaults,
   * as per the PIC16C745/765 data sheet.
   */

  /* Cancel a serial receive if one is happening */
  rcpod_UsartRxEnd(rcpod);

  rcpod_Poke(rcpod, RCPOD_REG_PORTA, 0);
  rcpod_Poke(rcpod, RCPOD_REG_PORTB, 0);
  rcpod_Poke(rcpod, RCPOD_REG_PORTC, 0);
  rcpod_Poke(rcpod, RCPOD_REG_PORTD, 0);
  rcpod_Poke(rcpod, RCPOD_REG_PORTE, 0);

  rcpod_Poke(rcpod, RCPOD_REG_TRISA, 0xFF);
  rcpod_Poke(rcpod, RCPOD_REG_TRISB, 0xFF);
  rcpod_Poke(rcpod, RCPOD_REG_TRISC, 0xFF);
  rcpod_Poke(rcpod, RCPOD_REG_TRISD, 0xFF);
  rcpod_Poke(rcpod, RCPOD_REG_TRISE, 0x07);

  rcpod_Poke(rcpod, RCPOD_REG_T1CON, 0);
  rcpod_Poke(rcpod, RCPOD_REG_T2CON, 0);

  rcpod_Poke(rcpod, RCPOD_REG_CCP1CON, 0);
  rcpod_Poke(rcpod, RCPOD_REG_CCP2CON, 0);

  rcpod_Poke(rcpod, RCPOD_REG_ADCON0, 0);
  rcpod_Poke(rcpod, RCPOD_REG_ADCON1, 0);

  rcpod_Poke(rcpod, RCPOD_REG_RCSTA, 0);
  rcpod_Poke(rcpod, RCPOD_REG_TXSTA, 2);
}


rcpod_dev* rcpod_InitSimpleWithoutReset(void) {
  /* Init libusb */
  usb_init();
  usb_find_busses();
  usb_find_devices();

  /* Init librcpod */
  rcpod_Init();
  rcpod_FindDevices();

  /* Give up if there's no rcpod device */
  if (!deviceList_head) {
    rcpod_HandleError("rcpod_InitSimple", ENODEV, "No device found");
    return NULL;
  }

  /* Open the first one */
  return rcpod_Open(deviceList_head);
}


rcpod_dev* rcpod_InitSimple(void) {
  rcpod_dev* rcpod = rcpod_InitSimpleWithoutReset();
  rcpod_Reset(rcpod);
  return rcpod;
}

/* The End */
