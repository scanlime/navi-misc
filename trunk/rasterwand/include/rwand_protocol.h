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

#endif

;//### The End ###

