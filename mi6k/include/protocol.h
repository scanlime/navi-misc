;//################################################################################
;//
;// protocol.h - Definitions describing the host to device protocol used by the
;//              MI6K This file is both valid C and assembly source, so it can be 
;//              shared by host and device code.
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
#define MI6K_PROTOCOL_VERSION  0x0100

;// Device vendor and product IDs.
;// The MI6K's device class and protocol are both set to 'vendor-specific'.
#define MI6K_VENDOR_ID   0xE461
#define MI6K_PRODUCT_ID  0x0001

;//************************************************** Control requests

;// Write up to 4 bytes to the VFD. The bytes
;// are stored contiguously in the setup packet's
;// wValue and wIndex fields. The VFD treats 0x00
;// as a no-op
#define MI6K_CTRL_VFD_WRITE   0x01

;// Turn on the VFD if wValue is 0x0001, turn it off
;// if wValue is 0x0000. Other values are undefined.
#define MI6K_CTRL_VFD_POWER   0x02

;// Set LED brightnesses. The brighness values range
;// from 0x0000 for fully off to 0x03FF for fully on.
;// wValue specifies the white LEDs brightness, wIndex
;// specifies the blue LEDs brightness.
#define MI6K_CTRL_LED_SET     0x03

;// Returns a 1-byte data packet with status bits
;// set according to the MI6K_STATUS_* constants.
#define MI6K_CTRL_STATUS      0x04

;//************************************************** Status bits

;// Set if external power is available
#define MI6K_STATUS_EXTPOWER  0x01

;// Set if the VFD is powered on
#define MI6K_STATUS_VFDPOWER  0x02

#endif

;//### The End ###

