;//################################################################################
;//
;// rwand_protocol.h - Definitions describing the host to device protocol used by the
;//                    Raster Wand. This file is both valid C and assembly source, so it can be
;//                    shared by the firmware and kernel code.
;//
;// This file is part of the Raster Wand project.
;// Micah Dowty <micah@navi.cx>
;//
;//###############################################################################

#ifndef __RWAND_PROTOCOL_H
#define __RWAND_PROTOCOL_H


;//************************************************** Idenfification

;// The protocol version, stored in binary coded decimal.
;// This is available from the device in the bcdDevice field
;// of its DEVICE descriptor.
#define RWAND_PROTOCOL_VERSION  0x0100

;// Device vendor and product IDs.
#define RWAND_VENDOR_ID   0xE461
#define RWAND_PRODUCT_ID  0x0005


;//************************************************** Control Requests

;// Return an 8-byte packet with the current edge buffer (4 16-bit LE words)
#define RWAND_CTRL_READ_EDGES		0x01

;// Return a 4-byte packet with the current predicted period and phase (2 16-bit LE words)
#define RWAND_CTRL_READ_PREDICTION	0x02

;// Set the phase to wValue and the period to wIndex
#define RWAND_CTRL_SET_PREDICTION	0x03

;// Set the coil driver to switch on when the predicted phase is between wValue and wIndex
#define RWAND_CTRL_SET_COIL_PHASE	0x04

#endif

;//### The End ###

