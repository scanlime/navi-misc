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

; Set the current state of both controllers. The bits of
; wValue are allocated as follows:
;
;   0 - Controller 1, D-Pad Up
;   1 - Controller 1, D-Pad Down
;   2 - Controller 1, D-Pad Left
;   3 - Controller 1, D-Pad Right
;   4 - Controller 1, A button
;   5 - Controller 1, B button
;   6 - Controller 1, C button
;   7 - Controller 1, Start button
;   8 - Controller 2, D-Pad Up
;   9 - Controller 2, D-Pad Down
;  10 - Controller 2, D-Pad Left
;  11 - Controller 2, D-Pad Right
;  12 - Controller 2, A button
;  13 - Controller 2, B button
;  14 - Controller 2, C button
;  15 - Controller 2, Start button
;
; And in wIndex:
;
;   0 - Reset button
;   1 - Power switch
;
#define GENESIS_CTRL_SET_STATUS 	0x01

#define STATUS_C1_UP	BufferData+wValue, 0
#define STATUS_C1_DOWN	BufferData+wValue, 1
#define STATUS_C1_LEFT	BufferData+wValue, 2
#define STATUS_C1_RIGHT	BufferData+wValue, 3
#define STATUS_C1_A	BufferData+wValue, 4
#define STATUS_C1_B	BufferData+wValue, 5
#define STATUS_C1_C	BufferData+wValue, 6
#define STATUS_C1_START	BufferData+wValue, 7
#define STATUS_C2_UP	BufferData+(wValue+1), 0
#define STATUS_C2_DOWN	BufferData+(wValue+1), 1
#define STATUS_C2_LEFT	BufferData+(wValue+1), 2
#define STATUS_C2_RIGHT	BufferData+(wValue+1), 3
#define STATUS_C2_A	BufferData+(wValue+1), 4
#define STATUS_C2_B	BufferData+(wValue+1), 5
#define STATUS_C2_C	BufferData+(wValue+1), 6
#define STATUS_C2_START	BufferData+(wValue+1), 7
#define STATUS_RESET	BufferData+wIndex, 0
#define STATUS_POWER	BufferData+wIndex, 1

#endif

; The End

