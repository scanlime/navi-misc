;################################################################################
;
; io.asm - Functions to manipulate general purpose IO pins using pin descriptors
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

	global	io_pin		; Current pin descriptor

	global	io_Assert	; Assert the pin descriptor passed in io_pin
	global	io_Deassert	; Invert the polarity of io_pin then assert it
	global	io_Read		; Test the value of the pin descriptor io_pin, return it in 'w'

bank1	udata

io_pin		res	1	; Current pin number, supplied by other modules

io_tmp		res	1
io_byte		res	1
io_mask		res	1	; Mask with the current I/O bit, decoded from io_pin
io_base_fsr	res	1	; Current I/O base- points to LATA for writes, PORTA for reads.

LATA		res	1	; Latch registers, holding the current output values of all ports.
LATB		res	1	;    These are used to avoid read-modify-write errors on the GPIOs.
LATC		res	1
LATD		res	1
LATE		res	1

	code

io_Deassert
	banksel	io_pin
	movlw	0x80		; Toggle the active low/high bit
	xorwf	io_pin, f	; and fall through to io_Assert...

io_Assert
	banksel	io_base_fsr	; For non-TRIS output, write via the latch registers
	movlw	LATA
	movwf	io_base_fsr

	pscall	io_SetFSR	; Point at the current port...
	xorlw	0		; w=1 if the port is invalid, give up
	btfss	STATUS, Z
	return
	pscall	io_SetMask	; Mask off the current bit

	movf	INDF, w		; Put the old value of this port in io_byte
	movwf	io_byte

	comf	io_mask, w	; AND it with the complement of our current mask
	andwf	io_byte, f

	movf	io_mask, w	; If it's active-high, OR our mask back in
	btfsc	io_pin, 7
	iorwf	io_byte, f

	movf	io_byte, w	; Put io_byte back in INDF
	movwf	INDF

	btfsc	io_pin, 6	; If that was a TRIS pin descriptor, we're done
	return

	banksel	io_base_fsr	; If not, we need to update the PORT with the new latch value
	movlw	PORTA
	movwf	io_base_fsr
	pscall	io_SetFSR

	movf	io_byte, w
	movwf	INDF
	return


	; Test the current pin descriptor, return its value in w
io_Read
	banksel	io_base_fsr	; Point at the correct PORT or TRIS register
	movlw	PORTA
	movwf	io_base_fsr
	pscall	io_SetFSR
	xorlw	0		; w=1 if the port is invalid, always return 0
	btfss	STATUS, Z
	retlw	0
	pscall	io_SetMask	; Mask off the current bit

	movf	INDF, w		; Copy the current port value into io_byte
	movwf	io_byte

	btfss	io_pin, 7	; If the 'active high' bit is not set, complement our port reading
	comf	io_byte, f

	movf	io_mask, w	; Test against our mask
	andwf	io_byte, w
	btfss	STATUS, Z
	retlw	1
	retlw	0


	;; Set FSR to point to the I/O port indicated by the current pin descriptor.
	;; If the pin descriptor is invalid, returns with w=1, on success returns w=0.
	;; For non-TRIS ports, uses the base address in io_base_fsr.
io_SetFSR
	bankisel PORTA		; All PORT, TRIS, and LAT registers are at an address < 0x100

	banksel	io_pin
	movlw	TRISA		; Set the base address to TRISA if the TRIS bit is set
	btfsc	io_pin, 6
	movwf	io_base_fsr

	rrf	io_pin, w	; Shift and mask the port number, put it in io_tmp
	movwf	io_tmp
	rrf	io_tmp, f
	rrf	io_tmp, w
	andlw	0x07
	movwf	io_tmp

	movlw	5		; 5 is PORTE, the highest allowed port
	subwf	io_tmp, w
	btfsc	STATUS, C	; If B=0, C=1 and the port is bad
	retlw	1
	decfsz	io_tmp, f	; Port 0 is a no-op
	retlw	1

	movf	io_base_fsr, w	; FSR = io_base_fsr + io_tmp
	addwf	io_tmp, w
	movwf	FSR
	retlw	0


	;; Set io_mask with the bit referred to in io_pin
io_SetMask
	banksel	io_pin
	andlw	0x07		; Mask off the bit number
	movwf	io_tmp		; ...and put it in io_tmp

	movlw	1		; Start with the mask 0x01
	movwf	io_mask

set_mask_loop
	movf	io_tmp, w	; Terminate when the bit count is zero
	btfsc	STATUS, Z
	return

	rlf	io_mask, f	; Otherwise rotate the IO mask and decrement the bit count
	decf	io_tmp, f
	psgoto	set_mask_loop


	end

;### The End ###
