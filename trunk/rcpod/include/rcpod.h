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

/*************************************************************************************/
/************************************************** Data structures ******************/
/*************************************************************************************/

/* Represents an open rcpod device. This is returned by rcpod_init_simple
 * or rcpod_open. It must be freed with rcpod_close.
 */
typedef struct {
  struct usb_dev_handle *usbdevh;     /* Opened USB device corresponding to the rcpod */
} rcpod_dev;

/* A librcpod error handler function, as passed to rcpod_SetErrorHandler.
 *   function: Name of the function where the error was first reported
 *        err: The C errno value of the failed function if applicable, otherwise zero
 *    message: A description of the error
 */
typedef void (rcpod_errorHandler)(const char *function, int err, const char *message);

/* A pin descriptor, used in the low-level commands to specify general purpose
 * I/O pins in a generic way. Constructed using the RCPOD_PIN macro.
 */
typedef unsigned char rcpod_pin;

/* Construct a pin descriptor, used to describe I/O ports to the low-level commands.
 *   polarity:  0 = active low, 1 = active high
 *   tristate:  0 = port bit, 1 = tristate bit
 *       port:  1 = PORTA ... 5 = PORTE
 *        bit:  0 = LSB ... 7 = MSB
 */
#define RCPOD_PIN(polarity, tristate, port, bit) \
        ( ((polarity)<<7) | ((tristate)<<6) | ((port)<<3) | (bit))


/*************************************************************************************/
/************************************************** Low-level initialization *********/
/*************************************************************************************/

/*
 * These functions are intended for applications that also use libusb for their,
 * own purposes, that must deal with multiple rcpods, or that must open an
 * rcpod device without resetting it.
 */

/* Initialize the rcpod library, doesn't initialize libusb. The only
 * functions that may be called before this are rcpod_SetErrorHandler and
 * rcpod_InitSimple.
 */
void rcpod_Init(void);

/* Scan the USB device tree for rcpods. Must be called after usb_init, usb_find_busses,
 * and usb_find_devices, must be called again if usb_find_busses/usb_find_devices are
 * called again.
 */
void rcpod_FindDevices(void);

/* Return a doubly linked list of usb_device structures for each rcpod found.
 * This is created in rcpod_find_devices, so the returned nodes must not be
 * modified or freed. Returns NULL if no devices were found.
 */
struct usb_device* rcpod_GetDevices(void);

/* Given a libusb usb_device, open it as an rcpod, returning an rcpod_dev */
rcpod_dev* rcpod_Open(struct usb_device *usbdev);

/* Close an rcpod_dev and free all memory associated with it */
void rcpod_Close(rcpod_dev *rcpod);

/* Put the I/O related registers on the rcpod in their default power-on state */
void rcpod_Reset(rcpod_dev *rcpod);

/* Set a new error handler, return the previous one. If handler is NULL,
 * the default error handler is restored. The default error handler
 * displays a message on stderr and calls exit(1).
 */
rcpod_errorHandler *rcpod_SetErrorHandler(rcpod_errorHandler *handler);


/*************************************************************************************/
/************************************************** High-level initialization ********/
/*************************************************************************************/

/* Initializes libusb and librcpod, finds the first rcpod device,
 * opens it, and resets it. If your application needs finer control
 * over the initialization steps, it may need to use the above functions directly.
 */
rcpod_dev* rcpod_InitSimple(void);

/* Like rcpod_InitSimple, but skip resetting the device. This is useful for programs
 * that only need to deal with one rcpod device, but need to preserve the previous
 * state of the device.
 */
rcpod_dev* rcpod_InitSimpleWithoutReset(void);


/*************************************************************************************/
/************************************************** Low-level commands ***************/
/*************************************************************************************/

/*
 * All functions in this section map directly to commands handled by the
 * rcpod firmware. This includes functions to read and write the rcpod's
 * address space, turn on and off GPIO pins, and set up serial transfers.
 */

/* Write one byte to the given 9-bit address in the rcpod's RAM */
void rcpod_CmdPoke(rcpod_dev* rcpod, int address, unsigned char data);

/* Read one byte from the given 9-bit address in the rcpod's RAM */
unsigned char rcpod_CmdPeek(rcpod_dev* rcpod, int address);

/* Write 4 bytes after the last byte poke'd */
void rcpod_CmdPoke4(rcpod_dev* rcpod, unsigned char data[4]);

