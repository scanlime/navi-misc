;//################################################################################
;//
;// protocol.h - Definitions describing the host to device protocol used by the
;//              MI6K This file is valid C and assembly source, so it can be 
;//              shared by host and device code.
;//
;// This file is part of the MI6K project.
;// Micah Dowty <micah@picogui.org>
;//
;//###############################################################################

#ifndef __MI6K_PROTOCOL_H
#define __MI6K_PROTOCOL_H

;// The protocol version, stored in binary coded decimal.
;// This is available from the device in the bcdDevice field
;// of its DEVICE descriptor.
#define MI6K_PROTOCOL_VERSION  0x0100

;// Device vendor and product IDs.
;// The MI6K's device class and protocol are both set to 'vendor-specific'.
#define MI6K_VENDOR_ID   0xE461
#define MI6K_PRODUCT_ID  0x0001

;// Vendor-specific control requests
#define MI6K_CTRL_VFD_WRITE  0x00

#endif

;//### The End ###

