;//################################################################################
;//
;// rcpod_protocol.h - Definitions describing the host to device protocol used by the
;//                    rcpod. This file is both valid C and assembly source, so it can be
;//                    shared by the firmware and librcpod.
;//
;// This file is part of the rcpod project.
;// Micah Dowty <micah@picogui.org>
;//
;//###############################################################################

#ifndef __RCPOD_PROTOCOL_H
#define __RCPOD_PROTOCOL_H


;//************************************************** Idenfification

;// The protocol version, stored in binary coded decimal.
;// This is available from the device in the bcdDevice field
;// of its DEVICE descriptor.
#define RCPOD_PROTOCOL_VERSION  0x0103

;// Device vendor and product IDs.
;// The RCPOD's device class and protocol are both set to 'vendor-specific'.
#define RCPOD_VENDOR_ID   0xE461
#define RCPOD_PRODUCT_ID  0x0002


;//************************************************** Pin Descriptors

;// Pin descriptors are used several places in the protocol,
;// where a pin on any I/O port with any polarity, or its corresponding
;// tristate register, need to be manipulated. Any pin can be set or cleared,
;// or turned into an input or an output with a pin descriptor.

;// A pin descriptor is represented as an 8-bit byte with the port number,
;// polarity, and bit number packed in. Note that zero is a no-op pin
;// descriptor, so a fixed-size block of pin descriptors can be processed
;// without any extra bytes for length.

;// MSB  7  6  5  4  3  2  1  0  LSB
;//      |  |  \_____/  \_____/
;//      |  |     |        |
;//      |  |     |        \___ Bit number
;//      |  |     |
;//      |  |     \____________ Port number, 0=no-op, 1=PORTA ... 5=PORTE
;//      |  |
;//      |  \__________________ 0=PORT*, 1=TRIS*
;//      |
;//      \_____________________ 0=active low, 1=active high


;//************************************************** Control requests

;//------------------ Peek/poke

;// Write an 8-bit value (wValue) to a 9-bit address in RAM (wIndex)
#define RCPOD_CTRL_POKE		0x01

;// Read an 8-bit value from the given 9-bit RAM address (wIndex), return
;// it in a 1-byte data packet.
#define RCPOD_CTRL_PEEK		0x02

;// Write 4 bytes sequentially after the last byte written with 'poke',
;// stored continuously in the wValue and wIndex fields.
#define RCPOD_CTRL_POKE4        0x03

;// Read 8 bytes starting at the address in wIndex
#define RCPOD_CTRL_PEEK8        0x04

;//------------------ Analog

;// Read all analog inputs at full precision, returns an 8-byte packet
#define RCPOD_CTRL_ANALOG_ALL   0x10

;//------------------ Asynchronous serial

;// Using current USART settings, perform an optional transmit and optional receive
;// using a buffer at the address in wIndex. The number of bytes specified in the
;// low byte of wValue are transmitted, then from the beginning of the same buffer
;// an asynchronous receive is started. It runs in the background, until the number
;// of bytes in the high byte of wValue have been received or UART_RX_END is called.
;// Either byte of wValue can be zero to perform only a transmit or only a receive.
#define RCPOD_CTRL_USART_TXRX   0x20

;// Cancels an ongoing USART receive, returns the number of bytes actually received
#define RCPOD_CTRL_USART_RX_END 0x21

;// Sets the pin descriptor to use as a USART transmit enable. When transmitting,
;// this pin is asserted, then after the transmission is complete it is deasserted.
;// This feature is disabled by setting the pin to zero (a no-op pin descriptor)
#define RCPOD_CTRL_USART_TXE    0x23

;//------------------ General purpose I/O

;// Assert up to 4 pin descriptors given in the bytes of wIndex and wValue.
;// Note that this can be used to deassert pin descriptors by toggling
;// their polairty first.
#define RCPOD_CTRL_GPIO_ASSERT  0x40

;// Read the value of the pin descriptor in wIndex
#define RCPOD_CTRL_GPIO_READ    0x41

#endif

;//### The End ###

