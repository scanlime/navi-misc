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
  /* Opened USB device corresponding to the rcpod */
  struct usb_dev_handle *usbdevh;

  /* Tracking information for the current serial receive.
   * Valid only when size != 0. If size == 0, there is no receive in progress.
   */
  struct {
    int  size;        /* Size of the receive buffer, as passed to rcpod_UsartTxRx() */
    int  address;     /* Address of the receive buffer, as passed to rcpod_UsartTxRx() */
    int  last_count8; /* The last return value of rcpod_UsartRxProgress */
    int  count;       /* The actual count of bytes received so far, accounting for rollovers in count8 */
    int  unread;      /* Number of received bytes that we haven't yet read */
    int  tail_index;  /* Index into the receive buffer where the oldest received byte is stored */
  } rx;
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
 * given address in the rcpod's RAM. Then, if 'rxBytes' is nonzero, start listening
 * for bytes in a separate receive task. These bytes are placed into the same buffer,
 * looping back to the beginning of the buffer after every 'rxBytes' bytes are received.
 * The receiver runs until the next call to UsartTxRx.
 * Either byte count may be zero to perform only a transmit/receive, both can be
 * zero to only cancel an in-progress receive.
 */
void rcpod_UsartTxRx(rcpod_dev* rcpod, int address, int txBytes, int rxBytes);

/* Return the number of bytes received so far, modulo 256.
 * This will not reset when the receive buffer wraps around.
 */
int rcpod_UsartRxProgress(rcpod_dev* rcpod);

/* Set the pin descriptor used as a USART transmit enable, for RS-485 or similar
 * protocols that require enabling a line driver. May be zero (a no-op pin descriptor)
 * to disable this feature.
 */
void rcpod_UsartTxe(rcpod_dev* rcpod, rcpod_pin txe);

/* Assert the given four pin descriptors, setting them to their active state */
void rcpod_GpioAssert4(rcpod_dev* rcpod, rcpod_pin pins[4]);


#endif /* __RCPOD_PRIVATE_H */

/* The End */