/* Read 8 bytes starting at the given 9-bit address in the rcpod's RAM,
 * into the provided buffer
 */
void rcpod_CmdPeek8(rcpod_dev* rcpod, int address, unsigned char data[8]);

/* Sample all 8 of the 8-bit A/D converter channels, fills the provided buffer */
void rcpod_CmdAnalogAll(rcpod_dev* rcpod, unsigned char buffer[8]);

/* Using current USART settings, transmit 'txBytes' bytes from the buffer at the
 * given address in the rcpod's RAM. Then, start listening for up to 'rxBytes'
 * to be placed in the same buffer. The receive runs in the background until
 * this byte count has been reached, or CmdUsartRxEnd is called. Either byte
 * count may be zero to perform only a transmit/receive.
 */
void rcpod_CmdUsartTxRx(rcpod_dev* rcpod, int address, int txBytes, int rxBytes);

/* Cancel the current receive started with CmdUsartTxRx, return the number of
 * bytes actually received
 */
int rcpod_CmdUsartRxEnd(rcpod_dev* rcpod);

/* Set the pin descriptor used as a USART transmit enable, for RS-485 or similar
 * protocols that require enabling a line driver. May be zero (a no-op pin descriptor)
 * to disable this feature.
 */
void rcpod_CmdUsartTxe(rcpod_dev* rcpod, rcpod_pin txe);

/* Assert the given four pin descriptors, setting them to their active state */
void rcpod_CmdGpioAssert(rcpod_dev* rcpod, rcpod_pin pins[4]);

/* Read the value of the given pin descriptor */
int rcpod_CmdGpioRead(rcpod_dev* rcpod, rcpod_pin pin);


/*************************************************************************************/
/************************************************** Constants ************************/
/*************************************************************************************/

/* Size of the address space reachable via peek and poke */
#define RCPOD_MEM_SIZE          0x0200

/* A subset of the PIC's hardware registers. Those that couldn't possibly
 * be useful to poke at via the rcpod have been omitted.
 */
#define RCPOD_REG_TMR0	        0x0001
#define RCPOD_REG_STATUS        0x0003
#define RCPOD_REG_PORTA	        0x0005
#define RCPOD_REG_PORTB	        0x0006
#define RCPOD_REG_PORTC	        0x0007
#define RCPOD_REG_PORTD	        0x0008
#define RCPOD_REG_PORTE	        0x0009
#define RCPOD_REG_PCLATH        0x000A
#define RCPOD_REG_INTCON        0x000B
#define RCPOD_REG_PIR1	        0x000C
#define RCPOD_REG_PIR2	        0x000D
#define RCPOD_REG_TMR1L	        0x000E
#define RCPOD_REG_TMR1H	        0x000F
#define RCPOD_REG_T1CON         0x0010
#define RCPOD_REG_TMR2	        0x0011
#define RCPOD_REG_T2CON	        0x0012
#define RCPOD_REG_CCPR1L        0x0015
#define RCPOD_REG_CCPR1H        0x0016
#define RCPOD_REG_CCP1CON       0x0017
#define RCPOD_REG_RCSTA         0x0018
#define RCPOD_REG_TXREG	        0x0019
#define RCPOD_REG_RCREG	        0x001A
#define RCPOD_REG_CCPR2L        0x001B
#define RCPOD_REG_CCPR2H        0x001C
#define RCPOD_REG_CCP2CON       0x001D
#define RCPOD_REG_ADRES	        0x001E
#define RCPOD_REG_ADCON0        0x001F
#define RCPOD_REG_OPTION_REG    0x0081
#define RCPOD_REG_TRISA	        0x0085
#define RCPOD_REG_TRISB	        0x0086
#define RCPOD_REG_TRISC	        0x0087
#define RCPOD_REG_TRISD	        0x0088
#define RCPOD_REG_TRISE	        0x0089
#define RCPOD_REG_PIE1	        0x008C
#define RCPOD_REG_PIE2	        0x008D
#define RCPOD_REG_PCON	        0x008E
#define RCPOD_REG_PR2	        0x0092
#define RCPOD_REG_TXSTA	        0x0098
#define RCPOD_REG_SPBRG	        0x0099
#define RCPOD_REG_ADCON1        0x009F


#endif /* __RCPOD_H */

/* The End */
