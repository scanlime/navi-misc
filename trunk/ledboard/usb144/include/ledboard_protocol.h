;//################################################################################
;//
;// ledboard_protocol.h - Definitions describing the host to device protocol used by the
;//                       LED board. This file is both valid C and assembly source, so it can be
;//                       shared by the firmware and host-side code.
;//
;// This file is part of the LED board project.
;// Micah Dowty <micah@navi.cx>
;//
;//###############################################################################

#ifndef __LEDBOARD_PROTOCOL_H
#define __LEDBOARD_PROTOCOL_H

;// 9 rows, 16 columns, one nybble per LED
#define LEDBOARD_VRAM_SIZE   0x48


;//************************************************** Idenfification

;// The protocol version, stored in binary coded decimal.
;// This is available from the device in the bcdDevice field
;// of its DEVICE descriptor.
#define LEDBOARD_PROTOCOL_VERSION  0x0100

;// Device vendor and product IDs.
#define LEDBOARD_VENDOR_ID   0xE461
#define LEDBOARD_PRODUCT_ID  0x0008


;//************************************************** Control Requests

;// Set the write pointer to the low byte of wValue, then write
;// the values at wValue+1, wIndex, and wIndex+1 to the backbuffer.
#define LEDBOARD_CTRL_RANDOM_WRITE3	0x01

;// Request a back->front page flip immediately preceeding the next display scan.
;// This doesn't return an ACK until a flip has completed successfully.
#define LEDBOARD_CTRL_FLIP              0x02

;// Starting at the current write pointer (reset on page flip) write the given
;// 12 bytes of data to the backbuffer, incrementing the write pointer as necessary.
;// The first 4 bytes come sequentially from wValue and wIndex, the other 8 bytes
;// are sent in a data packet.
#define LEDBOARD_CTRL_SEQ_WRITE12	0x03

;// Starting at the current write pointer (reset on page flip) write the given
;// 4 bytes in wValue and wIndex to the backbuffer
#define LEDBOARD_CTRL_SEQ_WRITE4	0x04

;// Set the current write pointer to the low byte of wValue,
;// write the contents of the following data packet (up to 8 bytes)
;// to the backbuffer.
#define LEDBOARD_CTRL_RANDOM_WRITE8	0x05

;// Copy wIndex bytes from the address at the low byte of wValue
;// to the address at the high byte of wValue. Leaves the write
;// pointer at the first byte after the end of the destination area.
;// wIndex must be nonzero.
#define LEDBOARD_CTRL_BLIT		0x06

#endif

;//### The End ###

