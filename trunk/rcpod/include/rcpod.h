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

/*************************************************************************************/
/************************************************** Data types ***********************/
/*************************************************************************************/

/* An opaque type representing an open rcpod device. This is returned by rcpod_init_simple
 * or rcpod_open. It must be freed with rcpod_close.
 */
typedef struct tag_rcpod_dev rcpod_dev;

/* A librcpod error handler function, as passed to rcpod_SetErrorHandler.
 *   function: Name of the function where the error was first reported
 *        err: The C errno value of the failed function if applicable, otherwise zero
 *    message: A description of the error
 */
typedef void (rcpod_errorHandler)(const char *function, int err, const char *message);

/* A pin descriptor, used in the low-level commands to specify general purpose
 * I/O pins in a generic way. It is made up of several bitfields described in detail in
 * rcpod_protocol.h. Pin descriptors can be constructed and manipulated using the RCPOD_PIN*
 * constants and macros defined below.
 */
typedef unsigned char rcpod_pin;


/*************************************************************************************/
/************************************************** High-level initialization ********/
/*************************************************************************************/

/* Initializes libusb and librcpod, finds the first rcpod device,
 * opens it, and resets it. If your application needs finer control
 * over the initialization steps, it may need to use the low-level functions directly.
 */
rcpod_dev* rcpod_InitSimple(void);

/* Like rcpod_InitSimple, but skip resetting the device. This is useful for programs
 * that only need to deal with one rcpod device, but need to preserve the previous
 * state of the device.
 */
rcpod_dev* rcpod_InitSimpleWithoutReset(void);


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
/************************************************** Memory Access ********************/
/*************************************************************************************/

/* Write one byte to the given 9-bit address in the rcpod's RAM
 * For interesting RAM addresses, refer to the constants section and
 * to the PIC16C745/765 data sheet.
 */
void rcpod_Poke(rcpod_dev* rcpod, int address, unsigned char data);

/* Read one byte from the given 9-bit address in the rcpod's RAM */
unsigned char rcpod_Peek(rcpod_dev* rcpod, int address);

/* Peek an arbitrary-sized block of the PIC's address space in the most efficient way.
 * reads 'bytes' bytes of data from the indicated address into the provided buffer.
 * The result is undefined if the reads extend past the PIC's address space.
 */
void rcpod_PeekBuffer(rcpod_dev* rcpod, int address, unsigned char *buffer, int bytes);

/* Write an arbitrary-sized block of data into the PIC's address space in the most
 * efficient way. Writes 'bytes' bytes of data from the given buffer into the
 * indicated address. The result is undefined if the writes extend past the PIC's
 * address space.
 */
void rcpod_PokeBuffer(rcpod_dev* rcpod, int address, unsigned char *buffer, int bytes);


/*************************************************************************************/
/************************************************** General-Purpose I/O **************/
/*************************************************************************************/

/* Assert one pin descriptor, setting it to its active state */
void rcpod_GpioAssert(rcpod_dev* rcpod, rcpod_pin pin);

/* Assert a block of pin descriptors in the most efficient way, in the order
 * given. Processes 'count' pin descriptors from the given array.
 */
void rcpod_GpioAssertBuffer(rcpod_dev* rcpod, rcpod_pin *pins, int count);

/* Deassert one pin descriptor, setting it to its inactive state.
 * Note that the terms assert/deassert are used rather than specifying the
 * logic level, since a pin descriptor may be active high or active low.
 */
void rcpod_GpioDeassert(rcpod_dev* rcpod, rcpod_pin pin);

/* Deassert a block of pin descriptors in the most efficient way, in the order
 * given. Processes 'count' pin descriptors from the given array.
 */
void rcpod_GpioDeassertBuffer(rcpod_dev* rcpod, rcpod_pin *pins, int count);

/* Test the value of the given pin descriptor */
int rcpod_GpioTest(rcpod_dev* rcpod, rcpod_pin pin);


/*************************************************************************************/
/************************************************** Analog I/O ***********************/
/*************************************************************************************/

