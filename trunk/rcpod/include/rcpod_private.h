/*
 * rcpod_private.h - Internal data structures and functions for librcpod
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

#ifndef __RCPOD_PRIVATE_H
#define __RCPOD_PRIVATE_H

#include <usb.h>
#include <rcpod.h>
#include <rcpod_protocol.h>


/*************************************************************************************/
/************************************************** Data types ***********************/
/*************************************************************************************/

/* Implementation of the abstract rcpod data type described in rcpod.h */
struct tag_rcpod_dev {
  struct usb_dev_handle *usbdevh;     /* Opened USB device corresponding to the rcpod */
};


/*************************************************************************************/
/************************************************** Error handling *******************/
/*************************************************************************************/

#define RCPOD_TIMEOUT 5000     /* Default USB timeout in milliseconds (5 seconds)
				* This needs to be big enough to transmit the entire
				* scratchpad buffer at ridiculously slow baud rates.
				*/

void rcpod_DefaultErrorHandler(const char *function, int err, const char *message);

/* Current error handler, initially set to the default */
extern rcpod_errorHandler *rcpod_HandleError;


/*************************************************************************************/
/************************************************** Low-level commands ***************/
/*************************************************************************************/

/* These are low-level command functions not exposed to the public API. Generally
 * this is because their interface is awkward and a higher-level function includes
 * all its useful functionality.
 */

/* Write 4 bytes after the last byte poked */
void rcpod_Poke4(rcpod_dev* rcpod, unsigned char data[4]);

/* Read 8 bytes starting at the given 9-bit address in the rcpod's RAM,
 * into the provided buffer
 */
void rcpod_Peek8(rcpod_dev* rcpod, int address, unsigned char data[8]);

/* Using current USART settings, transmit 'txBytes' bytes from the buffer at the
 * given address in the rcpod's RAM. Then, start listening for up to 'rxBytes'
 * to be placed in the same buffer. The receive runs in the background until
 * this byte count has been reached, or UsartRxEnd is called. Either byte
 * count may be zero to perform only a transmit/receive.
 */
void rcpod_UsartTxRx(rcpod_dev* rcpod, int address, int txBytes, int rxBytes);

/* Cancel the current receive started with UsartTxRx, return the number of
 * bytes actually received
 */
int rcpod_UsartRxEnd(rcpod_dev* rcpod);

/* Set the pin descriptor used as a USART transmit enable, for RS-485 or similar
 * protocols that require enabling a line driver. May be zero (a no-op pin descriptor)
 * to disable this feature.
 */
void rcpod_UsartTxe(rcpod_dev* rcpod, rcpod_pin txe);

/* Assert the given four pin descriptors, setting them to their active state */
void rcpod_GpioAssert4(rcpod_dev* rcpod, rcpod_pin pins[4]);


#endif /* __RCPOD_PRIVATE_H */

/* The End */
