;//################################################################################
;//
;// uvswitch_protocol.h - Definitions describing the host to device protocol used by the
;//                       uvswitch This file is both valid C and assembly source, so it can be
;//                       shared by host and device code.
;//
;// This file is part of the uvswitch project.
;// Micah Dowty <micah@picogui.org>
;//
;//###############################################################################

#ifndef __UVSWITCH_PROTOCOL_H
#define __UVSWITCH_PROTOCOL_H

;//************************************************** Idenfification

;// The protocol version, stored in binary coded decimal.
;// This is available from the device in the bcdDevice field
;// of its DEVICE descriptor.
#define UVSWITCH_PROTOCOL_VERSION  0x0101

;// Device vendor and product IDs.
;// The uvswitch's device class and protocol are both set to 'vendor-specific'.
#define UVSWITCH_VENDOR_ID   0xE461
#define UVSWITCH_PRODUCT_ID  0x0003

;//************************************************** Control requests

;// Set the current inputs. The current video input is the low byte of wValue,
;// the white audio input is the high byte of wIndex, and the red audio input
;// is the low byte of wIndex. Channels start at 1, a setting of 0 indicates
;// the output should be disconnected. If the high byte of wValue is nonzero,
;// the bypass input is active, otherwise the bypass output is connected to the
;// switch's video output.
#define UVSWITCH_CTRL_SWITCH	0x01

;//************************************************** Endpoints

;// The EP1 IN endpoint transmits 8-byte packets containing the video detector readings

#endif

;//### The End ###

