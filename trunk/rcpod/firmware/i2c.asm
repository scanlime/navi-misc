;################################################################################
;
; i2c.asm - Implements an I2C bus master in software
;
; The RCPOD project
; Copyright (C) 2004 Micah Dowty <micah@navi.cx>
;
;  This library is free software; you can redistribute it and/or
;  modify it under the terms of the GNU Lesser General Public
;  License as published by the Free Software Foundation; either
;  version 2.1 of the License, or (at your option) any later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;  Lesser General Public License for more details.
;
;  You should have received a copy of the GNU Lesser General Public
;  License along with this library; if not, write to the Free Software
;  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
;###############################################################################

	errorlevel -226		; suppress the crazy include file warnings on gpasm

#include <p16C765.inc>
#include "usb_defs.inc"

	errorlevel -302		; supress "register not in bank0, check page bits" message

	global	i2c_speed	; Current bus settings
	global	i2c_clock_pin
	global	i2c_data_pin
	global	i2c_address

	global	i2c_Write	; Write 'w' bytes from the buffer at IRP:FSR to the current address
	global	i2c_Read	; Write 'w' bytes into the buffer at IRP:FSR from the current address


bank1	udata

i2c_speed	res	1
i2c_clock_pin	res	1
i2c_data_pin	res	1
i2c_address	res	1

	code

;; **********************************************************************************
;; *************************************************************** Public Methods ***
;; **********************************************************************************

i2c_Write
	return

i2c_Read
	return

	end

;### The End ###