/* Sample all 8 of the 8-bit A/D converter channels, fills the provided buffer.
 * Faster than calling AnalogReadChannel, even when reading only one channel.
 */
void rcpod_AnalogReadAll(rcpod_dev* rcpod, unsigned char buffer[8]);

/* Read one of the 8 8-bit A/D converter channels. The result is undefined
 * if the channel is not in the range [0,7]. Always slower than using
 * rcpod_AnalogReadAll, but this function may be necessary if you require
 * extra aquisition time for reading high-impedance signals or you have other
 * special requirements.
 */
unsigned char rcpod_AnalogReadChannel(rcpod_dev* rcpod, int channel);


/*************************************************************************************/
/************************************************** Serial I/O ***********************/
/*************************************************************************************/



/*************************************************************************************/
/************************************************** Constants ************************/
/*************************************************************************************/

/* Size of the address space reachable via peek and poke */
#define RCPOD_MEM_SIZE          0x0200

/* The PIC's first bank of general-purpose RAM is designated as a scratchpad area.
 * Note that this is used in librcpod for serial I/O.
 */
#define RCPOD_REG_SCRATCHPAD    0x0020
#define RCPOD_SCRATCHPAD_SIZE   0x0050

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

/* A pin descriptor should be a logical or of one constant from each of the following sections... */
#define RCPOD_PIN_PORTA    (1 << 3)
#define RCPOD_PIN_PORTB    (2 << 3)
#define RCPOD_PIN_PORTC    (3 << 3)
#define RCPOD_PIN_PORTD    (4 << 3)
#define RCPOD_PIN_PORTE    (5 << 3)

#define RCPOD_PIN_TRIS     (1 << 6)
#define RCPOD_PIN_PORT     0

#define RCPOD_PIN_BIT0     0
#define RCPOD_PIN_BIT1     1
#define RCPOD_PIN_BIT2     2
#define RCPOD_PIN_BIT3     3
#define RCPOD_PIN_BIT4     4
#define RCPOD_PIN_BIT5     5
#define RCPOD_PIN_BIT6     6
#define RCPOD_PIN_BIT7     7

#define RCPOD_PIN_LOW      0
#define RCPOD_PIN_HIGH     (1 << 7)

/* Given any pin descriptor, create one which will convert the given pin into an output.
 * Note that deasserting/negating the returned pin descriptor will convert the given
 * pin into an input.
 */
#define RCPOD_OUTPUT(pin)    (((pin) | RCPOD_PIN_TRIS) & ~RCPOD_PIN_HIGH)

/* Like RCPOD_OUTPUT, but convert the given pin into an input */
#define RCPOD_INPUT(pin)    ((pin) | RCPOD_PIN_TRIS | RCPOD_PIN_HIGH)

/* Inverts the polarity of the given pin descriptor (swaps active low and active high) */
#define RCPOD_NEGATE(pin)  ((pin) ^ RCPOD_PIN_HIGH)

/* Macros for normal active high I/O port pin descriptors. Combined with the RCPOD_TRIS
 * and RCPOD_NEGATE macros, the above RCPOD_PIN_* constants shouldn't have to be used
 * directly by most programs. Note that this doesn't include all possible ports and pins,
 * only those that physically exist on the PIC16C765.
 *
 * Note that most of these pins default to general purpose I/O but that may be overridden
 * by using the PIC's built-in peripherals. Also note that RA4 is an open-drain output
 * (only able to drive to 0v or float, can't drive to 5v)
 *
 * Also note that pins supporting analog input (RA* except for RA4, RE*) default to analog
 * mode rather than digital. This is necessary because analog voltage levels on a pin
 * configured for digital may be damaging.
 */
