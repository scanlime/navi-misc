;//################################################################################
;//
;// mi6k_protocol.h - Definitions describing the host to device protocol used by the
;//                   MI6K This file is both valid C and assembly source, so it can be 
;//                   shared by host and device code.
;//
;// This file is part of the MI6K project.
;// Micah Dowty <micah@picogui.org>
;//
;//###############################################################################

#ifndef __MI6K_PROTOCOL_H
#define __MI6K_PROTOCOL_H

;//************************************************** Idenfification

;// The protocol version, stored in binary coded decimal.
;// This is available from the device in the bcdDevice field
;// of its DEVICE descriptor.
#define MI6K_PROTOCOL_VERSION  0x0102

;// Device vendor and product IDs.
;// The MI6K's device class and protocol are both set to 'vendor-specific'.
#define MI6K_VENDOR_ID   0xE461
#define MI6K_PRODUCT_ID  0x0001

;//************************************************** Control requests

;// Write up to 4 bytes to the VFD. The bytes
;// are stored contiguously in the setup packet's
;// wValue and wIndex fields. The VFD treats 0x00
;// as a no-op
#define MI6K_CTRL_VFD_WRITE		0x01

;// Turn on the VFD if wValue is 0x0001, turn it off
;// if wValue is 0x0000. Other values are undefined.
#define MI6K_CTRL_VFD_POWER		0x02

;// Set LED brightnesses. The brighness values range
;// from 0x0000 for fully off to 0x03FF for fully on.
;// wValue specifies the blue LEDs brightness, wIndex
;// specifies the white LEDs brightness.
#define MI6K_CTRL_LED_SET		0x03

;// Returns a 1-byte data packet with status bits
;// set according to the MI6K_STATUS_* constants.
#define MI6K_CTRL_STATUS		0x04

;// Queue two pulse/space pairs to be sent via the IR transmitter.
;// wIndex contains the first pulse in its low byte and the first space
;// in its high byte. wValue contains the second pulse and space.
;// Any value can be zero if it is not needed.
;// All lengths are specified in number of 38khz half-cycles. (13.16us units)
;// Pulses or spaces longer than 3.35ms must be split.
;// All IR values are buffered. The interrupt handler that reads the
;// buffer and actually sends IR signals is not started until the buffer
;// is full or until MI6K_CTRL_IR_FLUSH is sent.
#define MI6K_CTRL_IR_SEND		0x05

;// Start the IR transmit process to send the contents of the IR buffer,
;// if the buffer has data in it and the transmit process is not already running.
#define MI6K_CTRL_IR_FLUSH		0x06

;//************************************************** Status bits

;// Set if external power is available
#define MI6K_STATUS_EXTPOWER	0x01

;// Set if the VFD is powered on
#define MI6K_STATUS_VFDPOWER	0x02

;//************************************************** Endpoints

;// The EP1 IN endpoint is used for receiving IR timings.
;// It will send packets of either 4 or 8 bytes in length.
;// Each timer value is 16 bits, stored in little endian format.
;// The timing units are 4/3 microseconds. A value of 0xFFFF means
;// the timer overflowed. The first timer value in a packet is always
;// a pulse, pulses and spaces alternate.

#endif

;//### The End ###

