;//################################################################################
;//
;// fieldsensor_protocol.h - Definitions describing the host to device protocol used by the
;//                          fieldsensor. This file is both valid C and assembly source, so it can be
;//                          shared by host and device code.
;//
;// The USB Electric Field Sensor project
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

#ifndef __FIELDSENSOR_PROTOCOL_H
#define __FIELDSENSOR_PROTOCOL_H

;//************************************************** Idenfification

;// The protocol version, stored in binary coded decimal.
;// This is available from the device in the bcdDevice field
;// of its DEVICE descriptor.
#define EFS_PROTOCOL_VERSION  0x0100

;// Device vendor and product IDs.
;// The device class and protocol are both set to 'vendor-specific'.
#define EFS_VENDOR_ID   0xE461
#define EFS_PRODUCT_ID  0x0004


;//************************************************** Control requests

;// Set the modulation frequency for the channel given in wIndex.
;// wValue = 3.0e6 / frequency
;// 0 <= wValue <= 255
#define EFS_CTRL_SET_FREQUENCY		0x10


;//************************************************** Endpoints

;// The EP1 IN endpoint receives packets of the form:
;//
;//   bytes   description
;//  --------------------------
;//     1     Channel number
;//     1     Number of samples integrated
;//     1     DC signal level
;//     1     Noise level
;//     2     In-phase accumulator
;//     2     Quadrature accumulator
;//
;// Multibyte values are in little endian, to match the USB spec.

#endif

;//### The End ###