#define RCPOD_PIN_RA0      (RCPOD_PIN_PORTA | RCPOD_PIN_PORT | RCPOD_PIN_BIT0 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RA1      (RCPOD_PIN_PORTA | RCPOD_PIN_PORT | RCPOD_PIN_BIT1 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RA2      (RCPOD_PIN_PORTA | RCPOD_PIN_PORT | RCPOD_PIN_BIT2 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RA3      (RCPOD_PIN_PORTA | RCPOD_PIN_PORT | RCPOD_PIN_BIT3 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RA4      (RCPOD_PIN_PORTA | RCPOD_PIN_PORT | RCPOD_PIN_BIT4 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RA5      (RCPOD_PIN_PORTA | RCPOD_PIN_PORT | RCPOD_PIN_BIT5 | RCPOD_PIN_HIGH)

#define RCPOD_PIN_RB0      (RCPOD_PIN_PORTB | RCPOD_PIN_PORT | RCPOD_PIN_BIT0 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RB1      (RCPOD_PIN_PORTB | RCPOD_PIN_PORT | RCPOD_PIN_BIT1 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RB2      (RCPOD_PIN_PORTB | RCPOD_PIN_PORT | RCPOD_PIN_BIT2 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RB3      (RCPOD_PIN_PORTB | RCPOD_PIN_PORT | RCPOD_PIN_BIT3 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RB4      (RCPOD_PIN_PORTB | RCPOD_PIN_PORT | RCPOD_PIN_BIT4 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RB5      (RCPOD_PIN_PORTB | RCPOD_PIN_PORT | RCPOD_PIN_BIT5 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RB6      (RCPOD_PIN_PORTB | RCPOD_PIN_PORT | RCPOD_PIN_BIT6 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RB7      (RCPOD_PIN_PORTB | RCPOD_PIN_PORT | RCPOD_PIN_BIT7 | RCPOD_PIN_HIGH)

#define RCPOD_PIN_RC0      (RCPOD_PIN_PORTC | RCPOD_PIN_PORT | RCPOD_PIN_BIT0 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RC1      (RCPOD_PIN_PORTC | RCPOD_PIN_PORT | RCPOD_PIN_BIT1 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RC2      (RCPOD_PIN_PORTC | RCPOD_PIN_PORT | RCPOD_PIN_BIT2 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RC6      (RCPOD_PIN_PORTC | RCPOD_PIN_PORT | RCPOD_PIN_BIT6 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RC7      (RCPOD_PIN_PORTC | RCPOD_PIN_PORT | RCPOD_PIN_BIT7 | RCPOD_PIN_HIGH)

#define RCPOD_PIN_RD0      (RCPOD_PIN_PORTD | RCPOD_PIN_PORT | RCPOD_PIN_BIT0 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RD1      (RCPOD_PIN_PORTD | RCPOD_PIN_PORT | RCPOD_PIN_BIT1 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RD2      (RCPOD_PIN_PORTD | RCPOD_PIN_PORT | RCPOD_PIN_BIT2 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RD3      (RCPOD_PIN_PORTD | RCPOD_PIN_PORT | RCPOD_PIN_BIT3 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RD4      (RCPOD_PIN_PORTD | RCPOD_PIN_PORT | RCPOD_PIN_BIT4 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RD5      (RCPOD_PIN_PORTD | RCPOD_PIN_PORT | RCPOD_PIN_BIT5 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RD6      (RCPOD_PIN_PORTD | RCPOD_PIN_PORT | RCPOD_PIN_BIT6 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RD7      (RCPOD_PIN_PORTD | RCPOD_PIN_PORT | RCPOD_PIN_BIT7 | RCPOD_PIN_HIGH)

#define RCPOD_PIN_RE0      (RCPOD_PIN_PORTE | RCPOD_PIN_PORT | RCPOD_PIN_BIT0 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RE1      (RCPOD_PIN_PORTE | RCPOD_PIN_PORT | RCPOD_PIN_BIT1 | RCPOD_PIN_HIGH)
#define RCPOD_PIN_RE2      (RCPOD_PIN_PORTE | RCPOD_PIN_PORT | RCPOD_PIN_BIT2 | RCPOD_PIN_HIGH)


#endif /* __RCPOD_H */

/* The End */
