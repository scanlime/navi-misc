;################################################################################
;
; protocol.inc - Describes our device's protocol, used to communicate with it over USB
;
; Wasabi Net-controllers Project
; Micah Dowty <micah@navi.cx>
;
;###############################################################################

#ifndef __GENESIS_PROTOCOL_H
#define __GENESIS_PROTOCOL_H

;************************************************** Idenfification

; The protocol version, stored in binary coded decimal.
; This is available from the device in the bcdDevice field
; of its DEVICE descriptor.
#define GENESIS_PROTOCOL_VERSION  0x0100

; Device vendor and product IDs.
#define GENESIS_VENDOR_ID   0xE461
#define GENESIS_PRODUCT_ID  0x0009


;************************************************** Control Requests


#endif

; The End

