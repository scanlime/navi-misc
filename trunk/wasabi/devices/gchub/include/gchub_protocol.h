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
#define GCHUB_PROTOCOL_VERSION        0x0100

;// Device vendor and product IDs.
;// The device class and protocol are both set to 'vendor-specific'.
#define GCHUB_VENDOR_ID               0xE461
#define GCHUB_PRODUCT_ID              0x000A

;//************************************************** Control requests

;// Set the rumble motor enable bits for each controller,
;// from the four LSBs of wValue.
#define GCHUB_CTRL_SET_RUMBLE         0x01


;//************************************************** Endpoints

;// Endpoint EP1 IN sends 8 byte packets with controller
;// status. All controllers are polled in sequence, and up to two
;// packets are returned for each controller. Each EP1 packet
;// begins with a one-byte type code, identifying the controller
;// the packet describes and the format of the rest of the packet.
;// The low nybble of this code identifies the controller
;// (from 0 to 3) while the high nybble holds one of the
;// following codes:

;// Type code for packets identifying controllers that
;// are unplugged. No further information is encoded
;// in the packet.
#define GCHUB_PACKET_DISCONNECT  0x10

;// Type code for packets containing analog axis bytes:
;//   1. X axis
;//   2. Y axis
;//   3. C-stick X axis
;//   4. C-stick Y axis
;//   5. Left analog trigger
;//   6. Right analog trigger
#define GCHUB_PACKET_ANALOG      0x20

;// Packets containing button information
;//   1. Button bitfield:
;//       Bit 0: A button
;//       Bit 1: B button
;//       Bit 2: X button
;//       Bit 3: Y button
;//       Bit 4: Start button
;//   2. Button bitfield:
;//       Bit 0: D-pad Left
;//       Bit 1: D-pad Right
;//       Bit 2: D-pad down
;//       Bit 3: D-pad up
;//       Bit 4: Z button
;//       Bit 5: R button
;//       Bit 6: L button
#define GCHUB_PACKET_BUTTONS     0x30

#endif

;//### The End ###

