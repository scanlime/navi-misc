;//################################################################################
;//
;// laser_protocol.h - Definitions describing the host to device protocol used by the
;//                    USB Laser Projector. This file is both valid C and assembly source, so it can be
;//                    shared by the firmware and userspace code.
;//
;// This file is part of the Raster Wand project.
;// Micah Dowty <micah@navi.cx>
;//
;//###############################################################################

#ifndef __LASER_PROTOCOL_H
#define __LASER_PROTOCOL_H


;//************************************************** Idenfification

;// The protocol version, stored in binary coded decimal.
;// This is available from the device in the bcdDevice field
;// of its DEVICE descriptor.
#define LASER_PROTOCOL_VERSION  0x0100

;// Device vendor and product IDs.
#define LASER_VENDOR_ID   0xE461
#define LASER_PRODUCT_ID  0x0006


;//************************************************** Control Requests

;// Queue up an event, mapped as follows:
;//           wValue low: X axis, high byte
;//          wValue high: Y axis, high byte
;//           wIndex low: duration (0 is treated as 256)
;//   wIndex high, bit 7: coil driver enable
;//   wIndex high, bit 6: laser enable
;//   wIndex high, bit 5: Y axis, bit 1
;//   wIndex high, bit 4: Y axis, bit 0
;//   wIndex high, bit 3: reserved
;//   wIndex high, bit 2: reserved
;//   wIndex high, bit 1: X axis, bit 1
;//   wIndex high, bit 0: X axis, bit 0
;//
;// If there is insufficient room in the buffer for this event,
;// it will wait until there is room before queueing the new
;// event and sending an ACK.
;//
#define LASER_CTRL_ENQUEUE      0x01

;// Change the timebase adjustment to a 16-bit value stored in wValue.
;// Larger numbers shorten the units that event durations are measured in.
#define LASER_CTRL_SET_TIMEBASE 0x02

#endif

;//### The End ###

