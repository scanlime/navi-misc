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
#define RCPOD_PROTOCOL_VERSION  0x0120

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
;// an asynchronous receive is started. It runs in the background until another
;// transmit is started. The receive can be stopped with no other effects by sending
;// a USART_TXRX with both bytes of wValue set to zero.
;// The receive pointer wraps back to the beginning of the buffer after the number
;// of bytes specified in the high byte of wValue has been received.
;// Either byte of wValue can be zero to perform only a transmit or only a receive.
#define RCPOD_CTRL_USART_TXRX   0x20

;// Sets the pin descriptor to use as a USART transmit enable, in wValue.
;// When transmitting, this pin is asserted, then after the transmission is
;// complete it is deasserted. This feature is disabled by setting the pin to
;// zero (a no-op pin descriptor)
#define RCPOD_CTRL_USART_TXE    0x21

;// Returns the number of bytes received so far in the current serial receive.
;// Note that this can be larger than the receive buffer size if the receive
;// buffer has wrapped around. Note also that this value will wrap around once
;// it passes 255.
#define RCPOD_CTRL_USART_RX_PROGRESS 0x22

;//------------------ I2C Bus Master

;// Set the current I2C bus parameters.
;//    wIndex high:  Bus speed, an RCPOD_I2C_SPEED_* constant
;//    wIndex low:   Current slave address
;//    wValue high:  Pin descriptor for clock
;//    wValue low:   Pin descriptor for data (simulated open drain)
#define RCPOD_CTRL_I2C_SET      0x30

;// Perform short I2C bus writes of various sizes. The bytes to
;// write are stored sequentially in wIndex and wValue, when applicable.
;//
;// Returns a 1-byte packet indicating the number of successful writes, including
;// the address bytes.
#define RCPOD_CTRL_I2C_WRITE0   0x31
#define RCPOD_CTRL_I2C_WRITE1   0x32
#define RCPOD_CTRL_I2C_WRITE2   0x33
#define RCPOD_CTRL_I2C_WRITE3   0x34
#define RCPOD_CTRL_I2C_WRITE4   0x35

;// Perform an I2C bus read, of length given in the low byte of wValue,
;// up to a maximum of 8 bytes. Returns an empty packet if the address
;// byte is not acknowledged.
#define RCPOD_CTRL_I2C_READ8    0x36

;// Write one byte, from wValue, then read wIndex bytes of data, up to 8
;// bytes. This is an optimization for the common case where you need
;// to write a command address then read from that address.
#define RCPOD_CTRL_I2C_W1READ8  0x37

;// Optionally do a generic transmit to the current address, then optionally
;// do a generic receive. Uses a buffer at the address in wIndex. The low byte
;// of wValue holds the number of bytes to transmit, the high byte of wValue
;// holds the number of bytes to receive.
#define RCPOD_CTRL_I2C_TXRX     0x38


;//------------------ General purpose I/O

;// Assert up to 4 pin descriptors given in the bytes of wIndex and wValue.
;// Note that this can be used to deassert pin descriptors by toggling
;// their polairty first.
#define RCPOD_CTRL_GPIO_ASSERT  0x40

;// Read the value of the pin descriptor in wIndex
#define RCPOD_CTRL_GPIO_READ    0x41

#endif

;//### The End ###

