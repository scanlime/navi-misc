/*
 * rcpod.h - Public interfaces for librcpod
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

#ifndef __RCPOD_H
#define __RCPOD_H

#include <usb.h>


/************************************************** Data structures */

/* Represents an open rcpod device. This is returned by rcpod_init_simple
 * or rcpod_open. It must be freed with rcpod_close.
 */
typedef struct {
  struct usb_dev_handle *usbdevh;     /* Opened USB device corresponding to the rcpod */
} rcpod_dev;

/* A librcpod error handler function, as passed to rcpod_SetErrorHandler.
 * On error, this is called with the name of the failed function and its
 * return value.
 */
typedef void (rcpod_errorHandler)(const char *function, int retValue);


/************************************************** Low-level initialization */

/*
 * These functions are intended for applications
 * that also use libusb for their own purposes, or
 * applications that must deal with multiple rcpods.
 */

/* Initialize the rcpod library, doesn't initialize libusb. The only
 * functions that may be called before this are rcpod_SetErrorHandler and
 * rcpod_InitSimple.
 */
void rcpod_Init(void);

/* Scan the USB device tree for rcpods. Must be called after usb_init, usb_find_busses,
 * and usb_find_devices.
 */
int rcpod_FindDevices(void);

/* Return a doubly linked list of usb_device structures for each rcpod found.
 * This is created in rcpod_find_devices, so the returned nodes must not be
 * modified or freed. Returns NULL if no devices are found.
 */
struct usb_device* rcpod_GetDevices(void);

/* Given a libusb usb_device, open it as an rcpod, returning an rcpod_dev */
rcpod_dev* rcpod_Open(struct usb_device *usbdev);

/* Close an rcpod_dev and free all memory associated with it */
void rcpod_Close(rcpod_dev *rcpod);

/* Set a new error handler, return the previous one. If handler is NULL,
 * the default error handler is restored. The default error handler
 * displays a message on stderr and calls exit(1).
 */
rcpod_errorHandler *rcpod_SetErrorHandler(rcpod_errorHandler *handler);


/************************************************** High-level initialization */

/* Initializes libusb and librcpod, finds the first rcpod device, and opens it */
rcpod_dev* rcpod_InitSimple(void);


/************************************************** Low-level commands */

/*
 * All functions in this section map directly to commands handled by the
 * rcpod firmware. This includes functions to read and write the rcpod's
 * address space, turn on and off GPIO pins, and set up serial transfers.
 */

/* Write one byte to the given 9-bit address in the rcpod's RAM */
void rcpod_Poke(rcpod_dev* rcpod, int address, unsigned char data);

/* Read one byte from the given 9-bit address in the rcpod's RAM */
unsigned char rcpod_Peek(rcpod_dev* rcpod, unsigned char address);


#endif /* __RCPOD_H */

/* The End */
