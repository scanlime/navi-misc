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

	global	i2c_ack_count

	;; Put the bus in its idle state, resetting pin directions
	global	i2c_Reset

	;; Write/read 'w' bytes using the buffer at IRP:FSR and the current bus
	;; pins, speed, and address. Accumulates successful acks in i2c_ack_count.
	global	i2c_Write
	global	i2c_Read

	extern	io_pin
	extern	io_Assert
	extern	io_Deassert
	extern	io_Read

bank1	udata

i2c_speed	res	1
i2c_clock_pin	res	1
i2c_data_pin	res	1
i2c_address	res	1
i2c_ack_count	res	1

	code


;; **********************************************************************************
;; *************************************************************** I/O Utilities ****
;; **********************************************************************************

	;; Assert a pin descriptor, from the given file register
pin_assert macro reg
	banksel	reg
	movf	reg, w
	banksel	io_pin
	movwf	io_pin
	pscall	io_Assert
	endm

	;; Deassert a pin descriptor, from the given file register
pin_deassert macro reg
	banksel	reg
	movf	reg, w
	banksel	io_pin
	movwf	io_pin
	pscall	io_Deassert
	endm

	;; Make the given pin descriptor an output (TRIS deassert)
pin_output macro reg
	banksel	reg
	movf	reg, w
	iorlw	0x40		; Set the TRIS bit
	banksel	io_pin
	movwf	io_pin
	pscall	io_Deassert
	endm

	;; Make the given pin descriptor an output (TRIS assert)
pin_input macro reg
	banksel	reg
	movf	reg, w
	iorlw	0x40		; Set the TRIS bit
	banksel	io_pin
	movwf	io_pin
	pscall	io_Assert
	endm

	;; Manipulate the clock output
clock_high
	pin_assert i2c_clock_pin
	pin_output i2c_clock_pin
	return

clock_low
	pin_deassert i2c_clock_pin
	pin_output i2c_clock_pin
	return

	;; Manipulate the bidirectional open-drain data line
data_high
	pin_input i2c_data_pin
	return

data_low
	pin_deassert i2c_data_pin
	pin_output i2c_data_pin
	return

data_read
	banksel	i2c_data_pin
	movf	i2c_data_pin, w
	banksel	io_pin
	movwf	io_pin
	pscall	io_Read
	return

	;; Generate the appropriate delay between i2c bus transitions
i2c_delay
	return

	;; Put the bus into various states
i2c_stop macro
	pscall	data_low
	pscall	i2c_delay
	pscall	clock_high
	pscall	i2c_delay
	pscall	data_high
	endm

i2c_start macro
	pscall	clock_high
	pscall	data_high
	pscall	data_low
	pscall	i2c_delay
	pscall	clock_low
	endm

i2c_clockout macro
	pscall	clock_high
	pscall	i2c_delay
	pscall	clock_low
	endm


;; **********************************************************************************
;; *************************************************************** Public Methods ***
;; **********************************************************************************


i2c_Reset
	i2c_stop
	return

i2c_Write
	i2c_start

	i2c_stop
	return

i2c_Read
	i2c_start

	i2c_stop
	return

	end

;### The End ###
