;//################################################################################
;//
;// gchub_protocol.h - Definitions describing the host to device protocol used by the
;//                    Gamecube Controller Hub. This file is both valid C and assembly
;//                    source, so it can be shared by host and device code.
;//
;// The Gamecube Controller Hub project
;// Copyright (C) 2004 Micah Dowty <micah@navi.cx>
;//
;//  This library is free software; you can redistribute it and/or
;//  modify it under the terms of the GNU Lesser General Public
;//  License as published by the Free Software Foundation; either
;//  version 2.1 of the License, or (at your option) any later version.
;//
;//  This library is distributed in the hope that it will be useful,
;//  but WITHOUT ANY WARRANTY; without even the implied warranty of
;//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;//  Lesser General Public License for more details.
;//
;//  You should have received a copy of the GNU Lesser General Public
;//  License along with this library; if not, write to the Free Software
;//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;//
;//###############################################################################

#ifndef __GCHUB_PROTOCOL_H
#define __GCHUB_PROTOCOL_H

;//************************************************** Idenfification

;// The protocol version, stored in binary coded decimal.
;// This is available from the device in the bcdDevice field
;// of its DEVICE descriptor.
#define GCHUB_PROTOCOL_VERSION        0x0101

;// Device vendor and product IDs.
;// The device class and protocol are both set to 'vendor-specific'.
#define GCHUB_VENDOR_ID               0xE461
#define GCHUB_PRODUCT_ID              0x000A


;//************************************************** Control requests

;// Set the rumble motor and LED status. LEDs are packed into
;// the low 8 bits of wIndex, the rumble motor enable bits for
;// each controller are in the four LSBs of wValue.
#define GCHUB_CTRL_SET_STATUS    0x01

#define GCHUB_RUMBLE_PORT0       0x01
#define GCHUB_RUMBLE_PORT1       0x02
#define GCHUB_RUMBLE_PORT2       0x04
#define GCHUB_RUMBLE_PORT3       0x08

#define GCHUB_LED_PORT0_RED      0x80
#define GCHUB_LED_PORT0_GREEN    0x40
#define GCHUB_LED_PORT1_RED      0x20
#define GCHUB_LED_PORT1_GREEN    0x10
#define GCHUB_LED_PORT2_RED      0x08
#define GCHUB_LED_PORT2_GREEN    0x04
#define GCHUB_LED_PORT3_RED      0x02
#define GCHUB_LED_PORT3_GREEN    0x01


;//************************************************** Endpoints

;// Endpoint EP1 IN sends 8 byte packets with controller
;// status. All controllers are polled in sequence, and up to two
;// packets are returned for each controller. Each EP1 packet
;// begins with a one-byte type code, identifying the controller
;// the packet describes and the format of the rest of the packet.
;// The low nybble of this code identifies the controller
;// (from 0 to 3) while the high nybble holds one of the
;// following codes.

;// (note that all bytes not mentioned are undefined)

;// also note that in some cases it will require multiple
;// packets to fully describe the state of the controller.
;// In this case, the current state should only be pushed
;// to an application when the GCHUB_SYNC bit is set:
#define GCHUB_SYNC               0x80

;// Type code for packets identifying controllers that
;// are unplugged. No meaningful data follows the code byte.
#define GCHUB_PACKET_DISCONNECT  0x10

;// Type code for packets containing analog axis bytes:
;//   1. X axis
;//   2. Y axis
;//   3. C-stick X axis
;//   4. C-stick Y axis
;//   5. Left analog trigger
;//   6. Right analog trigger
#define GCHUB_PACKET_ANALOG      0x20

;// Packets containing button information, as a 16-bit
;// big-endian bitfield containing GCHUB_BUTTON_* flags.
#define GCHUB_PACKET_BUTTONS     0x30

#define GCHUB_BUTTON_DPAD_LEFT   0x0001
#define GCHUB_BUTTON_DPAD_RIGHT  0x0002
#define GCHUB_BUTTON_DPAD_DOWN   0x0004
#define GCHUB_BUTTON_DPAD_UP     0x0008
#define GCHUB_BUTTON_Z           0x0010
#define GCHUB_BUTTON_R           0x0020
#define GCHUB_BUTTON_L           0x0040
#define GCHUB_BUTTON_A           0x0100
#define GCHUB_BUTTON_B           0x0200
#define GCHUB_BUTTON_X           0x0400
#define GCHUB_BUTTON_Y           0x0800
#define GCHUB_BUTTON_START       0x1000

#endif

;//### The End ###

