;//################################################################################
;//
;// rcpod_protocol.h - Definitions describing the host to device protocol used by the
;//                    rcpod This file is both valid C and assembly source, so it can be
;//                    shared by host and device code.
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
#define RCPOD_PROTOCOL_VERSION  0x0101

;// Device vendor and product IDs.
;// The RCPOD's device class and protocol are both set to 'vendor-specific'.
#define RCPOD_VENDOR_ID   0xE461
#define RCPOD_PRODUCT_ID  0x0002

;//************************************************** Control requests

;// Write an 8-bit value (wValue) to a 9-bit address in RAM (wIndex)
#define RCPOD_CTRL_POKE		0x01

;// Read an 8-bit value from the given 9-bit RAM address (wIndex), return
;// it in a 1-byte data packet.
#define RCPOD_CTRL_PEEK		0x02

#endif

;//### The End ###

